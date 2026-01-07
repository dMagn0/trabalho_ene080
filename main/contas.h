#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 64

static float DINHEIRO = 0;

typedef struct {
    char chave[9];
    char nome[64];
    float saldo;
} conta_t;

typedef enum {
    OP_SUCESSO = 0,
    OP_CANCELADA = 1,
    OP_INVALIDA = 2
} Operacao;

int get_num_contas();
conta_t get_conta_por_indice(int indice);
Operacao get_conta_por_chave(char* chave, conta_t* conta);

Operacao cadastra_conta(conta_t nova_conta);
Operacao remove_conta(char* chave);
Operacao deposito(char* chave, float valor);
Operacao saque(char* chave, float valor);