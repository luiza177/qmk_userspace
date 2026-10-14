// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    _QWERTY = 0,
    _NAV,
    _SYM,
    _FUNCTION,
    _ADJUST,
    // _GAMING,
    // _GTR_PRO
};

// Q: keep default mods other than home-row? yes, probably
// Q: possible to check system to determine keys?
// TODO: add homerow mods
// TODO: redo sym+num, nav/adjust, f-key(keep) layers
// TODO: adjust trackpad inertia

// TODO: gaming layer
// TODO: guitar-pro layer

// TODO: KC_WBAK and _WFWD (browser backward/forward)
// TODO: undo/redo
// TODO: brightness up/down
// TODO: mute with vol up/down click

// TODO: UNDERGLOW
// TODO: Per-key / per-layer lighting: assign each layer a color (num = blue, sym = green, adj = red, f = magenta, ? = yellow, ? = media = orange)

/* HINT: QMK FEATURE CRASH COURSE: =========================================
 *
 * ---- SPECIAL KEYS --------------------------------------------------
 * _______ / KC_TRNS   Transparent: use the key from the layer below
 * XXXXXXX / KC_NO     Does nothing (blocks the key)
 * CW_TOGG             Caps Word: capitalizes one word, auto-ends
 *                     (needs CAPS_WORD_ENABLE = yes in rules.mk)
 * QK_REP              Repeat the last key pressed
 * QK_AREP             Alternate repeat (can be customized per key)
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
 *                     (also S(kc), C(kc), A(kc), G(kc))
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
 * ---- COMBOS (press several keys together = another key) ------------
 * Needs COMBO_ENABLE = yes in rules.mk. Easiest: create combos.def
 * next to keymap.c, one line per combo:
 *     COMB(esc_combo, KC_ESC, KC_Q, KC_W)
 *     COMB(ent_combo, KC_ENT, KC_K, KC_L)
 * Timing: COMBO_TERM (config.h) = max ms between the key presses.
 * Keep combos on keys you don't roll through while typing.
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

#define SYM MO(_SYM)
#define NAV MO(_NAV)
#define FKEYS MO(_FUNCTION)
#define ADJUST MO(_ADJUST)

#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)

// Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
// The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
// produces the key `tap` when tapped (i.e. pressed and released).

// There is an extra row added for the Halcyon modules. Currently only the Encoder module is
// supported but we reserve 5 keys per half for future expansion. Your personal keymap will also
// need to be updated to include this row, and the `LAYOUT` macro will need to be updated to
// `LAYOUT` in order to compile.

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* ORIGINAL:
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : |Ctrl/' |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |  { [ |CapsLk|  |F-keys|  ] } |   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LGUI | LAlt/| Space| Nav  |  | Sym  | Space| AltGr| RGUI | Menu |
 *                        |      |      | Enter|      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
/* CUSTOM:
 * Base Layer: QWERTY
 *  Q: Enc1/Enc2 click?
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  | - _                      = + |   H  |   J  |   K  |   L  | ;  : | Ctrl/' | or Enter?
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | ENC1 | Print|  |MsL * |F-Keys|   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Adj/ | LGUI |Space/| LAlt/| Del  |  |MsR **|AltGr/|Space/| RGUI | ENC2 |
 *                        | Menu |      |  Sym | Bkspc|      |  | Bksp | Enter| Nav  |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_QWERTY] = LAYOUT(
     KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                        KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
     CTL_ESC , KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,                                        KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
     KC_LSFT , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_LBRC,KC_PSCR,     FKEYS  , KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                ADJUST , KC_LGUI, KC_SPC , ALT_ENT, NAV   ,     SYM    , KC_RALT, KC_SPC , KC_RGUI, KC_APP
    ),

/* ORIGINAL:
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | PgUp | Home |   ↑  | End  | VolUp| Delete |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  GUI |  Alt | Ctrl | Shift|      |                              | PgDn |  ←   |   ↓  |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |ScLck |  |      |      | Pause|M Prev|M Play|M Next|VolMut| PrtSc  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
/* CUSTOM:
 * Nav Layer: Media, navigation, Q: adjustments?
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | Home | PgUp | PgDn | End  | Ins  | Delete |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  GUI |  Alt | Ctrl | Shift|      |                              |  ←   |   ↓  |   ↑  |   →  |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |VolMut|      |  |      |      |M Prev|MPPlay|M Next|      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |  nav |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT(
      _______, _______, _______, _______, _______, _______,                                     KC_HOME, KC_PGUP, KC_PGDN, KC_END,   KC_INS,  KC_DEL,
      _______, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _______,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, _______, _______,
      _______, _______, _______, _______, _______, _______, KC_MUTE, _______, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |    `   |  1   |  2   |  3   |  4   |  5   |                              |   6  |  7   |  8   |  9   |  0   |   =    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |    ~   |  !   |  @   |  #   |  $   |  %   |                              |   ^  |  &   |  *   |  (   |  )   |   +    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |    |   |   \  |  :   |  ;   |  -   |  [   |  {   |      |  |      |   }  |   ]  |  _   |  ,   |  .   |  /   |   ?    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |  sym |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_SYM] = LAYOUT(
      KC_GRV ,   KC_1 ,   KC_2 ,   KC_3 ,   KC_4 ,   KC_5 ,                                       KC_6 ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_0 , KC_EQL ,
     KC_TILD , KC_EXLM,  KC_AT , KC_HASH,  KC_DLR, KC_PERC,                                     KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
     KC_PIPE , KC_BSLS, KC_COLN, KC_SCLN, KC_MINS, KC_LBRC, KC_LCBR, _______, _______, KC_RCBR, KC_RBRC, KC_UNDS, KC_COMM,  KC_DOT, KC_SLSH, KC_QUES,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

// /*
//  * Symbol layer
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
    // Q: Priorities(?):   [{(" ~` \| -+=_ ]}) esszet euro !@#$%^&* --> do we need the number symbols when the num layer exists?
//     [_SYM] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F9  | F10  | F11  | F12  |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F5  |  F6  |  F7  |  F8  |      |                              |      | Shift| Ctrl |  Alt |  GUI |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |  F1  |  F2  |  F3  |  F4  |      |      |      |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FUNCTION] = LAYOUT(
      _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, _______,                                     _______, _______, _______, _______, _______, _______,
      _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,                                     _______, KC_RSFT, KC_RCTL, KC_LALT, KC_RGUI, _______,
      _______,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

// /*
//  * Number/F-key Layer
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |  F9  | F10  | F11  | F12  |      |                              |      |   7  |   8  |   9  |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |  F5  |  F6  |  F7  |  F8  |      |                              |      |   4  |   5  |   6  |  0?  | enter? |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |  F1  |  F2  |  F3  |  F4  |      |      |      |  |      |      |      |   1  |   2  |   3  |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
    // TODO: other num-pad keys around numbers +-=*/. enter
