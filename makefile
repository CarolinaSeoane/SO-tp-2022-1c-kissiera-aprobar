all:
	make -C cpu
	make -C kernel
	make -C consola
	make -C memoria
	./ejecutar.sh

clean:
	make clean -C kernel
	make clean -C consola
	make clean -C memoria
	make clean -C cpu