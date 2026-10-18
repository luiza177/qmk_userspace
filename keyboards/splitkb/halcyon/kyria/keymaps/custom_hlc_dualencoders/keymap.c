// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include "keycodes.h"
#include "modifiers.h"
#include "rgb_matrix.h"
#include QMK_KEYBOARD_H

enum layers {
    _QWERTY = 0,
    _NUM_FUNC,
    _SYM,
    _NAV,
    _ADJUST,
    // _GAMING,
    // _GTR_PRO
};

// TODO: adjust trackpad inertia
// TODO: detect OS for ESZETT and EURO keycommands / undo/redo

// LAYERS
// TODO: gaming layer
// TODO: guitar-pro layer

// LIGHTS and RGB
// TODO: UNDERGLOW
// TODO: Per-key / per-layer lighting: assign each layer a color (num = blue, sym = green, adj = red, f = magenta, ? = yellow, ? = media = orange)

/* HINT: QMK FEATURE CRASH COURSE: =========================================
 *
 * ---- SPECIAL KEYS --------------------------------------------------
 * _______ / KC_TRNS   Transparent: use the key from the layer below
 * XXXXXXX / KC_NO     Does nothing (blocks the key)
 * CW_TOGG             Caps Word: capitalizes one word, auto-ends
 *                     (needs CAPS_WORD_ENABLE = yes in rules.mk)
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
 * ---- MODIFIERS ------------------------------------------------------
 * KC_LSFT KC_LCTL KC_LALT KC_LGUI   Left Shift / Ctrl / Alt / Cmd(Win)
 * KC_RSFT KC_RCTL KC_RALT KC_RGUI   Right versions
 *
 * LSFT(kc) LCTL(kc) LALT(kc) LGUI(kc)  Send kc with that modifier
 *                     (also S(kc), C(kc), A(kc), G(kc)) // HINT: maybe this for homerow in sym layer?
 *
 * HYPR(kc)            Ctrl+Shift+Alt+Gui together with kc
 * MEH(kc)             Ctrl+Shift+Alt together with kc
 * OSM(MOD_LSFT)       One-shot modifier: applies to the next key only
 *                     (also MOD_LCTL, MOD_LALT, MOD_LGUI, MOD_RSFT...)
 *
 * ---- MOD-TAPS (tap = key, hold = modifier) -------------------------
 * MT(MOD_LSFT, KC_A)  General form: tap A, hold Shift
 * LSFT_T(kc) RSFT_T(kc)   Shift on hold
 * LCTL_T(kc) RCTL_T(kc)   Ctrl on hold
 * LALT_T(kc) RALT_T(kc)   Alt on hold
 * LGUI_T(kc) RGUI_T(kc)   Cmd/Win on hold
 * Home row mods = mod-taps on the home row keys (A S D F / J K L ;).
 * Misfires are tuned in config.h (TAPPING_TERM, CHORDAL_HOLD,
 * FLOW_TAP_TERM, QUICK_TAP_TERM, PERMISSIVE_HOLD).
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
 * MS_UP MS_DOWN MS_LEFT MS_RGHT   Move cursor (needs MOUSEKEY_ENABLE)
 * MS_ACL0 MS_ACL1 MS_ACL2   Cursor speed presets
 * (older QMK names: KC_BTN1, KC_WH_U, KC_MS_U, KC_ACL0, ...)
 *
 * ---- HANDY CALLBACKS ---------------------------------------------------
 *   get_tapping_term(keycode, record)      Per-key tap-hold timing
 *                                          (needs TAPPING_TERM_PER_KEY)
 *   layer_state_set_user(state)             React to layer changes
 *   keyboard_post_init_user()               One-time startup code
 * =========================================================================*/

// Aliases for readability

// Layer macros
#define SPC_NUM LT(_NUM_FUNC, KC_SPC)
#define SPC_SYM LT(_SYM, KC_SPC)
#define BKSPC_NAV LT(_NAV, KC_BSPC)
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

// Combos
enum combos { JK_ESC, DF_TAB, LSEMI_ENTER };

const uint16_t PROGMEM jk_combo[]    = {SFT_J, CTL_K, COMBO_END};
const uint16_t PROGMEM df_combo[]    = {CTL_D, SFT_F, COMBO_END};
const uint16_t PROGMEM lsemi_combo[] = {ALT_L, GUI_SEMI, COMBO_END};

