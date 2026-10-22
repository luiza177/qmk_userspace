RGB_MATRIX_EFFECT(layer_colors)

#define MOUSE_BTN_1 37
#define MOUSE_BTN_2 42

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#include "ui_state.h"

// ---- palette names (grid cells use these) ----
enum {
    C_OFF, C_DIM, C_VDIM,                       // off, transparent keys, very dim
    C_KEY, C_HOME, C_MOD, C_PUN, C_PUNH,       // base layer
    C_TR, C_TRD, C_TG, C_TGD, C_TY, C_TYD, C_TB, C_TBD, C_TC, // thumbs: adjust/sym/nav/num active keys, base cyan and dimmed versions
    C_NUM, C_OPR, C_NUMT,                       // num layer
    C_F1, C_F2, C_F3, C_F4, C_F5, C_F6,
    C_F7, C_F8, C_F9, C_F10, C_F11, C_F12,     // func gradient
    C_BRK, C_MTH, C_COD, C_CODH, C_TXT, C_TXTH, C_SYMT,        // sym layer
    C_ARW, C_NAVK, C_NAVM, C_MED, C_NAVT,      // nav layer
    C_ADJ, C_ACC, C_BL, C_BOOT, C_ADJT,        // adjust layer
    C_GBASE, C_GNUM, C_GFUNC, C_GSYM, C_GNAV, C_GADJ,   // underglow
    C_MS, // mouse buttons
    C_ST_ON, C_ST_CAPS, C_ST_MAC, C_ST_WIN, C_ST_SWAP, C_ST_SWAPPED, // stateful buttons
    C_COUNT
};

// ---- palette values {hue, sat, val}: tune by eye ----
static const uint8_t PROGMEM palette[][3] = {
    [C_OFF]   = {  0,   0,   0},
    [C_DIM]   = {161,  70, 120},   // transparent keys
    [C_VDIM]  = {  0,   0,   8},   // back/forward

    [C_KEY]   = {131,  85, 200},   // base letters, warm neutral
    [C_HOME]  = {131,  47, 255},   // home-row mods
    [C_MOD]   = {161, 150, 150},   // outer mods, dim
    [C_PUN]   = { 93, 120, 200},   // , . / '
    [C_PUNH]  = { 93,  80, 255},   // ;   (bright + tint)

    [C_TR]    = {252, 255, 255},   // adjust thumb
    [C_TRD]   = {252, 255, 200},   // adjust thumb dimmed
    [C_TG]    = { 93, 255, 255},   // sym thumb
    [C_TGD]   = { 93, 255, 200},   // sym thumb dimmed
    [C_TY]    = { 29, 255, 255},   // nav thumb
    [C_TYD]   = { 29, 255, 200},   // nav thumb dimmed
    [C_TB]    = {157, 255, 255},   // num thumb
    [C_TBD]   = {157, 255, 200},   // num thumb dimmed
    [C_TC]    = {131, 250, 230},   // other thumbs on base

    [C_NUM]   = {164, 255, 255},   // digits
    [C_OPR]   = {133, 239, 230},   // operators, enter, print screen
    [C_NUMT]  = {159, 255, 180},   // idle thumbs, lock

    [C_F1]    = {190, 255, 255},   // purple -> pink ramp: hue only
    [C_F2]    = {194, 255, 255},
    [C_F3]    = {198, 255, 255},
    [C_F4]    = {202, 255, 255},
    [C_F5]    = {206, 255, 240},
    [C_F6]    = {210, 255, 240},
    [C_F7]    = {214, 255, 240},
    [C_F8]    = {218, 255, 240},
    [C_F9]    = {222, 255, 200},
    [C_F10]   = {226, 255, 230},
    [C_F11]   = {230, 255, 230},
    [C_F12]   = {234, 255, 230},

    [C_BRK]   = { 96, 220, 230},   // ( ) [ ] { }   teal
    [C_MTH]   = { 70, 255, 215},   // + - * / = %   lime
    [C_COD]   = { 88, 255, 220},   // | & \ ` #  emerald
    [C_CODH]  = { 88, 250, 245},   // | & \ ` #  emerald - homerow
    [C_TXT]   = { 80, 255, 216},   // ! @ $ ^ _ " ? mint
    [C_TXTH]  = { 80, 250, 255},   // ! @ $ ^ _ " ? mint - homerow
    [C_SYMT]  = { 80, 255, 200},   // idle thumbs, lock, enter

    [C_ARW]   = { 30, 100, 255},   // arrows
    [C_NAVK]  = { 28, 150, 176},   // home/end/pgup/pgdn/ins
    [C_NAVM]  = { 28, 255, 180},   // gui/alt/ctrl/shift
    [C_MED]   = { 13, 255, 255},   // media
    [C_NAVT]  = { 28, 255, 150},   // idle thumbs, lock, enter

    [C_ADJ]   = {245, 255, 208},   // RGB controls
    [C_ACC]   = {  9, 255, 255},   // mouse accel
    [C_BL]    = {250, 255, 150},   // backlight
    [C_BOOT]  = { 74, 255, 255},   // QK_BOOT
    [C_ADJT]  = {250, 255, 180},   // idle thumbs

    [C_MS]    = { 12, 200, 255},   // mouse buttons

    [C_ST_ON]   = {  0,   0, 255},   // layer locked
    [C_ST_CAPS] = { 35, 255, 255},   // caps word on
    [C_ST_MAC]  = {  0,   0, 255},   // OS toggle key: macOS
    [C_ST_WIN]  = {160, 255, 255},   // OS toggle key: Windows/Linux
    [C_ST_SWAP] = { 12, 200, 200},   // button swap normal
    [C_ST_SWAPPED] = {  0, 255, 255},   // button swap active

    [C_GBASE] = {132,  64,  180},   // underglow
    [C_GNUM]  = {163, 255,  180},
    [C_GFUNC] = {223, 255,  180},
    [C_GSYM]  = { 85, 255,  180},
    [C_GNAV]  = { 35, 255,  180},
    [C_GADJ]  = {248, 255,  180},
};
_Static_assert(ARRAY_SIZE(palette) == C_COUNT, "palette and enum are out of sync");

