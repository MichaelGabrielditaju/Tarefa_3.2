#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "ws2812.pio.h"
#include "matrizes.h"
#include "neopixel_pio.h"
#include "ws2818b.pio.h"
#include "inc/ssd1306.h"  // Biblioteca para o LCD
#include "inc/font.h"     // Biblioteca para a fonte

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO_LCD 0x3C

#define LED_COUNT 25
#define LED_PIN 7
#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12
#define BOTAO_A 5
#define BOTAO_B 6
#define DEBOUNCE_TIME 200  // Debounce para os botões em ms

#define BUFFER_SIZE 32 // Tamanho máximo da string
char input_buffer[BUFFER_SIZE]; // Buffer para armazenar a string
int buffer_index = 0; // Índice atual no buffer

PIO np_pio;
uint sm;
npLED_t leds[LED_COUNT];
volatile int numero_atual = 0; // Número exibido atualmente (0 a 9)
ssd1306_t ssd; // Estrutura do display

// Estados dos LEDs
static bool led_verde_state = false;
static bool led_azul_state = false;

// Inicializa os LEDs da matriz
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, false);
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Inicializa o LCD
void initDisplay() {
    i2c_init(I2C_PORT, 400 * 1000); // I2C em 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura pinos I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA); // Ativa pull-up
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO_LCD, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_send_data(&ssd); // Envia os dados
}

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

int getIndex(int x, int y) {
    if (y % 2 == 0) {
        return 24 - (y * 5 + x);
    } else {
        return 24 - (y * 5 + (4 - x));
    }
}

// Função para desenhar a matriz com base nos valores RGB
void desenha_fig(int matriz[5][5][3]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int index = getIndex(i, j);
            npSetLED(index, matriz[j][i][0], matriz[j][i][1], matriz[j][i][2]);
        }
    }
    npWrite();
}

// Função para tratar entrada via UART
void tratar_uart(char entrada) {
    if (entrada >= '0' && entrada <= '9') {
        numero_atual = entrada - '0'; // Converte o caractere para número
        desenha_fig(matrizes[numero_atual]); // Exibe o número na matriz
        ssd1306_fill(&ssd, false); // Limpa o display
        ssd1306_draw_string(&ssd, "O caractere", 8, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "informado foi", 8, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, "o numero", 8, 50);
        ssd1306_send_data(&ssd); // Atualiza o display
    } else if ((entrada >= 'a' && entrada <= 'z') || (entrada >= 'A' && entrada <= 'Z')) {
        // Exibe a letra no LCD
        ssd1306_fill(&ssd, false); // Limpa o display
        
        ssd1306_draw_string(&ssd, (char[]){entrada, '\0'}, 0, 0); // Desenha a letra
        ssd1306_send_data(&ssd); // Atualiza o display
    } else {
        npClear(); // Limpa a matriz para outros caracteres
        ssd1306_fill(&ssd, false); // Limpa o display
        ssd1306_draw_string(&ssd, "O caractere", 8, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "informado eh", 8, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, "invalido", 8, 50);
        ssd1306_send_data(&ssd); // Atualiza o display
        desenha_fig(matriz_apagada); // Exibe a matriz apagada
        npWrite();
    }
}

void inicializa_leds() {
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);
}

// Função para lidar com o botão A
void botao_a_callback(uint gpio, uint32_t events) {
    // Debounce simples
    static absolute_time_t last_time = {0};
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_time, now) > 300000) { // 300 ms debounce
        printf("Botão A pressionado\n");  // Mensagem de debug
        led_verde_state = !led_verde_state; // Alterna o estado do LED verde
        gpio_put(LED_VERDE, led_verde_state); // Atualiza o LED

        // Mensagem no LCD e no Serial Monitor
        ssd1306_fill(&ssd, false);
         if (led_azul_state) {
            ssd1306_draw_string(&ssd, "LED Azul: ON", 0, 10);
            printf("LED Azul: Ligado\n");
        } else {
            ssd1306_draw_string(&ssd, "LED Azul: OFF", 0, 10);
            printf("LED Azul: Desligado\n");
        }
        if (led_verde_state) {
            
            ssd1306_draw_string(&ssd, "LED Verde: ON", 0, 0);
            printf("LED Verde: Ligado\n");
        } else {
            ssd1306_draw_string(&ssd, "LED Verde: OFF", 0, 0);
            printf("LED Verde: Desligado\n");
        }
        ssd1306_send_data(&ssd); // Atualiza o display
    }
    last_time = now;
}

// Função para lidar com o botão B
void botao_b_callback(uint gpio, uint32_t events) {
    // Debounce simples
    static absolute_time_t last_time = {0};
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_time, now) > 300000) { // 300 ms debounce
        printf("Botão B pressionado\n");  // Mensagem de debug
        led_azul_state = !led_azul_state; // Alterna o estado do LED azul
        gpio_put(LED_AZUL, led_azul_state); // Atualiza o LED

        // Mensagem no LCD e no Serial Monitor
        ssd1306_fill(&ssd, false);
        if (led_azul_state) {
            ssd1306_draw_string(&ssd, "LED Azul: ON", 0, 10);
            printf("LED Azul: Ligado\n");
        } else {
            ssd1306_draw_string(&ssd, "LED Azul: OFF", 0, 10);
            printf("LED Azul: Desligado\n");
        }
        if (led_verde_state) {
            
            ssd1306_draw_string(&ssd, "LED Verde: ON", 0, 0);
            printf("LED Verde: Ligado\n");
        } else {
            ssd1306_draw_string(&ssd, "LED Verde: OFF", 0, 0);
            printf("LED Verde: Desligado\n");
        }
        ssd1306_send_data(&ssd); // Atualiza o display
    }
    last_time = now;
}

void botoes_callback(uint gpio, uint32_t events) {
    if (gpio == BOTAO_A) {
        botao_a_callback(gpio, events);
    } else if (gpio == BOTAO_B) {
        botao_b_callback(gpio, events);
    }
}

void inicializa_botoes() {
    // Configura os botões
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &botoes_callback);
    
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);
}

void menuEntrada()
{
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_draw_string(&ssd, "  EmBaRcAtEcH", 8, 10); // Desenha uma string
    sleep_ms(2000);
    ssd1306_send_data(&ssd); // Atualiza o display
    ssd1306_draw_string(&ssd, "    MICHAEL", 8, 30); // Desenha uma string
    sleep_ms(2000);
    ssd1306_send_data(&ssd); // Atualiza o display
    ssd1306_draw_string(&ssd, "    gabriel", 8, 50);
    sleep_ms(2000);
    ssd1306_send_data(&ssd); // Atualiza o display
}

int main() {
    stdio_init_all();
    npInit(LED_PIN);
    initDisplay(); // Inicializa o LCD    
    inicializa_leds(); // Inicializa os LEDs
    inicializa_botoes(); // Inicializa os botões

    // Exibe o número inicial na matriz
    desenha_fig(matriz_apagada);
    menuEntrada();
    sleep_ms(1000);

    while (true) {
        // Verifica se há entrada na UART
        if (stdio_usb_connected() && getchar_timeout_us(0) != PICO_ERROR_TIMEOUT) {
            char entrada = getchar();
            tratar_uart(entrada); // Trata a entrada recebida
        }
        sleep_ms(10); // Pequeno atraso para evitar leituras excessivas
    }
}