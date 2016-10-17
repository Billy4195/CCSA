default: workload cpulimit

cpulimit: 
	cd cpulimit-master && $(MAKE)
	cp cpulimit-master/src/cpulimit ./

workload: workload.c
	gcc workload.c -o workload -lm
run: workload
	bash workload.sh cpu 40 mem 30 io 40 t 30
long: workload
	./workload cpu 20 mem 20 io 10 t 10
