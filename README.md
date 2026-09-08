# Automação de Esteira - TCC

Sistema de automação industrial que simula a dispensação de produtos através de uma esteira, controlada por uma interface web em tempo real.

## 🏭 Arquitetura do Sistema

```
┌─────────────────────┐
│   SITE ONLINE       │
│ (GitHub Pages)      │
│ index.html          │
└──────────┬──────────┘
           │ JSON
           ↓
┌─────────────────────┐
│  SERVIDOR NODE.JS   │
│  server.js          │
│ (seu computador)    │
└──────────┬──────────┘
           │ Serial
           ↓
┌─────────────────────┐
│   ARDUINO MEGA      │
│  arduino_sketch.ino │
│                     │
│ ├─ 4 Servos (cores)│
│ └─ 1 Motor Esteira │
└─────────────────────┘
```

## 📋 Arquivos do Projeto

- **arduino_sketch.ino** - Código Arduino (controla servos e motor)
- **server.js** - Servidor Node.js (intermedia site ↔ Arduino)
- **index.html** - Interface web (acessa online)
- **package.json** - Dependências Node.js
- **README.md** - Este arquivo

## 🔧 Componentes de Hardware

- **Arduino Mega 2560**
- **4 Servos** (empurradores das bolinhas) - Pinos 22, 24, 26, 28
- **1 Servo TT** (motor da esteira) - Pino 30
- **Bolinhas coloridas** (vermelho, azul, verde, amarelo)

## ⚙️ Configuração Inicial

### 1️⃣ Arduino (IDE do Arduino)

1. Abra a **IDE do Arduino**
2. Copie o código de `arduino_sketch.ino`
3. Instale a biblioteca: **Sketch > Incluir Biblioteca > Gerenciar Bibliotecas**
   - Procure por **ArduinoJson** (Benoit Blanchon)
4. Conecte o Arduino via USB
5. **Ferramentas > Porta** - anote a porta (COM3, COM5, /dev/ttyUSB0, etc)
6. Carregue o código no Arduino

### 2️⃣ Node.js (seu computador)

1. Instale **Node.js** (https://nodejs.org)
2. Abra o terminal na pasta do projeto
3. Execute: `npm install`
4. Edite `server.js` e altere:
   ```javascript
   const NOME_DA_PORTA = "COM3";   // ← Sua porta aqui
   ```
5. Execute: `npm start`
   - Você verá: `✓ SERVIDOR RODANDO`

### 3️⃣ Site Online

1. O site já está em: **https://martinsbrazztcc.github.io/SiteTCC/**
2. Com `npm start` rodando, você pode fazer compras
3. O servidor enviará comandos ao Arduino
4. Arduino controla os servos e a esteira

## 🚀 Como Usar

### Via Site Online:

1. Acesse: https://martinsbrazztcc.github.io/SiteTCC/
2. Clique em "Ver Produto"
3. Escolha a quantidade
4. Clique em "Comprar"
5. A esteira processa o pedido!

### Fluxo Completo:

```
1. Usuário clica "Comprar" no site
   ↓
2. Site envia JSON: {"produto":"bola_vermelha","quantidade":2}
   ↓
3. Node.js recebe e valida
   ↓
4. Node.js envia pela serial ao Arduino
   ↓
5. Arduino recebe e executa:
   - Servo vermelho empurra a bolinha
   - Esteira transporta por 3 segundos
   - Repete para quantidade solicitada
   ↓
6. Arduino envia resposta: {"status":"ok","enviado":2}
   ↓
7. Node.js retorna ao site
   ↓
8. Site mostra: "✓ Pedido enviado!"
```

## 🔌 Configuração de Pinos (Arduino Mega)

| Componente | Pino | Função |
|-----------|------|--------|
| Servo Vermelho | 22 | Empurra bolinha vermelha |
| Servo Azul | 24 | Empurra bolinha azul |
| Servo Verde | 26 | Empurra bolinha verde |
| Servo Amarelo | 28 | Empurra bolinha amarela |
| Servo TT (Esteira) | 30 | Move a esteira |

## ⏱️ Timings

| Ação | Tempo |
|------|-------|
| Servo empurrando | 500 ms |
| Acomodação da bolinha | 150 ms |
| Esteira em movimento | 3000 ms (3 segundos) |

## 🧪 Troubleshooting

### ❌ "Arduino não está conectado"

- Verifique se o Arduino está conectado via USB
- Confirme a porta em `server.js` (**Ferramentas > Porta** na IDE)
- Tente outra porta USB
- Reinicie o servidor: `npm start`

### ❌ "Servidor não responde"

- Verifique se `npm start` está rodando
- Confirme que Node.js está instalado: `node -v`
- Verifique se a porta 3000 não está em uso

### ❌ Servo não se mexe

- Verifique os pinos no Arduino (22, 24, 26, 28, 30)
- Confirme se o código foi carregado corretamente
- Teste com um comando simples via serial

### ❌ Imagens não aparecem no site

- Faça upload das imagens no repositório GitHub:
  - `bola_vermelha.png`
  - `bola_azul.png`
  - `bola_verde.png`
  - `bola_amarela.png`

## 📊 Status do Sistema

Acesse `http://localhost:3000/status` para ver:
- Status do servidor
- Status da conexão Arduino
- Porta configurada

## 🎓 Para Apresentação do TCC

1. **Prepare o Arduino** com o código carregado
2. **Inicie o Node.js**: `npm start`
3. **Acesse o site online** no navegador
4. **Demonstre**: Clique em "Comprar" e mostre a esteira funcionando
5. **Explique o fluxo** entre site, servidor e Arduino

## 📝 Notas Importantes

- O servidor Node.js **DEVE estar rodando** para o site funcionar
- A comunicação é em **JSON** dos dois lados
- O Arduino responde com status de sucesso ou erro
- Máximo de 20 bolinhas por pedido

## 📧 Suporte

Se tiver dúvidas sobre o código ou funcionamento, revise:
1. Comentários no código (bem explicados)
2. Logs no terminal (mostram o que está acontecendo)
3. Console do navegador (F12) para erros do site