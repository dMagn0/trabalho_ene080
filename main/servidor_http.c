#include "servidor_http.h"

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

extern const uint8_t cadastro_html_start[] asm("_binary_cadastro_html_start");
extern const uint8_t cadastro_html_end[]   asm("_binary_cadastro_html_end");

extern const uint8_t monitoramento_html_start[] asm("_binary_monitoramento_html_start");
extern const uint8_t monitoramento_html_end[]   asm("_binary_monitoramento_html_end");

extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[]   asm("_binary_app_js_end");

extern const uint8_t app_monit_js_start[] asm("_binary_app_monit_js_start");
extern const uint8_t app_monit_js_end[]   asm("_binary_app_monit_js_end");

static const char *TAG = "ESP32 Server";

static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_MAXIMUM_RETRY      5

#define NAO_CONECTOU 0
#define CONECTOU 1


static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < WIFI_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }   
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else
    {
        ESP_LOGI(TAG, "connect to the AP fail");
    }

}

char wifi_connection()
{
    char retorno = NAO_CONECTOU;
    // 1 - Wi-Fi/LwIP Init Phase

    // esp_netif_init();                    // TCP/IP initiation 					s1.1
    // esp_event_loop_create_default();     // event loop 			                s1.2
    // esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_initiation)); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration));

    // 3 - Wi-Fi Start Phase
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", SSID, PASS);
        retorno = CONECTOU;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", SSID, PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler));
    vEventGroupDelete(s_wifi_event_group);

    return retorno;
}

esp_err_t get_app(httpd_req_t *req){
    const char *uri = req->uri;

    if (strcmp(uri, "/app/app.js") == 0) {

        httpd_resp_set_type(req, "application/javascript");
        httpd_resp_send(req, app_js_start, app_js_end - app_js_start);
        return ESP_OK;

    } else if (strcmp(uri, "/app/app_monit.js") == 0) {

        httpd_resp_set_type(req, "application/javascript");
        httpd_resp_send(req, app_monit_js_start, app_monit_js_end - app_monit_js_start);
        return ESP_OK;
    }

    httpd_resp_send_404(req);
    return ESP_FAIL;
    
}
esp_err_t get_pages(httpd_req_t *req){
    const char *uri = req->uri;

    if (strcmp(uri, "/") == 0) {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, index_html_start, index_html_end - index_html_start);
        return ESP_OK;

    } else if (strcmp(uri, "/cadastro") == 0) {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, cadastro_html_start, cadastro_html_end - cadastro_html_start);
        return ESP_OK;

    } else if (strcmp(uri, "/monitoramento") == 0) {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, monitoramento_html_start, monitoramento_html_end - monitoramento_html_start);
        return ESP_OK;
    }

    httpd_resp_send_404(req);
    return ESP_FAIL;
}

esp_err_t get_dados_sensores(httpd_req_t *req){
    httpd_resp_set_type(req, "application/json");

    dados_sensores dados;

    xQueuePeek(fila_sensor_to_servidor, &dados, 0);

    char buf[128];
    snprintf(buf, sizeof(buf),
        "{"
        "\"gas\":%d,"
        "\"chamas\":%d,"
        "\"temperatura\":%d,"
        "\"umidade\":%d"
        "}",
        dados.gas,
        dados.chamas,
        dados.temperatura,
        dados.umidade
    );

    // printf("%s", buf);
    httpd_resp_sendstr(req, buf);

    return ESP_OK;
}
esp_err_t get_dados_cadastro(httpd_req_t *req){

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr_chunk(req, "[");

    bool first = true;
    int num_contas = get_num_contas();

    for (int i = 0; i < num_contas; i++) {
        if (!first) httpd_resp_sendstr_chunk(req, ",");
        first = false;
        conta_t conta = get_conta_por_indice(i); 

        char buf[128];
        snprintf(buf, sizeof(buf),
          "{\"chave\":\"%s\",\"nome\":\"%s\",\"saldo\":%.2f}",
          conta.chave, conta.nome, conta.saldo);
        httpd_resp_sendstr_chunk(req, buf);
    }

    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}
esp_err_t get_data(httpd_req_t *req){
    const char *uri = req->uri;

    if (strcmp(uri, "/get_data/monitoramento") == 0) {
        return get_dados_sensores(req);
    } else if (strcmp(uri, "/get_data/cadastro") == 0) {
        return get_dados_cadastro(req);
    }

    httpd_resp_send_404(req);
    return ESP_FAIL;

}

esp_err_t users_post(httpd_req_t *req){

    char buf[128];
    httpd_req_recv(req, buf, sizeof(buf));
    
    conta_t nova_conta = {0}; 
    char saldo[16];
    httpd_query_key_value(buf, "chave", nova_conta.chave, sizeof(nova_conta.chave));
    httpd_query_key_value(buf, "nome", nova_conta.nome, sizeof(nova_conta.nome));
    httpd_query_key_value(buf, "saldo", saldo, sizeof(saldo));

    nova_conta.saldo = atof(saldo);
    cadastra_conta(nova_conta);
    
    return httpd_resp_sendstr(req, "OK");
}

esp_err_t users_delete(httpd_req_t *req){
    char buf[64], chave[9];
    httpd_req_get_url_query_str(req, buf, sizeof(buf));
    httpd_query_key_value(buf, "chave", chave, sizeof(chave));

    remove_conta(chave);

    return httpd_resp_sendstr(req, "OK");

}

/* URI handler structure for GET /uri */
    httpd_uri_t uri_pages = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = get_pages,
        .user_ctx = NULL};
        
    httpd_uri_t uri_app = {
        .uri = "/app/*",
        .method = HTTP_GET,
        .handler = get_app,
        .user_ctx = NULL};

    httpd_uri_t uri_get_data = {
        .uri      = "/get_data/*",
        .method   = HTTP_GET,
        .handler  = get_data,
        .user_ctx = NULL};

    httpd_uri_t uri_users_post = {
        .uri      = "/users",
        .method   = HTTP_POST,
        .handler  = users_post,
        .user_ctx = NULL};
    httpd_uri_t uri_users_delete = {
        .uri      = "/users",
        .method   = HTTP_DELETE,   // ex: /users?del=ABCDEF01
        .handler  = users_delete,
        .user_ctx = NULL};



httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_pages);
        httpd_register_uri_handler(server, &uri_app);

        httpd_register_uri_handler(server, &uri_get_data);
        
        httpd_register_uri_handler(server, &uri_users_post);
        httpd_register_uri_handler(server, &uri_users_delete);
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
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    if(wifi_connection() == NAO_CONECTOU) return;
    start_webserver();
}