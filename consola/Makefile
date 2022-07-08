# Globales
CC = gcc
OUTFILE = consola.out

### Reglas ###

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)

SHARED = $(shell find ../shared/obj/*.o)

build: makefolder makeshared $(OBJ)
	$(CC) -o $(OUTFILE) main.c $(OBJ) $(SHARED) $(LINK)

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

LINK = -lpthread -lcommons -lm -lreadline
CFLAGS = -c

debug: CFLAGS := $(CFLAGS) -g
debug: makeshared_debug build

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi

makeshared:
	make -C ../shared

makeshared_debug:
	make debug -C ../shared

# Clean

clean:
	rm -rf obj
	rm -rf *.out
	rm -rf *.log
	make clean -C ../shared

# Ejecutar terminales

instruccion:
	gnome-terminal -- bash -c './consola.out instrucciones.txt 250; sleep 20'
	gnome-terminal -- bash -c './consola.out inexit.txt 640; sleep 20' 

base:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=10000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=4/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=100000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=4096/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=64/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=2000/' ../memoria/memoria.config
	
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/BASE_1.txt 200; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/BASE_2.txt 300; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/BASE_2.txt 300; sleep 20'

planificacion:
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_2.txt 456; sleep 20'

suspension:
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_1.txt 500; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_2.txt 342; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_3.txt 344; sleep 20'
