#include "cpu/exec.h"

make_EHelper(mov);
make_EHelper(movzx);
make_EHelper(movsx);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(lidt);
make_EHelper(nop);
make_EHelper(lea);

make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(ret);

make_EHelper(push);
make_EHelper(pop);

make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(setcc);

make_EHelper(add);
make_EHelper(or);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(and);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(cmp);

make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);

make_EHelper(cltd);
make_EHelper(cwtl);
