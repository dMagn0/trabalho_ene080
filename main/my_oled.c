#include "my_oled.h"

/*  troca   */
#include "my_rfid.h"
/*          */


SSD1306_t dev;

void display_init(void) {
    
    i2c_master_init(&dev, OLED_GPIO_SDA, OLED_GPIO_SCL, CONFIG_RESET_GPIO);

	ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
}

void escreve_oled(char linha, char *data_m, size_t msize){
    // uint16_t data;
    // char data_m[200];
    // size_t msize;
    // while (1)
    // {
        // msize = xMessageBufferReceive(buffer_rfid, data_m, sizeof(data_m), portMAX_DELAY);
        // ESP_LOGI("LEITURA", "%.*s", msize, data_m);
        ssd1306_clear_screen(&dev, false);
        ssd1306_display_text(&dev, linha, data_m, msize, false);
    // }

}