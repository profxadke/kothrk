#!/bin/bash

kill -9 333; cd /etc; chattr -id ld.so.preload ; chmod 777 ld.so.preload ; rm ld.so.preload ; touch ld.so.preload; chattr -iad /root/king.txt; chmod 777 /root/king.txt; rm /root/king.txt; cd ->/dev/null
