
/*Ramazan Ozdemir Server */



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
	

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
 
#define SHMSZ 1024
#define BUFSIZE 1024 /* Buffer Size */

typedef struct server
{
	/* data */
	char cDirectory[BUFSIZE][BUFSIZE];
	int size;
}server;

typedef struct client
{
	int size;
	int pid;
}client;

typedef struct words
{
	char **words;
	int *wordsSize;
	int sizeW;
}words;

typedef struct allWords
{
	/* data */
	char words[BUFSIZE][BUFSIZE];
	int wordsSize[BUFSIZE];
	int size;
}allWords;

typedef struct points
{
	int point[BUFSIZE];
	int size;
	int isSent;
}points;
typedef struct Miner
{
	double  startTime;
	double stopTime;
	int price;
	int pid;
	
}Miner;

typedef struct Miners
{
	Miner workingMiners[BUFSIZE];
	Miner pendingMiners[BUFSIZE];
	Miner servedMiners[BUFSIZE];
	int sizeWorking;
	int sizePending;
	int sizeServed;
}Miners;



int isCreateClient(int pid1,int pid2);
/*CTRL-C handler */
void handle( int sig);

void signalPipeHandle(int signo);/*Pipe handler*/
void my_sigchld_handler(int sig);/*child process handler */
void  ALARMhandler(int sig);

/*gives operation name number size and client fifoname and calculate it sent client*/
void calculateOperatipn(char fifoName1[],char *operation,int * numbers,int size);

/*my server takes pipe parameter and fifo name connect client */


/*my sleep dies after 30 seconds if nothing connected */
void mySleep(clock_t sec);


int isInside(char *word);


char** Make2DintArray(int arraySizeX, int arraySizeY);
void freeArray(char **arr, int arraysSizeX);
void*  server2(void *ignoreMe);

/*global wariables*/
char LOGFILE[BUFSIZE];
FILE *logfp;

int fifo;

char **directory;
int directorySize=0;
int capacity=0;
char FIFO_FILE[BUFSIZE];
static int childProcess[BUFSIZE];
allWords *kelime;
allWords *oldKelime;

char **allOfWords;
int *allOfWordsTimes;
int allOfWordsSize=0;
int allOfWordsCap=0;

static int totalClient=0;
int totalClientPid[BUFSIZE];

points *clientPoint;

pthread_attr_t tattr; /* Thread Attrebute */
pthread_t  thread_id[PIPE_BUF]; /* Thread ID */
int intThread_count=(-1);
char fileNames[PIPE_BUF][PIPE_BUF];
pthread_mutex_t lock;


Miners miners;
clock_t start;
double duration;
time_t rawtime;
struct tm * timeinfo;



int main (int argc, char *argv[])
{

	int i=0,fifo,minerDirectoryNum=0;
	strcpy(LOGFILE,"Server.Log");




    start = clock();

	miners.sizeServed=0;
	miners.sizePending=0;
	miners.sizeWorking=0;

	signal( SIGINT, handle);

	unlink(LOGFILE);
	logfp=fopen(LOGFILE,"w");
	if(logfp==NULL)
		return -1;


	clock_t start;
    double duration;

    start = clock();

	if(argc<1){
		printf("Usege : ./mine directory1 directory2 ... directoryn\n");
		return 0;
	}

	printf(".............................  The Mine is starting  .............\n");

	allOfWords=Make2DintArray(BUFSIZE,BUFSIZE);
	allOfWordsTimes=(int*) malloc(sizeof(int)*BUFSIZE);
	allOfWordsCap=BUFSIZE;

	strcpy(FIFO_FILE,"fifo");
	

	directory=Make2DintArray(argc*2,1024);
	capacity=argc*2;


	for(i=1;i<argc;i++){
		strcpy(directory[directorySize],argv[i]);
		directorySize++;
		//printf("%s\n",directory[i]);
	}
	char *ignoreMe;
	int err;
	if (pthread_mutex_init(&lock, NULL) != 0)/*mutex init */
	    {
	        printf("\n mutex init failed\n");
	        return 0;
	    }
		for(;;){
	            intThread_count++;/* Thread sayisini tutar. */
				if (pthread_create(&thread_id[i],NULL, server2, &ignoreMe)!=0){
					fprintf(stderr, "Failed to create thread: %s\n", strerror(err));
					return -1;
					}

				
		//}
		//for(i=0;i<BUFSIZE;i++)
		//{
			
			if (err = pthread_join(thread_id[i], NULL))
			    {
					fprintf(stderr, "Failed to join thread: %s\n", strerror(err));
					return -1;
				}
				

				
		}

		/*mutex destroyed*/
		pthread_mutex_destroy(&lock);
	

	
	freeArray(directory,capacity);
	freeArray(allOfWords,allOfWordsCap);
	unlink(FIFO_FILE);
	fclose(logfp);



}

