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
#define MIN(a,b) (((a)<(b))?(a):(b))

double hz;
double limit=-1.0;

typedef struct {
        unsigned long cpu_time;
        struct timeval last_update;
}proc_t;


void get_Hz();
unsigned long get_total_mem();
unsigned long get_max_io();
int do_cpu_run();
int do_mem_run(int,unsigned long);
int do_io_run(int);
int limit_cpu(int ,int ,proc_t* ,proc_t* );
int check_mem_worker(int);
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
        unsigned long total_mem;
        proc_t proc_cpu1,proc_cpu2;
        cpu_status = mem_status = io_status = -1;
        get_Hz();
        total_mem = get_total_mem();
//        get_max_io();
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
                        break;
                  case -1:
                        printf("fork process error while adding cpu load\n");
                        break;
                  default:
                        update_proc(pid,&proc_cpu1);
                        proc_list[CPU_i] = pid;
                        cpu_status = 1;
                        printf("CPU worker %d forked\n",pid);
                        ++children;
                        usleep(5000);
                }
        }
        if( pid != 0 &&  mem_load > 0 ){
                switch(pid = fork()){
                  case 0:
                        if(run_time != -1)
                                alarm(run_time);
                        do_mem_run(mem_load,total_mem);
                        break;
                  case -1:
                        printf("fork process error while adding memory load\n");
                        break;
                  default:
                        proc_list[MEM_i] = pid;
                        mem_status = 1;
                        printf("MEMORY worker %d forked\n",pid);
                        ++children;
                }
        }
        if( pid != 0 && io_load > 0 ){
                switch(pid = fork()){
                  case 0:
                        if(run_time != -1)
                                alarm(run_time);
                        do_io_run(io_load);
                        break;
                  case -1:
                        printf("fork process error while adding io load\n");
                        break;
                  default:
                        proc_list[IO_i] = pid;
                        io_status = 1;
                        printf("IO worker %d fored\n",pid);
                        ++children;
                }
        }
        while( children ){
                if(cpu_status == 1)
                        cpu_status = limit_cpu(proc_list[CPU_i],cpu_load,&proc_cpu1,&proc_cpu2);
                        if(!cpu_status)
                                children--;
                if(mem_status == 1){
                        mem_status = check_mem_worker(proc_list[MEM_i]);
                        if(!mem_status)
                                children--;
                }
                if(io_status == 1){

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

unsigned long get_total_mem(){
        FILE* fp;
        unsigned long mem;
        char name[50],unit[5];


        fp = fopen("/proc/meminfo","r");
        fscanf(fp,"%s %lu %s",name,&mem,unit);

        printf("total mem~~~~~~~~~~~~~~~ %lu\n",mem);
        return mem;
}

unsigned long get_max_io(){
        char name[]="./test.XXXXXX";
        int fd,i;
        FILE *fp;
        unsigned long long bytes= 1024 * 1024 * 1024;
        int chunck = 1024 * 1024;
        char buf[1024*1024*1024];
        char dev_name[32];
        unsigned int major,minor,wr_ticks,ios_pgr,tot_ticks,rq_ticks;
        unsigned long rd_ios,rd_merges,rd_ticks, wr_ios;        
        unsigned long wr_merges, rd_sec, wr_sec;
        unsigned long pre_rd_sec,pre_wr_sec,cur_rd_sec,cur_wr_sec;
        unsigned long pre_rd_tick,pre_wr_tick,cur_rd_tick,cur_wr_tick;
        if( (fd = mkstemp(name)) == -1 ){
                printf("mkstemp %s error ",name);
        }
        if( unlink(name) == -1 ){
                printf("unlink %s error ",name);
        }
        for(i=0;i < bytes;i++){
                buf[i] = i * 191 ;
        }
        fp = fopen("/proc/diskstats","r");
        while(fscanf(fp,"%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
              &major, &minor, dev_name,
              &rd_ios, &rd_merges, &rd_sec, &rd_ticks,
              &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks) != EOF){

                if(strstr(dev_name,"sd")){
                        printf("%lu %lu %lu %u\n",rd_sec,wr_sec,rd_ticks,wr_ticks);
                        pre_rd_sec = rd_sec;
                        pre_wr_sec = wr_sec;
                        pre_rd_tick = rd_ticks;
                        pre_wr_tick = wr_ticks;
                        break;
                }
        }      
        fclose(fp);
                if(write(fd,buf,1024*1024*1024) == -1){
                        printf("write file error\n");
                }
        fp = fopen("/proc/diskstats","r");
        while(fscanf(fp,"%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
              &major, &minor, dev_name,
              &rd_ios, &rd_merges, &rd_sec, &rd_ticks,
              &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks) != EOF){

                if(strstr(dev_name,"sd")){
                        printf("%lu %lu %lu %u\n",rd_sec,wr_sec,rd_ticks,wr_ticks);
                        cur_rd_sec = rd_sec;
                        cur_wr_sec = wr_sec;
                        cur_rd_tick = rd_ticks;
                        cur_wr_tick = wr_ticks;
                        break;
                }
        }      
        fclose(fp);
        close(fd);
        printf("%lu %lu %lu\n", cur_rd_sec - pre_rd_sec,cur_wr_sec - pre_wr_sec,cur_rd_tick - pre_rd_tick);
        
        printf("read speed %lf\n",(double)( cur_rd_sec - pre_rd_sec )  / ( cur_rd_tick - pre_rd_tick ) );
        printf("write speed %lf\n",(double)( cur_wr_sec - pre_wr_sec ) / ( cur_wr_tick - pre_wr_tick ) );

        return 0;


}

int do_cpu_run(){
        while(1){
                sqrt(rand());
        }

        return 0;
}
int do_mem_run(int mem_load,unsigned long total_mem){
        unsigned long long bytes = total_mem * 1024; 
        char* ptr;
        int i;
        bytes *= mem_load/100.0;
//        return 0;
        ptr = (char*)malloc(bytes* sizeof(char));
        while(1){         
                for(i=0;i<bytes;i++){
                        ptr[i] = 'a';
                }
                usleep(100);
        }
        return 0;
}
int do_io_run(int io_load){
        char name[]="./workload.XXXXXX";
        int fd,i;
        char buf[1024*1024*512];
        for(i=0;i<1024*1024*512;i++){
                buf[i] = i*191;
        }
        printf("enter io\n");
        while(1){
                if( (fd = mkstemp(name)) == -1){
                        printf("mkstemp %s error\n",name);
                        break;
                }
                if( unlink(name) == -1 ){
                        printf("unlink %s error\n",name);
                        break;
                }
                write(fd,buf,1024*1024*512);
                close(fd);
        }
        return 0;

}
int limit_cpu(int pid,int cpu_load,proc_t* proc1,proc_t* proc2){
        double cpu_usage;
        unsigned long dt;
        struct timespec sleep_time;
        struct timespec work_time;
        int status=0;
        static int c=0;

        memset(&sleep_time, 0, sizeof(struct timespec));
        memset(&work_time, 0, sizeof(struct timespec));

        waitpid(pid,&status,WNOHANG);
        if(WIFSIGNALED(status)){
                printf("signaled\n");
                return 0;
        }
        update_proc(pid,proc2);
        dt = timediff(&proc2->last_update,&proc1->last_update)/1000; //ms
         
        //printf("dt ---- %lu\n",dt);

        cpu_usage = 1.0 * (proc2->cpu_time-proc1->cpu_time)/dt;
        printf("CPU Usage: %lf\n",cpu_usage);
        if(cpu_usage <= 0.1)
                return 1;
        if( limit  < 0 ){
                limit = cpu_load/100.0;
        }else{
                limit = MIN(limit / cpu_usage * (cpu_load/100.0),1);
        }

        work_time.tv_nsec = TIME_SLOT * 1000 * limit;
        sleep_time.tv_nsec = TIME_SLOT * 1000 - work_time.tv_nsec;
        //printf("sleep time : %lu\n",sleep_time);
//        if(c % 50 == 0)
                printf("work : %6ld /// sleep : %6ld /// limit : %lf\n",work_time.tv_nsec/1000,sleep_time.tv_nsec/1000,limit);
        if(c > 0 && c % 5000 == 0){
                printf("\n");
                return 0;
}
        if(kill(pid,SIGCONT)!= 0){
                printf("SIGSTOP failed\n");
        } 
//        printf("limit %lf\n",limit);
        nanosleep(work_time,NULL);
        if (kill(pid,SIGSTOP) != 0) {
                printf("SIGCONT failed\n");
        }
        nanosleep(sleep_time,NULL);

        swap_proc(&proc1,&proc2);
        c++;
        return 1;
}
int check_mem_worker(int pid){
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
