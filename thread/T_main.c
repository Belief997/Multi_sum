#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<sys/time.h>
#include<inttypes.h>

#define DEBUG 
//#define LOCK

#ifdef DEBUG
#define LOG_DEBUG(fmt,...) {printf("[D][%s:%d]"fmt"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);}
#else
#define LOG_DEBUG(fmt,...) {;}
#endif

#define TimeDiff(te,ts)    (double)((te.tv_sec-ts.tv_sec) + (te.tv_usec-ts.tv_usec)/1000000.0)

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long long u64;

static u64 MAX = (((u64)1<<32)-1);            // default:4294967295
static u64 CHECK = 9223372034707292160;       // default:right answer for check
static u64 sum_all=0;           // result
static struct timeval Ts, Te;   // time point
static u16 N =1;                // number of son threads
#ifdef LOCK
    pthread_mutex_t mutex;
#endif



//son thread func
void* thread_sum_func(void* arg)
{
    u8 *data = ((u8*)arg);  //start point
    u64 i = *data;
    u64 sum =0;

#ifndef LOCK
//    LOG_DEBUG("data %u \n", *data);
//    LOG_DEBUG("i %llu \n",i );
    while(i <=MAX)
    {
        sum += i ;
        i += N;
    }
//    LOG_DEBUG("i %llu sum %llu \n",i,sum);
    pthread_exit((void *)sum);
#else
    pthread_mutex_lock(&mutex);
    while(i <=MAX)
    {
        sum_all += i ;
        i += N;
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
#endif

}

#define PATH_IN "./input.txt"
#define PATH_OUT "./output.txt"
int read_file(void)
{
    FILE *fp;
    char buff[30];
    char tmp[30];
    char tmp1[30];

    if((access(PATH_IN,F_OK)) == -1) return -1;

    if((fp = fopen(PATH_IN,"r")) == NULL)
    {
        printf("read input.txt err!\n");
        return -1;
    }
    fgets(buff,1024,fp);  
//    LOG_DEBUG("input[N]: %s", buff);

    sscanf(buff, "N=%s", tmp);
//    LOG_DEBUG("get[N]: %s", tmp);
    if((atoi(tmp) > 0) && (atoi(tmp) < 100))
    {
        N = atoi(tmp);
    }else return -1;

    fgets(buff,1024,fp);  
//    LOG_DEBUG("input[N]: %s", buff);
    sscanf(buff, "M=%s", tmp);
//    LOG_DEBUG("get[M]: %s", tmp);
//    LOG_DEBUG("%llu ,%llu ", atol(tmp), (((u64)1<<32)-1));
    if((atol(tmp) > 0) && (atol(tmp) <= (((u64)1<<32)-1)))
    {
        MAX = atol(tmp);
    }else return -1;

    fclose(fp);
    return 0;
}

int write_file(void)
{
//    FILE *fp;
    char buff[40];
//    snprintf(buff,40,"%llu",sum_all);
//    LOG_DEBUG("write %s", buff);

//    fp = fopen(PATH_OUT, "w+");
//    if(!fp) return -1;
//    if(fwrite((const void*)buff, sizeof(char), strlen(buff), fp) < 0) return -1;

    snprintf(buff,40,"echo %llu > "PATH_OUT"",sum_all);
    system(buff);
    return 0;
}

int main(int argc, char ** argv)
{
    u64 i=0;
    u8 *num;

    if(argc > 1)
    LOG_DEBUG("%d %d", argc, atoi(argv[1]));
    
    if((argc == 2) && (atoi(argv[1]) < 100) && (atoi(argv[1]) > 0))
    {
        N = atoi(argv[1]);
    }else if(argc == 1)
    {
        
        if(read_file() == -1)
        {
            printf("read input.txt err!\n");
            return -1;
        }        
        LOG_DEBUG("get N: %u M: %llu", N, MAX);
        CHECK = (1 + MAX) * MAX / 2;
    }else
    {
        printf("argument err!\n");
        return -1;
    }
    
    pthread_t  *pthread_id = NULL; 

    //time: start record
    gettimeofday(&Ts,NULL);

    pthread_id = (pthread_t*)malloc(N * sizeof(pthread_t));
    num = (u8*)malloc(N * sizeof(u8));

    
    for(i=0; i < N; i++)
    {
        num[i] = i+1; 
    }

#ifdef LOCK
    pthread_mutex_init(&mutex, NULL);
#endif

    //creat son thread
    for(i=0;i<N;i++)
    {
//        LOG_DEBUG("i %llu  %u \n",i, *(num+i));
        if(pthread_create(pthread_id+i,NULL,thread_sum_func, num+i) != 0)
        {
            LOG_DEBUG("ERR\n");

        }
    }

#ifndef LOCK
    //gather son thread result
    for(i=0;i<N;i++)
    {
        u64 tmp ;
        pthread_join(pthread_id[i],(void*)&tmp);
//        LOG_DEBUG("tmp  %llu \n", tmp);
        sum_all += tmp;
    }
#else
//    LOG_DEBUG("THIS IS LOCK");
       for(i=0;i<N;i++)
    {
        pthread_join(pthread_id[i],NULL);
    } 
#endif

    //time: end record
    gettimeofday(&Te,NULL);

    //outpur result
    if(write_file() < 0) printf("write file err!\n");

    //print check
    printf("N: %u, M: %llu, sum:%llu, runtime is %f\n",N, MAX, sum_all, TimeDiff(Te,Ts));
    if(sum_all == CHECK) LOG_DEBUG(" PASS! \n");
    
    free(pthread_id);
    free(num);
    return 0;
}


