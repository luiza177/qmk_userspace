// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include "keycodes.h"
#include "modifiers.h"
#include "pointing_device.h"
#include "quantum.h"
#include "report.h"
#include "rgb_matrix.h"
#include "timer.h"
#include "transactions.h"
#include "ui_state.h"
#include QMK_KEYBOARD_H

enum layers {
    _QWERTY = 0,
    _GAMING,
    _NUM_FUNC,
    _SYM,
    _NAV,
    _ADJUST,
    _AUTO_MOUSE,
    // _GTR_PRO
};

enum custom_keycodes {
    U_SWAP_BTN = SAFE_RANGE,
    U_OS_TOGG,
    U_WBAK,
    U_WFWD,
    U_EUR,
    U_ESZ,
    U_RGB_FX_TOGG,
    U_SNIPE,
    U_CPI_UP,
    U_CPI_DN,
};

// Aliases for readability
// Layer macros
#define SPC_NUM LT(_NUM_FUNC, KC_SPC)
#define SPC_SYM LT(_SYM, KC_SPC)
#define BKSPC_NAV LT(_NAV, KC_BSPC)
#define ENT_NAV LT(_NAV, KC_ENT)
#define MENU_ADJ LT(_ADJUST, KC_APP)

// Mod-tap macros
// Base layer:
#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)
// homerow mods
#define GUI_A MT(MOD_LGUI, KC_A)
#define GUI_SEMI MT(MOD_LGUI, KC_SCLN)
#define ALT_S MT(MOD_LALT, KC_S)
#define ALT_L MT(MOD_LALT, KC_L)
#define CTL_D MT(MOD_LCTL, KC_D)
#define CTL_K MT(MOD_LCTL, KC_K)
#define SFT_F MT(MOD_LSFT, KC_F)
#define SFT_J MT(MOD_RSFT, KC_J)
// num-func layer
#define GUI_F8 MT(MOD_LCTL, KC_F8)
#define GUI_PPLUS MT(MOD_LCTL, KC_PPLS)
#define ALT_F7 MT(MOD_LALT, KC_F7)
#define ALT_6 MT(MOD_LALT, KC_6)
#define CTL_F6 MT(MOD_LCTL, KC_F6)
#define CTL_5 MT(MOD_LCTL, KC_5)
#define SFT_F5 MT(MOD_LSFT, KC_F5)
#define SFT_4 MT(MOD_RSFT, KC_4)
// sym layer
#define SYM_EXLM GUI_T(KC_1)
#define SYM_AT ALT_T(KC_2)
#define SYM_CIRC CTL_T(KC_6)
#define SYM_PIPE SFT_T(KC_BSLS)

#define SET_MAC (1 << 0)
#define SET_SWAP (1 << 1)
#define SET_RGB_FX_OFF (1 << 2)

static bool    swap_btns    = false;
static uint8_t swapped_held = 0; // button bits we are currently holding
static bool    is_mac       = false;
static bool    rgb_fx_on    = true;
ui_state_t     ui_state;

#define SNIPE_DIV 33
static bool sniping = false;

static const uint16_t cpi_pct[] = {25, 50, 66, 75, 100, 150, 200}; // % of the board's own CPI
#define CPI_LEVELS ARRAY_SIZE(cpi_pct)
#define CPI_DEFAULT_LEVEL 1 // the 50 entry = half of today's speed

#define SET_CPI_POS 3 // bits 3-5 (yours use bits 0-2)
#define SET_CPI_MASK (7u << SET_CPI_POS)

static uint8_t  cpi_level = CPI_DEFAULT_LEVEL;
static uint16_t cpi_base  = 0;

static void cpi_apply(void) {
    if (!cpi_base) return;
    uint32_t cpi = (uint32_t)cpi_base * cpi_pct[cpi_level] / 100;
    if (sniping) cpi = cpi * SNIPE_DIV / 100;
    if (cpi < 1) cpi = 1;
    pointing_device_set_cpi(cpi);
}

