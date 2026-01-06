#include "utilities.h"
#include "my_rfid.h"
#include "my_oled.h"
#include "sensores.h"
#include "servidor_http.h"




void monitora_sensores(void *pvparameters){
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

void monitora_rfid(void *pvparameters){
    char data_m[200];
    char linha_oled[32];
    size_t msize;
    while (1)
    {
        msize = xMessageBufferReceive(buffer_rfid, data_m, sizeof(data_m), portMAX_DELAY);
        ESP_LOGI("LEITURA", "chave: %.*s", msize, data_m);

        apaga_oled();
        conta_t* conta = {0};
        switch (get_conta_por_chave(data_m, conta)){
            case OP_CANCELADA: /*nao tem */
            break;
            case OP_INVALIDA:
                escreve_oled(data_m, msize, 3);
                strcpy(linha_oled, "CARTAO NAO CADASTRADO");
                escreve_oled(linha_oled, strlen(linha_oled), 1);
                break;
            case OP_SUCESSO:
                escreve_oled(conta->nome, strlen(conta->nome), 1);
                snprintf(linha_oled, sizeof(linha_oled), "R$ %.2f", conta->saldo);
                escreve_oled(linha_oled, strlen(linha_oled), 3);
            break;
        }
    }
}

void app_main()
{
    display_init();
    rfid_main();
    dht11_main();
    vTaskDelay(pdMS_TO_TICKS(5000));
    http_main();

    xTaskCreate(monitora_sensores, "sensores", 2048, NULL, 2, NULL);
    xTaskCreate(monitora_rfid, "rfid", 2048, NULL, 2, NULL);

}

