
/*Ramazan  Ozdemir Client */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define BUFSIZE 1024

void sigHandle(int sig);/*signal handler */

void signalPipeHandle(int signo);

/*taking logfile arguments size operation name and integer array returns an integer array*/
int argumentsHandle(FILE *logp,char *argv[],int argc,char operation[],int *numbers);


int fifo1;
int fd1; /* mkfifo icin */
char FIFO_FILE[BUFSIZE];/* Fifo name icin tanimlanan degisken */
char FIFO_FILE1[BUFSIZE];/* Fifo name icin tanimlanan degisken */
char *childKill;/* Serverda bagli oldugu childin pid ini ogrenmek icin tanimlandi */
FILE* logfp;/* logfile icin */


int main(int argc, char *argv[])
{
	int fifo,fifo1;
	int len;/* read kontrolu */
	char buf[BUFSIZE];/* Yazma islemi icin gerekli string */
	char buf2[BUFSIZE];/* Okuma islemi icin gerekli string */
	int *numPtr;
	int myTime;
	int tempInt=0;
	char logName[BUFSIZE];

	/* calistirilirken 3 arguman olmasi gerekir */
    if ( argc >12)
    {
        fprintf(stderr, "Usage: %s fifoName <Time#> operation k1 k2 ... kn max k8\n", argv[0]);
        exit(-1);
    }
   
    sprintf(logName,"logfile.%d.log",getpid());/*unique log file acilir*/
  	logfp=fopen(logName,"w");
  	if(logfp==NULL)
  	{
  		perror("File not open");
  		exit(1);
  	}
	sprintf(FIFO_FILE,"%s",argv[1]);/* Fifo Name */

    /* Ctrl + C   sinyali yakalanir */
  	signal(SIGINT,sigHandle);
    signal(SIGPIPE, signalPipeHandle);
    /* Fifo acilir */
	

	
/*****************************************/
	/* serverin integral clientine veri gonderdigi fifo olusturulur */

    printf("CALİSİYOR...\n");
    printf("WAİTİNG SERVER...\n");
	while(1){

		/*degiskenler*/
		char operation[BUFSIZE];
		char clientFifo[BUFSIZE];
		int numbers[12];
		int size=0;
		int i=0;
		int clientPid=getpid();

		char logContext[BUFSIZE];

		size= argumentsHandle(logfp,argv,argc,operation,numbers);/*argumanlar check yapiliypr*/

		myTime=atoi(argv[2]);

		//fprintf(stderr, "%d\n",size);

		fifo = open( FIFO_FILE, O_WRONLY);/*main fifo aciliyro*/
		if( fifo < 0 ) {
			perror( "FIFO open" );
			exit(1);
		}
		
		
		/* servera gerekli veriler yazilir */
		
		strcpy(buf,operation);

		if( write( fifo, buf, sizeof(buf) ) < 0 )
		{
			perror( "child write to FIFO" );
			exit(1);
		}

		close(fifo);
	
		/* serverden yazilani okur. */

		fifo = open( FIFO_FILE, O_RDONLY);
		if( fifo < 0 ) {
			perror( "FIFO open" );
			exit(1);
		}
	

		if(read(fifo,clientFifo,sizeof(clientFifo))<0)
		{
			perror( "child read to FIFO" );
			exit(1);
		}

		printf("client fifo name %s\n",clientFifo);
		close(fifo);

		printf("Waiting Server For Calculation\n");

		fifo = open( clientFifo, O_WRONLY);
		if( fifo < 0 ) {
			perror( "FIFO open" );
			exit(1);
		}

		if( write( fifo, &clientPid, sizeof(int) ) < 0 )/*sent server pid num */
		{
				perror( "child write to FIFO" );
				exit(1);
		}

		if( write( fifo, &myTime, sizeof(int) ) < 0 )/*sent server time */
		{
				perror( "child write to FIFO" );
				exit(1);
		}

		if( write( fifo,&size, sizeof(int) ) < 0 )/*sent server  arguments size and argumants*/
		{
			perror( "child write to FIFO" );
			exit(1);
		}
		for(i=0;i<size;i++)
		{
			
			tempInt=numbers[i];
			if( write( fifo, &tempInt, sizeof(int) ) < 0 )
			{
				perror( "child write to FIFO" );
				exit(1);
			}
		}
		
			
		
		close(fifo);

		fifo = open( clientFifo, O_RDONLY);
		if( fifo < 0 ) {
			perror( "FIFO open" );
			exit(1);
		}
		
		if( read( fifo, logContext, sizeof(logContext) ) < 0 )/*hesap sonuclarini okur */
		{
				perror( "child read to FIFO" );
				exit(1);
		}
		//printf("%s\n",logContext);
		fprintf(logfp,"Successfull\n" "request pid =%d \n%s\n",getpid(),logContext);
		fclose(logfp);
		close(fifo);
	

		
			
			
		
		
		fprintf(stderr,"Client quit ...\n");
		exit(0);
	}
/***********************************************/
	return 0;
}
/* Signal Handle */
void sigHandle( int sig)
{
	fprintf(logfp, "Ctrl-C pressed and quit\n");
	exit(sig);
}

