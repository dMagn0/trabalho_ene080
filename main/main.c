#include "utilities.h"

MessageBufferHandle_t buffer_rfid;

static const char *TAG = "rc522-basic-example";

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

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;

SSD1306_t dev;

void display_init(void) {
    
    i2c_master_init(&dev, OLED_GPIO_SDA, OLED_GPIO_SCL, CONFIG_RESET_GPIO);

	ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
}


static void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE) {
      
        char uid_str[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
        // RC522_RETURN_ON_ERROR(rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str)));
        rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str));
        printf("%s\n", uid_str);

        xMessageBufferSend(buffer_rfid, uid_str, sizeof(uid_str), portMAX_DELAY);
        rc522_picc_print(picc);

    }
    else if (picc->state == RC522_PICC_STATE_IDLE && event->old_state >= RC522_PICC_STATE_ACTIVE) {
        ESP_LOGI(TAG, "Card has been removed");
    }
}

void escreve_oled(void *pvparameters){
    uint16_t data;
    char data_m[200];
    size_t msize;
    while (1)
    {
        msize = xMessageBufferReceive(buffer_rfid, data_m, sizeof(data_m), portMAX_DELAY);
        ESP_LOGI("LEITURA", "%.*s", msize, data_m);
        ssd1306_clear_screen(&dev, false);
        ssd1306_display_text(&dev, 0, data_m, msize, false);

    }

}

void app_main()
{
    buffer_rfid = xMessageBufferCreate(200);

    display_init();

    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);
    rc522_start(scanner);

    xTaskCreate(escreve_oled, "oled", 4096, NULL, 2, NULL);


}

