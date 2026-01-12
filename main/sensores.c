#include "sensores.h"

QueueHandle_t fila_sensor, fila_sensor_to_servidor;;

void leitura_sensor(void *pvparameters)
{
    dados_sensores dados;
    struct dht11_reading data_dht11;

    while (1)
    {
        data_dht11 = DHT11_read();

        
        int gas_raw = adc1_get_raw(ADC1_CHANNEL_0);
        //dados.chamas = adc1_get_raw(ADC1_CHANNEL_3);
        dados.chamas = gpio_get_level(SENSOR_CHAMA);
        dados.temperatura = data_dht11.temperature;
        dados.umidade = data_dht11.humidity;

        dados.gas = gas_raw * 100 / 4095;
        
        xQueueOverwrite(fila_sensor_to_servidor, &dados);
        xQueueOverwrite(fila_sensor, &dados);

        vTaskDelay(pdMS_TO_TICKS(500));  // Aguarda 0,5 segundos
    }
}

void dht11_main(void)
{
    DHT11_init(DHT11_PIN);

    adc1_config_width(ADC_WIDTH_BIT_12);                  // resolução 12 bits
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // atenuação para 0-3.9V
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11); // atenuação para 0-3.9V


    fila_sensor = xQueueCreate(1, sizeof( dados_sensores ));
    fila_sensor_to_servidor = xQueueCreate(1, sizeof( dados_sensores ));

    /*tirar*/
    xTaskCreate(leitura_sensor, "leitura de sensores", 2048, NULL, 2, NULL);
}
