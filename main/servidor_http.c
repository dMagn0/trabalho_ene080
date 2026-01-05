#include "servidor_http.h"

extern const uint8_t index_html_start[] asm("_binary_html_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_html_index_html_end");

static const char *TAG = "ESP32 Server";

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

esp_err_t get_handler(httpd_req_t *req){
    const size_t html_size = index_html_end - index_html_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, html_size);

    /*
        const char resp[] = "<!DOCTYPE HTML><html><head>\
                            <title>ESP Input Form</title>\
                            <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
                            </head><body>\
                            <form action=\"/get\">\
                                str: <input type=\"text\" name=\"str\">\
                                <input type=\"submit\" value=\"Submit\">\
                            </form><br>\
                            <form action=\"/get\">\
                                int: <input type=\"text\" name=\"int\">\
                                <input type=\"submit\" value=\"Submit\">\
                            </form><br>\
                            </body></html>";
    
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    */

    return ESP_OK;
}

esp_err_t users_get_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr_chunk(req, "[");

    bool first = true;
    for (int i = 0; i < num_contas_cadastradas; i++) {
        if (!first) httpd_resp_sendstr_chunk(req, ",");
        first = false;

        char buf[128];
        snprintf(buf, sizeof(buf),
          "{\"chave\":\"%s\",\"nome\":\"%s\",\"saldo\":%.2f}",
          contas_cadastradas[i].chave, contas_cadastradas[i].nome, contas_cadastradas[i].saldo);
        httpd_resp_sendstr_chunk(req, buf);
    }

    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

esp_err_t users_post_handler(httpd_req_t *req){

    char buf[128];
    httpd_req_recv(req, buf, sizeof(buf));

    if (num_contas_cadastradas == MAX_USERS){
        return httpd_resp_sendstr(req, "LIMITE DE USUARIOS CADASTRADOS JA FOI ATINGIDO");
    }
    

    char chave[9], nome[64], saldo[16];
    httpd_query_key_value(buf, "chave", chave, sizeof(chave));
    httpd_query_key_value(buf, "nome", nome, sizeof(nome));
    httpd_query_key_value(buf, "saldo", saldo, sizeof(saldo));

    for (int i = 0; i < num_contas_cadastradas; i++) {
        if(contas_cadastradas[i].chave == chave){
            return httpd_resp_sendstr(req, "CONTA JA FOI CADASTRADA");
        }
    }

    strcpy(contas_cadastradas[num_contas_cadastradas].chave, chave);
    strcpy(contas_cadastradas[num_contas_cadastradas].nome, nome);
    contas_cadastradas[num_contas_cadastradas].saldo = atof(saldo);
    num_contas_cadastradas ++;
    
    /*muda*/
    return httpd_resp_sendstr(req, "OK");
}

esp_err_t users_delete_handler(httpd_req_t *req){
    char buf[64], chave[9];
    httpd_req_get_url_query_str(req, buf, sizeof(buf));
    httpd_query_key_value(buf, "chave", chave, sizeof(chave));

    if(num_contas_cadastradas == 0){
        return httpd_resp_sendstr(req, "NAO EXISTE CONTA CADASTRADA");
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
        return httpd_resp_sendstr(req, "CONTA NAO ENCONTRADA");
    }
    
    num_contas_cadastradas --;
    /*muda*/
    return httpd_resp_sendstr(req, "OK");

}

/* URI handler structure for GET /uri */
    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_users_get = {
        .uri      = "/users",
        .method   = HTTP_GET,
        .handler  = users_get_handler,
        .user_ctx = NULL
    };
    httpd_uri_t uri_users_post = {
        .uri      = "/users",
        .method   = HTTP_POST,
        .handler  = users_post_handler,
        .user_ctx = NULL
    };

    httpd_uri_t uri_users_delete = {
        .uri      = "/users",
        .method   = HTTP_DELETE,   // ex: /users?del=ABCDEF01
        .handler  = users_delete_handler,
        .user_ctx = NULL
    };


httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &users_get_handler);
        httpd_register_uri_handler(server, &users_post_handler);
        httpd_register_uri_handler(server, &users_delete_handler);
    }
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server)
    {
        httpd_stop(server);
    }
}

void http_main(void)
{
    nvs_flash_init();
    wifi_connection();
    start_webserver();
}