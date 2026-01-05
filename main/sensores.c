#include "sensores.h"

QueueHandle_t fila_sensor;

void leitura_sensor(void *pvparameters)
{
    struct dados_sensores dados;
    struct dht11_reading data_dht11;

    while (1)
    {
        data_dht11 = DHT11_read();

        dados.gas = 1;
        dados.temperatura = data_dht11.temperature;
        dados.umidade = data_dht11.humidity;
        
        xQueueSendToBack(fila_sensor, &dados, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));  // Aguarda 0,5 segundos
    }
}

void dht11_main(void)
{
    DHT11_init(DHT11_PIN);
    // struct dados_sensores dados;

    fila_sensor = xQueueCreate(4, sizeof( struct dados_sensores ));

    /*tirar*/
    // xTaskCreate(display_sens, "display", 2048, NULL, 2, NULL);
}
