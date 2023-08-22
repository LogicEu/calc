# calc Makefile

CC=gcc
SRC=main.c
STD=-std=c89
OPT=-O2
WFLAGS=-Wall -Wextra
EXE=calc

SCRIPT=build.sh

CFLAGS= $(STD) $(OPT) $(WFLAGS) $(INC)

$(EXE): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

clean: $(SCRIPT)
	./$^ $@

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@./$^ $@
