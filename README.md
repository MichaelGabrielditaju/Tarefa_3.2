# Projeto de Sistema Embarcado com Raspberry Pi Pico

## **Descrição Geral**
Este projeto implementa um sistema embarcado interativo utilizando a Raspberry Pi Pico. Ele integra diversos componentes, como LEDs RGB, matriz de LEDs WS2812, display SSD1306 e botões físicos, para criar um sistema de entrada, processamento e exibição de informações. O código foi desenvolvido com foco em modularidade, organização e atendimento aos requisitos propostos.

---

## **Funcionalidades**

### 1. **Controle do Display SSD1306**
- Utiliza o protocolo I2C para comunicação.
- Exibe caracteres recebidos via Serial Monitor.
- Suporte para caracteres alfabéticos (maiúsculos e minúsculos) e numéricos.
- Mensagens informativas e de boas-vindas exibidas no display durante a inicialização e interação com os botões.

---

### 2. **Entrada de Caracteres via Serial Monitor**
- Caracteres são recebidos pelo Serial Monitor do VS Code via UART.
- Funcionalidades associadas:
  - **Números (0 a 9):**
    - Exibidos na matriz de LEDs WS2812 em formato 5x5.
    - Mensagens relacionadas são exibidas no display SSD1306.
  - **Letras (a-z, A-Z):**
    - Exibidas diretamente no display SSD1306.
  - **Caracteres Inválidos:**
    - A matriz de LEDs é apagada.
    - Mensagem de erro é exibida no display SSD1306.

---

### 3. **Interação com Botões**
#### Botão A:
- Alterna o estado do LED RGB verde (ligado/desligado).
- Registro da interação:
  - Mensagem no **display SSD1306** indicando o estado do LED.
  - Mensagem no **Serial Monitor** detalhando a operação.
  
#### Botão B:
- Alterna o estado do LED RGB azul (ligado/desligado).
- Registro da interação:
  - Mensagem no **display SSD1306** indicando o estado do LED.
  - Mensagem no **Serial Monitor** detalhando a operação.

---

### 4. **Controle de Matriz WS2812**
- Matriz de LEDs 5x5 controlada via protocolo PIO.
- Representação visual dos números (0 a 9) enviados via UART.
- Uso de valores RGB para criar as figuras correspondentes.

---

### 5. **Debouncing e Interrupções**
- **Debouncing:** Implementado via software para evitar leituras duplicadas dos botões.
- **Interrupções:** Utilizadas para lidar com os eventos dos botões, garantindo baixa latência e alta responsividade.

---

### 6. **Envio de Mensagens pela UART**
- Todas as interações com botões e entrada de caracteres são registradas no Serial Monitor para debug e validação.

---

## **Requisitos Atendidos**
Este projeto cumpre os seguintes requisitos propostos:
1. **Uso de interrupções:** Implementado para funcionalidades dos botões.
2. **Debouncing via software:** Tratamento implementado para evitar múltiplos registros de uma única interação.
3. **Controle de LEDs:** Controle integrado de LEDs comuns e LEDs WS2812.
4. **Uso do display SSD1306:** Suporte para caracteres maiúsculos e minúsculos.
5. **Comunicação serial via UART:** Entrada de dados e envio de mensagens para o Serial Monitor.
6. **Organização do código:** Código bem estruturado, comentado e modular.

---

## **Como Usar**
1. Conecte a Raspberry Pi Pico ao computador via USB.
2. Utilize o Serial Monitor do VS Code para interagir com o sistema.
   - Envie números para exibir símbolos na matriz de LEDs WS2812.
   - Envie letras para exibi-las no display SSD1306.
3. Interaja com os botões físicos:
   - **Botão A:** Alterna o estado do LED RGB verde.
   - **Botão B:** Alterna o estado do LED RGB azul.
4. Observe as mensagens exibidas no Serial Monitor e no display SSD1306.

---

## **Dependências**
- Biblioteca `ssd1306.h` para controle do display OLED.
- Biblioteca `font.h` para definição de caracteres.
- Bibliotecas para controle de LEDs WS2812 e GPIO da Raspberry Pi Pico.

---

## **Contribuidores**
- **Michael Gabriel Cavalcante Batista**
- **Projeto desenvolvido no EmBaRcAtEcH**
---

## **Link para o vídeo no YouTube**
- **https://youtube.com/shorts/7X57HLwehyk?si=OSbE5C53R_gs8927**
  
