#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  //这里调用之前的push
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  //这里调用之前的pop
  rtl_pop(&t2);
  operand_write(id_dest, &t2);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    rtl_lr_w(&t0, R_AX);
    rtl_sext(&t0, &t0, 2);
    rtl_sari(&t0,&t0,31);
    rtl_sr_w(R_DX,&t0);
  }
  else {
    //TODO();
    rtl_sari(&cpu.edx, &cpu.eax, 31);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

// make_EHelper(cwtl) {
//   if (decoding.is_operand_size_16) {
//     //TODO();
//     rtl_lr_b(&t0, R_AL);
//     rtl_sext(&t0, &t0, 1);
//     reg_w(R_AX) = t0;
//   }
//   else {
//     //TODO();
//     rtl_lr_w(&t0, R_AX);
//     rtl_sext(&t0, &t0, 2);
//     reg_l(R_EAX) = t0;
//   }
//   print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
// }
//error!!!!!!!!!!!!!!!!!!!!!!!!!!!
make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
    rtl_lr_b(&t0,R_AL);
    rtl_sext(&t0,&t0,1);
    rtl_sr_w(R_AX,&t0);
  }
  else {
    //TODO();
    rtl_lr_w(&t0,R_AX);
    rtl_sext(&t0,&t0,2);
    rtl_sr_w(R_EAX,&t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}
// make_EHelper(cwtl) {
//   if (decoding.is_operand_size_16) {
//     //TODO();
//     rtl_sext(&t0,&cpu.eax,1);
//     cpu.eax = (cpu.eax & 0xffff0000) | (t0 & 0xffff);
//   }
//   else {
//     //TODO();
//     rtl_sext(&t0,&cpu.eax,2);
//     cpu.eax = t0;
//   }

//   print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
// }
// make_EHelper(cwtl) {
//   if (decoding.is_operand_size_16) {
//   //TODO();
//   rtl_lr(&t0,R_AL,1);
//   t0=(int16_t)(int8_t)(uint8_t)t0;
//   rtl_sr(R_AX,2,&t0);
//   } else{
//   //TODO();
//   rtl_lr(&t0,R_AX,2);
//   t0=(int32_t)(int16_t)(uint16_t)t0;
//   rtl_sr(R_EAX,4,&t0);
//   }
//   print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
// }

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