//     [_NUM] = LAYOUT(
//       _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, _______,                                     _______,    KC_7,    KC_8,    KC_9, _______, _______,
//       _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,                                     _______,    KC_4,    KC_5,    KC_6, _______, _______,
//       _______,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, _______, _______, _______, _______, _______,    KC_1,    KC_2,    KC_3, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),


/*
 * Adjust Layer: Default layer settings, RGB
 *  Q: move LED adjustments to left side or remove entirely/move to NAV/MEDIA layer?
 *  Q: add screen backlight?
 *  ORGINAL:
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT(
      _______, _______, _______, _______, _______, _______,                                    _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______, _______,                                    RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, _______,
      _______, _______, _______, _______, _______, _______,_______, _______, _______, _______, _______, RM_SATD, RM_HUED, RM_VALD, RM_PREV, _______,
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
    [_NAV] =        { _______, _______, _______, _______, _______ },
    [_SYM] =        { _______, _______, _______, _______, _______ },
    [_FUNCTION] =   { _______, _______, _______, _______, _______ },
    [_ADJUST] =     { _______, _______, _______, _______, _______ }
};

const uint16_t right_halcyon_buttons[10][5] = {
    [_QWERTY] =     { KC_MUTE, _______, _______, _______, _______ },
    [_NAV] =        { _______, _______, _______, _______, _______ },
    [_SYM] =        { _______, _______, _______, _______, _______ },
    [_FUNCTION] =   { _______, _______, _______, _______, _______ },
    [_ADJUST] =     { _______, _______, _______, _______, _______ }
};
#endif
