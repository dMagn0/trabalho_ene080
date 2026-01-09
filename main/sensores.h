#pragma once
#include "utilities.h"

extern QueueHandle_t fila_sensor;
extern QueueHandle_t fila_sensor_to_servidor;

typedef struct{
    int gas;
    int chamas;
    int temperatura;
    int umidade;
}dados_sensores;

void leitura_sensor(void *pvparameters);

void display_sens(void *pvparameters);

void dht11_main(void);