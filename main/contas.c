#include "contas.h"

#include <esp_log.h>
#include <stdbool.h>

#define TAG "CONTAS"

static int num_contas_cadastradas = 0;
static conta_t contas_cadastradas[MAX_USERS];

int get_num_contas(){
    return num_contas_cadastradas;
}

Operacao get_conta(char* chave, conta_t** conta){
    for(int i = 0; i<num_contas_cadastradas;i++){
        if(strcmp(contas_cadastradas[i].chave, chave) == 0){
            
            *conta = &contas_cadastradas[i];
            return OP_SUCESSO; 
        }
    }
    return OP_INVALIDA;
}

void normaliza_chave(const char* chave_entrada, char* chave_saida) {
    int j = 0;

    for (int i = 0; chave_entrada[i] != '\0'; i++) {
        if (chave_entrada[i] != ' ') {
            chave_saida[j++] = chave_entrada[i];
        }
    }
    chave_saida[j] = '\0';
}

Operacao get_conta_por_chave(char* chave, conta_t* conta){
    char chave_normalizada[32];

    normaliza_chave(chave, chave_normalizada);
    
    printf("n = %d\nch_n = %s\nch = %s\ncadas = %s\n", num_contas_cadastradas, chave_normalizada,chave,contas_cadastradas[0].chave);
    printf("0");

    for (int i = 0; i < num_contas_cadastradas; i++) {
        printf("1");
        if (strcmp(contas_cadastradas[i].chave, chave_normalizada) == 0) {
            printf("2");
            *conta = contas_cadastradas[i];
            return OP_SUCESSO;
        }
    }
    printf("3");

    return OP_INVALIDA;
}


conta_t get_conta_por_indice(int indice){
    if(indice>=num_contas_cadastradas)return (conta_t){0};

    return contas_cadastradas[indice];
}


Operacao cadastra_conta(conta_t nova_conta){
    
    if (num_contas_cadastradas == MAX_USERS){
        ESP_LOGW(TAG, "LIMITE DE USUARIOS CADASTRADOS JA FOI ATINGIDO");
        return (OP_CANCELADA);
    }
    
    for (int i = 0; i < num_contas_cadastradas; i++) {
        if(strcmp(contas_cadastradas[i].chave,nova_conta.chave) == 0){
            ESP_LOGW(TAG, "NUMERO DO CARTAO JA FOI CADASTRADA");
            return (OP_INVALIDA);
        }
    }
    strcpy(contas_cadastradas[num_contas_cadastradas].chave, nova_conta.chave);
    strcpy(contas_cadastradas[num_contas_cadastradas].nome, nova_conta.nome);
    contas_cadastradas[num_contas_cadastradas].saldo = nova_conta.saldo;

    // contas_cadastradas[num_contas_cadastradas] = nova_conta;

    ESP_LOGI(TAG,"Nova conta cadastrada (%d): %s, %s, %.2f",(num_contas_cadastradas+1), contas_cadastradas[num_contas_cadastradas].chave,contas_cadastradas[num_contas_cadastradas].nome, contas_cadastradas[num_contas_cadastradas].saldo);
    num_contas_cadastradas ++; 
    return OP_SUCESSO;
}

Operacao remove_conta(char* chave){
    if(num_contas_cadastradas == 0){
        ESP_LOGW(TAG,"NAO EXISTE CONTA CADASTRADA");
        return OP_INVALIDA;
    }

    bool achou = false;
    for (int i = 0; i < num_contas_cadastradas; i++) {
        if (!achou && strcmp(contas_cadastradas[i].chave, chave) == 0)
        {
            achou = true;
        }

        if (achou && i < num_contas_cadastradas - 1)
        {
            contas_cadastradas[i] = contas_cadastradas[i + 1];
        }
    }

    if(!achou){
        ESP_LOGW(TAG,"CONTA NAO ENCONTRADA");
        return OP_INVALIDA;
    }
    ESP_LOGI(TAG,"Conta deletada: %s\n",chave);
    num_contas_cadastradas --;
    return OP_SUCESSO;
}

Operacao deposito(char* chave, float valor){
    conta_t* conta = {0};

    if(valor < 0){
        ESP_LOGI(TAG,"Valor inválido");
        return OP_CANCELADA;
    }

    if (get_conta(chave, &conta) != OP_SUCESSO){
        ESP_LOGW(TAG,"CONTA NAO ENCONTRADA");
        return OP_INVALIDA;
    }
    
    conta->saldo += valor;
    ESP_LOGI(TAG,"Valor depositado");
    return OP_SUCESSO;
    
}

Operacao saque(char* chave, float valor){
    conta_t* conta = {0};

    if(valor < 0 ){
        ESP_LOGI(TAG,"Valor inválido");
        return OP_CANCELADA;
    }
    
    if (get_conta(chave, &conta) != OP_SUCESSO){
        ESP_LOGW(TAG,"CONTA NAO ENCONTRADA");
        return OP_INVALIDA;
    }
    
    if(valor > conta->saldo){
        ESP_LOGI(TAG,"Valor inválido");
        return OP_CANCELADA;
    }
    conta->saldo -= valor;
    DINHEIRO += valor;
    ESP_LOGI(TAG,"Valor retirado");
    return OP_SUCESSO;
    
}

