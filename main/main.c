#include "utilities.h"
#include "my_rfid.h"
#include "my_oled.h"
#include "sensores.h"
#include "servidor_http.h"



void monitora_sensores(void *pvparameters){
    dados_sensores dados = {0};
    while (1)
    {
        // ESP_LOGI("Display", "Recebendo valor médio de Temperatura e Umidade");
        //atualiza_dados_sensores(dados);
        xQueueReceive(fila_sensor, &dados, portMAX_DELAY);
        ESP_LOGI("Display", "Média gas: %d", dados.gas);
        ESP_LOGI("Display", "Média chamas: %d", dados.chamas);
        ESP_LOGI("Display", "Média temperatura: %d", dados.temperatura);
        ESP_LOGI("Display", "Média umidade: %d", dados.umidade);    
        
    }

}

static int TEMPO_DE_BLOQUEIO_DA_FILA = 400;
void monitora_rfid(void *pvparameters){
    char data_m[200];
    char linha_oled[32];
    size_t msize;
    while (1)
    {
        rfid_start();
        msize = xMessageBufferReceive(buffer_rfid, data_m, sizeof(data_m), portMAX_DELAY);
        rfid_pausa_leitura();

        // ESP_LOGI("LEITURA", "chave: %.*s", msize, data_m);

        apaga_oled();
        conta_t conta;
        switch (get_conta_por_chave(data_m, &conta)){
            case OP_INVALIDA:

                escreve_oled(data_m, msize, 3);

                strcpy(linha_oled, "CARTAO NAO");
                escreve_oled(linha_oled, strlen(linha_oled), 1);

                strcpy(linha_oled, "CADASTRADO");
                escreve_oled(linha_oled, strlen(linha_oled), 2);

            break;
            case OP_SUCESSO:
                switch (saque(conta.chave,5)){
                    case OP_CANCELADA:
                        escreve_oled(conta.nome, strlen(conta.nome), 1);
                        
                        strcpy(linha_oled, "Acesso negado");
                        escreve_oled(linha_oled, strlen(linha_oled), 2);

                        snprintf(linha_oled, sizeof(linha_oled), "R$ %.2f", conta.saldo);
                        escreve_oled(linha_oled, strlen(linha_oled), 3);    

                    break;
                    case OP_SUCESSO:
                        escreve_oled(conta.nome, strlen(conta.nome), 1);
                        snprintf(linha_oled, sizeof(linha_oled), "R$ %.2f", conta.saldo);
                        escreve_oled(linha_oled, strlen(linha_oled), 3);

                    break;
                    default:
                        ESP_LOGE("LEITURA", "Falha no saque.");
                    break;
                }
            break;
            default:
                ESP_LOGE("LEITURA", "Falha no get conta.");
            break;
        }

        vTaskDelay(TEMPO_DE_BLOQUEIO_DA_FILA);
    }
}

void app_main()
{
    display_init();
    rfid_main();
    // dht11_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    http_main();

    // xTaskCreate(monitora_sensores, "sensores", 2048, NULL, 2, NULL);
    xTaskCreate(monitora_rfid, "rfid", 4096, NULL, 2, NULL);

}

