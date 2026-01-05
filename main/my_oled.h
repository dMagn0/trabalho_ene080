#pragma once

#include "utilities.h"

extern MessageBufferHandle_t buffer_rfid;

void display_init(void);

void escreve_oled(char linha, char *data_m, size_t msize);