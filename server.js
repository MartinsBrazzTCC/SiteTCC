/*
  server.js
  ---------
  Backend que fica no meio do caminho entre o site (navegador) e o Arduino.

  Agora a comunicação é 100% em JSON dos dois lados:

    Node -> Arduino (via serial):
      {"produto":"bola_vermelha","quantidade":2}\n
    Arduino -> Node (via serial), quando termina:
      {"status":"ok","produto":"bola_vermelha","enviado":2}\n
      ou
      {"status":"erro","mensagem":"..."}\n
  O Arduino usa a biblioteca ArduinoJson para ler e montar essas mensagens
  (veja arduino_sketch/arduino_sketch.ino).

  COMO USAR:
    1) Instale o Node.js (https://nodejs.org)
    2) No terminal, dentro desta pasta, rode:
         npm install
    3) Descubra o nome da porta do seu Arduino:
         - Windows: algo como "COM3", "COM5" (veja na IDE do Arduino em
           Ferramentas > Porta)
         - Linux/Mac: algo como "/dev/ttyUSB0" ou "/dev/cu.usbmodem14101"
    4) Ajuste a constante NOME_DA_PORTA abaixo
    5) Rode: node server.js
    6) Abra o index.html no navegador
*/

const express = require("express");
const cors = require("cors");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");

// ======== CONFIGURAÇÃO — AJUSTE AQUI ========
const NOME_DA_PORTA = "COM3";   // troque pela porta do seu Arduino
const VELOCIDADE = 9600;        // precisa bater com o Serial.begin() do Arduino
const TEMPO_LIMITE_MS = 15000;  // tempo máximo esperando o Arduino responder (esteira + servos podem demorar)
// =============================================

const app = express();
app.use(cors());          // permite que o index.html (aberto em outra origem) chame este servidor
app.use(express.json());  // entende o JSON enviado pelo fetch()

const PRODUTOS_VALIDOS = ["bola_vermelha", "bola_azul", "bola_verde", "bola_amarela"];

let porta;
let parser;
let portaConectada = false;

// Fila simples: só processamos um pedido por vez, porque o Arduino é
// sequencial (servo + esteira). Isso evita comandos se misturando na serial.
let pedidoEmAndamento = null;

function conectarArduino() {
    porta = new SerialPort({ path: NOME_DA_PORTA, baudRate: VELOCIDADE }, (erro) => {
        if (erro) {
            portaConectada = false;
            console.error("✗ Não foi possível abrir a porta", NOME_DA_PORTA, "-", erro.message);
            console.error("  Verifique se o Arduino está conectado e se o nome da porta está certo.");
            return;
        }
        portaConectada = true;
        console.log("✓ Conectado ao Arduino na porta", NOME_DA_PORTA);
    });

    parser = porta.pipe(new ReadlineParser({ delimiter: "\n" }));

    parser.on("data", (linha) => {
        linha = linha.trim();
        if (!linha) return;

        console.log("Arduino disse:", linha);

        // Ignora mensagens de log/depuração que não são JSON
        // (ex: "Arduino pronto." no setup())
        let dados;
        try {
            dados = JSON.parse(linha);
        } catch {
            return; // não era JSON, provavelmente um log — ignora
        }

        if (pedidoEmAndamento) {
            clearTimeout(pedidoEmAndamento.timeout);

            if (dados.status === "ok") {
                pedidoEmAndamento.resolver({
                    mensagem: `Esteira concluída: ${dados.enviado}x ${dados.produto}`
                });
            } else {
                pedidoEmAndamento.rejeitar(new Error(dados.mensagem || "Arduino reportou erro"));
            }

            pedidoEmAndamento = null;
        }
    });

    porta.on("close", () => {
        portaConectada = false;
        console.log("Conexão com o Arduino foi fechada.");
    });
}

conectarArduino();

app.post("/comprar", (req, res) => {
    const { produto, quantidade } = req.body;

    if (!produto || !PRODUTOS_VALIDOS.includes(produto)) {
        return res.status(400).json({ mensagem: "Produto inválido: " + produto });
    }

    const qtd = Number(quantidade);
    if (!qtd || qtd < 1 || qtd > 20) {
        return res.status(400).json({ mensagem: "Quantidade inválida (use 1 a 20)" });
    }

    if (!portaConectada) {
        return res.status(503).json({
            mensagem: "Arduino não está conectado. Verifique o cabo USB e a porta configurada em server.js."
        });
    }

    if (pedidoEmAndamento) {
        return res.status(429).json({
            mensagem: "A esteira ainda está processando o pedido anterior. Aguarde."
        });
    }

    // Comando em JSON enviado ao Arduino
    const comando = JSON.stringify({ produto, quantidade: qtd }) + "\n";

    const promessa = new Promise((resolver, rejeitar) => {
        const timeout = setTimeout(() => {
            pedidoEmAndamento = null;
            rejeitar(new Error("O Arduino não respondeu a tempo."));
        }, TEMPO_LIMITE_MS);

        pedidoEmAndamento = { resolver, rejeitar, timeout };
    });

    porta.write(comando, (erro) => {
        if (erro) {
            clearTimeout(pedidoEmAndamento?.timeout);
            pedidoEmAndamento = null;
            return res.status(500).json({ mensagem: "Falha ao enviar comando ao Arduino." });
        }
        console.log("Enviado ao Arduino:", comando.trim());
    });

    promessa
        .then((resultado) => res.json(resultado))
        .catch((erro) => res.status(500).json({ mensagem: erro.message }));
});

const PORTA_HTTP = 3000;
app.listen(PORTA_HTTP, () => {
    console.log("Servidor rodando em http://localhost:" + PORTA_HTTP);
});