static void ui_state_sync_handler(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
    if (in_len == sizeof(ui_state_t)) {
        memcpy(&ui_state, in_data, sizeof(ui_state_t));
    }
}

static void settings_save(void) {
    eeconfig_update_user((is_mac ? SET_MAC : 0) | (swap_btns ? SET_SWAP : 0) | (rgb_fx_on ? 0 : SET_RGB_FX_OFF) | ((uint32_t)((cpi_level - CPI_DEFAULT_LEVEL) & 7) << SET_CPI_POS));
}

static void set_mac(bool mac) {
    if (mac != is_mac) {
        is_mac = mac;
        settings_save();
    }
}

static void set_swap_btns(bool swap) {
    swap_btns    = swap;
    swapped_held = 0; // avoid a stuck button when toggling mid-hold
    settings_save();
}

static void set_rgb_fx_on(bool enabled) {
    if (rgb_fx_on != enabled) {
        rgb_fx_on = enabled;
        settings_save();
    }
}

// ==============================================================
#ifdef HLC_TFT_DISPLAY
bool hlc_macos_mode(void) {
    return ui_state.is_mac;
}

bool hlc_mouse_btn_mode(void) {
    return ui_state.swap_btns;
}
#endif

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
void pointing_device_init_user(void) {
    set_auto_mouse_enable(true);
}
#endif

#define TAP_SHIFTED(kc)                               \
    if (record->tap.count && record->event.pressed) { \
        tap_code16(kc);                               \
        return false;                                 \
    }                                                 \
    break

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case U_SWAP_BTN:
            if (record->event.pressed) {
                set_swap_btns(!swap_btns);
            }
            return false;
        case MS_BTN1:
        case MS_BTN2:
            if (swap_btns) {
                uint8_t btn = (keycode == MS_BTN1) ? (1 << 1) : (1 << 0);
                if (record->event.pressed)
                    swapped_held |= btn;
                else
                    swapped_held &= ~btn;
                return false;
            }
            break;
        case U_OS_TOGG:
            if (record->event.pressed) set_mac(!is_mac);
            return false;
        case U_WBAK:
            if (record->event.pressed) tap_code16(is_mac ? G(KC_LEFT) : A(KC_LEFT));
            return false;
        case U_WFWD:
            if (record->event.pressed) tap_code16(is_mac ? G(KC_RIGHT) : A(KC_RIGHT));
            return false;
        case U_EUR:
            if (record->event.pressed) tap_code16(is_mac ? S(A(KC_2)) : RALT(KC_5));
            return false;
        case U_ESZ:
            if (record->event.pressed) tap_code16(is_mac ? A(KC_S) : RALT(KC_S));
            return false;
        case SYM_EXLM:
            TAP_SHIFTED(KC_EXLM);
        case SYM_AT:
            TAP_SHIFTED(KC_AT);
        case SYM_CIRC:
            TAP_SHIFTED(KC_CIRC);
        case SYM_PIPE:
            TAP_SHIFTED(KC_PIPE);
        case KC_HOME:
            if (is_mac) {
                if (record->event.pressed) tap_code16(G(KC_LEFT));
                return false;
            }
            break;
        case KC_END:
            if (is_mac) {
                if (record->event.pressed) tap_code16(G(KC_RIGHT));
                return false;
            }
            break;
        case U_RGB_FX_TOGG:
            if (record->event.pressed) {
                set_rgb_fx_on(!rgb_fx_on);
            }
            return false;
        case U_SNIPE:
            sniping = record->event.pressed;
            cpi_apply();
            return false;
        case U_CPI_UP:
            if (record->event.pressed && cpi_level < CPI_LEVELS - 1) {
                cpi_level++;
                cpi_apply();
                settings_save();
            }
            return false;
        case U_CPI_DN:
            if (record->event.pressed && cpi_level > 0) {
                cpi_level--;
                cpi_apply();
                settings_save();
            }
            return false;
    }
    return true;
}

