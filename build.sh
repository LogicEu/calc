#!/bin/bash

cc=gcc
src=calc.c
tst=test.c
name=calc
tstname=test

flags=(
    -std=c89
    -O2
    -Wall
    -Wextra
)

cmd() {
    echo "$@" && $@
}

build() {
    cmd $cc $src -o $name ${flags[*]}
}

ctest() {
    cmd $cc $tst -o $tstname ${flags[*]}
}

cleanf() {
    [ -f $1 ] && cmd rm -f $1
}

clean() {
    cleanf $name
    cleanf $tstname
    return 0 
}

install() {
    [ "$EUID" -ne 0 ] && echo "$0: use with sudo to install" && exit
    build && cmd mv $name /usr/local/bin
    echo "$0: installed $name in /usr/local/bin"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "$0: use with sudo to uninstall" && exit
    cleanf /usr/local/bin/$name
    echo "$0: uninstalled $name from /usr/local/bin"
    return 0
}

case "$1" in
    "build")
        build;;
    "test")
        ctest;;
    "all")
        build && ctest;;
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        echo "$0 usage:"
        echo -e "[ build ]\t: compile calc.c as 'calc'"
        echo -e "[ test ]\t: compile test.c as 'test'"
        echo -e "[ all ]\t: compile both executables"
        echo -e "[ clean ]\t: remove local builds"
        echo -e "[ install ]\t: install executable in /usr/local/bin"
        echo -e "[ uninstall ]\t: uninstall executable from /usr/local/bin";;
esac

