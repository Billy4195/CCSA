#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#define CPU_I 0
#define MEM_I 1
#define IO_I 2

int cpu_run();
int mem_run(double);
int io_run();
char cpu_handler();
char mem_handler(int);
char io_handler();
unsigned long get_total_mem();

int main(int argc,char **argv){
        int i;
        double cpu_load,mem_load,io_load;
        int timeout=0;
        char do_cpu,do_mem,do_io;
        char cpu_bl,mem_bl,io_bl;
        int proc_arr[3];
        
        do_cpu = do_mem = do_io = 0;
        cpu_bl = mem_bl = io_bl = 0;

        //parse argument
        for(i=1;i<argc;i++){
                if(strcmp(argv[i],"cpu")  == 0){
                        cpu_load = atoi(argv[++i]) / 100.0;
                        do_cpu = 1;
                }else if(strcmp(argv[i],"mem") == 0){
                        mem_load = atoi(argv[++i]) / 100.0;
                        do_mem = 1;
                }else if(strcmp(argv[i],"io") == 0){
                        io_load = atoi(argv[++i]) / 100.0;
                        do_io = 1;
                }else if(strcmp(argv[i],"t") == 0){
                        timeout = atoi(argv[++i]);
                }else{
                        printf("Unrecongnize argument --- %s %s\n",argv[i],argv[++i]);
                        exit(1);
                }
        }

        printf("CPU load %.2lf\n",cpu_load);
        printf("Memory load %.2lf\n",mem_load);
        printf("IO load %.2lf\n",io_load);
        printf("Timeout %d\n",timeout);

        int pid;
        if(do_cpu){
                switch(pid = fork()){
                  case 0:
                        if(timeout)
                                alarm(timeout);
                        exit(cpu_run());
                        break;
                  case -1:
                        printf("error while fork cpu worker\n");
                        break;
                  default:
                        cpu_bl = 1;
                        proc_arr[CPU_I] = pid;
                        printf("CPU worker %d forked\n",pid);
                }
        }
        if(do_mem){
                switch(pid = fork()){
                  case 0:
                        if(timeout)
                                alarm(timeout);
                        exit(mem_run(mem_load));
                        break;
                  case -1:
                        printf("error while fork memory worker\n");
                        break;
                  default:
                        mem_bl = 1;
                        proc_arr[MEM_I] = pid;
                        printf("Memory worker %d forked\n",pid);
                }
        }
//        if(do_io){
//                switch(pid = fork()){
//                  case 0:
//                        if(timeout)
//                               alarm(timeout);
//                        exit(io_run());
//                        break;
//                  case -1:
//                        printf("error while fork io worker\n");
//                        break;
//                  default:
//                        io_bl = 1;
//                        proc_arr[IO_I] = pid;
//                        printf("IO worker %d forked\n",pid);
//                }
//        }
        

        while(cpu_bl + mem_bl + io_bl){
                cpu_bl = cpu_handler();
                mem_bl = mem_handler(proc_arr[MEM_I]);
                io_bl = io_handler();
        }

        return 0;

}

int cpu_run(){
        while(1){
                sqrt(rand());
        }

        return 0;
}

int mem_run(double load){
        unsigned long total_mem;
        unsigned long long use_bytes;
        char *ptr;
        int i;

        total_mem = get_total_mem();
        use_bytes = total_mem * 1024 *load;
        if( (ptr = malloc( use_bytes * sizeof(char))) == NULL){
                printf("Memory worker malloc failed\n");
        }
        

        while(1){
                for(i=0;i<use_bytes;i++){
                        ptr[i] = 'a';
                }
        }

        return 0;
}

int io_run(){
        char name[]="./workload.XXXXXX";
        int fd;
        int i;
        char buf[1024*1024*1024];
        printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
        for(i=0;i<sizeof(buf);i++){
                buf[i] = i*191;
        }
        printf("%lu \n",sizeof(buf));
        int c=0;
        while(1){
                if( (fd= mkstemp(name)) == -1){
                        printf("IO worker mkstemp %s failed %d\n",name,errno);
                        return 1;
                }
                if( unlink(name) == -1){
                        printf("IO worker unlink %s failed %d\n",name,errno);
                        return 1;
                }
                write(fd,buf,sizeof(buf));
                close(fd);
//                strcpy(name,"./workload.XXXXXX");
        }

        return 0;
}

char cpu_handler(){
        return 0;
}
char mem_handler(int pid){
        int status;
        waitpid(pid,&status,WNOHANG);
        if(WIFSIGNALED(status)){
                printf("Memory worker signaled\n");
                return 0;
        }
        return 1;
}
char io_handler(){
        return 0;
}
unsigned long get_total_mem(){
        FILE* fd;
        char str[50],unit[5];
        unsigned long total_mem;

        fd = fopen("/proc/meminfo","r");
        fscanf(fd,"%s %lu %s",str,&total_mem,unit);

        return total_mem;
}
