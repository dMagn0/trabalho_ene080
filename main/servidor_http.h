#pragma once

#include "utilities.h"
#include "sensores.h"

extern MessageBufferHandle_t buffer_ip;

void stop_webserver(httpd_handle_t server);
void http_main(void);


