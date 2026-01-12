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

            let riskGas = document.getElementById("risk_gas");
            if (dados.gas > 60) {
                riskGas.className = "risk critico";
            } else if (dados.gas > 30) {
                riskGas.className = "risk alerta";
            } else {
                riskGas.className = "risk ok";
            }

            let riskChamas = document.getElementById("risk_chamas");
            if (dados.chamas > 0) {
                riskChamas.className = "risk critico";
            } else {
                riskChamas.className = "risk ok";
            }

            let riskTemp = document.getElementById("risk_temp");
            if (dados.temperatura > 40 || dados.temperatura < 10) {
                riskTemp.className = "risk critico";
            } else if (dados.temperatura > 35 || dados.temperatura < 20) {
                riskTemp.className = "risk alerta";
            } else {
                riskTemp.className = "risk ok";
            }

            let riskUmi = document.getElementById("risk_umi");
            if (dados.umidade < 30 || dados.umidade > 70) {
                riskTemp.className = "risk critico";
            } else if (dados.umidade < 40 || dados.umidade > 60) {
                riskUmi.className = "risk alerta";
            } else {
                riskUmi.className = "risk ok";
            }
        })
        .catch(err => {
            console.error("Falha ao atualizar dados:", err);
        });
}


// Atualiza a cada 1 segundo
setInterval(atualizarDados, 1000);