cmd_/home/ingdex/task3/hello.ko := ld -r -m elf_x86_64 -z max-page-size=0x200000 -T ./scripts/module-common.lds --build-id  -o /home/ingdex/task3/hello.ko /home/ingdex/task3/hello.o /home/ingdex/task3/hello.mod.o ;  true