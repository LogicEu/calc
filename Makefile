# calc Makefile

CC=gcc
SRC=calc.c
TST=test.c
STD=-std=c89
OPT=-O2
WFLAGS=-Wall -Wextra
EXE=calc

SCRIPT=build.sh

CFLAGS= $(STD) $(OPT) $(WFLAGS) $(INC)

.PHONY: all test clean install uninstall

$(EXE): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

all: $(EXE) test

test: $(TST)
	$(CC) -o $@ $^ $(CFLAGS)

clean: $(SCRIPT)
	$(RM) $(EXE)
	$(RM) test

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@./$^ $@

