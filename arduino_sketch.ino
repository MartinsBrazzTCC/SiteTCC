/*
  arduino_sketch.ino
  -------------------
  Simulação de automação de esteira de entregas.

  Recebe do server.js um JSON pela porta serial, ex:
    {"produto":"bola_vermelha","quantidade":2}

  Para cada bolinha do pedido:
    1) Move o servo da cor correspondente para empurrar a bolinha
       do reservatório para a esteira
    2) Volta o servo para a posição de repouso
    3) Liga o motor da esteira por TEMPO_ESTEIRA_MS (transporta a bolinha
       até o ponto de entrega)
    4) Desliga o motor da esteira

  Ao final do pedido, responde ao server.js com um JSON de status:
    {"status":"ok","produto":"bola_vermelha","enviado":2}
    {"status":"erro","mensagem":"..."}

  BIBLIOTECAS NECESSÁRIAS (instalar pela IDE do Arduino em
  Sketch > Incluir Biblioteca > Gerenciar Bibliotecas):
    - ArduinoJson (por Benoit Blanchon)
    - Servo (já vem por padrão na IDE)

  LIGAÇÕES (ajuste conforme sua montagem real):
    - 4 servos, um por cor, empurrando a bolinha do reservatório
      para a esteira
    - 1 motor DC da esteira, controlado por um único transistor/MOSFET
      usado como chave liga/desliga (a esteira só gira num sentido,
      então não precisamos de ponte H). Ligações:
        Pino do Arduino -> resistor -> base/gate do transistor
        Emissor/fonte do transistor -> GND (comum com o Arduino)
        Coletor/dreno do transistor -> terminal (-) do motor
        Terminal (+) do motor -> alimentação externa
        Diodo (ex: 1N4007) em paralelo com o motor, protegendo
        contra pico de tensão ao desligar
*/

#include <ArduinoJson.h>
#include <Servo.h>

// ======== PINOS DOS SERVOS (separação por cor) ========
#define PINO_SERVO_VERMELHO 9
#define PINO_SERVO_AZUL     10
#define PINO_SERVO_VERDE    11
#define PINO_SERVO_AMARELO  12

// Ângulos do servo: repouso (não bloqueia a bolinha) e empurrando (libera a bolinha)
#define ANGULO_REPOUSO   0
#define ANGULO_EMPURRAR  90
#define TEMPO_EMPURRAR_MS 400   // tempo parado na posição "empurrar" antes de voltar

// ======== PINO DO MOTOR DA ESTEIRA (transistor/MOSFET como chave liga/desliga) ========
#define PINO_MOTOR_ESTEIRA 6   // vai na base/gate do transistor (via resistor)

#define VELOCIDADE_ESTEIRA 200      // 0-255 (usa PWM; 255 = velocidade máxima)
#define TEMPO_ESTEIRA_MS   3000     // esteira roda 3s por bolinha, conforme definido no TCC

Servo servoVermelho, servoAzul, servoVerde, servoAmarelo;

// Buffer para acumular a linha JSON recebida pela serial
String bufferEntrada = "";

void setup() {
  Serial.begin(9600);

  servoVermelho.attach(PINO_SERVO_VERMELHO);
  servoAzul.attach(PINO_SERVO_AZUL);
  servoVerde.attach(PINO_SERVO_VERDE);
  servoAmarelo.attach(PINO_SERVO_AMARELO);

  posicionarTodosServos(ANGULO_REPOUSO);

  pinMode(PINO_MOTOR_ESTEIRA, OUTPUT);
  pararEsteira();

  Serial.println("Arduino pronto.");
}

void loop() {
  // Lê a serial byte a byte até formar uma linha completa (até '\n')
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

  // ArduinoJson: aloca um documento do tamanho certo para o JSON esperado
  StaticJsonDocument<128> doc;
  DeserializationError erro = deserializeJson(doc, linha);

  if (erro) {
    enviarErro("JSON invalido: " + String(erro.c_str()));
    return;
  }

  const char* produto = doc["produto"];
  int quantidade = doc["quantidade"] | 0;

  if (!produto || quantidade <= 0) {
    enviarErro("Campos 'produto'/'quantidade' ausentes ou invalidos");
    return;
  }

  Servo* servo = servoDoProduto(produto);
  if (servo == nullptr) {
    enviarErro(String("Produto desconhecido: ") + produto);
    return;
  }

  for (int i = 0; i < quantidade; i++) {
    despacharBolinha(servo);
  }

  enviarSucesso(produto, quantidade);
}

Servo* servoDoProduto(const char* produto) {
  String p = String(produto);
  if (p == "bola_vermelha") return &servoVermelho;
  if (p == "bola_azul")     return &servoAzul;
  if (p == "bola_verde")    return &servoVerde;
  if (p == "bola_amarela")  return &servoAmarelo;
  return nullptr;
}

void despacharBolinha(Servo* servo) {
  // 1) Empurra a bolinha do reservatório para a esteira
  servo->write(ANGULO_EMPURRAR);
  delay(TEMPO_EMPURRAR_MS);
  servo->write(ANGULO_REPOUSO);
  delay(150); // pequena pausa para a bolinha acomodar na esteira

  // 2) Liga a esteira para transportar a bolinha até o destino
  ligarEsteira();
  delay(TEMPO_ESTEIRA_MS);
  pararEsteira();
}

void ligarEsteira() {
  // PWM no pino do transistor: controla a velocidade do motor
  // (255 = sempre ligado / velocidade máxima)
  analogWrite(PINO_MOTOR_ESTEIRA, VELOCIDADE_ESTEIRA);
}

void pararEsteira() {
  analogWrite(PINO_MOTOR_ESTEIRA, 0);
}

void posicionarTodosServos(int angulo) {
  servoVermelho.write(angulo);
  servoAzul.write(angulo);
  servoVerde.write(angulo);
  servoAmarelo.write(angulo);
}

void enviarSucesso(const char* produto, int quantidade) {
  StaticJsonDocument<128> resposta;
  resposta["status"] = "ok";
  resposta["produto"] = produto;
  resposta["enviado"] = quantidade;
  serializeJson(resposta, Serial);
  Serial.println();
}

void enviarErro(String mensagem) {
  StaticJsonDocument<128> resposta;
  resposta["status"] = "erro";
  resposta["mensagem"] = mensagem;
  serializeJson(resposta, Serial);
  Serial.println();
}