report_mouse_t pointing_device_task_user(report_mouse_t r) {
    if (swap_btns && (r.buttons & 0x01)) {
        r.buttons = (r.buttons & ~0x01) | 0x02;
    }
    r.buttons |= swapped_held; // reapply every report
    return r;
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_SYNC_UI_STATE, ui_state_sync_handler);
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_layer_colors);

    uint32_t s = eeconfig_read_user();
    is_mac     = (s & SET_MAC) != 0;
    swap_btns  = (s & SET_SWAP) != 0;
    rgb_fx_on  = !(s & SET_RGB_FX_OFF);

    uint8_t stored = (s & SET_CPI_MASK) >> SET_CPI_POS;
    cpi_level      = (stored + CPI_DEFAULT_LEVEL) & 7;
    if (cpi_level >= CPI_LEVELS) cpi_level = CPI_DEFAULT_LEVEL;

    cpi_base = pointing_device_get_cpi(); // the board's own default
    cpi_apply();
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) return;
    static ui_state_t last;
    static uint32_t   last_sync = 0;

    ui_state_t now = {.is_mac = is_mac, .swap_btns = swap_btns, .caps_word = is_caps_word_on(), .locked = is_layer_locked(get_highest_layer(layer_state)), .rgb_fx_on = rgb_fx_on};
    ui_state       = now;

    if (memcmp(&now, &last, sizeof now) != 0 || timer_elapsed32(last_sync) > 1000) {
        if (transaction_rpc_send(USER_SYNC_UI_STATE, sizeof now, &now)) {
            last      = now;
            last_sync = timer_read32();
        }
    }
}

bool process_detected_host_os_user(os_variant_t detected_os) {
    switch (detected_os) {
        case OS_MACOS:
        case OS_IOS:
            set_mac(true);
            break;
        case OS_WINDOWS:
        case OS_LINUX:
            set_mac(false);
            break;
        default:
            break; // OS_UNSURE: keep the saved/manual value
    }
    return true;
}

// ===================================================================
// TODO: guitar-pro layer + LED map + display icon
// TODO: RGB FX for adj layer RGB toggle / remove/adjust mods rgb fx
// FIXME: stepped encoder click legs are BROKEN

/* HINT: QMK FEATURE CRASH COURSE: =========================================
 *
 * ---- LAYER SWITCHING ------------------------------------------------
 * MO(n)               Momentary: layer n active while held
 * LT(n, kc)           Layer-tap: tap sends kc, hold activates layer n
 * TG(n)               Toggle layer n on/off
 * TO(n)               Switch to layer n, turning off the others
 * TT(n)               Tap-toggle: hold = momentary, tap several times
 *                     = toggle (count set by TAPPING_TOGGLE)
 * OSL(n)              One-shot layer: applies to the next key only
 * DF(n)               Set default (base) layer until reboot
 * PDF(n)              Set default layer permanently (saved to EEPROM)
 * LM(n, mod)          Layer n active with modifier mod held
 *                     (e.g. LM(_NAV, MOD_LCTL))
 *
 * Layer functions for C code (process_record_user, etc.):
 *   layer_on(n)  layer_off(n)  layer_move(n)  layer_invert(n)
 *   get_highest_layer(layer_state)  -> current top layer
 *   layer_state_set_user(state)     -> runs on every layer change
 *
 * ---- TAP DANCE (different action per tap count) --------------------
 * Needs TAP_DANCE_ENABLE = yes in rules.mk.
 * In keymap:     TD(TD_NAME)
 * Declare:       enum { TD_NAME };
 * Define:        tap_dance_action_t tap_dance_actions[] = {
 *                    [TD_NAME] = ACTION_TAP_DANCE_DOUBLE(KC_A, KC_B),
 *                };
 *   ACTION_TAP_DANCE_DOUBLE(single, double)  1 tap vs 2 taps
 *   ACTION_TAP_DANCE_LAYER_TOGGLE(kc, layer) tap = kc, 2 taps = toggle
 *   ACTION_TAP_DANCE_FN_ADVANCED(...)        full custom control
 * Timing uses TAPPING_TERM.
 *
 * ---- KEY OVERRIDES (change a modifier+key result) ------------------
 * Needs KEY_OVERRIDE_ENABLE = yes in rules.mk.
 *   const key_override_t del_override =
 *       ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);   // Shift+Bksp = Del
 *   const key_override_t *key_overrides[] = { &del_override };
 *
 * ---- CUSTOM KEYCODES / MACROS --------------------------------------
 *   enum custom_keycodes { MY_KEY = SAFE_RANGE };   // declare
 *   bool process_record_user(uint16_t keycode, keyrecord_t *record) {
 *       switch (keycode) {
 *           case MY_KEY:
 *               if (record->event.pressed) { SEND_STRING("hello"); }
 *               return false;   // false = "I handled it"
 *       }
 *       return true;            // true = let QMK handle it normally
 *   }
 *   SEND_STRING("text")   types a string
 *   tap_code(KC_X)        taps a key
 *   register_code(kc) / unregister_code(kc)   hold / release
 *
 * ---- MOUSE KEYS (buttons/wheel on layers; works with the trackpad) -
 * MS_BTN1 MS_BTN2 MS_BTN3   Left / right / middle click
 * MS_WHLU MS_WHLD           Scroll up / down
 * MS_WHLL MS_WHLR           Scroll left / right
 *
 * =========================================================================*/

