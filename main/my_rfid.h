#pragma once

#include "utilities.h"

extern MessageBufferHandle_t buffer_rfid;

void rfid_start();
void rfid_pausa_leitura();
void rfid_main();