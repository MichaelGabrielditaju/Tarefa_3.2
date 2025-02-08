# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configuração da extensão do VS Code
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

# Importa o Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

project(programa C CXX ASM)

# Inicializa o SDK
pico_sdk_init()

# Adiciona o executável
add_executable(programa programa.c inc/ssd1306.c)

# Gera o cabeçalho do PIO
pico_generate_pio_header(programa ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(programa ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Nome do programa e versão
pico_set_program_name(programa "programa")
pico_set_program_version(programa "0.1")

# Ativa saída UART/USB
pico_enable_stdio_uart(programa 0)
pico_enable_stdio_usb(programa 1)

# Adiciona a biblioteca padrão e hardware_pio
target_link_libraries(programa
    pico_stdlib
    hardware_pio
    hardware_i2c
)

# Adiciona os arquivos de include
target_include_directories(programa PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Gera arquivos de saída adicionais
pico_add_extra_outputs(programa)
