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


void escreve_oled(char *linha_0, size_t msize_0, char *linha_3, size_t msize_3){

    ssd1306_clear_screen(&dev, false);
    ssd1306_display_text(&dev, 0, linha_0, msize_0, false);
    ssd1306_display_text(&dev, 1, linha_3, msize_3, false);
    
}