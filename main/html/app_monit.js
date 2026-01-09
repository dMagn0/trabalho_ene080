// Simulação de dados vindos do ESP32
function atualizarDados() {
    fetch("/monitoramento/dados")  
        .then(response => {
            if (!response.ok) {
                throw new Error("Erro na resposta do servidor");
            }
            return response.json();
        })
        .then(dados => {
            
            gas.textContent = dados.gas.toFixed(1);
            chamas.textContent = dados.chamas.toFixed(1);
            temperatura.textContent = dados.temperatura.toFixed(1);
            umidade.textContent = dados.umidade.toFixed(1);

            
            document.getElementById("barra_temp").style.width =
                Math.min(dados.temperatura/4095, 100) + "%";
            document.getElementById("barra_monit").style.width =
                Math.min(dados.umidade/4095, 100) + "%";
        })
        .catch(err => {
            console.error("Falha ao atualizar dados:", err);
        });
}


// Atualiza a cada 1 segundo
setInterval(atualizarDados, 1000);