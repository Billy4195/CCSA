#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define TIME_SLOT 100000
#define CPU_i 0
#define MEM_i 1
#define IO_i 2

double hz;

typedef struct {
        unsigned long cpu_time;
        struct timeval last_update;
}proc_t;

void get_Hz();
int do_cpu_run();
int limit_cpu(int ,int ,proc_t* ,proc_t* );
void update_proc(int ,proc_t*);
unsigned long get_cputime(int );
inline unsigned long timediff(const struct timeval*,const struct timeval*);
void swap_proc(proc_t** proc1,proc_t** proc2);

int main(int argc, char** argv){
        int i,pid,children=0;
        int proc_list[3];
        int cpu_status,mem_status,io_status;
        int  cpu_load,mem_load,io_load;
        long run_time=-1;
        proc_t proc_cpu1,proc_cpu2;
        cpu_status = mem_status = io_status = -1;
        get_Hz();
        for(i=1; i < argc; i++){
                char* arg = argv[i];
                if(strcmp (arg, "cpu") == 0){
                        cpu_load = atoi(argv[++i]);
                }else if(strcmp (arg, "mem") == 0 ){
                        mem_load = atoi(argv[++i]);
                }else if(strcmp (arg, "io") == 0){
                        io_load = atoi(argv[++i]);
                }else if(strcmp (arg, "t") == 0){
                        run_time = atoi(argv[++i]);
                }else{
                        printf("Unrecongnize argument %s",argv[i]);
                }
        }
        printf("CPU load %d\n",cpu_load);
        printf("Memory load %d\n",mem_load);
        printf("IO load %d\n",io_load);
        printf("Run Time %ld\n",run_time);
        if( cpu_load > 0 ){
                switch(pid = fork()){
                  case 0:
                        if(run_time != -1)
                                alarm(run_time);
                        do_cpu_run(cpu_load);
                  case -1:
                        printf("fork process error while adding cpu load\n");
                  default:
                        update_proc(pid,&proc_cpu1);
                        proc_list[CPU_i] = pid;
                        cpu_status = 1;
                        printf("CPU worker %d forked\n",pid);
                        ++children;
                }
        }
        if( mem_load > 0 ){
        
        }
        if( io_load > 0 ){
        
        }
        while( children ){
                if(cpu_status == 1)
                        cpu_status = limit_cpu(proc_list[CPU_i],cpu_load,&proc_cpu1,&proc_cpu2);
                        if(!cpu_status)
                                children--;
                if(mem_status){

                }
                        

        }
//        while( children ){
//                int status, ret;
//                if( (pid = wait(&status)) > 0 ){
//                        if(WIFEXITED(status)){
//                                if((ret = WEXITSTATUS(status)) ==0){
//                                        printf("Worker %d exit normally\n",pid);
//                                }else{
//                                        printf("Worker %d exit return error %d\n",pid,ret);
//                                }
//                        }else if (WIFSIGNALED(status)){
//                                if((ret = WTERMSIG(status)) == SIGALRM){
//                                        printf("Worker %d signalled normally\n",pid);
//                                }else{
//                                        printf("Worker %d got signal %d\n",pid,ret);
//                                }
//                        }
//                        --children;
//                }else{
//                        printf("ERROR wait for worker\n");
//                }
//        }
        return 0;
}
void get_Hz(){
        hz = (double)sysconf(_SC_CLK_TCK);
}

int do_cpu_run(){
        while(1){
                sqrt(rand());
        }

        return 0;
}
int limit_cpu(int pid,int cpu_load,proc_t* proc1,proc_t* proc2){
        static double limit=-1.0;
        double cpu_usage;
        unsigned long dt;
        unsigned long sleep_time;

        update_proc(pid,proc2);
        dt = timediff(&proc2->last_update,&proc1->last_update)/1000; //ms
         
        printf("dt ---- %lu\n",dt);

        cpu_usage = 1.0 * (proc2->cpu_time-proc1->cpu_time)/dt;
        printf("CPU Usage: %lf\n",cpu_usage);
        swap_proc(&proc1,&proc2);


        return 0;
}

void update_proc(int pid,proc_t * proc){
        proc->cpu_time = get_cputime(pid);
        gettimeofday(&proc->last_update,NULL);
}

unsigned long get_cputime(int pid){
        char statfile[50];
        char buf[1024];
        FILE* fp;
        int i;
        unsigned long cpu_time=0;
        sprintf(statfile,"/proc/%d/stat",pid);
        fp = fopen(statfile,"r");
        if(fgets(buf,sizeof(buf),fp) == NULL){
                printf("Read %s Failed",statfile);
                exit(1);
        }
        fclose(fp);
        char *token = strtok(buf," ");
        for(i=0;i<13;i++){
                token = strtok(NULL," ");
        }
        cpu_time = atoi(token) * 1000 / hz;
        token = strtok(NULL," ");
        cpu_time += atoi(token) * 1000 / hz;
        return cpu_time;
}
unsigned long timediff(const struct timeval* t1,const struct timeval* t2){
        return (t1->tv_sec - t2->tv_sec) * 1000000 + (t1->tv_usec - t2->tv_usec);        
}

void swap_proc(proc_t** proc1,proc_t** proc2){
        proc_t* tmp;
        tmp = *proc1;
        *proc1 = *proc2;
        *proc2 = tmp;
}
