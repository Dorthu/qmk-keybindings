#include QMK_KEYBOARD_H

enum layers {
    _SLACK,
    _DOCS,
    _RGB,
    _GODOT,
    TOTAL_LAYERS,
};

enum custom_keycodes {
    // ** GLOBAL **
    // custom keycode for jumping to target layer, used on all layers by knob
    CUSTOM_JUMP_LAYER = SAFE_RANGE,

    // *** SLACK LAYER ***
    // custom keycode for slack reaction modifier
    CUSTOM_SLACK_MOD,
    // custom keycodes for slack emotes
    CUSTOM_S1,
    CUSTOM_S2,
    CUSTOM_S3,
    CUSTOM_S4,
    CUSTOM_S5,
    CUSTOM_S6,
    CUSTOM_S7,
    CUSTOM_S8,
    CUSTOM_S9,
    CUSTOM_S10,
    CUSTOM_S11,
};

// these are for CUSTOM_Sx, with index 0 being CUSTOM_S2 and index 10 being CUSTOM_S11
const char* emote_lookup[11] = {
    "lenny2", // top row, first key
    "pharahshrug", // top row, second key
    "shrug-guy", // top row, third key
    "+1", // top row, fourth key
    "joy", // middle row, first key
    "awesome", // middle row, second key
    "duckhunt", // middle row, third key
    "hehehehe", // middle row, fourth key
    // bottom row, first key reserved for CUSTOM_SLACK_MOD
    "sparkly", // bottom row, second key
    "oof", // bottom row, third key
    "eyes", // bottom row, fourth key
};


const char* layer_name(int layer) {
    // these returned strings should be of length 6
    switch (layer) {
        case _DOCS:
            return PSTR("Docs  ");
        case _RGB:
            return PSTR("RGB   ");
        case _GODOT:
            return PSTR("Godot ");
        case _SLACK:
            return PSTR("Slack ");
        default:
            return PSTR("???   ");
    }
}

// this is the target for CUSTOM_JUMP_LAYER
int to_layer = 0;

// this records the pressed state of CUSTOM_SLACK_MOD
bool slack_mod_pressed = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_RGB] = LAYOUT_ortho_3x4(
        CUSTOM_JUMP_LAYER,
		RGB_MOD, RGB_HUI, RGB_HUD, RGB_RMOD, 
		RGB_SPI, RGB_SPD, RGB_VAI, RGB_VAD, 
		RGB_SAI, RGB_SAD, RGB_M_T, KC_NO
        ),

    [_GODOT] = LAYOUT_ortho_3x4(
        CUSTOM_JUMP_LAYER,
        KC_Q, KC_W, KC_E, KC_S,
        KC_F5,	KC_NO,	KC_NO,	KC_NO,
        KC_NO,	KC_NO,	KC_NO,	KC_NO
        ),
  
    [_DOCS] = LAYOUT_ortho_3x4(
        CUSTOM_JUMP_LAYER,
        KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO,
        LCMD(LALT(KC_1)), LCMD(LALT(KC_2)), LCMD(LALT(KC_3)), LCMD(LALT(KC_0))
        ),

    [_SLACK] = LAYOUT_ortho_3x4(
        CUSTOM_JUMP_LAYER,
        CUSTOM_S1, CUSTOM_S2, CUSTOM_S3, CUSTOM_S4,
        CUSTOM_S5, CUSTOM_S6, CUSTOM_S7, CUSTOM_S8,
        CUSTOM_SLACK_MOD, CUSTOM_S9, CUSTOM_S10, CUSTOM_S11
        ),
};

/*
 Returns a string to be sent for a CUSTOM_Sx key, including supporting the modifier
 key CUSTOM_SLACK_MOD

 NOTE: This must be called with a key in the range of CUSTOM_S1 to CUSTOM_S11
 */
const char* slack_emote(uint16_t keycode) {
    return emote_lookup[keycode - CUSTOM_S1];
}

