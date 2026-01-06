#pragma once

#include <sys/param.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <freertos/stream_buffer.h>
#include <freertos/message_buffer.h>
#include <freertos/timers.h>

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include <dht11.h>

#include <inttypes.h>
#include "rc522.h"
#include "driver/spi_master.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h"

#include "ssd1306.h"
#include "driver/i2c.h"

// #define SSID "dmgn"
// #define PASS "66666666"
#define SSID "Label Aulas"
#define PASS ""

#define OLED_GPIO_SDA 21
#define OLED_GPIO_SCL 22

#define DHT11_PIN 4

#define SENSOR_GAS 36
#define SENSOR_CHAMA 39
#define SENSOR_MOVIMENTO 34

#define RC522_SPI_BUS_GPIO_MISO    (19)
#define RC522_SPI_BUS_GPIO_MOSI    (23)
#define RC522_SPI_BUS_GPIO_SCLK    (18)
#define RC522_SPI_SCANNER_GPIO_SDA (5)
#define RC522_SCANNER_GPIO_RST     (-1) // soft-reset, ou 22 para manual



#define MAX_USERS 64
typedef struct {
    char chave[9];
    char nome[64];
    float saldo;
} conta_t;

static int num_contas_cadastradas = 0;
static conta_t contas_cadastradas[MAX_USERS];