#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O: {
      rtl_get_OF(&t0);
      if (t0 == 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NO: {
      rtl_get_OF(&t0);
      if (t0 == 0){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_B: {
      rtl_get_CF(&t0);
      if (t0 == 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NB: {
      rtl_get_CF(&t0);
      if (t0 == 0){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_E: {
      rtl_get_ZF(&t0);
      if (t0 == 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NE: {
      rtl_get_ZF(&t0);
      if (t0 == 0){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_BE: {
      rtl_get_CF(&t0);
      rtl_get_ZF(&t1);
      if (t0 == 1 || t1 == 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NBE: {
      rtl_get_CF(&t0);
      rtl_get_ZF(&t1);
      if (t0 == 0 && t1 == 0){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_S: {
      rtl_get_SF(&t0);
      if (t0 == 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NS: {
      rtl_get_SF(&t0);
      if (t0 == 0){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_L: {
      rtl_get_OF(&t0);
      rtl_get_SF(&t1);
      if (t0 != t1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NL: {
      rtl_get_OF(&t0);
      rtl_get_SF(&t1);
      if (t0 == t1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_LE: {
      rtl_get_OF(&t0);
      rtl_get_SF(&t1);
      rtl_get_ZF(&t2);
      if (t0 != t1 || t2 == 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    case CC_NLE: {
      rtl_get_OF(&t0);
      rtl_get_SF(&t1);
      rtl_get_ZF(&t2);
      if (t0 == t1 && t2 != 1){
        *dest = 1;
      } else {
        *dest = 0;
      }
      break;
    }
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
    case CC_NP: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
