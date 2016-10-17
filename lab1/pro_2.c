/* Short test program to test the pthread_setaffinity_np
* (which sets the affinity of threads to processors).
* Compile: gcc pthread_setaffinity_np_test.c
*              -o pthread_setaffinity_np_test -lm -lpthread
* Usage: ./pthread_setaffinity_test
*
* Open a "top"-window at the same time and see all the work
* being done on CPU 0 first and after a short wait on CPU 1.
* Repeat with different numbers to make sure, it is not a
* coincidence.
*/
#define _GNU_SOURCE
 
#include <stdio.h>
#include <math.h>
#include <pthread.h>

cpu_set_t cpuset,cpuget;
pthread_mutex_t mutex;
double sum=0;

double waste_time(long n,long x)
{
    pthread_t child_thread;
    double res = 0;
    long i = x+1;

    while (i <= n * 200000000) {
        res += sqrt(i);
        i+= 2;
    }
    pthread_mutex_lock(&mutex);

    sum += res;
    
    pthread_mutex_unlock(&mutex);
}
 
void *thread_func(void *param)
{   
    pthread_mutex_lock(&mutex); 
    /* bind process to processor 0 */
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) !=0) {
        perror("pthread_setaffinity_np");
    }  
    CPU_XOR(&cpuset, &cpuset, &cpuget);
    pthread_mutex_unlock(&mutex);

//    printf("Core 0 is running!\n");
    /* waste some time so the work is visible with "top" */
	  waste_time(5 ,(long) param );
    pthread_exit(NULL);

}
 
int main(int argc, char *argv[])
{ 
    long i;
    CPU_ZERO(&cpuget);
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset); /* cpu 0 is in cpuset now */
    CPU_SET(0, &cpuget);
    CPU_SET(1, &cpuget); 
    pthread_t my_thread[2];
    time_t startwtime, endwtime;
    startwtime = time (NULL); 
    for(i=0;i<2;i++){
        if (pthread_create(&my_thread[i], NULL, thread_func,(void*) i) != 0) {
            perror("pthread_create");
        }
    }
    for(i=0;i<2;i++){
        pthread_join(my_thread[i], NULL);
    }
    printf("result: %f\n", sum);

    endwtime = time (NULL);
    printf ("wall clock time = %d\n",(int) (endwtime - startwtime));
    return 0;
}


