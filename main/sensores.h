#pragma once
#include "utilities.h"

extern QueueHandle_t fila_sensor;

struct dados_sensores {
    int gas;
    int chamas;
    int temperatura;
    int umidade;
};

void leitura_sensor(void *pvparameters);

void display_sens(void *pvparameters);

void dht11_main(void);