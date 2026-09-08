# Loja das Bolinhas - TCC

Sistema automatizado de dispensação de produtos (bolinhas coloridas) usando Arduino, Node.js e uma interface web.

## 📋 Estrutura do Projeto

- **arduino_sketch.ino** - Código Arduino que controla os servos e motor da esteira
- **server.js** - Backend Node.js que conecta o site ao Arduino via porta serial
- **index.html** - Interface web (e-commerce)
- **package.json** - Dependências Node.js

## 🚀 Como Usar

### 1. Preparar o Arduino

1. Abra a **IDE do Arduino** (https://www.arduino.cc/en/software)
2. Copie o código de `arduino_sketch.ino`
3. Vá em **Sketch > Incluir Biblioteca > Gerenciar Bibliotecas**
4. Instale a biblioteca **ArduinoJson** (por Benoit Blanchon)
5. Conecte seu Arduino ao computador
6. Em **Ferramentas > Porta**, anote o nome da porta (ex: COM3, COM5)
7. Faça upload do código para o Arduino

### 2. Preparar o Node.js

1. Instale o **Node.js** (https://nodejs.org)
2. Abra o terminal/prompt na pasta do projeto
3. Execute: `npm install`
4. Abra o arquivo `server.js` e altere a linha:
   ```javascript
   const NOME_DA_PORTA = "COM3";   // Coloque a porta do seu Arduino aqui
   ```
5. Execute: `npm start`
   - Você verá: `Servidor rodando em http://localhost:3000`

### 3. Abrir o Site no Navegador

1. Abra o arquivo `index.html` diretamente no navegador (duplo clique ou drag & drop)
2. Ou, se quiser que o navegador abra automaticamente, coloque este código em um arquivo `.bat` (Windows):
   ```batch
   @echo off
   start http://localhost:3000
   start index.html
   ```
3. O site da "Loja das Bolinhas" abrirá normalmente

## 🎯 Fluxo de Funcionamento

1. **Usuário** clica em "Ver Produto" e escolhe uma quantidade
2. **Navegador** envia a solicitação para `server.js` via HTTP POST
3. **Node.js** converte para JSON e envia ao Arduino pela porta serial
4. **Arduino** recebe, controla os servos e motor da esteira
5. **Arduino** responde com o status da operação
6. **Node.js** retorna o resultado ao navegador
7. **Navegador** exibe mensagem de sucesso/erro

## ⚙️ Configurações Importantes

- **Porta Serial**: Ajuste em `server.js` (linha com `NOME_DA_PORTA`)
- **Velocidade Serial**: 9600 bps (igual no Arduino e Node.js)
- **Tempo da Esteira**: 3000 ms (3 segundos) por bolinha
- **Pinos do Arduino**:
  - Servo Vermelho: Pino 9
  - Servo Azul: Pino 10
  - Servo Verde: Pino 11
  - Servo Amarelo: Pino 12
  - Motor Esteira: Pino 6

## 📸 Imagens Necessárias

Crie/coloque estas imagens na mesma pasta que o `index.html`:
- `bola_vermelha.png`
- `bola_azul.png`
- `bola_verde.png`
- `bola_amarela.png`

## 🔧 Troubleshooting

**Mensagem: "Arduino não está conectado"**
- Verifique se o Arduino está conectado ao USB
- Confirme o nome da porta em `server.js`
- Faça upload do código Arduino novamente

**Mensagem: "Servidor não responde"**
- Verifique se rodou `npm start` com sucesso
- Confirme que o Node.js está instalado: `node -v`

**JSON inválido no Arduino**
- Verifique se a biblioteca ArduinoJson está instalada
- Confirme que a taxa de baud é 9600
