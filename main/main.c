#include "utilities.h"
#include "my_rfid.h"
#include "my_oled.h"
#include "sensores.h"




void display_sens(void *pvparameters)
{
    struct dados_sensores dados;
    while (1)
    {
        ESP_LOGI("Display", "Recebendo valor médio de Temperatura e Umidade");

        xQueueReceive(fila_sensor, &dados, portMAX_DELAY);
        ESP_LOGI("Display", "Média gas: %d", dados.gas);
        ESP_LOGI("Display", "Média temperatura: %d", dados.temperatura);
        ESP_LOGI("Display", "Média umidade: %d", dados.umidade);
    }
}


void app_main()
{
    display_init();
    rfid_main();
    dht11_main();

    // xTaskCreate(escreve_oled, "oled", 4096, NULL, 2, NULL);
    xTaskCreate(leitura_sensor, "temp_umid", 2048, NULL, 2, NULL);
    xTaskCreate(display_sens, "display", 2048, NULL, 2, NULL);

    // char datam[200]  = "denis magno\0";

    // escreve_oled(0,datam,sizeof(datam));
    // vTaskDelay(pdMS_TO_TICKS(2000));
    // escreve_oled(2,datam,sizeof(datam));
    // escreve_oled(0,datam,sizeof(datam));
    // vTaskDelay(pdMS_TO_TICKS(2000));
    // escreve_oled(3,datam,sizeof(datam));

}

