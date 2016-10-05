#include <stdio.h>


int main(){
        FILE* fp;
        char cpu_name[20];
        long pre[8],aft[8];
        long Total,PreTotal,totald,idled;
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
        printf("CPU useage : %lf\n",cpu_use);
        return 0;
}