combo_t key_combos[] = {
    [JK_ESC]      = COMBO(jk_combo, KC_ESC),
    [DF_TAB]      = COMBO(df_combo, KC_TAB),
    [LSEMI_ENTER] = COMBO(lsemi_combo, KC_ENTER),
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
 *  TODO: change thumb alt into nav (right), so thumb bkspc is free. left thumb alt can be??
 *  TODO: right pinky bkspc and shift for - and =???
 *  Q: Trilayers?
 *  Q: undo/redo
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  | - _                      = + |   H  |   J  |   K  |   L  | ;  : | Ctrl/' | or Enter?
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |      |CapsW?|  | back |  fwd |   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Adj/ | LGUI |Space/| LAlt/| Del  |  |Bkspc/| Alt/ |Space/| RGUI | ???  |
 *                        | Menu |      |  Sym | Bkspc|      |  | Nav  | Enter| Num  |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_QWERTY] = LAYOUT(
     KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                          KC_Y,    KC_U,     KC_I,   KC_O,    KC_P,  KC_BSPC,
     CTL_ESC , GUI_A,  ALT_S  ,  CTL_D ,   SFT_F,   KC_G ,                                          KC_H,   SFT_J,    CTL_K,  ALT_L,GUI_SEMI, CTL_QUOT,
     KC_LSFT , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B ,XXXXXXX, CW_TOGG,     KC_WBAK, KC_WFWD,   KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH,  KC_RSFT,
                               MENU_ADJ, KC_LGUI, SPC_SYM,ALT_ENT,  KC_DEL,   BKSPC_NAV, ALT_ENT,SPC_NUM, KC_RGUI,  XXXXXXX
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
 * |        |      |  EUR |   ~  |   `  |   (  |                              |   )  |   +  |   *  |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |   !  |  @   |   ^  |   |  |   [  |                              |   ]  |   -  |   =  |   _  |   "  |  enter |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |eszett|  $   |   &  |   \  |   {  |      |LyrLck|  |      |      |   }  |   /  |   %  |   #  |   ?  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+-------------+--------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
// TEST: needs testing and adjusting positions
// FIXME: modtap for sym layer... note that if mod-tap mod is pressed before layer change, mod remains activated
    [_SYM] = LAYOUT(
      _______, XXXXXXX, XXXXXXX, KC_TILD,  KC_GRV, KC_LPRN,                                     KC_RPRN, KC_PLUS, KC_ASTR, XXXXXXX, XXXXXXX, _______,
      _______, KC_EXLM,   KC_AT, KC_CIRC, KC_PIPE, KC_LBRC,                                     KC_RBRC, KC_MINS,  KC_EQL, KC_UNDS, KC_DQUO,  KC_ENT,
      _______, XXXXXXX,  KC_DLR, KC_AMPR, KC_BSLS, KC_LCBR, _______, QK_LLCK, _______, _______, KC_RCBR, KC_SLSH, KC_PERC, KC_HASH, KC_QUES, _______,
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
 *  TODO: trackpad tap = LMB or RMB
 *  TODO: MacOS toggle, for undo/redo, fwd/back,  etc
 *  HINT: CG_TOGG for swapping CTRL and GUI, AG_TOGG for ALT and GUI
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              |      |accel2|accel1|accel0|      |  boot  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |      | BLI  |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      | BLD  |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT(
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                    XXXXXXX, MS_ACL2, MS_ACL1, MS_ACL0, XXXXXXX, QK_BOOT,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   BL_UP, XXXXXXX,                                    RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, XXXXXXX,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, BL_DOWN, XXXXXXX,XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RM_SATD, RM_HUED, RM_VALD, RM_PREV, XXXXXXX,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______
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
    [_NUM_FUNC] =   { _______, _______, _______, _______, _______ },
    [_SYM] =        { _______, _______, _______, _______, _______ },
    [_NAV] =        { _______, _______, _______, _______, _______ },
    [_ADJUST] =     { _______, _______, _______, _______, _______ }
};

const uint16_t right_halcyon_buttons[10][5] = {
    [_QWERTY] =     { KC_MUTE, _______, _______, _______, _______ },
    [_NUM_FUNC] =   { _______, _______, _______, _______, _______ },
    [_SYM] =        { _______, _______, _______, _______, _______ },
    [_NAV] =        { _______, _______, _______, _______, _______ },
    [_ADJUST] =     { _______, _______, _______, _______, _______ }
};
#endif

void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_layer_colors);
}
