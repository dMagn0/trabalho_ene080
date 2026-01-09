idf_component_register(SRCS "servidor_http.c" "sensores.c" "my_oled.c" "my_rfid.c" "main.c" "contas.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES 
                        "html/index.html"
                        "html/app.js"
                        "html/app_monit.js"
                        "html/cadastro.html"
                        "html/monitoramento.html"
                    )