int isCreateClient(int pid1,int pid2)
{
	if(pid1==pid2 || pid2==0)
		return 0;
	else
		return 1;
}

int isInside(char *word)
{
	int i=0;
	/*icindeyse 1 degilse 0*/
	for(i=0;i<allOfWordsSize;i++)
	{
		if(strcmp(allOfWords[i],word)==0)
			return i;
	}
	return -1;

}
void*  server2(void *ignoreMe)
{


	pthread_mutex_lock(&lock);/*mutex locked*/

	unlink(FIFO_FILE);
	if (mkfifo(FIFO_FILE, 0666) == -1){
		perror("error");
		exit(1);
	}
	
	int shmid,shmid2;
	char c,tmp;
	key_t key1,key2;
	char *shm, *s;
	int myI=0;
	server *myServer;
	client *myClient;
	char buf[BUFSIZE];

	int i=0,fifo,minerDirectoryNum=0;
	int clientPid;
	int clientTotalPoint=0;
	
	clock_t startTime;

	key1=1234;
	key2=1111;
	

	for(i=0;i<miners.sizeServed;i++)
	{
		fprintf(logfp, "Served Miners : %d\n",miners.servedMiners[i].pid);
		fprintf(logfp, "Server miners : start time : %f  Finish time : %f  Given Price : %d\n",miners.servedMiners[i].startTime,miners.servedMiners[i].stopTime,miners.servedMiners[i].price);
	}


	for(i=0;i<miners.sizePending;i++)
	{
		fprintf(logfp, "Pending Miners : %d\n",miners.servedMiners[i].pid);
		fprintf(logfp, "Pending miners : start time : %f  \n",miners.servedMiners[i].startTime);
	}
	
	start:
 	
	printf("-----------------------------Clients Waiting...---------------------------\n");
	fifo=open(FIFO_FILE,O_RDONLY);
	if(fifo==-1)
	{
		perror("exit exception");
		exit(1);
	}

	if(read(fifo,&minerDirectoryNum,sizeof(int))<0)
	{
		perror("read error \n");
		exit(1);
	}


	
	//if(minerDirectoryNum>0)
		//intThread_count++;

	fprintf(logfp,"----------------------Connecting a client ----------------------\n");
	printf("%d\n",minerDirectoryNum);

	if(read(fifo,&clientPid,sizeof(int))<0)
	{
		perror("read error \n");
		exit(1);
	}

	/*if(minerDirectoryNum>=directorySize)
	{
		kill(clientPid,SIGKILL);
		pthread_detach(pthread_self());
		
	}*/
	//start=clock();
	duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
	miners.workingMiners[0].startTime=duration;
	miners.workingMiners[0].pid=clientPid;
	miners.sizeWorking++;


	//fprintf(logfp, "Currently Working a miner  %d\n", clientPid);

	totalClientPid[totalClient]=clientPid;
	totalClient++;

	printf("Client Pid %d\n",clientPid);
	close(fifo);

	fifo=open(FIFO_FILE,O_WRONLY);
	int k=0,j=0;
	if(fifo==-1)
	{
		perror("exit exception");
		exit(1);
	}

	if(minerDirectoryNum<=directorySize)
	{
		
		
		for(k=0;k<minerDirectoryNum;k++){
			if(write(fifo,directory[k],sizeof(directory[k]))<0)
			{
				perror("write error \n");
				exit(1);
			}
		}
		
		for(k;k<directorySize;k++)
		{
			strcpy(directory[j],directory[k]);
			j++;

		}
		directorySize=directorySize-minerDirectoryNum;
		
		
	}
	else
	{
		
		strcpy(buf,"error");
		//printf("Error cikmadi \n");
		if(write(fifo,buf,sizeof(buf))<0)
			{
				perror("write error \n");
				exit(1);
			}
		
		miners.pendingMiners[miners.sizePending].startTime=duration;
		miners.pendingMiners[miners.sizePending].pid=clientPid;
		miners.sizePending++;
		goto end;
		
		

	}
	close(fifo);

	
	key1=ftok("./",'x');
	/*veri alinmasi lazim*/
	if ((shmid = shmget(key1,sizeof(struct allWords), IPC_CREAT |0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((kelime = (allWords*)shmat(shmid, NULL, 0)) == (void *) -1) {
		perror("shmat");
		exit(1);
	}

	if(kelime<0)
	{
		perror("cant create");
		exit(1);
	}


	key2=ftok("./",'y');
	/*veri alinmasi lazim*/
	if ((shmid2 = shmget(key2,sizeof(struct points), IPC_CREAT |0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((clientPoint = (points*)shmat(shmid2, NULL, 0)) == (void *) -1) {
		perror("shmat");
		exit(1);
	}

	if(clientPoint<0)
	{
		perror("cant create");
		exit(1);
	}

	clientPoint->isSent=0;
	myI=0;
	
	oldKelime=kelime;
	//printf("mimer : %d\n",minerDirectoryNum);
	while(myI<minerDirectoryNum){
		memset(kelime,0,BUFSIZE);
		sleep(1);
		if(kelime==NULL)
			continue;
		
		else if(strcmp(kelime->words[0],"")!=0){ 
			int l=0;
			for(l=0;l<kelime->size;l++)
			{
				int index=isInside(kelime->words[l]);
				if(index!=-1){
					clientTotalPoint+=kelime->wordsSize[l];
					clientPoint->point[l]=kelime->wordsSize[l];
					allOfWordsTimes[index]+=kelime->wordsSize[l];

				}
				else
				{
					/*ilk kelimeden 10 aliyor sonrakiler yine 1 */
					if(allOfWordsSize>=allOfWordsCap)
					{
						int m=allOfWordsCap+1;
						allOfWords=(char**) realloc(allOfWords,sizeof(char*)*m);
						allOfWordsTimes=(int*)realloc(allOfWordsTimes,sizeof(int*)*m);
						
						//for(allOfWordsCap;allOfWordsCap<allOfWordsCap*2;allOfWordsCap++)
						//{
							allOfWords[allOfWordsCap]=(char*) malloc(sizeof(char )* BUFSIZE);
						//}
							allOfWordsCap++;
					}
					strcpy(allOfWords[allOfWordsSize],kelime->words[l]);
					//printf("%s\n",kelime->words[l]);
					allOfWordsTimes[allOfWordsSize]=kelime->wordsSize[l];

					allOfWordsSize++;

					clientTotalPoint+=10+kelime->wordsSize[l]-1;
					clientPoint->point[l]=10+kelime->wordsSize[l]-1;
				}

			}
			clientPoint->size=l;
			clientPoint->isSent=1;

		}
		

		oldKelime=kelime;

		myI++;
		//printf("%d\n",myClient->size);
	}
	printf("clientTotalPoint was given %d\n",clientTotalPoint);
	miners.workingMiners[0].price=clientTotalPoint;
	//sleep(1);

	


	

	int subDirectoryNum=0;
	fifo=open(FIFO_FILE,O_RDONLY);


	if(fifo==-1)
	{
		perror("exit exception");
		exit(1);
	}
	if(read(fifo,&subDirectoryNum,sizeof(int))<0)
	{
		perror("write error \n");
		exit(1);
	}
	//printf("%d\n",subDirectoryNum);

	for(i=0;i<subDirectoryNum;i++)
	{
		if(read(fifo,buf,sizeof(buf))<0)
		{
			perror("write error \n");
			exit(1);
		}
		//printf("%s\n",buf);
		if(capacity<=directorySize)
		{
			directory= (char**) realloc(directory,(capacity+1) *sizeof(char *) );
			if(directory==NULL)
			{
				exit(1);
			}
			directory[capacity]=(char*)malloc(BUFSIZE*sizeof(char));
			capacity++;
		}
		strcpy(directory[directorySize],buf);
		directorySize++;

	}
	close(fifo);

	duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
	miners.workingMiners[miners.sizeWorking].stopTime=duration;

	fprintf(logfp, "Working Miners : %d\n",miners.workingMiners[0].pid);
	fprintf(logfp, "Working miners : start time : %f  Finish time : %f  Given Price : %d \n",miners.workingMiners[0].startTime,miners.workingMiners[0].stopTime,miners.workingMiners[0].price);
	miners.servedMiners[miners.sizeServed]=miners.workingMiners[0];
	miners.sizeServed++;

	miners.sizeWorking=0;

   if(shmdt(kelime) != 0)
		fprintf(stderr, "Could not close memory segment.\n");
	if(shmdt(clientPoint) != 0)
		fprintf(stderr, "Could not close memory segment.\n");

	printf("Attention  please ....Client is quiting...........\n");

	end:
	pthread_mutex_unlock(&lock);/*mutex acilir*/


}

/*array a yer alinir*/
char** Make2DintArray(int arraySizeX, int arraySizeY) {
    
    char** theArray;
    theArray = (char**) malloc(arraySizeX*sizeof(char*));
    int i;
    for (i = 0; i < arraySizeX; i++)
    {
        theArray[i] = (char*) malloc(arraySizeY*sizeof(char));
    }
    return theArray;
}
/* Gives malloc allcotions*/
void freeArray(char **arr, int arraysSizeX)
{
    int i;
    for (i = 0; i < arraysSizeX; ++i) {
        free(arr[i]);
    }
    free(arr);
}





void handle( int sig)
{
	
	int i;
	duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
	fprintf(logfp, "Total  time %.3f\n",duration);
	fprintf(logfp, "\n\n********************** Words ********************\n");

	
	/*if(kelime!=NULL)
		if(shmdt(kelime) != 0)
			fprintf(stderr, "Could not close memory segment.\n");
	if(clientPoint!=NULL)
		if(shmdt(clientPoint) != 0)
			fprintf(stderr, "Could not close memory segment.\n");*/

	for(i=0;i<allOfWordsSize;i++)
	{
		//printf("%s : %d\n",allOfWords[i],allOfWordsTimes[i]);
		fprintf(logfp, "%s : %d\n",allOfWords[i],allOfWordsTimes[i]);
	}

	if(directory!=NULL)
		freeArray(directory,capacity);
	if(allOfWords!=NULL)
		freeArray(allOfWords,allOfWordsCap);


	fprintf(logfp, "CTRL-C Handled \n");
	printf("***************************** The program is ended.........  **************\n");
	unlink(FIFO_FILE); /*Unlink fifo file */

	fclose(logfp);
	miners.sizeWorking++;

	for(i=0;i<intThread_count+1;i++)
	{
		pthread_detach(thread_id[i]);
	}

	for(i=0;i<miners.sizeWorking;i++)
		kill(miners.workingMiners[i].pid,SIGKILL);
	
	

	//fprintf(stderr,"%d --- %d\n",capacity,allOfWordsCap);
	
	killpg(getpgrp(),SIGINT);/*kill all zombi process*/
	exit(sig);
}