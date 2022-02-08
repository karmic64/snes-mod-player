ifdef COMSPEC
DOTEXE := .exe
else
DOTEXE :=
endif


CFLAGS := -s -Ofast -Wall




.PHONY: default

default: main.smc




mod-deps.txt: mods gen-mod-deps.sh
	./gen-mod-deps.sh

include mod-deps.txt



%$(DOTEXE): %.c
	$(CC) $(CFLAGS) -o "$@" "$<"


generated-data.asm: gen-data$(DOTEXE)
	./gen-data

cmods/%.cmod: mods/% convert$(DOTEXE)
	./convert "$<" "$@"



spc.spco: spc.spcasm generated-data.asm
	wla-spc700 -o "$@" "$<"

spc.spcbin: spc.spcln  spc.spco
	wlalink -b "$<" "$@"



mod-list.asm: mods gen-mod-list$(DOTEXE)
	./gen-mod-list

main.smc: main.asm spc.spcbin $(OUT_MODS) snes.inc mod-list.asm spc-snes.asm
	64tass -a -f -o "$@" "$<"
