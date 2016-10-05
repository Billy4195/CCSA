#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>

int do_cpu_run(int cpu_load);

int main(int argc, char** argv){
        int i,pid,children=0;
        int cpu_load,mem_load,io_load;
        for(i=1; i < argc; i++){
                char* arg = argv[i];
                if(strcmp (arg, "cpu") == 0){
                        cpu_load = atoi(argv[++i]);
                }else if(strcmp (arg, "mem") == 0 ){
                        mem_load = atoi(argv[++i]);
                }else if(strcmp (arg, "io") == 0){
                        io_load = atoi(argv[++i]);
                }else{
                        printf("Unrecongnize argument %s",argv[i]);
                }
        }
        printf("CPU load %d\n",cpu_load);
        printf("Memory load %d\n",mem_load);
        printf("IO load %d\n",io_load);
        if( cpu_load > 0 ){
                switch(pid = fork()){
                  case 0:
                        alarm(5);
                        exit( do_cpu_run(cpu_load) );
                  case -1:
                        printf("fork process error while adding cpu load\n");
                  default:
                        printf("CPU worker %d forked\n",pid);
                        ++children;
                }
        }
        if( mem_load > 0 ){
        
        }
        if( io_load > 0 ){
        
        }
        while( children ){
                int status, ret;
                if( (pid = wait(&status)) > 0 ){
                        if(WIFEXITED(status)){
                                if((ret = WEXITSTATUS(status)) ==0){
                                        printf("Worker %d exit normally\n",pid);
                                }else{
                                        printf("Worker %d exit return error %d\n",pid,ret);
                                }
                        }else if (WIFSIGNALED(status)){
                                if((ret = WTERMSIG(status)) == SIGALRM){
                                        printf("Worker %d signalled normally\n",pid);
                                }else{
                                        printf("Worker %d got signal %d\n",pid,ret);
                                }
                        }
                        --children;
                }else{
                        printf("ERROR wait for worker\n");
                }
        }
        return 0;
}

int do_cpu_run(int cpu_load){
        while(1){
          sqrt(rand());
        }

        return 0;
}
