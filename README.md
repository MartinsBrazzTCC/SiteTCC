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

---

# ⚙️ GUIA DE INSTALAÇÃO PASSO A PASSO

## 1️⃣ INSTALAR NODE.JS

### Passo 1: Baixar Node.js

1. Abra o navegador e acesse: **https://nodejs.org/en/download/prebuilt-installer**
2. Clique em **"Windows Installer (.msi)"** (escolha a versão **LTS** - a maior em verde)
3. Aguarde o download terminar

### Passo 2: Instalar Node.js

1. **Abra o arquivo baixado** (duplo clique)
2. Clique em **"Next"** para avançar
3. Aceite os termos e clique **"Next"** novamente
4. ⚠️ **IMPORTANTE:** Na próxima tela, **MARQUE** as opções:
   - ☑️ `Add to PATH`
   - ☑️ `Automatically install necessary tools`
5. Clique em **"Install"**
6. Aguarde a instalação terminar (pode demorar 2-3 minutos)
7. Clique em **"Finish"**

### Passo 3: Reiniciar o computador

⚠️ **OBRIGATÓRIO:** Reinicie o computador depois de instalar o Node.js

### Passo 4: Verificar instalação

1. Abra o **Prompt de Comando** (pressione Windows + R, digite `cmd`, aperte Enter)
2. Digite o comando:
   ```
   node -v
   ```
3. Deve aparecer um número de versão (ex: `v20.0.0`) ✅
4. Digite o comando:
   ```
   npm -v
   ```
5. Deve aparecer um número de versão (ex: `9.6.7`) ✅

Se aparecerem os números de versão, o Node.js foi instalado com sucesso!

---

## 2️⃣ PREPARAR O PROJETO

### Passo 1: Baixar os arquivos do repositório

1. Acesse: **https://github.com/MartinsBrazzTCC/SiteTCC**
2. Clique no botão verde **"<> Code"**
3. Clique em **"Download ZIP"**
4. Aguarde o download terminar

### Passo 2: Extrair os arquivos

1. Abra a pasta de Downloads (ou onde você baixou)
2. Procure pelo arquivo **"SiteTCC-main.zip"**
3. **Clique com botão direito** nele
4. Clique em **"Extrair tudo..."**
5. Escolha uma pasta fácil (ex: Desktop ou Documentos)
6. Clique em **"Extrair"**

### Passo 3: Abrir a pasta

1. Abra a pasta extraída
2. Você deve ver os arquivos:
   - `arduino_sketch.ino`
   - `server.js`
   - `package.json`
   - `index.html`
   - `README.md`
   - As imagens das bolinhas

---

## 3️⃣ INSTALAR AS DEPENDÊNCIAS

### Passo 1: Abrir o terminal na pasta correta

**Opção A (Mais fácil):**
1. Abra a pasta do projeto
2. **Clique na barra de endereço** (onde mostra o caminho da pasta)
3. Apague tudo e digite: `cmd`
4. Aperte **Enter**
5. Um terminal preto vai abrir **dentro da pasta** ✅

**Opção B (Pelo terminal):**
1. Abra o **Prompt de Comando**
2. Digite:
   ```
   cd C:\Users\SeuUsuario\Desktop\SiteTCC-main
   ```
   *(ajuste o caminho conforme onde você extraiu a pasta)*
3. Aperte **Enter**

### Passo 2: Instalar as dependências

1. No terminal, digite:
   ```
   npm install
   ```
2. Aperte **Enter**
3. Aguarde terminar (pode demorar 1-2 minutos)
4. Quando terminar, deve aparecer algo como:
   ```
   added XXX packages
   ```

**Pronto!** As dependências foram instaladas ✅

---

## 4️⃣ CONFIGURAR A PORTA DO ARDUINO

### Passo 1: Descubrir a porta do Arduino

