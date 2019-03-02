#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/ipc.h>
#include<sys/msg.h>

//#define PIPE
//#define DEBUG

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
typedef struct{
    long int type;
    u64 sum;
}MSG;

static u64 MAX = (((u64)1<<32)-1);            // default:4294967295
static u64 CHECK = 9223372034707292160;       // default:right answer for check
static u64 sum_all=0;           // result
static struct timeval Ts, Te;   // time point
static u16 N =100;                // number of son threads



#define PATH_IN "input.txt"
#define PATH_OUT "output.txt"
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
    LOG_DEBUG("get[N]: %s", tmp);
    if((atoi(tmp) > 0) && (atoi(tmp) < 100))
    {
        N = atoi(tmp);
    }else return -1;

    fgets(buff,1024,fp);  
//    LOG_DEBUG("input[N]: %s", buff);
    sscanf(buff, "M=%s", tmp);
    LOG_DEBUG("get[M]: %s", tmp);
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


int main(int argc, char * * argv)
{
    pid_t *pid;
    u64 i = 0;

//get input 
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

    pid = (pid_t*)malloc(sizeof(pid_t) * N);
    gettimeofday(&Ts,NULL);

#ifdef PIPE
        int fd[2];
        if(pipe(fd)<0)
        {
            LOG_DEBUG("pipe failed!\n");
            exit(1);
        }
#else
        int msgid;
        long msg_rec_type = 0;// recv first msg
        msgid = msgget((key_t)1234, 0666|IPC_CREAT);
        if(msgid == -1)
        {
            printf("msgget err\n");
            return -1;
        }
#endif


// generate sons
    for(i=0;i<N;i++)
    {
        pid[i] = fork();
        if(pid[i] == 0) // son
        {
            break;
        }
    }

//    LOG_DEBUG("i:%llu", i);

    if(i < N)   // task of son
    {
        u64 sum = 0;
        MSG msg;
        i++;
#ifdef PIPE
        close(fd[0]);
#endif
        while(i <= MAX)
        {
            sum += i;
            i += N;
        }
//        LOG_DEBUG("i: %llu sum: %llu", i, sum);
        
#ifdef PIPE
        write(fd[1],&sum, 8);
#else
        msg.type = 1;
        if(msgsnd(msgid, (void*)&sum, 8, 0)==-1)
        {
            printf("msg snd err\n");
            exit(1);
        }
#endif
        exit(0);
    }

     for(i=0;i<N;i++)  // gather sum from sons
    {
        u64 tmp = 0;
#ifdef PIPE
        close(fd[1]);
        wait(NULL);
        read(fd[0], &tmp, 8);
#else
        wait(NULL);
        if(msgrcv(msgid,(void*)&tmp, 8, msg_rec_type, 0) == -1)
        {
            printf("msg recv err\n");
            return -1;
        }
#endif    
//        LOG_DEBUG("tmp %llu ", tmp);
        sum_all += tmp;
    }   

    gettimeofday(&Te,NULL);
     
    //outpur result
    if(write_file() < 0) printf("write file err!\n");
  
    printf("N: %u, M: %llu, sum:%llu, runtime is %f\n",N, MAX, sum_all, TimeDiff(Te,Ts));
    if(sum_all == CHECK) LOG_DEBUG(" PASS! \n");

    free(pid);    
    return 0;
}