// Combos
enum combos { JK_ESC, DF_TAB, LSEMI_ENTER, MS_MID };

const uint16_t PROGMEM jk_combo[]    = {SFT_J, CTL_K, COMBO_END};
const uint16_t PROGMEM df_combo[]    = {CTL_D, SFT_F, COMBO_END};
const uint16_t PROGMEM lsemi_combo[] = {ALT_L, GUI_SEMI, COMBO_END};
const uint16_t PROGMEM mouse_combo[] = {MS_BTN1, MS_BTN2, COMBO_END};

combo_t key_combos[] = {
    [JK_ESC]      = COMBO(jk_combo, KC_ESC),
    [DF_TAB]      = COMBO(df_combo, KC_TAB),
    [LSEMI_ENTER] = COMBO(lsemi_combo, KC_ENTER),
    [MS_MID]      = COMBO(mouse_combo, MS_BTN3),
};

bool get_combo_must_tap(uint16_t index, combo_t *combo) {
    switch (index) {
        case JK_ESC:
        case DF_TAB:
        case LSEMI_ENTER:
            return true;
    }
    return false;
}

// There is an extra row added for the Halcyon modules. Currently only the Encoder module is
// supported but we reserve 5 keys per half for future expansion. Your personal keymap will also
// need to be updated to include this row, and the `LAYOUT` macro will need to be updated to
// `LAYOUT` in order to compile.

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *  Q: right pinky bkspc and shift for - and =???

 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : | Ctrl/' | or Enter?
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |      |CapsW?|  | back |  fwd |   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Adj/ | Game |Space/| Esc  | Del  |  |Bkspc | Nav/ |Space/| RGUI | ???  |
 *                        | Menu |      |  Sym |      |      |  |      | Enter| Num  |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_QWERTY] = LAYOUT(
        KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,       KC_T ,                                          KC_Y,    KC_U,     KC_I,   KC_O,    KC_P,  KC_BSPC,
        CTL_ESC , GUI_A,  ALT_S  ,  CTL_D ,   SFT_F,       KC_G ,                                          KC_H,   SFT_J,    CTL_K,  ALT_L,GUI_SEMI, CTL_QUOT,
        KC_LSFT , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,       KC_B , XXXXXXX, CW_TOGG,     U_WBAK,  U_WFWD,   KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH,  KC_RSFT,
                                  MENU_ADJ, MO(_GAMING), SPC_SYM,  KC_ESC,  KC_DEL,    KC_BSPC, ENT_NAV, SPC_NUM, KC_RGUI, S(KC_SLSH)
    ),

