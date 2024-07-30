obj-m += rootkit.o

all:
	gcc -shared -o rootkit.so -fPIC rootkit.c -ldl

clean:
	rm -f rootkit.so

