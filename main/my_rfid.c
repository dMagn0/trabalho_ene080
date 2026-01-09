#include "my_rfid.h"

MessageBufferHandle_t buffer_rfid;

static const char *TAG = "rc522";

static rc522_spi_config_t driver_config = {
    .host_id = SPI3_HOST,
    .bus_config = &(spi_bus_config_t){
        .miso_io_num = RC522_SPI_BUS_GPIO_MISO,
        .mosi_io_num = RC522_SPI_BUS_GPIO_MOSI,
        .sclk_io_num = RC522_SPI_BUS_GPIO_SCLK,
    },
    .dev_config = {
        .spics_io_num = RC522_SPI_SCANNER_GPIO_SDA,
    },
    .rst_io_num = RC522_SCANNER_GPIO_RST,
};

typedef enum
{
    RC522_STATE_UNDEFINED = 0,
    RC522_STATE_CREATED,
    RC522_STATE_POLLING, /*<! Scanning for nearby PICCs */
    RC522_STATE_PAUSED,
} rc522_state_t;
struct rc522
{
    rc522_config_t *config;               /*<! Configuration */
    bool exit_requested;                  /*<! Indicates whether polling task exit is requested */
    TaskHandle_t task_handle;             /*<! Handle of task */
    esp_event_loop_handle_t event_handle; /*<! Handle of event loop */
    rc522_state_t state;                  /*<! Current state */
    rc522_picc_t picc;
    EventGroupHandle_t bits;
};

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;

static void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data){
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE) {
      
        char uid_str[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
        //RC522_RETURN_ON_ERROR(rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str)));
        rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str));
        //printf("%s\n", uid_str);

        xMessageBufferSend(buffer_rfid, uid_str, sizeof(uid_str), portMAX_DELAY);
        // rc522_picc_print(picc);

    }
    else if (picc->state == RC522_PICC_STATE_IDLE && event->old_state >= RC522_PICC_STATE_ACTIVE) {
        ESP_LOGI(TAG, "Cartão Removido");
    }
}


void rfid_pausa_leitura(){
    rc522_pause(scanner);
}
void rfid_start(){
    if(scanner->state == 2/*RC522_STATE_POLLING*/){
        return;
    }
    rc522_start(scanner);
}

void rfid_main(){
    buffer_rfid = xMessageBufferCreate(64);

    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);
    rc522_start(scanner);
}