all:
	make -C memoria
	make -C cpu
	make -C kernel
	make -C consola
	./ejecutar.sh

clean:
	make clean -C kernel
	make clean -C consola
	make clean -C memoria
	make clean -C cpu

ins:
	make instruccion -C consola

base:
	make base -C consola

planificacion_fifo:
	make planificacion_fifo -C consola

planificacion_srt:
	make planificacion_srt -C consola

suspension_fifo:
	make suspension_fifo -C consola

suspension_srt:
	make suspension_srt -C consola

memoria_clock:
	make memoria_clock -C consola

memoria_clock_m:
	make memoria_clock_m -C consola

tlb_fifo:
	make tlb_fifo -C consola

tlb_lru:
	make tlb_lru -C consola

integral:
	make integral -C consola