// ---- GRID: arguments follow your LAYOUT() order; names are LED numbers ----
// gl/gr = left/right underglow
#define GRID(gl, gr, \
    k30,k29,k28,k27,k26,k25,                  k56,k57,k58,k59,k60,k61, \
    k24,k23,k22,k21,k20,k19,                  k50,k51,k52,k53,k54,k55, \
    k18,k17,k16,k15,k14,k13,k12,k11,  k42,k43,k44,k45,k46,k47,k48,k49, \
                k10,k9, k8, k7, k6,   k37,k38,k39,k40,k41) \
{ \
    [0 ... 5] = gl, [31 ... 36] = gr, \
    [30]=k30, [29]=k29, [28]=k28, [27]=k27, [26]=k26, [25]=k25, \
    [56]=k56, [57]=k57, [58]=k58, [59]=k59, [60]=k60, [61]=k61, \
    [24]=k24, [23]=k23, [22]=k22, [21]=k21, [20]=k20, [19]=k19, \
    [50]=k50, [51]=k51, [52]=k52, [53]=k53, [54]=k54, [55]=k55, \
    [18]=k18, [17]=k17, [16]=k16, [15]=k15, [14]=k14, [13]=k13, [12]=k12, [11]=k11, \
    [42]=k42, [43]=k43, [44]=k44, [45]=k45, [46]=k46, [47]=k47, [48]=k48, [49]=k49, \
    [10]=k10, [9]=k9,   [8]=k8,   [7]=k7,   [6]=k6, \
    [37]=k37, [38]=k38, [39]=k39, [40]=k40, [41]=k41 \
}

