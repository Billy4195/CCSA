#include <stdio.h>
#include <string.h>



int main(int argc, char** argv){
        int i;
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
        return 0;
}
