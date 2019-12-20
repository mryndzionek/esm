#ifndef INCLUDE_KEYCODE_EXTRA_H_
#define INCLUDE_KEYCODE_EXTRA_H_

#define _______ KC_TRNS
#define XXXXXXX KC_NO

#define QK_LSFT (0x0200)

#define LSFT(kc) (QK_LSFT | (kc))
#define IS_LSFT(kc) ((QK_LSFT & (kc)) == QK_LSFT)

#define KC_TILD LSFT(KC_GRV)  // ~
#define KC_TILDE KC_TILD

#define KC_EXLM LSFT(KC_1)  // !
#define KC_EXCLAIM KC_EXLM

#define KC_AT LSFT(KC_2)  // @

#define KC_HASH LSFT(KC_3)  // #

#define KC_DLR LSFT(KC_4)  // $
#define KC_DOLLAR KC_DLR

#define KC_PERC LSFT(KC_5)  // %
#define KC_PERCENT KC_PERC

#define KC_CIRC LSFT(KC_6)  // ^
#define KC_CIRCUMFLEX KC_CIRC

#define KC_AMPR LSFT(KC_7)  // &
#define KC_AMPERSAND KC_AMPR

#define KC_ASTR LSFT(KC_8)  // *
#define KC_ASTERISK KC_ASTR

#define KC_LPRN LSFT(KC_9)  // (
#define KC_LEFT_PAREN KC_LPRN

#define KC_RPRN LSFT(KC_0)  // )
#define KC_RIGHT_PAREN KC_RPRN

#define KC_UNDS LSFT(KC_MINS)  // _
#define KC_UNDERSCORE KC_UNDS

#define KC_PLUS LSFT(KC_EQL)  // +

#define KC_LCBR LSFT(KC_LBRC)  // {
#define KC_LEFT_CURLY_BRACE KC_LCBR

#define KC_RCBR LSFT(KC_RBRC)  // }
#define KC_RIGHT_CURLY_BRACE KC_RCBR

#define KC_LABK LSFT(KC_COMM)  // <
#define KC_LEFT_ANGLE_BRACKET KC_LABK

#define KC_RABK LSFT(KC_DOT)  // >
#define KC_RIGHT_ANGLE_BRACKET KC_RABK

#define KC_COLN LSFT(KC_SCLN)  // :
#define KC_COLON KC_COLN

#define KC_PIPE LSFT(KC_BSLS)  // |

#define KC_LT LSFT(KC_COMM)  // <

#define KC_GT LSFT(KC_DOT)  // >

#define KC_QUES LSFT(KC_SLSH)  // ?
#define KC_QUESTION KC_QUES

#define KC_DQT LSFT(KC_QUOT)  // "
#define KC_DOUBLE_QUOTE KC_DQT
#define KC_DQUO KC_DQT

#define KC_DELT KC_DELETE  // Del key (four letter code)

#endif /* INCLUDE_KEYCODE_EXTRA_H_ */