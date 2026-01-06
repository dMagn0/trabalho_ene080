#include "my_oled.h"

SSD1306_t dev;

void display_init(void) {
    
    i2c_master_init(&dev, OLED_GPIO_SDA, OLED_GPIO_SCL, CONFIG_RESET_GPIO);

	ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
}

void apaga_oled(){
    ssd1306_clear_screen(&dev, false);
}
void escreve_oled(char *frase, size_t msize, int linha){

    ssd1306_display_text(&dev, linha, frase, msize, false);    
}