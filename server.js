/*
  server.js
  ---------
  Servidor Node.js que conecta o site (online) ao Arduino (local)
  
  Fluxo:
    Site online → POST /comprar → Node.js → Arduino via Serial
    Arduino responde → Node.js → Retorna ao site
  
  COMO USAR:
    1) npm install
    2) Ajuste NOME_DA_PORTA para sua porta Arduino
    3) node server.js
    4) Acesse o site online
*/

const express = require("express");
const cors = require("cors");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");

// ======== CONFIGURAÇÃO - AJUSTE AQUI ========
const NOME_DA_PORTA = "COM3";   // Mude para sua porta (COM3, COM5, /dev/ttyUSB0, etc)
const VELOCIDADE = 9600;         // Deve bater com Serial.begin() do Arduino
const TEMPO_LIMITE_MS = 20000;   // Timeout para resposta do Arduino
// =============================================

const app = express();
app.use(cors());
app.use(express.json());

const PRODUTOS_VALIDOS = ["bola_vermelha", "bola_azul", "bola_verde", "bola_amarela"];

let porta;
let parser;
let portaConectada = false;
let pedidoEmAndamento = null;

// ======== CONEXÃO COM O ARDUINO ========
function conectarArduino() {
  porta = new SerialPort(
    { path: NOME_DA_PORTA, baudRate: VELOCIDADE },
    (erro) => {
      if (erro) {
        portaConectada = false;
        console.error("✗ ERRO: Não foi possível abrir a porta", NOME_DA_PORTA);
        console.error("  Verifique se:");
        console.error("  1. O Arduino está conectado via USB");
        console.error("  2. A porta está correta (veja na IDE do Arduino)");
        console.error("  3. Nenhum outro programa está usando a porta");
        return;
      }
      portaConectada = true;
      console.log("✓ Conectado ao Arduino na porta", NOME_DA_PORTA);
    }
  );

  parser = porta.pipe(new ReadlineParser({ delimiter: "\n" }));

  // Recebe dados do Arduino
  parser.on("data", (linha) => {
    linha = linha.trim();
    if (!linha) return;

    console.log("📨 Arduino respondeu:", linha);

    // Tenta fazer parse do JSON
    let dados;
    try {
      dados = JSON.parse(linha);
    } catch {
      // Se não for JSON, provavelmente é um log - ignora
      return;
    }

    // Se tem um pedido aguardando resposta
    if (pedidoEmAndamento) {
      clearTimeout(pedidoEmAndamento.timeout);

      if (dados.status === "ok") {
        pedidoEmAndamento.resolver({
          mensagem: `✓ Esteira concluída: ${dados.enviado}x ${dados.produto}`
        });
      } else {
        pedidoEmAndamento.rejeitar(
          new Error(dados.mensagem || "Arduino reportou erro")
        );
      }

      pedidoEmAndamento = null;
    }
  });

  porta.on("close", () => {
    portaConectada = false;
    console.log("⚠ Conexão com o Arduino foi fechada.");
  });

  porta.on("error", (erro) => {
    console.error("✗ Erro na porta serial:", erro.message);
  });
}

// Conecta ao Arduino na inicialização
conectarArduino();

// ======== ROTA: RECEBER PEDIDOS DO SITE ========
app.post("/comprar", (req, res) => {
  const { produto, quantidade } = req.body;

  // Valida produto
  if (!produto || !PRODUTOS_VALIDOS.includes(produto)) {
    return res.status(400).json({
      mensagem: "❌ Produto inválido: " + produto
    });
  }

  // Valida quantidade
  const qtd = Number(quantidade);
  if (!qtd || qtd < 1 || qtd > 20) {
    return res.status(400).json({
      mensagem: "❌ Quantidade deve ser entre 1 e 20"
    });
  }

  // Verifica se Arduino está conectado
  if (!portaConectada) {
    return res.status(503).json({
      mensagem:
        "❌ Arduino não está conectado. Verifique o USB e a porta em server.js"
    });
  }

  // Verifica se já há um pedido em andamento
  if (pedidoEmAndamento) {
    return res.status(429).json({
      mensagem: "⏳ A esteira ainda está processando. Aguarde..."
    });
  }

  // Monta o comando JSON
  const comando = JSON.stringify({ produto, quantidade: qtd }) + "\n";

  // Cria uma promise para aguardar a resposta do Arduino
  const promessa = new Promise((resolver, rejeitar) => {
    const timeout = setTimeout(() => {
      pedidoEmAndamento = null;
      rejeitar(new Error("O Arduino não respondeu a tempo."));
    }, TEMPO_LIMITE_MS);

    pedidoEmAndamento = { resolver, rejeitar, timeout };
  });

  // Envia o comando via serial
  porta.write(comando, (erro) => {
    if (erro) {
      clearTimeout(pedidoEmAndamento?.timeout);
      pedidoEmAndamento = null;
      return res.status(500).json({
        mensagem: "❌ Falha ao enviar comando ao Arduino"
      });
    }
    console.log("📤 Enviado ao Arduino:", comando.trim());
  });

  // Aguarda a resposta do Arduino
  promessa
    .then((resultado) => res.json(resultado))
    .catch((erro) => {
      res.status(500).json({
        mensagem: "❌ " + erro.message
      });
    });
});

// ======== HEALTH CHECK ========
app.get("/status", (req, res) => {
  res.json({
    servidor: "✓ Rodando",
    arduino: portaConectada ? "✓ Conectado" : "✗ Desconectado",
    porta: NOME_DA_PORTA
  });
});

// ======== INICIAR SERVIDOR ========
const PORTA_HTTP = 3000;
app.listen(PORTA_HTTP, () => {
  console.log("\n");
  console.log("╔════════════════════════════════════════╗");
  console.log("║  SERVIDOR RODANDO                      ║");
  console.log("║  http://localhost:" + PORTA_HTTP + "              ║");
  console.log("╚════════════════════════════════════════╝");
  console.log("\n");
  console.log("Aguardando conexão com Arduino...");
  console.log("Verifique se a porta está correta em server.js\n");
});