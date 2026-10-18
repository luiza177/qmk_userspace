RGB_MATRIX_EFFECT(layer_colors)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

typedef struct { uint8_t r, g, b; } color_t;

typedef struct {
    uint8_t first, last;   // inclusive LED index range
    color_t color;
} region_t;

typedef struct {
    color_t         underglow;
    color_t         background;   // any key LED not covered by a region
    const region_t *regions;
    uint8_t         count;
} layer_def_t;

// ---- colors ----
#define COL_OFF     {0x00, 0x00, 0x00}
#define COL_WHITE   {0xff, 0xff, 0xff}
#define COL_YELLOW  {0xff, 0xff, 0x00}
#define COL_ORANGE  {0xff, 0x88, 0x00}
#define COL_CYAN    {0x00, 0xff, 0xff}
#define COL_BLUE    {0x00, 0x00, 0xff}
#define COL_MAGENTA {0xff, 0x00, 0xff}
#define COL_GREEN   {0x00, 0xff, 0x00}
#define COL_RED     {0xff, 0x00, 0x00}

// ---- LED groups (shared between layers) ----
#define LEFT_THUMB   6, 12
#define RIGHT_THUMB  37, 41

// TODO: Per-key / per-layer lighting: assign each layer a color (num = blue, sym = green, adj = red, f = magenta, ? = yellow, ? = media = orange)

// ---- layer 0: base ----
static const region_t base_regions[] = {
    {LEFT_THUMB,  COL_CYAN},
    {RIGHT_THUMB, COL_CYAN},
    {42, 43, COL_OFF},
    {18, 18, COL_CYAN}, {24, 24, COL_CYAN}, {30, 30, COL_CYAN},
    {49, 49, COL_CYAN}, {61, 61, COL_CYAN},
};

// ---- layer 1: num/func ----
static const region_t num_func_regions[] = {
    {LEFT_THUMB,  COL_CYAN},
    {RIGHT_THUMB, COL_CYAN},

    {50, 50, COL_CYAN}, {56, 56, COL_CYAN}, {48, 48, COL_CYAN}, {54, 55, COL_CYAN}, {60, 60, COL_CYAN},
    {44, 47, COL_BLUE},    {51, 53, COL_BLUE},    {57, 59, COL_BLUE},     // num
    {14, 17, COL_MAGENTA}, {20, 23, COL_MAGENTA}, {26, 29, COL_MAGENTA},  // func

    {18, 18, COL_CYAN}, {24, 24, COL_CYAN}, {30, 30, COL_CYAN},
    {49, 49, COL_CYAN}, {61, 61, COL_CYAN},
};

// ---- layer 2: sym ----
static const region_t sym_regions[] = {
    // keys with a symbol assigned
    {13, 17, COL_GREEN}, {19, 23, COL_GREEN}, {25, 28, COL_GREEN},
    {44, 48, COL_GREEN}, {50, 58, COL_GREEN},

    // assumed transparent: thumbs and mod positions
    {LEFT_THUMB,  COL_CYAN}, {11, 11, COL_GREEN},
    {RIGHT_THUMB, COL_CYAN},
    {18, 18, COL_CYAN}, {24, 24, COL_CYAN}, {30, 30, COL_CYAN},
    {49, 49, COL_CYAN}, {61, 61, COL_CYAN},
};

// ---- layer 3: nav ----
static const region_t nav_regions[] = {
    {20, 23, COL_YELLOW}, {44, 46, COL_ORANGE}, {50,53, COL_YELLOW}, {55, 60, COL_YELLOW},

    {LEFT_THUMB,  COL_CYAN}, {11, 12, COL_YELLOW},
    {RIGHT_THUMB, COL_CYAN},
    {18, 18, COL_CYAN}, {24, 24, COL_CYAN}, {30, 30, COL_CYAN},
    {49, 49, COL_CYAN}, {61, 61, COL_CYAN},
};

// ---- layer 4: adjust ----
static const region_t adj_regions[] = {
    {14, 14, COL_RED}, {20, 20, COL_RED}, {50, 54, COL_RED}, {45, 48, COL_RED}, {57, 59, COL_RED}, {61, 61, COL_RED},

    {LEFT_THUMB,  COL_CYAN},
    {RIGHT_THUMB, COL_CYAN},
};

// ---- layer 5: gaming ----

// ---- layer 6: guitar pro ----

// Index = layer number. Keep in sync with your keymap's layer enum.
static const layer_def_t layers[] = {
    [0] = { COL_WHITE, COL_WHITE, base_regions,     ARRAY_SIZE(base_regions) },
    [1] = { COL_CYAN,   COL_OFF,   num_func_regions, ARRAY_SIZE(num_func_regions) },
    [2] = { COL_GREEN,  COL_OFF,   sym_regions,      ARRAY_SIZE(sym_regions) },
    [3] = { COL_YELLOW,  COL_OFF,   nav_regions,      ARRAY_SIZE(nav_regions) },
    [4] = { COL_RED,  COL_OFF,   adj_regions,      ARRAY_SIZE(adj_regions) },
};

static color_t color_for_led(const layer_def_t *def, uint8_t i) {
    if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_UNDERGLOW)) {
        return def->underglow;
    }
    for (uint8_t n = 0; n < def->count; n++) {   // first match wins
        if (i >= def->regions[n].first && i <= def->regions[n].last) {
            return def->regions[n].color;
        }
    }
    return def->background;
}

static bool layer_colors(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    if (layer >= ARRAY_SIZE(layers)) layer = 0;   // undefined layers use base
    const layer_def_t *def = &layers[layer];

    uint8_t val = rgb_matrix_get_val();           // keeps brightness keys working
    for (uint8_t i = led_min; i < led_max; i++) {
        color_t c = color_for_led(def, i);
        rgb_matrix_set_color(i, c.r * val / 255, c.g * val / 255, c.b * val / 255);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif
