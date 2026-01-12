// Simulação de dados vindos do ESP32
function atualizarDados() {
    fetch("get_data/monitoramento")  
        .then(response => {
            if (!response.ok) {
                throw new Error("Erro na resposta do servidor");
            }
            return response.json();
        })
        .then(dados => {
            
            gas.textContent = dados.gas.toFixed(1);
            chamas.textContent = dados.chamas.toFixed(1);
            temperatura.textContent = dados.temperatura;
            umidade.textContent = dados.umidade;

            let temp_prc = ((dados.temperatura)+10)*(100/60)
            temp_prc = Math.max(0, Math.min(temp_prc, 100));
            document.getElementById("barra_temp").style.width = temp_prc + "%";

            let umid = Math.max(0, Math.min(dados.umidade, 100));
            document.getElementById("barra_umi").style.width = umid + "%";

            /* GAS */
            let lblGas = document.getElementById("lbl_gas_alert");
            if (dados.gas > 60) {
                lblGas.className = "lbl critico";
            } else if (dados.gas > 30) {
                lblGas.className = "lbl alerta";
            } else {
                lblGas.className = "lbl ok";
            }

            /* CHAMAS */
            let lblChamas = document.getElementById("lbl_chamas_alert");
            if (dados.chamas > 0) {
                lblChamas.className = "lbl critico";
            } else {
                lblChamas.className = "lbl ok";
            }

            /* TEMPERATURA */
            let lblTemp = document.getElementById("lbl_temp_alert");
            if (dados.temperatura > 40 || dados.temperatura < 10) {
                lblTemp.className = "lbl critico";
            } else if (dados.temperatura > 35 || dados.temperatura < 20) {
                lblTemp.className = "lbl alerta";
            } else {
                lblTemp.className = "lbl ok";
            }

            /* UMIDADE */
            let lblUmi = document.getElementById("lbl_umi_alert");
            if (dados.umidade < 30 || dados.umidade > 70) {
                lblUmi.className = "lbl critico";
            } else if (dados.umidade < 40 || dados.umidade > 60) {
                lblUmi.className = "lbl alerta";
            } else {
                lblUmi.className = "lbl ok";
            }

        })
        .catch(err => {
            console.error("Falha ao atualizar dados:", err);
        });
}


// Atualiza a cada 1 segundo
setInterval(atualizarDados, 1000);