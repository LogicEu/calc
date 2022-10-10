# calc Makefile

CC=gcc
SRC=main.c
STD=-std=c89
WFLAGS=-Wall -Wextra
EXE=calc

SCRIPT=build.sh

CFLAGS= $(STD) $(WFLAGS) $(INC)

$(EXE): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

clean: $(SCRIPT)
	./$^ $@

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@./$^ $@
