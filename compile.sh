#!/bin/bash

cd ..
if [[ -f rk.tar.gz ]]; then
	rm rk.tar.gz
fi
if [[ -d rk ]]; then
	rm -rf rk
fi
mkdir rk
cp ./rootkit/* rk
tar cvzf ./rk.tar.gz ./rk
cp ./rk.tar.gz ./rootkit
cd ->/dev/null
