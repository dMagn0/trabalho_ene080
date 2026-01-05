#include "my_rfid.h"

MessageBufferHandle_t buffer_rfid;

static void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE) {
      
        char uid_str[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
        //RC522_RETURN_ON_ERROR(rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str)));
        rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str));
        //printf("%s\n", uid_str);

        xMessageBufferSend(buffer_rfid, uid_str, sizeof(uid_str), portMAX_DELAY);
        rc522_picc_print(picc);

    }
    else if (picc->state == RC522_PICC_STATE_IDLE && event->old_state >= RC522_PICC_STATE_ACTIVE) {
        ESP_LOGI(TAG, "Cartão Removido");
    }
}

void rfid_main(){
    buffer_rfid = xMessageBufferCreate(200);

    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);
    rc522_start(scanner);
}