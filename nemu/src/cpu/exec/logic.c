#include "cpu/exec.h"

make_EHelper(test) {
  // TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtl_or(&t0, &id_dest->val, &id_src->val); //TODO-ADD xor->or
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0, &id_dest->val, &id_src->val); // TODO-ADD shli->shl
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  // TODO();
  rtl_li(&t0, id_dest->val);
  rtl_not(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(not);
}

make_EHelper(rol) {
  // rtl_li(&t0, id_src->val);
  // switch(id_dest->width) {
  //   case 1:
  //     rtl_li(&t1, 0x80);
  //     break;
  //   case 2:
  //     rtl_li(&t1, 0x8000);
  //     break;
  //   case 4:
  //     rtl_li(&t1, 0x80000000);
  //     break;
  //   default:
  //     panic("rol: no support width");
  // }

  // while (t0 > 0) {
  //   rtl_li(&t2, (id_dest->val & t1) ? 1 : 0);
  //   rtl_shli(&id_dest->val, &id_dest->val, 1);
  //   rtl_add(&id_dest->val, &id_dest->val, &t2);
  //   rtl_subi(&t0, &t0, 1);
  // }
  // operand_write(id_dest, &id_dest->val);
  // if (id_src->val == 1) {
  //   rtl_get_CF(&t0);
  //   rtl_li(&t1, ((1 << (id_dest->width * 8 - 1)) & id_dest->val) ? 1 : 0);
  //   if (t0 == t1) {
  //     rtl_set_OF(&tzero);
  //   } 
  //   else {
  //     t0 = 1;
  //     rtl_set_OF(&t0);
  //   }
  // }
  t0 = id_dest->width * 8 - id_src->val;
  rtl_shr(&t1, &id_dest->val, &t0);
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  rtl_or(&t0, &t1, &t2);
  operand_write(id_dest, &t0);
  print_asm_template2(rol);
}