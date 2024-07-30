#!/bin/bash

kill -9 333; chattr -id /etc/ld.so.preload ; chmod 777 /etc/ld.so.preload ; rm /etc/ld.so.preload ; touch /etc/ld.so.preload; chattr -iad /root/king.txt; chmod 777 /root/king.txt; rm /root/king.txt
