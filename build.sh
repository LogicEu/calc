#!/bin/bash

cc=gcc
src=main.c
name=calc

flags=(
    -std=c89
    -O2
    -Wall
    -Wextra
)

comp() {
    $cc $src -o $name ${flags[*]}
    echo "$cc $src -o $name ${flags[*]}"
}

cleanf() {
    [ -f $1 ] && rm $1 && echo "deleted $1"
}

clean() {
    cleanf $name
    return 0 
}

install() {
    [ "$EUID" -ne 0 ] && echo "Use with 'sudo' to install" && exit

    comp && mv $name /usr/local/bin
    echo "Successfully installed $name in /usr/local/bin"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Use with 'sudo' to uninstall" && exit

    cleanf /usr/local/bin/$name
    echo "Successfully uninstalled $name in /usr/local/bin"
    return 0
}


case "$1" in
    "comp")
        comp;;
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        echo "Use with 'comp' to compile."
        echo "Run 'clean' to remove local builds."
        echo "Run 'install' to install in /usr/local/bin";;
esac
