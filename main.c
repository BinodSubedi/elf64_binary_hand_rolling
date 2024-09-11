#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argC, char *args[]) {

  Elf64_Ehdr elfH = {
      .e_ident = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB,
                  EV_CURRENT, ELFOSABI_SYSV, 0, 0, 0, 0, 0, 0, 0, 0},
      .e_type = 2,
      .e_machine = EM_X86_64,
      .e_version = EV_CURRENT,
      .e_entry =
          0x40007f, // cause 120 bytes for program header and since we are
                    // putting hello!\n [7 bytes] into same R EXEC section
      // 0x7f = 127 so 120+7 = 127 where first instruction of program start
      .e_phoff = 64,
      .e_shoff = 0,
      .e_flags = 0,
      .e_ehsize = 64,
      .e_phentsize = 56,
      .e_phnum = 1,
      .e_shentsize = 64,
      .e_shnum = 0,
      .e_shstrndx = 0,

  };

  Elf64_Phdr phdr = {.p_type = PT_LOAD,
                     .p_flags = PF_X | PF_R,
                     .p_offset = 0x78,    /* Segment file offset */
                     .p_vaddr = 0x400078, /* Segment virtual address */
                     .p_paddr = 0x400078, /* Segment physical address */
                     .p_filesz = 46,      /* Segment size in file */
                     .p_memsz = 46,       /* Segment size in memory */
                     .p_align = 0x8};

  // writing program section in most compact way with hello!\n at the top

  char code[] = {0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21,
                 0x0a, // hello!\n
                 0xb8, 1,    0,    0,    0,    0xbf, 1, 0, 0,    0,    0x48,
                 0xbe, // mov rsi, 64 bit pointer
                 0x78, 0,    0x40, 0,    0,    0,    0, 0, 0xba, 0x07, 0,
                 0,    0,    0x0f, 0x05, 0xb8, 0x3c, 0, 0, 0,    0xbf, 0,
                 0,    0,    0,    0x0f, 0x05};

  FILE *f = fopen("bin", "w");
  if (!f) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  size_t s = fwrite(&elfH, 1, sizeof(elfH), f);
  if (s != sizeof(elfH)) {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }

  s = fwrite(&phdr, 1, sizeof(phdr), f);
  if (s != sizeof(phdr)) {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }

  s = fwrite(&code, 1, sizeof(code), f);

  if (s != sizeof(code)) {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }

  fclose(f);

  return 0;
}