// Index = layer number (0 base, 1 num/func, 2 sym, 3 nav, 4 adjust)
static const uint8_t PROGMEM layer_map[][RGB_MATRIX_LED_COUNT] = {

    // ---- 0: base ----
    [0] = GRID(C_GBASE, C_GBASE,
        C_MOD,  C_KEY,  C_KEY,  C_KEY,  C_KEY,  C_KEY,                                  C_KEY,  C_KEY,  C_KEY,  C_KEY,  C_KEY,  C_MOD,
        C_MOD,  C_HOME, C_HOME, C_HOME, C_HOME, C_KEY,                                  C_KEY,  C_HOME, C_HOME, C_HOME, C_PUNH, C_PUN,
        C_MOD,  C_KEY,  C_KEY,  C_KEY,  C_KEY,  C_KEY,  C_OFF,  C_MOD,  C_DIM,  C_DIM,  C_KEY,  C_KEY,  C_PUN,  C_PUN,  C_PUN,  C_MOD,
                                C_TR,   C_TC,   C_TG,   C_TC,   C_TC,   C_TC,   C_TY,   C_TB,   C_TC,   C_OFF
    ),

    // ---- 1: num / func ----
    [1] = GRID(C_GFUNC, C_GNUM,
        C_DIM,  C_F9,   C_F10,  C_F11,  C_F12,  C_OFF,                                   C_OPR,  C_NUM,  C_NUM,  C_NUM,  C_OPR,  C_DIM,
        C_DIM,  C_F8,   C_F7,   C_F6,   C_F5,   C_OPR,                                   C_OPR,  C_NUM,  C_NUM,  C_NUM,  C_OPR,  C_OPR,
        C_DIM,  C_F4,   C_F3,   C_F2,   C_F1,   C_OFF, C_DIM,   C_TB,   C_VDIM,  C_VDIM, C_NUM,  C_NUM,  C_NUM,  C_NUM,  C_OPR,  C_DIM,
                                C_TRD,  C_NUMT, C_TGD, C_NUMT,  C_NUMT, C_NUMT,  C_TYD,  C_TB,   C_NUMT, C_OFF
    ),

    // ---- 2: sym ----
    [2] = GRID(C_GSYM, C_GSYM,
        C_DIM,  C_OFF,  C_TXT,  C_TXT,  C_COD,  C_BRK,                                  C_BRK,  C_MTH,  C_MTH,  C_TXT,  C_TXT,  C_DIM,
        C_DIM,  C_TXTH, C_TXTH, C_TXTH, C_CODH, C_BRK,                                  C_BRK,  C_MTH,  C_MTH,  C_TXT,  C_TXT,  C_SYMT,
        C_DIM,  C_OFF,  C_OFF,  C_COD,  C_COD,  C_BRK,  C_DIM,  C_TG,   C_VDIM, C_VDIM, C_BRK,  C_MTH,  C_MTH,  C_COD,  C_TXT,  C_DIM,
                                C_TRD,  C_SYMT, C_TG,   C_SYMT, C_SYMT, C_SYMT, C_TYD,  C_TBD,  C_SYMT, C_OFF
    ),

    // ---- 3: nav ----
    [3] = GRID(C_GNAV, C_GNAV,
        C_DIM,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,                                  C_NAVK, C_NAVK, C_NAVK, C_NAVK, C_NAVK, C_DIM,
        C_DIM,  C_NAVM, C_NAVM, C_NAVM, C_NAVM, C_OFF,                                  C_ARW,  C_ARW,  C_ARW,  C_ARW,  C_OFF,  C_NAVT,
        C_DIM,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_MED,  C_TY,   C_VDIM, C_VDIM, C_MED,  C_MED,  C_MED,  C_OFF,  C_OFF,  C_DIM,
                                C_TRD,  C_NAVT, C_TGD,  C_NAVT, C_NAVT, C_NAVT, C_TY,   C_TBD,  C_NAVT, C_OFF
    ),

    // ---- 4: adjust ----
    [4] = GRID(C_GADJ, C_GADJ,
        C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_ST_WIN,                               C_ST_SWAP,C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_BOOT,
        C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_BL,   C_OFF,                                  C_ADJ,    C_ADJ,  C_ADJ,  C_ADJ,  C_ADJ,  C_OFF,
        C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_BL,   C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,    C_ADJ,  C_ADJ,  C_ADJ,  C_ADJ,  C_OFF,
                                C_TR,   C_ADJT, C_TGD,  C_ADJT, C_ADJT, C_ADJT, C_TYD,  C_TBD,    C_ADJT, C_OFF
    ),
};

// ===========================================================================
#define IDLE_FADE_START_MS   60000   // start fading after 30s without input
#define IDLE_FADE_LENGTH_MS  3000    // fade takes 5 s
#define IDLE_FADE_FLOOR      51      // end brightness, 0-255 (51 = 20%, 0 = fully off)

static uint8_t state_override(uint8_t i, uint8_t layer, uint8_t c) {
    // Base layer, caps word is on
    if (layer == 0 && i == 11) {
        if (ui_state.locked) return C_ST_ON;
    }
    // Num, Sym or Nav layer are locked
    if (layer == 1 || layer == 2 || layer == 3) {
        if (i == 11 && ui_state.locked) return C_ST_ON;
    }
    // adjust layer: toggle keys show their state (use your actual LEDs)
    if (layer == 4) {
        if (i == 25 && ui_state.is_mac) return C_ST_MAC;
        if (i == 56 && ui_state.swap_btns) return C_ST_SWAPPED;
    }
    return c;
}

