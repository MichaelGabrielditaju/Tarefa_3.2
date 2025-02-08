#pragma once

#include "hardware/pio.h"

extern const pio_program_t ws2812_program;

void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, uint freq, bool rgbw);
