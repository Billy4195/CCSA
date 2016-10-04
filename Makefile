source=pthread_setaffinity_np_test.c pro_1.c pro_2.c pro_3.c
executable=pthread_setaffinity_np_test pro_1 pro_2 pro_3


all: pthread_setaffinity_np_test pro_1 pro_2 pro_3

pthread_setaffinity_np_test: pthread_setaffinity_np_test.c
	gcc $< -o $@ -lm -lpthread
pro_1: pro_1.c
	gcc $< -o $@ -lm -lpthread
pro_2: pro_2.c
	gcc $< -o $@ -lm -lpthread
pro_3: pro_3.c
	gcc $< -o $@ -lm -lpthread
clean: 
	rm pthread_setaffinity_np_test pro_1 pro_2 pro_3 
