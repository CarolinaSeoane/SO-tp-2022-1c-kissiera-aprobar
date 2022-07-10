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
	make base_config -C consola
	make all
	make base_instrucciones -C consola	

planificacion_fifo:
	make planificacion_fifo_config -C consola
	make all
	make planificacion_fifo_instrucciones -C consola	

planificacion_srt:
	make planificacion_srt_config -C consola
	make all
	make planificacion_srt_instrucciones -C consola	

suspension_fifo:
	make suspension_fifo_config -C consola
	make all
	make suspension_fifo_instrucciones -C consola	

suspension_srt:
	make suspension_srt_config -C consola
	make all
	make suspension_srt_instrucciones -C consola	

memoria_clock:
	make memoria_clock_config -C consola
	make all
	make memoria_clock_instrucciones -C consola	

memoria_clock_m:
	make memoria_clock_m_config -C consola
	make all
	make memoria_clock_m_instrucciones -C consola	

tlb_fifo:
	make tlb_fifo_config -C consola
	make all
	make tlb_fifo_instrucciones -C consola	

tlb_lru:
	make tlb_lru_config -C consola
	make all
	make tlb_fifo_instrucciones -C consola	

integral:
	make integral_config -C consola
	make all
	make integral_instrucciones -C consola	