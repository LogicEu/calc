# calc

Extremely simple calculator for terminal use. 

It is able to parse, evaluate and solve C-like constant integer expressions.
All the operators that are legal in C constant expressions are supported by
this calculator, having the same precedences and syntax as in C. All operations
and values are seen as signed long 8 byte integers. It provides the same syntax
for constant integer value representations as in C, supporting decimal, hexadecimal
and octal representations.

Every expresion provided will be parsed and evaluated. It's recommended
to put quotes around your expression so your shell won't escape or do operations 
on the characters needed by some of the operators like *, &, |, (), etc.
Alphabetic letters and assignment operators in expressions are not supported.

The project has under 300 lines of code and is written completely in C89, using
only two functions from the C standard library, printf and strtol. It uses
reverse polish notationd and the shunting yard algorithm to parse expressions
using stacks and no recursion.

## usage

| Option | Description |
| --- | --- |
| -d | print output in decimal base (default) |
| -o | print output in octal base with a leading zero |
| -x | print output in hexadecimal base using lower case format |
| -X | print output in hexadecimal base using upper case format |
| -h | print this help message |

Here are some valid example expressions:

```shell
calc '1 + (2 - 3) * 4 / 5'
```
```shell
calc '-(0xFF - (255 << 1)) >= !(61278 % 256) * ((16 << 2) & 0x1) == ~01'
```
```shell
calc '1 >= 2' '3 + 4' '!(5 % 3)'
```
## build

There is a Makefile and a build.sh shell script to compile the proyect,
nevertheless, calc is very simple and depends on only two functions from
the C standard library (printf and strtol), making compilation extremely
simple and easy to support in a number of platforms. The complete source 
code is under 300 lines of code and resides completely within the calc.c
file. Here are three easy ways to compile calc:

* Make
```shell
make 
```
* Bash
```shell
./build.sh build
```
* GCC
```shell
gcc calc.c -o calc -std=c89 -O2 -Wall -Wextra
```
## install

The build.sh shell script provides an easy installation, putting the compiled
executable in your /usr/local/bin directory so you can just type 'calc' in your
terminal and use it anywhere.

```shell
./build.sh install
```

Note: this script command should be invoked with sudo.

