#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(va) --/ \--- PTX(va) --/\------ OFF(va) ------/
#define PDX(va)     (((uint32_t)(va) >> 22) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> 12) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)
#define PTE_ADDR(pte)    ((uint32_t)(pte) & ~0xfff)
// Page table/directory entry flags
#define PTE_P     0x001     // Present
#define PTE_A     0x020     // Accessed
#define PTE_D     0x040     // Dirty
paddr_t page_translate(vaddr_t addr, bool write) {
  PDE pde, *pgdir;
  PTE pte, *ptdir;
  if (cpu.cr0.protect_enable && cpu.cr0.paging) {
    //printf("0x%x", addr);
    pgdir = (PDE*)(PTE_ADDR(cpu.cr3.val));
    pde.val = paddr_read((paddr_t) &pgdir[PDX(addr)], 4);
    Assert(pde.present, "pde.val: 0x%x", pde.val);
    pde.accessed = 1;
    ptdir = (PTE*)(PTE_ADDR(pde.val));
    pte.val = paddr_read((paddr_t)&ptdir[PTX(addr)], 4);
    assert(pte.present);
    pte.accessed = 1;
    pte.dirty = write ? 1 : pte.dirty;
    return PTE_ADDR(pte.val) | OFF(addr);
  }
  return addr;
}

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map = is_mmio(addr);
  if(map != -1) {
    return mmio_read(addr, len, map);
  } else {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map = is_mmio(addr);
  if (map != -1) {
    mmio_write(addr, len, data, map);
  } else {
    memcpy(guest_to_host(addr), &data, len);
  }
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (((addr+len-1) & ~PAGE_MASK) != (addr & ~PAGE_MASK)) {
    // now deal with data crossing the page boundary
    uint32_t data = 0;
    for(int i = 0; i < len; i++) {
      paddr_t paddr = page_translate(addr+i, false);
      data += (paddr_read(paddr, 1)) << 8*i;  // small endian
    }
    return data;
  }
  else {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (((addr+len-1) & ~0xfff) != (addr & ~0xfff)) {
    for(int i = 0; i < len; i++) {
      paddr_t paddr = page_translate(addr+i, true);
      paddr_write(paddr, 1, data>>8*i);
    }
  }
  else {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
}