1. Conecte o **Arduino Mega 2560** via USB no computador
2. Abra a **IDE do Arduino** (https://www.arduino.cc/en/software)
3. Vá em **Ferramentas > Porta**
4. Anote o nome da porta que aparece (ex: **COM3**, **COM5**, **/dev/ttyUSB0**)

### Passo 2: Atualizar o arquivo server.js

1. Abra a pasta do projeto
2. **Clique com botão direito** no arquivo `server.js`
3. Clique em **"Abrir com > Bloco de Notas"** (ou VS Code)
4. Procure pela linha:
   ```javascript
   const NOME_DA_PORTA = "COM3";
   ```
5. **Troque "COM3" pela porta do seu Arduino** (ex: `"COM5"`, `"COM7"`)
6. **Salve o arquivo** (Ctrl + S)
7. **Feche o editor**

---

## 5️⃣ CARREGAR O CÓDIGO NO ARDUINO

### Passo 1: Instalar a biblioteca ArduinoJson

1. Abra a **IDE do Arduino**
2. Vá em **Sketch > Incluir Biblioteca > Gerenciar Bibliotecas**
3. Na caixa de busca, digite: `ArduinoJson`
4. Procure por **"ArduinoJson"** de **Benoit Blanchon**
5. Clique em **"Instalar"**
6. Aguarde terminar

### Passo 2: Abrir o código

1. Na **IDE do Arduino**, vá em **File > Open**
2. Procure pela pasta do projeto
3. Selecione o arquivo **`arduino_sketch.ino`**
4. Clique em **"Abrir"**

### Passo 3: Configurar a placa

1. Vá em **Ferramentas > Placa**
2. Procure e selecione **"Arduino Mega 2560"**

### Passo 4: Fazer upload

1. Verifique se o Arduino está conectado via USB
2. Clique no botão ➤ **"Upload"** (ou pressione Ctrl + U)
3. Aguarde aparecer a mensagem: **"Sketch uploaded successfully"** ✅

**Pronto!** O código foi carregado no Arduino ✅

---

## 6️⃣ INICIAR O SERVIDOR

### Passo 1: Abrir o terminal

1. Abra a pasta do projeto
2. **Clique na barra de endereço** e digite: `cmd`
3. Aperte **Enter**

### Passo 2: Rodar o servidor

1. No terminal, digite:
   ```
   npm start
   ```
2. Aperte **Enter**
3. Deve aparecer:
   ```
   ╔════════════════════════════════════════╗
   ║  SERVIDOR RODANDO                      ║
   ║  http://localhost:3000              ║
   ╚════════════════════════════════════════╝
   
   ✓ Conectado ao Arduino na porta COM3
   ```

**✅ O servidor está rodando!** Deixe esse terminal aberto.

---

## 7️⃣ TESTAR O SISTEMA

### Passo 1: Verificar o status

1. Abra um navegador (Chrome, Firefox, etc)
2. Digite na barra de endereço:
   ```
   http://localhost:3000/status
   ```
3. Aperte **Enter**
4. Deve aparecer:
   ```json
   {
     "servidor": "✓ Rodando",
     "arduino": "✓ Conectado",
     "porta": "COM3"
   }
   ```

Se aparecer isso, **tudo está certo!** ✅

### Passo 2: Acessar o site online

1. Abra um novo abadaba (ou aba) no navegador
2. Digite:
   ```
   https://martinsbrazztcc.github.io/SiteTCC/
   ```
3. Aperte **Enter**
4. O site deve abrir normalmente

### Passo 3: Fazer um teste

1. Clique em **"Ver Produto"** (em qualquer bolinha)
2. Escolha uma quantidade (ex: 2)
3. Clique em **"Comprar"**
4. O Arduino deve:
   - Mover o servo correspondente (empurra a bolinha)
   - Ligar a esteira por 3 segundos
   - Repetir conforme a quantidade
5. Deve aparecer no site: **"✓ Pedido enviado!"**

**🎉 SUCESSO! Sistema funcionando!**

---

## 🧪 TROUBLESHOOTING

### ❌ "npm não é reconhecido como um comando"
- **Solução:** Reinstale o Node.js e **marque a opção "Add to PATH"** na instalação
- Reinicie o computador depois

### ❌ "Arduino não está conectado" (no /status)
- **Solução:** Verifique se a porta em `server.js` está correta
- Tente reconectar o Arduino via USB
- Verifique em **Ferramentas > Porta** da IDE do Arduino

### ❌ "Servidor não responde"
- **Solução:** Certifique-se que `npm start` está rodando no terminal
- O terminal não pode ser fechado enquanto o servidor está ativo

### ❌ Servo não se mexe
- **Solução:** Verifique os pinos do Arduino (22, 24, 26, 28, 30)
- Confirme que o código foi carregado corretamente
- Tente reconectar o Arduino

### ❌ Site mostra erro "Falha ao conectar"
- **Solução:** Verifique se `npm start` está rodando
- Verifique se a porta 3000 não está em uso por outro programa

### ❌ Imagens não aparecem no site
- **Solução:** As imagens já estão no repositório, devem funcionar automaticamente
- Se não funcionarem, verifique sua conexão com a internet

---

## 📝 RESUMO RÁPIDO

```
1. Instalar Node.js (https://nodejs.org)
2. Baixar projeto (https://github.com/MartinsBrazzTCC/SiteTCC)
3. Extrair arquivos
4. Abrir terminal na pasta
5. Executar: npm install
6. Editar server.js (ajustar porta Arduino)
7. Carregar código no Arduino
8. Executar: npm start
9. Acessar: https://martinsbrazztcc.github.io/SiteTCC/
10. Fazer teste clicando em "Comprar"
```

---

## 🔌 Configuração de Pinos (Arduino Mega)

| Componente | Pino | Função |
|-----------|------|--------|
| Servo Vermelho | 22 | Empurra bolinha vermelha |
| Servo Azul | 24 | Empurra bolinha azul |
| Servo Verde | 26 | Empurra bolinha verde |
| Servo Amarelo | 28 | Empurra bolinha amarela |
| Servo TT (Esteira) | 30 | Move a esteira |

---

## ⏱️ Timings

| Ação | Tempo |
|------|-------|
| Servo empurrando | 500 ms |
| Acomodação da bolinha | 150 ms |
| Esteira em movimento | 3000 ms (3 segundos) |

---

## 🎓 Para Apresentação do TCC

1. **Prepare o Arduino** com o código carregado
2. **Inicie o Node.js**: `npm start` (deixe rodando)
3. **Acesse o site online** no navegador
4. **Demonstre**: Clique em "Comprar" e mostre a esteira funcionando
5. **Explique o fluxo** entre site, servidor e Arduino

---

## 📧 Dúvidas?

Se tiver dúvidas:
1. Revise este README passo a passo
2. Verifique os logs no terminal (eles mostram o que está acontecendo)
3. Verifique o console do navegador (pressione F12)
4. Revise os comentários no código (estão bem explicados)
