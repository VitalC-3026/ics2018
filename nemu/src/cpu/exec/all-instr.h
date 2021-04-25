#include "cpu/exec.h"

// data-mov.c
make_EHelper(mov);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(lea);

// prefix.c
make_EHelper(real);
make_EHelper(operand_size);

// special.c
make_EHelper(nop);
make_EHelper(inv);
make_EHelper(nemu_trap);

// system.c
make_EHelper(lidt);

// control.c
make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(ret);

// arith.c
make_EHelper(add);
make_EHelper(adc);
make_EHelper(cmp);
make_EHelper(sub);
make_EHelper(sbb);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);

// logic.c
make_EHelper(test);
make_EHelper(and);
make_EHelper(or);
make_EHelper(xor);

// cc.c
make_EHelper(setcc);