/*
 * Number/F-key Layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F9  | F10  | F11  | F12  |      |                              | num /|   7  |   8  |   9  | num =|        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F8  |  F7  |  F6  |  F5  | Print|                              | num *|   4  |   5  |   6  | num +| enter? |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |  F4  |  F3  |  F2  |  F1  |      |      |LyrLck|  |      |      |   0  |   1  |   2  |   3  | num -|        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NUM_FUNC] = LAYOUT(
      _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, XXXXXXX,                                     KC_PSLS,    KC_7,    KC_8,    KC_9, KC_PEQL, _______,
      _______,  GUI_F8,  ALT_F7,  CTL_F6,  SFT_F5, KC_PSCR,                                     KC_PAST,   SFT_4,   CTL_5,  ALT_6,GUI_PPLUS, KC_PENT,
      _______,  KC_F4 ,  KC_F3 ,  KC_F2 ,  KC_F1 , XXXXXXX, _______, QK_LLCK, _______, _______,    KC_0,    KC_1,    KC_2,    KC_3, KC_PMNS, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

/*
 * Symbol layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |  $   |   ~  |   `  |   (  |                              |   )  |   +  |   *  |  EUR |eszett|        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |   !  |  @   |   ^  |   |  |   [  |                              |   ]  |   -  |   =  |   _  |   "  |  enter |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |   &  |   \  |   {  |      |LyrLck|  |      |      |   }  |   /  |   %  |   #  |   ?  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+-------------+--------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */

    [_SYM] = LAYOUT(
      _______, XXXXXXX,  KC_DLR, KC_TILD,  KC_GRV, KC_LPRN,                                     KC_RPRN, KC_PLUS, KC_ASTR,   U_EUR,   U_ESZ, _______,
      _______,SYM_EXLM,  SYM_AT,SYM_CIRC,SYM_PIPE, KC_LBRC,                                     KC_RBRC, KC_MINS,  KC_EQL, KC_UNDS, KC_DQUO,  KC_ENT,
      _______, XXXXXXX, XXXXXXX, KC_AMPR, KC_BSLS, KC_LCBR, XXXXXXX, QK_LLCK, _______, _______, KC_RCBR, KC_SLSH, KC_PERC, KC_HASH, KC_QUES, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

/*
 * Nav Layer: Media, navigation
 *
 *  TODO: add MacOS display brightness, KC_BRIU/KC_BRID
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | Home | PgDn | PgUp | End  | Ins  |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  GUI |  Alt | Ctrl | Shift|      |                              |  ←   |   ↓  |   ↑  |   →  |      |  ENTER |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |VolMut|LyrLck|  |      |      |M Prev|MPPlay|M Next|      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT(
      _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END,  KC_INS, _______,
      _______, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX,  KC_ENT,
      _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MUTE, QK_LLCK, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),


/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |OStogg|                              |swap m|      | CPI- | CPI+ |      |  boot  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |      | BLI  |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      | BLD  |      |      |      |  |      |      |RGBtog| SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT(
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, U_OS_TOGG,                                        U_SWAP_BTN, XXXXXXX,U_CPI_DN,U_CPI_UP, XXXXXXX, QK_BOOT,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   BL_UP,   XXXXXXX,                                           RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, XXXXXXX,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, BL_DOWN,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, U_RGB_FX_TOGG, RM_SATD, RM_HUED, RM_VALD, RM_PREV, XXXXXXX,
                                 _______, _______,   _______, _______, _______, _______, _______,       _______, _______, _______
    ),

/*
 * Gaming layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  ??? |   Q  |   X  |   E  |   R  |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Shift|   A  |   W  |   D  |   F  |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        | Ctrl |   Z  |   S  |   C  |   B  |      |LyrLck|  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      | Space|      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_GAMING] = LAYOUT(
      _______, XXXXXXX, KC_Q,  KC_X,    KC_E,    KC_R,                                        _______, _______, _______, _______, _______, _______,
      _______, KC_LSFT, KC_A,  KC_W,    KC_D,    KC_F,                                        _______, _______, _______, _______, _______, _______,
      _______, KC_LCTL, KC_Z,  KC_S,    KC_C,    KC_B,    _______, QK_LLCK, _______, _______, _______, _______, _______, _______, _______, _______,
                               _______, _______, KC_SPC,  _______, _______, _______, _______, _______, _______, _______
    ),

// /*
//  *  Guitar Pro layer
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |   7  |   8  |   9  |      |                              |alt+← |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |   G? |  A4  |  ^5  |  S6  |      |                              |  ←   |  S↓  |  ^↑  |  A→  |  G?  |  ENTER |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |  Bend  |   0  |   1  |   2  |   3  |      |      |LyrLck|  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//    // Q: Bend(b), stacc(!), dotted(.), triplet(/), inc/dec length(num+/-), PM(p), harmonic
//    // Q: reverse sides?
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),

/*
 * Auto-Mouse layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | snip?|      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |  LMB | snip?|      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |  RMB |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_AUTO_MOUSE] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                                     U_SNIPE, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, MS_BTN1, _______, _______, _______, _______, _______, _______, _______,
                               _______, _______, _______, _______, _______, MS_BTN2, _______, _______, _______, _______
    ),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};

#if defined(ENCODER_MAP_ENABLE)
// Q: MIDI?
// TODO: word/line back/forth? in different layers or left?
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(MS_WHLU, MS_WHLD),  ENCODER_CCW_CW(MS_WHLU, MS_WHLD)  },
    [1] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [2] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [3] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [4] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [5] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [6] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
};

// WARN: might be wrong
// bool encoder_update_user(uint8_t index, bool clockwise) {
//    if (index == 0) {                              // left encoder
//        if (get_mods() & MOD_MASK_SHIFT) {
//            tap_code(clockwise ? MS_WHLR : MS_WHLL);   // Shift held: horizontal scroll
//        } else {
//            tap_code(clockwise ? MS_WHLD : MS_WHLU);   // normal: vertical scroll
//        }
//    }
//    return false;   // false = "I handled it"
// }

// or for sticky switching encoder modes:

// enum custom_keycodes { ENC_MODE = SAFE_RANGE };
// static uint8_t enc_mode = 0;     // 0 = scroll, 1 = zoom, 2 = volume
//
// bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//    if (keycode == ENC_MODE && record->event.pressed) {
//        enc_mode = (enc_mode + 1) % 3;
//        return false;
//    }
//    return true;
// }
//
// bool encoder_update_user(uint8_t index, bool clockwise) {
//    if (index == 0) {
//        switch (enc_mode) {
//            case 0: tap_code(clockwise ? MS_WHLD : MS_WHLU); break;
//            case 1: tap_code16(clockwise ? LGUI(KC_EQL) : LGUI(KC_MINS)); break;  // zoom (Cmd on macOS)
//            case 2: tap_code(clockwise ? KC_VOLU : KC_VOLD); break;
//        }
//    }
//    return false;
// }
#endif

// WARNING: leave these alone?
#if defined (HALCYON_ENABLE)
const uint16_t left_halcyon_buttons[10][5] = {
    [_QWERTY] =     { KC_MUTE, _______, _______, _______, _______ },
    [_GAMING] =     { _______, _______, _______, _______, _______ },
    [_NUM_FUNC] =   { _______, _______, _______, _______, _______ },
    [_SYM] =        { _______, _______, _______, _______, _______ },
    [_NAV] =        { _______, _______, _______, _______, _______ },
    [_ADJUST] =     { _______, _______, _______, _______, _______ },
    [_AUTO_MOUSE] = { _______, _______, _______, _______, _______ }
};

const uint16_t right_halcyon_buttons[10][5] = {
    [_QWERTY] =     { KC_MUTE, _______, _______, _______, _______ },
    [_GAMING] =     { _______, _______, _______, _______, _______ },
    [_NUM_FUNC] =   { _______, _______, _______, _______, _______ },
    [_SYM] =        { _______, _______, _______, _______, _______ },
    [_NAV] =        { _______, _______, _______, _______, _______ },
    [_ADJUST] =     { _______, _______, _______, _______, _______ },
    [_AUTO_MOUSE] = { _______, _______, _______, _______, _______ }
};
#endif
