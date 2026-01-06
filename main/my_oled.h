#pragma once

#include "utilities.h"

extern MessageBufferHandle_t buffer_rfid;

void display_init(void);

void escreve_oled(char *linha_0, size_t msize_0, char *linha_3, size_t msize_3);