obj-m += rootkit.o

all:
	gcc -shared -o rootkit.so -fPIC rootkit.c -ldl

arko:
	gcc -shared -o rootkitv2.so -fPIC rootkitv2.c -ldl


clean:
	rm -f rootkit.so

