idf_component_register(SRCS "servidor_http.c" "sensores.c" "my_oled.c" "my_rfid.c" "main.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES 
                        "html/index.html"
                        "html/app.js"
                    )
