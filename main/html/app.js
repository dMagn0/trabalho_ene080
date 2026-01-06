
async function carregarUsuarios() {
    const resp = await fetch("/users");
    const usuarios = await resp.json();

    const tbody = document.getElementById("tabelaUsuarios");
    tbody.innerHTML = "";

    usuarios.forEach(u => {
        tbody.innerHTML += `
            <tr>
                <td class="hex">${u.chave}</td>
                <td>${u.nome}</td>
                <td>R$ ${u.saldo}</td>
                <td>
                    <button class="btn-remove" onclick="removerUsuario('${u.chave}')">
                        Remover
                    </button>
                </td>
            </tr>`;
    });
}

async function removerUsuario(chave) {
    await fetch("/users?chave=" + chave, { method: "DELETE" });
    carregarUsuarios();
}

document.getElementById("formAdd").onsubmit = async e => {
    
    e.preventDefault();
    
    const formData = new FormData(e.target);
    const chave = formData.get("chave").toUpperCase();
    
    if (!/^[0-9A-F]{8}$/.test(formData.get("chave"))) {
        alert("Chave inválida. Use exatamente 8 caracteres hexadecimais.");
        return;
    }
    
    await fetch("/users", {
        method: "POST",
        body: new URLSearchParams(formData)
    });

    e.target.reset();
    carregarUsuarios();
};

carregarUsuarios();
