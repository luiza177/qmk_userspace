#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool is_mac;
    bool swap_btns;
    bool caps_word;
    bool locked; // current layer is layer-locked
    bool rgb_fx_on;
} ui_state_t;

extern ui_state_t ui_state;