/* Signal PIPE */
void signalPipeHandle(int signo)
{
	printf("***Client Killed***\n");
}

/*taking argumants and handled */
int argumentsHandle(FILE *logp,char *argv[],int argc,char operation[],int *numbers)
{

	int size=0;
	int count=0;
	
	if(argc<=4 || atoi(argv[2])<0 || argc>12)
	{
		fprintf(stderr, "Usage: ./client mainFifoName operation  num1,...,numk max num 8\n");

        exit(-1);
	}
	//perror("erro1");

	//perror("erro1");
	strcpy(operation,argv[3]);
	//perror("erro1");
	if(strcmp(argv[3],"operation2")==0 && argc<=12 && (argc-4)%3==0)/*operation2 must have 3 ve katlari eleman sayasi*/
	{
		
		int sum=0;
		for(size=0;size<argc-4;size++)
		{
			sum+=atoi(argv[size+4]);
			numbers[size]=atoi(argv[size+4]);

		}
		if(sum<=0)
		{
			perror("Sum of element zero");
			fprintf(logp,"Sum of element zero\n");
			exit(1);
		}
		return size;

	}
	else if(strcmp(argv[3],"operation1")==0 && argc<=12  && (argc-4)%3==0)/*operation1 must have 3 ve katlari eleman sayasi*/
	{
		
		if(atoi(argv[argc-1])==0)
		{
				perror("Division by zero");
				fprintf(logp, "Division by zero\n");
				exit(1);
		}
				
		for(size=0;size<argc-4;size++)
		{
			
			numbers[size]=atoi(argv[size+4]);

		}
		
		return size;
	}
	else if(strcmp(argv[3],"operation3")==0 && argc<=12 && (argc-4)%3==0)/*operation2 must have 3 ve katlari eleman sayasi*/
	{

		for(size=0;size<argc-4;size++)
		{
			numbers[size]=atoi(argv[size+4]);

		}
		int a,b,c;

		a=atoi(argv[4]);
		b=atoi(argv[5]);
		c=atoi(argv[6]);
		if(b*b-4*a*c<0)
		{
			perror("Delta is Negative");
			fprintf(logp, "Delta is Negative");
			exit(1);
		}
		if(size==10){
			a=atoi(argv[7]);
			b=atoi(argv[8]);
			c=atoi(argv[9]);
			if(b*b-4*a*c<0)
			{
				perror("Delta is Negative");
				fprintf(logp, "Delta is Negative");
				exit(1);
			}
		}
		return size;
	}

	else if((strcmp(argv[3],"operation4")==0 && argc<=12  && (argc-4)%4==0))/*operation4 must have 4 ve katlari eleman sayasi*/
	{

		for(size=0;size<argc-4;size++)
		{
			numbers[size]=atoi(argv[size+4]);

		}
		return size;
	}
	else
	{
		{
			perror("Undefined operation");
			fprintf(logp, "Undefined operation\n");
			exit(1);
		}
	}

}