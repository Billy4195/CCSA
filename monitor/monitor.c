#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(){
        FILE* fp;
        char cpu_name[20],buf[50],unit[5];
        long pre[8],aft[8];
        long Total,PreTotal,totald,idled;
        long num,mem_tol,mem_free,mem_buf,mem_cache,mem_used;
        double cpu_use;
        unsigned int major,minor,wr_ticks,ios_pgr,tot_ticks,rq_ticks;
        char dev_name[128];
        unsigned long rd_ios,rd_merges_or_rd_sec,rd_ticks_or_wr_sec, wr_ios;
        unsigned long wr_merges, rd_sec_or_wr_ios, wr_sec;
        unsigned long pre_rd_sec,pre_wr_sec,aft_rd_sec,aft_wr_sec;
        unsigned int hz;
        long ticks;

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
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld %ld",cpu_name,&pre[0],&pre[1],
            &pre[2],&pre[3],&pre[4],&pre[5],&pre[6],&pre[7]);
        fclose(fp);

        fp = fopen("/proc/diskstats","r");
        while(fscanf(fp,"%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
                &major, &minor, dev_name,
                &rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
                &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks) != EOF){
                if(strstr(dev_name,"sd")){
//                        if(dev_name[2] > 'a' && dev_name[2] < 'z'){
//                                printf("~~~~~~~%s %lu %lu\n",dev_name,rd_sec_or_wr_ios,wr_sec);
                                pre_rd_sec = rd_sec_or_wr_ios;
                                pre_wr_sec = wr_sec;
                                break;
//                        }
                }
        }
        fclose(fp);

        sleep(5);
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld %ld",cpu_name,&aft[0],&aft[1],
            &aft[2],&aft[3],&aft[4],&aft[5],&aft[6],&aft[7]);
        fclose(fp);

        fp = fopen("/proc/diskstats","r");
        while(fscanf(fp,"%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
                &major, &minor, dev_name,
                &rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
                &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks) != EOF){
                if(strstr(dev_name,"sd")){
//                        if(dev_name[2] > 'a' && dev_name[2] < 'z'){
//                                printf("~~~~~~%s %lu %lu\n",dev_name,rd_sec_or_wr_ios,wr_sec);
                                aft_rd_sec = rd_sec_or_wr_ios;
                                aft_wr_sec = wr_sec;
                                break;
//                        }
                }
        }
        fclose(fp);

        Total = aft[0]+aft[1]+aft[2]+aft[3]+aft[4]+aft[5]+aft[6]+aft[7];
        PreTotal = pre[0]+pre[1]+pre[2]+pre[3]+pre[4]+pre[5]+pre[6]+pre[7];
        totald = Total - PreTotal;
        idled = (aft[3]+aft[4]) - (pre[3]+pre[4]);
        cpu_use = (totald - idled)/(double)totald*100;
        printf("CPU useage : %lf %%\n",cpu_use);




        mem_used = mem_tol - mem_free - mem_buf - mem_cache;
        printf("Memory Usage: %ld/%ld MB (%lf %%)\n",mem_used/1024,mem_tol/1024
            , (double)mem_used/mem_tol*100);

        if((hz =(unsigned int) sysconf(_SC_CLK_TCK) ) == -1){
          perror("sysconf error");
        }
        printf("HZ = %d\n",hz);
        printf("Read speed %lf MB/s\n",(double)(aft_rd_sec-pre_rd_sec)/(double)((unsigned long)Total-(unsigned long)PreTotal)*hz/1024);
        printf("Write speed %lf MB/s\n",(double)(aft_wr_sec-pre_wr_sec)/(double)((unsigned long)Total-(unsigned long)PreTotal)*hz/1024);
        return 0;
}
