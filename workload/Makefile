default: workload cpulimit

cpulimit: 
	cd cpulimit-master && $(MAKE)
	cp cpulimit-master/src/cpulimit ./

workload: workload.c
	gcc workload.c -o workload -lm
run: workload
	./workload cpu 20 mem 20 io 10 t 5
long: workload
	./workload cpu 20 mem 20 io 10 t 10
