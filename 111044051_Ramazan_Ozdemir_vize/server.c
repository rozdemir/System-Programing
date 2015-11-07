
/*Ramazan Ozdemir Server */




#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <termios.h>


#define BUFSIZE 1024 /* Buffer Size */


/*CTRL-C handler */
void handle( int sig);

void signalPipeHandle(int signo);/*Pipe handler*/
void my_sigchld_handler(int sig);/*child process handler */
void  ALARMhandler(int sig);

/*gives operation name number size and client fifoname and calculate it sent client*/
void calculateOperatipn(char fifoName1[],char *operation,int * numbers,int size);

/*my server takes pipe parameter and fifo name connect client */
int server(int fd[2],int fifo);

/*my sleep dies after 30 seconds if nothing connected */
void mySleep(clock_t sec);

/*Main fifo name*/
char chrPtrFIFO_FILE[BUFSIZE];


/*global wariables*/

int fifo;
int sayac=0;
int *myTime;
/*my static variables*/
static int *newLen=0;
static int *oldLen=0;
static int *myLen=0;
static int *childCounter=0;
static int *clientPid=0;

static int childProcess[BUFSIZE];

int main (int argc, char *argv[])
{
	int fd; /* File Descripter (for make fifo) */
	int len;/* read kontrolu */
	char *clientPid1; /* for Client PID */
	char buf[BUFSIZE]; /* Okuma islemi icin gerekli string */



	/*   Taking on internet stackoverflow   for changing data value in processes */
   	myTime = mmap(NULL, sizeof *myTime, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   	oldLen = mmap(NULL, sizeof *oldLen, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   	newLen = mmap(NULL, sizeof *newLen, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   	myLen = mmap(NULL, sizeof *myLen, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   	childCounter = mmap(NULL, sizeof *childCounter, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   	clientPid = mmap(NULL, sizeof *clientPid, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   	*myTime=5;

	/* calistirilirken 2 arguman olmasi gerekir */
    if ( argc != 2 )
    {
        fprintf(stderr, "Usage: %s <MainFifoName>\n", argv[0]);
        exit(-1);
    }

	/* Ctrl + C   sinyali yakalanir */
   
    /*taking on internet */
    struct sigaction sa; 
	memset(&sa, 0, sizeof(sa)); 	
	sa.sa_handler = my_sigchld_handler; 
	sigaction(SIGCHLD, &sa, NULL);

	signal(SIGALRM, ALARMhandler);
	/* Ctrl + C   sinyali yakalanir */
    signal( SIGINT, handle);
    signal(SIGPIPE, signalPipeHandle);

	   


    /* clientin programa baglanacagi fifo olusturulur. */
    sprintf(chrPtrFIFO_FILE,"%s",argv[1]);
	if (fd=mkfifo(chrPtrFIFO_FILE, 0666) == -1){
		perror("error");
		exit(1);
	}

	
	
	while(1)
	{

		int i=0,size=0;
		char operation[BUFSIZE];

		
		int fdx[2];

		//server(fdx,fifo);
		mySleep(*myTime); /*sleep fonsiyonu bekliyor ve server a baglanan olursa calisiyor*/
		//alarm(1);
		//server(fdx,fifo);
		//alarm(1000000);
		//myFoo();

		
		close(fifo);
		unlink(chrPtrFIFO_FILE);
		exit(1);

	}
}
/*calculate operation and sent it client */
void calculateOperatipn(char fifoName1[],char *operation,int * numbers,int size)
{
	int i=0;
	/**/
	char context[1024];
	int fifox;
	int pid=getpid();

	fifox=open(fifoName1,O_WRONLY);
	if(fifox==-1)
	{
		perror("exit exception");
		exit(1);
	}

	if(strcmp(operation,"operation1")==0)
	{
		int a,b,c,a2,b2,c2;
		int resultSize=size/3;
		
		a=numbers[0];
		b=numbers[1];
		c=numbers[2];
		if(size==6)
		{
			a2=numbers[3];
			b2=numbers[4];
			c2=numbers[5];
		}
		//printf("%d %d %d\n",a,b,c);
		double result=sqrt( (pow(a,2)+pow(b,2)) );
		result/=abs(c);

		double result2=sqrt( (pow(a2,2)+pow(b2,2)) );
		result2/=abs(c2);


		//printf("operation1 = %f\n",result);
		if(size==3)
			sprintf(context,"pid =%d \n%s%f",pid,"results = ",result);
		else if(size==6)
			sprintf(context,"pid =%d \n%s%f",pid,"result1 ve 2  = ",result,"  ",result2);

	}
	else if(strcmp(operation,"operation2")==0)
	{
		double sum=0,sum2=0,sum3=0,sum4=0;
		int a,b,c;
		int a2,b2,c2;
		int a3,b3,c3;
		int a4,b4,c4;
		a=numbers[0];
		b=numbers[1];
		c=numbers[2];
		for(i=0;i<2;i++)
		{
			sum+=numbers[i];
		}
		if(size==4)
		{
			for(i=2;i<4;i++)
			{
				sum2+=numbers[i];
			}
			sum2=sqrt(sum2);
		}
		if(size==6)
		{
			for(i=4;i<6;i++)
			{
				sum3+=numbers[i];
			}
			sum3=sqrt(sum3);
		}

		if(size==8)
		{
			for(i=6;i<8;i++)
			{
				sum4+=numbers[i];
			}
			sum4=sqrt(sum4);
		}
		//printf("operation2 = %f\n",sqrt(sum));
		if(size==2)sprintf(context,"pid =%d  \n%s%f",pid,"sqrt sum  = ",sqrt(sum));

		else if(size==4)sprintf(context,"pid =%d \n%s%f%s%f",pid,"sqrt sum  = ",sqrt(sum),"..  sqrt sum2 ",sum2);
		else if(size==6)
			sprintf(context,"pid =%d \n%s%f%s%f%s%f",pid,"sqrt sum  = ",sqrt(sum),"..  sqrt sum2 ",sum2," .. sqrt sum3 ",sum3);

		else if(size==8)
			sprintf(context,"pid =%d \n%s%f%s%f%s%f%s%f",pid,"sqrt sum  = ",sqrt(sum),"..  sqrt sum2 ",sum2," .. sqrt sum3 ",sum3,"sqrt sum 4 ",sum4);
	}

	else if(strcmp(operation,"operation3")==0)
	{
		int a,b,c,delta;
		int a2,b2,c2,delta2;
		
		a=numbers[0];
		b=numbers[1];
		c=numbers[2];
		delta= (b*b)-(4*a*c);
		
		delta=sqrt(delta);


		if(size==6)
		{
			a2=numbers[3];
			b2=numbers[4];
			c2=numbers[5];
			delta2= (b2*b2)-(4*a2*c2);
		
			delta2=sqrt(delta2);
		}
		//printf("operation3 root1 = %f root2 = %f\n",((double)(-b+delta)/(2*a)),( (double)(-b-delta)/(2*a) ) );
		if(size==3)
			sprintf(context,"pid =%d\n%s%f%s%f",pid,"operation3 root1 =",((double)(-b+delta)/(2*a)),"\nroot2 =",( (double)(-b-delta)/(2*a) ));
		else if (size==6){
			sprintf(context,"pid =%d\n%s%f%s%f\n%s%f%s%f",pid,"operation3 root1 =",((double)(-b+delta)/(2*a)),"\nroot2 =",( (double)(-b-delta)/(2*a) ),
												 "operation3.2 root1 =",((double)(-b+delta)/(2*a)),"\nroot2 =",( (double)(-b-delta)/(2*a) ));
		}

	}

	else if(strcmp(operation,"operation4")==0)
	{
		int a,b,c,d;
		int a2,b2,c2,d2;
		a=numbers[0];
		b=numbers[1];
		c=numbers[2];
		d= numbers[3];
		if(size==8)
		{
			a2=numbers[4];
			b2=numbers[5];
			c2=numbers[6];
			d2=numbers[7];
		}
		//printf("operation4  (%dx+%d)/ (%dx+%d)",-d,b,c,-a);
		if(size==4)
			sprintf(context,"pid = %d \n%s%d%s%d%s%d%s%d ",pid,"operation4 ",-d,"x +",b,"/",c,"x + ",-a);
		else if(size==8)
			sprintf(context,"pid = %d \n%s%d%s%d%s%d%s%d\n%s%d%s%d%s%d%s%d ",pid,"operation4 ",-d,"x +",b,"/",c,"x + ",-a
																 ,"operation4 ",-d2,"x +",b2,"/",c2,"x + ",-a2);
	}
	if(write(fifox,context,sizeof(context))<0)
	{
		perror("write error \n");
		exit(1);
	}
	close(fifo);


}
/*server connect client and calculate then sent it */
int server(int fd[2],int fifo)

{
	/*variables*/
	char fifoName1[BUFSIZE];
	char clientFifo[BUFSIZE];
	char buf[BUFSIZE];
	int fifo1;
	int pid;
	int len=0;
	int tempInt=0;
	int tempTime;
	char operationName[BUFSIZE];

	/*pipe making */
	if(pipe(fd)==-1)
	{
		perror("Failed to create pipe");
		return;
	}

	pid = fork(); /* proses oluşturulur */

    /* Error Handiling */

    if(pid == -1)
    {
        perror("Error: Unexpected things fork \n");
        exit(1);
    }

    /* pid 0 ise child dir. */
    if (pid == 0) 
    {
    	/*degiskenler */
    	int i=0,size=0,childPid=getpid();
    	int tempPid=0;
    	int numbers[12];
		char operation[BUFSIZE];/*operation name */

		//childProcess[*childCounter]=getpid();
		//printf("%d\n",getpid());
		//*childCounter=*childCounter+1;
		//printf("%d\n",*childCounter);
		
		fifo = open( chrPtrFIFO_FILE,O_RDONLY);/*opening main fifo for reading */
		if( fifo < 0 ) {
			perror("FIFO open" );
			exit(0);
		}
		*newLen=*newLen+1;
		//printf("come come pelase\n");
		len=read( fifo, operation,sizeof(operation));/*operation name reading from client*/
		
		//*myLen=len;
		//printf("come come pelase\n");
		if(len<=0)
		{
			perror("not read");
			exit(1);
		
		}
		
		close(fifo);

		

		printf("Connecting client..............\n");

		sprintf(fifoName1,"%s%d","FIFO.",getpid());/*client fifo unique name making */

		close(fd[0]);
		if(write(fd[1],&childPid,sizeof(int))<=0)/**/
		{
			perror("Failed to write ");
			exit(1);
		}
    	
		close(fd[0]);
		
		if(write(fd[1],fifoName1,sizeof(fifoName1))<=0)/*writing client fifoname from parents with pipe*/
		{
			perror("Failed to write ");
			exit(1);
		}

		

		if (fifo1=mkfifo(fifoName1, 0666) == -1){/*making client fifo*/
			perror("Failed to create myfifo");
			exit(1);
		}


		

		fifo1=open(fifoName1,O_RDONLY);/*opening client fifo */

    	if(fifo1<0)
    	{
    		perror("fifo open read");
    		exit(1);
    	}

    	/*reading informations from client */

    	if(read(fifo1,&tempPid,sizeof(int))<0)
		{
			perror("error read ");
			exit(1);
		}
		*clientPid=tempPid;
    	if(read(fifo1,&tempTime,sizeof(int))<0)
		{
			perror("error read ");
			exit(1);
		}

		*myTime=tempTime;

		if(read(fifo1,&size,sizeof(int))<0)
		{
			perror("error read ");
			exit(1);
		}
			//printf("size = %d\n",size);


		/*reading from client arguments */
		for(i=0;i<size;i++)
		{
		
			if(read(fifo1,&tempInt,sizeof(int))<0){
				perror("error read ");
				exit(1);
				
			}
			numbers[i]=tempInt;

		}
		
		
		close(fifo1);
		
		//printf("time = %d\n",myTime);
		printf("Waiting calculate Operation until given time\n");
		usleep(1000000*(*myTime));/*waiting client given times */

		calculateOperatipn(fifoName1,operation,numbers,size);/*calcululate operation and sent to client */
		printf("New Client Waiting Press Quit CTRL-C\n");

		


		*oldLen=*oldLen+1;
		
		 exit(1);
    }
    else
    {
    	/*making fifo */
    	int tempPid;
    	if(*myLen<=0 && *childCounter>0)
    	{
    		unlink(chrPtrFIFO_FILE);
    		exit(1);
    	}

    	close(fd[1]);

    	*myLen=read(fd[0],&tempPid,sizeof(int));
    	if(*myLen<=0)
    	{
    		perror("Failed to read ");
    		exit(1);
    	}

    	close(fd[1]);
    	//printf("close 1 %d\n",close(fd[1]));
    	//printf("Connecting client \n");


    	/*reading with pipe client fifo name from childs math server*/
    	*myLen=read(fd[0],clientFifo,sizeof(clientFifo));
    	if(*myLen<=0)
    	{
    		perror("Failed to read ");
    		exit(1);
    	}
    	
    	
    	childProcess[*childCounter]=tempPid;
    	*childCounter=*childCounter+1;
    	printf("client fifo %s\n",clientFifo);

    	fifo1=open(chrPtrFIFO_FILE,O_WRONLY);/*main fifo opening*/
    	if(fifo1<0)
    	{
    		perror("fifo open");
    		exit(1);
    	}
    	/*client fifo name sends client */
    	if(write(fifo1,clientFifo,sizeof(clientFifo))<0)/*write client fifo name to main fifo */
    	{
    		perror("Failed to write");
    		exit(1);
    	}
    	close(fifo1);

        wait(NULL);/* parent child lari bekler. */
     
        unlink(clientFifo);
        return 0;
       
    }
    return -1;
    

}
/* Signal Handle */
void handle( int sig)
{
	int i,clintPidT=*clientPid;
	char deleteFifoNames[BUFSIZE];
	unlink(chrPtrFIFO_FILE); /*Unlink fifo file */
	for(i=0;i<*childCounter;i++)
	{
		sprintf(deleteFifoNames,"%s%d","FIFO.",childProcess[i]);
		unlink(deleteFifoNames);
	}
	kill(clintPidT,SIGKILL);/*Kill client process */
	killpg(getpgrp(),SIGINT);/*kill all zombi process*/
	
	exit(sig);
}

/* Signal PIPE */
void signalPipeHandle(int signo)
{
	printf("***Client Killed***\n");
	//exit(1);
}

void my_sigchld_handler(int sig) 
{ 
	pid_t p; int status;
 	while ((p=waitpid(-1, &status, WNOHANG)) != -1) 
 	{ /* Handle the death of pid p */ 

 	}
} 
/*taking on internet */
void  ALARMhandler(int sig)
{
	int fdx[2],fifo;
  signal(SIGALRM, SIG_IGN);          /* ignore this signal       */
  //server(fdx,fifo);
  signal(SIGALRM, ALARMhandler);     /* reinstall the handler    */
}


void mySleep(clock_t sec) 
// clock_t is a like typedef unsigned int clock_t. Use clock_t instead of integer in this context 
{ 
	int flag=0;
	int fdx[2];
	clock_t start_time = clock(); 
	
	//printf("%ld\n",start_time);
	
	sec=30;
	clock_t end_time = sec * 1000000 + start_time;

	printf("Waiting Client to Communication\n");

		
		

		
		start_time = clock();
		sec=5;
		*myLen=1;
		end_time = sec * 1000000 + start_time;
		//server(fdx,fifo);
		int c=0;
		server(fdx,fifo);
		while(clock() <= end_time)
		{
		 		
		 		int i=0,temp=*childCounter;
		 		/*if(temp==*childCounter){
		 			printf("girdi\n");
		 			server(fdx,fifo);
		 		}*/
		 		for(i=0;i<*childCounter;i++)
		 			server(fdx,fifo);
		 		
		 		
		 		//alarm(30);
		 		
		 		
		 		/*for(i=0;i<*childCounter;i++)
		 		{
		 			printf("%d\n",childProcess[i]);
		 		}*/
	
		}



	return;
	

	

}
