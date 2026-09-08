/*
  arduino_sketch.ino
  -------------------
  Automação de esteira de entregas - TCC
  
  Recebe comandos JSON do servidor Node.js pela porta serial:
    {"produto":"bola_vermelha","quantidade":2}
  
  Controla:
    - 4 servos empurradores (um por cor)
    - 1 servo TT motor para a esteira
  
  Responde com status JSON:
    {"status":"ok","produto":"bola_vermelha","enviado":2}
    {"status":"erro","mensagem":"..."}
  
  BIBLIOTECAS NECESSÁRIAS:
    - ArduinoJson (por Benoit Blanchon)
    - Servo (padrão do Arduino)
*/

#include <ArduinoJson.h>
#include <Servo.h>

// ======== PINOS DOS SERVOS EMPURRADORES (por cor) ========
#define PINO_SERVO_VERMELHO 22
#define PINO_SERVO_AZUL     24
#define PINO_SERVO_VERDE    26
#define PINO_SERVO_AMARELO  28

// ======== PINO DO SERVO TT (motor da esteira) ========
#define PINO_SERVO_ESTEIRA  30

// ======== ÂNGULOS E TIMINGS ========
#define ANGULO_REPOUSO      0   // servo parado (não empurra)
#define ANGULO_EMPURRAR     80    // servo empurra bolinha
#define TEMPO_EMPURRAR_MS   1000  // tempo na posição de empurrão
#define TEMPO_ESTEIRA_MS    3000 // esteira roda 3s por bolinha

// ======== VELOCIDADES DO SERVO TT ========
#define SERVO_PARADO        0   // servo parado
#define SERVO_FRENTE        45   // servo girando (esteira andando)

// Declaração dos servos
Servo servoVermelho, servoAzul, servoVerde, servoAmarelo, servoEsteira;

// Buffer para acumular a linha JSON recebida
String bufferEntrada = "";

void setup() {
  Serial.begin(9600);
  
  // Conecta os servos empurradores
  servoVermelho.attach(PINO_SERVO_VERMELHO);
  servoAzul.attach(PINO_SERVO_AZUL);
  servoVerde.attach(PINO_SERVO_VERDE);
  servoAmarelo.attach(PINO_SERVO_AMARELO);
  
  // Conecta o servo TT da esteira
  servoEsteira.attach(PINO_SERVO_ESTEIRA);
  
  // Posiciona todos os servos em repouso
  posicionarTodosServos(ANGULO_REPOUSO);
  pararEsteira();
  
  Serial.println("{\"evento\":\"sistema_iniciado\"}");
}

void loop() {
  // Lê a serial byte a byte até formar uma linha completa
  while (Serial.available() > 0) {
    char c = Serial.read();
    
    if (c == '\n') {
      processarComando(bufferEntrada);
      bufferEntrada = "";
    } else {
      bufferEntrada += c;
    }
  }
}

void processarComando(String linha) {
  linha.trim();
  if (linha.length() == 0) return;
  
  // Parse do JSON
  StaticJsonDocument<128> doc;
  DeserializationError erro = deserializeJson(doc, linha);
  
  if (erro) {
    enviarErro("JSON invalido: " + String(erro.c_str()));
    return;
  }
  
  const char* produto = doc["produto"];
  int quantidade = doc["quantidade"] | 0;
  
  if (!produto || quantidade <= 0) {
    enviarErro("Campos 'produto' ou 'quantidade' invalidos");
    return;
  }
  
  Servo* servo = servoDoProduto(produto);
  if (servo == nullptr) {
    enviarErro("Produto desconhecido: " + String(produto));
    return;
  }
  
  // Processa cada bolinha
  for (int i = 0; i < quantidade; i++) {
    despacharBolinha(servo);
  }
  
  enviarSucesso(produto, quantidade);
}

// Retorna o servo correto baseado no produto
Servo* servoDoProduto(const char* produto) {
  String p = String(produto);
  if (p == "bola_vermelha") return &servoVermelho;
  if (p == "bola_azul")     return &servoAzul;
  if (p == "bola_verde")    return &servoVerde;
  if (p == "bola_amarela")  return &servoAmarelo;
  return nullptr;
}

// Dispara uma bolinha (servo + esteira)
void despacharBolinha(Servo* servo) {
  // 1) Empurra a bolinha do reservatório para a esteira
  servo->write(ANGULO_EMPURRAR);
  delay(TEMPO_EMPURRAR_MS);
  servo->write(ANGULO_REPOUSO);
  delay(150);
  
  // 2) Liga a esteira para transportar a bolinha
  ligarEsteira();
  delay(TEMPO_ESTEIRA_MS);
  pararEsteira();
}

// Liga o servo TT (esteira se move)
void ligarEsteira() {
  servoEsteira.write(SERVO_FRENTE);
}

// Para o servo TT (esteira parada)
void pararEsteira() {
  servoEsteira.write(SERVO_PARADO);
}

// Posiciona todos os servos empurradores
void posicionarTodosServos(int angulo) {
  servoVermelho.write(angulo);
  servoAzul.write(angulo);
  servoVerde.write(angulo);
  servoAmarelo.write(angulo);
}

// Envia resposta de sucesso
void enviarSucesso(const char* produto, int quantidade) {
  StaticJsonDocument<128> resposta;
  resposta["status"] = "ok";
  resposta["produto"] = produto;
  resposta["enviado"] = quantidade;
  serializeJson(resposta, Serial);
  Serial.println();
}

// Envia resposta de erro
void enviarErro(String mensagem) {
  StaticJsonDocument<128> resposta;
  resposta["status"] = "erro";
  resposta["mensagem"] = mensagem;
  serializeJson(resposta, Serial);
  Serial.println();
}