/* 
    Per the docs here_, we're using a custom keycode to capture the layer jump on
    the knob press.  This is the only keycode handled here; in all other cases this
    returns ``false``` and indicates that QMK should process the keycode normally.

    .. _here: https://docs.qmk.fm/#/feature_macros
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
        case CUSTOM_JUMP_LAYER:
            if (record->event.pressed) {
                layer_move(to_layer);
            }
            return false;
        case CUSTOM_S1:
        case CUSTOM_S2:
        case CUSTOM_S3:
        case CUSTOM_S4:
        case CUSTOM_S5:
        case CUSTOM_S6:
        case CUSTOM_S7:
        case CUSTOM_S8:
        case CUSTOM_S9:
        case CUSTOM_S10:
        case CUSTOM_S11:
            if (record->event.pressed) {
                if (slack_mod_pressed) {
                    send_string(SS_LCMD(SS_LSFT("\\")));
                } else {
                    send_string(":");
                }
                send_string(slack_emote(keycode));
                if (slack_mod_pressed) {
                    /*
                     how long to wait between entering a slack emoji reaction key sequence (CMD+SHIFT+\
                     plus the emoji name) and pressing enter.  This is necessary because slack is slow,
                     and without this long of a delay it will submit the reaction before finding the correct
                     emote in the menu.  It's an awkwardly long time, but I'm not sure I can do better
                     */
                    send_string(SS_DELAY(1000) "\n");
                } else {
                    send_string(": ");
                }
            }
            break;
        case CUSTOM_SLACK_MOD:
            // holding this key changes the emitted values for the CUSTOM_Sx codes,
            // but it doesn't do anything on its own
            slack_mod_pressed = record->event.pressed;
            return false;
    }

    return true;
}

// Spinning the knob changes jump layer target
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        to_layer++;
        if (to_layer >= TOTAL_LAYERS) { to_layer = 0; }
    } else {
        to_layer--;
        if (to_layer < 0) { to_layer = TOTAL_LAYERS-1; }
    }

    return false;
}



#ifdef OLED_ENABLE

//------------------------------------------------------------------------------------------------

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_0;  // flips the display 180 degrees if offhand
}
 

static void render_logo(void) {     // Render MechWild "MW" Logo
    static const char PROGMEM logo_1[] = {0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x04, 0x06, 0x82, 0xc3, 0x43, 0x61, 0xa1, 0xa1,0xa1, 0xa1, 0x61, 0x43, 0xc3, 0x82, 0x06, 0x04, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00, 0x00};
    static const char PROGMEM logo_2[] = {0xf8, 0x1e, 0xc3, 0xf8, 0x5c, 0x76, 0x7b, 0x6d, 0x75, 0x55, 0x55, 0x75, 0x6d, 0x7b, 0x76, 0x5e,0x7e, 0x77, 0x5b, 0x6d, 0x75, 0x55, 0x55, 0x75, 0x4d, 0x7b, 0x66, 0x5c, 0xf0, 0xc3, 0x3e, 0xf0};
    static const char PROGMEM logo_3[] = {0x1f, 0x78, 0xc1, 0x0f, 0x38, 0xe0, 0xc0, 0x00, 0x00, 0x02, 0x0e, 0x0e, 0x8e, 0xc6, 0xc0, 0x40,0x40, 0xc0, 0xc6, 0x8e, 0x0e, 0x0e, 0x02, 0x00, 0x80, 0xc0, 0x60, 0x38, 0x0f, 0xc0, 0x7c, 0x0f};
    static const char PROGMEM logo_4[] = {0x00, 0x00, 0x01, 0x03, 0x06, 0x0c, 0x18, 0x31, 0x23, 0x66, 0x44, 0x44, 0xc4, 0xc4, 0x84, 0x84,0x84, 0x84, 0xc4, 0xc4, 0x44, 0x46, 0x66, 0x23, 0x11, 0x18, 0x0c, 0x06, 0x03, 0x00, 0x00, 0x00};
    oled_set_cursor(0,0);
    oled_write_raw_P(logo_1, sizeof(logo_1));
    oled_set_cursor(0,1);
    oled_write_raw_P(logo_2, sizeof(logo_2));
    oled_set_cursor(0,2);
    oled_write_raw_P(logo_3, sizeof(logo_3));
    oled_set_cursor(0,3);
    oled_write_raw_P(logo_4, sizeof(logo_4));
}
//-----------



bool oled_task_user(void) {

    
    render_logo();
    oled_set_cursor(7,0);
    oled_write_P(PSTR("The Mad Noodle"), false);

    oled_set_cursor(7,1);
    oled_write_P(PSTR("Layer: "), false);

    oled_write_P(layer_name(get_highest_layer(layer_state)), false);

  	oled_set_cursor(7,2);
    oled_write_P(PSTR("Jump:  "), false);
    oled_write_P(layer_name(to_layer), false);

return false;
}

#endif
