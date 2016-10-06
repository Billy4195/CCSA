#include <stdio.h>


int main(){
        FILE* fp;
        char cpu_name[20],buf[50],unit[5];
        long pre[8],aft[8];
        long Total,PreTotal,totald,idled;
        long num,mem_tol,mem_free,mem_buf,mem_cache,mem_used;
        double cpu_use;
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld %ld",cpu_name,&pre[0],&pre[1],
            &pre[2],&pre[3],&pre[4],&pre[5],&pre[6],&pre[7]);
//        printf("%s %ld %ld %ld %ld %ld %ld %ld %ld",cpu_name,pre[0],pre[1],
//            pre[2],pre[3],pre[4],pre[5],pre[6],pre[7]);
        fclose(fp);
        sleep(1);
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld %ld",cpu_name,&aft[0],&aft[1],
            &aft[2],&aft[3],&aft[4],&aft[5],&aft[6],&aft[7]);
        fclose(fp);

        Total = aft[0]+aft[1]+aft[2]+aft[3]+aft[4]+aft[5]+aft[6]+aft[7];
        PreTotal = pre[0]+pre[1]+pre[2]+pre[3]+pre[4]+pre[5]+pre[6]+pre[7];
        totald = Total - PreTotal;
        idled = (aft[3]+aft[4]) - (pre[3]+pre[4]);
        cpu_use = (totald - idled)/(double)totald*100;
        printf("CPU useage : %lf %%\n",cpu_use);

        fp = fopen("/proc/meminfo","r");
        fscanf(fp,"%s %ld %s",buf,&num,unit);
        mem_tol = num;
        fscanf(fp,"%s %ld %s",buf,&num,unit);
        mem_free = num;
        fscanf(fp,"%s %ld %s",buf,&num,unit);
        fscanf(fp,"%s %ld %s",buf,&num,unit);
        mem_buf = num;
        fscanf(fp,"%s %ld %s",buf,&num,unit);
        mem_cache = num;
        fclose(fp);

        mem_used = mem_tol - mem_free - mem_buf - mem_cache;
        printf("Memory Usage: %ld/%ld MB (%lf %%)\n",mem_used/1024,mem_tol/1024, (double)mem_used/mem_tol*100);

        return 0;
}
