#pragma once

#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD

// #undef TAPPING_TERM
// #define TAPPING_TERM 200 // INFO: ms between tap/hold (170-250 ms usually)
// // #define TAPPING_TERM_PER_KEY

// INFO: tap then press again within this = key repeat, not hold
#define QUICK_TAP_TERM 100

// HINT: Needs hand info; if it misbehaves, see QMK's Chordal Hold docs for chordal_hold_layout
// #define CHORDAL_HOLD // INFO: opposite-hand rolls = hold, same-hand rolls = tap

// #define FLOW_TAP_TERM 150 // INFO: during fast typing, mod-taps act as plain taps

// Older alternatives (usually skip these if using Chordal Hold):
// #define PERMISSIVE_HOLD          // hold triggers if another key is pressed and released
// #define HOLD_ON_OTHER_KEY_PRESS  // hold triggers as soon as another key is pressed
// #define RETRO_TAPPING            // hold with no other key still sends the tap

// INFO: for Combos (a+b=c)
#define COMBO_TERM 30 // ms window to press all combo keys
#undef COMBO_MUST_PRESS_IN_ORDER
#define COMBO_MUST_TAP_PER_COMBO
// #define COMBO_ONLY_FROM_LAYER 0  // combos work only on this layer

// INFO: for encoders
// HINT: One value per encoder, in encoder index order (left/right). Stepped encoders usually want 4 or 2, smooth 1 or 2.
// #undef ENCODER_RESOLUTION
// #define ENCODER_RESOLUTIONS { 2, 4 }

// INFO: Auto Mouse layer
// HINT: Also call set_auto_mouse_layer(_MOUSE) in pointing_device_init_user().
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 5 // _AUTO_MOUSE
#define AUTO_MOUSE_TIME 400        // ms before dropping back to typing
// #define AUTO_MOUSE_THRESHOLD 10  // movement needed to activate (raise if triggered by accident)
// // #define AUTO_MOUSE_DELAY 400     // ms after a keypress before the mouse layer can re-trigger