static uint8_t idle_scale(void) {
    uint32_t idle = last_input_activity_elapsed();
    if (idle <= IDLE_FADE_START_MS) return 255;

    uint32_t f = idle - IDLE_FADE_START_MS;
    if (f >= IDLE_FADE_LENGTH_MS) return IDLE_FADE_FLOOR;

    return 255 - (uint8_t)((255 - IDLE_FADE_FLOOR) * f / IDLE_FADE_LENGTH_MS);
}

#define RAINBOW_SPREAD  1      // Q: delete?? hue change across the board; higher = more colors at once
#define RAINBOW_MIN_V   140    // rainbow LEDs are never dimmer than this

#define GROUP(name, ...) static const uint8_t name[] = { __VA_ARGS__ }

GROUP(LEDS_GLOW,  0, 1, 2, 3, 4, 5, 31, 32, 33, 34, 35, 36);
GROUP(LEDS_THUMB_MODS, 11, 6, 7, 9, 18, 24, 30, 61, 49, 40, 43, 42, 37);
GROUP(LEDS_WASD,  22, 21, 20, 15);

enum { FX_CYCLE, FX_SWEEP }; // cycle: one color for the group; sweep: colors travel along it

typedef struct {
    const uint8_t *leds;
    uint8_t        count;
    uint16_t       layers; // bitmask of layers the rule applies to
    uint8_t        mode;
} fx_rule_t;

#define LAYERS(n) (1u << (n))
#define RULE(g, layers, mode) { g, ARRAY_SIZE(g), (layers), (mode) }

static const fx_rule_t fx_rules[] = {
    RULE(LEDS_GLOW, LAYERS(0), FX_CYCLE),
    RULE(LEDS_THUMB_MODS, LAYERS(0), FX_SWEEP),
    // RULE(LEDS_WASD, LAYERS(5), FX_SWEEP),
};

static const fx_rule_t *fx_find(uint8_t i, uint8_t layer) {
    for (uint8_t r = 0; r < ARRAY_SIZE(fx_rules); r++) {
        if (!(fx_rules[r].layers & LAYERS(layer))) continue;
        for (uint8_t k = 0; k < fx_rules[r].count; k++) {
            if (fx_rules[r].leds[k] == i) return &fx_rules[r];
        }
    }
    return NULL; // first matching rule wins
}

static HSV apply_fx(uint8_t i, uint8_t layer, HSV hsv) {
    uint8_t spd = rgb_matrix_config.speed;
    uint8_t tb  = scale16by8(g_rgb_timer, spd / 4);  // breathing clock
    uint8_t ts  = scale16by8(g_rgb_timer, spd / 8);  // sweep clock
    uint8_t tc  = scale16by8(g_rgb_timer, spd / 32); // cycle clock

    const fx_rule_t *rule = fx_find(i, layer);
    if (rule) {
        uint8_t offset = (rule->mode == FX_SWEEP) ? (uint8_t)(g_led_config.point[i].x) : 0;
        uint8_t clock = (rule->mode == FX_SWEEP) ? ts : tc;
        hsv.h = offset - clock;
        hsv.s = 255;
        hsv.v = MAX(hsv.v, RAINBOW_MIN_V);
    }

    // caps word / layer lock LED
    if (layer == 0 && i == 11 && ui_state.caps_word) {
        hsv.v = scale8(hsv.v, 110 + scale8(sin8(tb),  145));
    }

    return hsv;
}

// ===========================================================================
static bool layer_colors(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    bool mouse_on = layer_state_is(AUTO_MOUSE_DEFAULT_LAYER);
    layer_state_t base_state = (layer_state & ~((layer_state_t)1 << AUTO_MOUSE_DEFAULT_LAYER))
                             | default_layer_state;

    uint8_t layer = get_highest_layer(base_state);
    if (layer >= ARRAY_SIZE(layer_map)) layer = 0;   // undefined layers use base

    uint8_t val = rgb_matrix_get_val() * idle_scale() / 255; // keeps brightness keys working
    for (uint8_t i = led_min; i < led_max; i++) {
        uint8_t c = pgm_read_byte(&layer_map[layer][i]);
        if (mouse_on && (i == MOUSE_BTN_1 || i == MOUSE_BTN_2)) c = C_MS;
        c = state_override(i, layer, c);
        HSV hsv = {
            pgm_read_byte(&palette[c][0]),
            pgm_read_byte(&palette[c][1]),
            pgm_read_byte(&palette[c][2])
        };
        hsv = apply_fx(i, layer, hsv);
        hsv.v = (uint16_t)hsv.v * val / 255;
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif
