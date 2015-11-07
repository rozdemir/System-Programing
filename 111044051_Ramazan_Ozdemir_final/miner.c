
/*Ramazan  Ozdemir Client */


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
 
#define SHMSZ     1024
#define BUFSIZE 1024
#define MAXWORDS 1000000

typedef struct server
{
	int sizeS;
	char cDirectory[BUFSIZE][BUFSIZE];
	
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




void handle( int sig);

int main2(char *directory);
/*Takes file directory and return integer */
int countWords(char *fileDir);


int addWordsFile(char **words,int arraySizeX);

/*it takes fileName char* */
/* return max line boyut  */
int getBoyut(char *file);

/*it takes filename char* */
/* return toplam satir sayisi */
int getLineNum(char *file);

/*it takes char ** and arraySizeX return void  */
/* verilen iki boyutlu array i free yapar  */
void freeArray(char **arr, int arraysSizeX);

/*is takes two size as a integer and return char ** */
/* verilen iki boyutlu array a size lar kadar yer alir */
char** Make2DintArray(int arraySizeX, int arraySizeY);

/*is takes fileName and char ** and arraySize as a integer return void */
/* verilen array a dosyadan okuma yapar  */
void readData(char *file,char **arr,int arraySizeX);


/*it takes array and it sizex return total words inside*/
int countWordsInArray(char **arr,int arraySizeX);


/*it takes array and return all words in it */
char ** readWords(char **arr,int arraySizeX,int wordCount);


/*it is reading temp file and adding words array after that delete temp file return set words size*/
int readWordsFile(char **words,int *wordsCount);


/*it is checking a word is in words array if is contain return its index else return -1*/
int isContain(char **words,char *word,int arraySizeX);


void* threadOperation(void* data);
char** Make2DintArray(int arraySizeX, int arraySizeY);
void freeArray(char **arr, int arraysSizeX);
int client2();


int fifo1;
int fd1; /* mkfifo icin */
char FIFO_FILE[BUFSIZE];/* Fifo name icin tanimlanan degisken */
char FIFO_FILE1[BUFSIZE];/* Fifo name icin tanimlanan degisken */
char *childKill;/* Serverda bagli oldugu childin pid ini ogrenmek icin tanimlandi */
FILE* logfp;/* logfile icin */
char LOGFILE[BUFSIZE];

int directoryNum=0;
int subDirectoryNum=0;


char **directory;

char subDirectory[1024][1024];



/*Static degiskenler */
static int totalWords=0;
static int totalFile=0;
static int totalDirectory=0;
static int sizeOfMyWords=0;

int totalReadFile=0;
pthread_attr_t tattr; /* Thread Attrebute */
pthread_t  thread_id[PIPE_BUF]; /* Thread ID */
int intThread_count=(-1);
char fileNames[PIPE_BUF][PIPE_BUF];
pthread_mutex_t lock;

char **wordFind;
int *wordsCount;

/* olusturulan thread e gonderilern void pointer fonksiyon */



server sendDirectories;
words * kelime;
words **allKelime;
allWords *tmp;
int globalSize=0;

allWords *t2;
clock_t startTime;


int main(int argc, char *argv[])
{

	if(argc!=2 || atoi(argv[1])<1){
		printf("Usuge : ./miner n(integer) and must be greater than zero\n");
		return 1;
	}
	printf("------------------ Client is started ... -----------------\n");
	startTime=clock();
	t2=(allWords*) malloc(sizeof(allWords));

	int fifo;
	int pid=getpid();
	sprintf(LOGFILE,"log.%d",getpid());
	directoryNum=atoi(argv[1]);
	directory=Make2DintArray(directoryNum,1024);

	printf("Connecting Server ....\n");

	

	sleep(1);
	int i=0;
	strcpy(FIFO_FILE,"fifo");

	fifo=open(FIFO_FILE,O_WRONLY);

	if(fifo==-1)
	{
		perror("exit exception");
		exit(1);
	}

	if(write(fifo,&directoryNum,sizeof(int))<0)
	{
		perror("write error \n");
		exit(1);
	
	}

	if(write(fifo,&pid,sizeof(int))<0)
	{
		perror("write error \n");
		exit(1);
	
	}		
	close(fifo);


	fifo=open(FIFO_FILE,O_RDONLY);

	if(fifo==-1)
	{
		perror("exit exception");
		exit(1);
	}
	
	for(i=0;i<directoryNum;i++){
		if(read(fifo,directory[i],sizeof(directory[i]))<0)
		{
			perror("write error \n");
			exit(1);
		
		}
		if(strcmp(directory[0],"error")==0)
		{
			perror("pending");
			exit(0);
		}
		printf("%s\n",directory[i]);
	}	
	close(fifo);
	client2();
/***********************************************/
	printf("------------------   Quit Server...-----------------\n");
	freeArray(directory,directoryNum);
	unlink("tempLog.txt");
	return 0;
}

int client2()
{

	int shmid,myI;
	int shmid2;int shmid3;int shmid4;
	key_t key1,key2,key3,key4;
	int i,fifo;
	points *clientPoint;
	allWords* t3;


	key1=1111;
	key1=ftok("./",'x');
	/*veri gonderilmeli*/
	if ((shmid = shmget(key1,sizeof(struct allWords),IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		return 1;
	}

	if ((tmp = (allWords*)shmat(shmid, NULL, 0)) == (void *) -1) {
		perror("shmat");
		return 1;
	}

	if(tmp<0)
	{
		perror("cant create");
		return 0;
	}


	key2=ftok("./",'y');
	/*veri alinmasi lazim*/
	if ((shmid2 = shmget(key2,sizeof(struct points), IPC_CREAT |0666)) < 0) {
		perror("shmget");
		return 1;
	}

	if ((clientPoint = (points*)shmat(shmid2, NULL, 0)) == (void *) -1) {
		perror("shmat");
		return 1;
	}

	if(clientPoint<0)
	{
		perror("cant create");
		return 0;
	}

	//memset(t2,0,BUFSIZE);
	int k;
	printf("direc : %d\n",directoryNum);
	//t2=tmp;
	logfp=fopen(LOGFILE,"w");
	if(logfp==NULL)
		exit(1);
	fprintf(logfp, "Time of start : %ld\n",startTime);
	for (i = 0; i < directoryNum; ++i)
	{
	
		//t2=tmp;
		//sleep(1);

		if(main2(directory[i])==1)
		{	for(k=0;k<t2->size;k++)
			{
				//
				
				strcpy(tmp->words[k],t2->words[k]);
				tmp->wordsSize[k]=t2->wordsSize[k];
				tmp->size=t2->size;

			}
			while(clientPoint->isSent!=1)
				sleep(1);
			//printf("clientPoint %d\n",clientPoint->point[0]);
			

			for(i=0;i<clientPoint->size;i++)
			{
				fprintf(logfp,"%d. %s  %d times  %d coins \n",i+1,tmp->words[i],tmp->wordsSize[i],clientPoint->point[i]);	
			}
			fprintf(logfp, "time of finish %ld\n",clock());
			fprintf(logfp, "Searced file num : %d\n",totalFile);
			fprintf(logfp, "Number Of Pending File %d\n",subDirectoryNum);
			//*tmp=*t2;
			//printf("gelmedi %s\n",t2->words[0]);
		}	
		
	}

	


	fclose(logfp);
	

	

 	/*sending sub directoryies to server */
 	fifo=open(FIFO_FILE,O_WRONLY);

	if(fifo==-1)
	{
		perror("exit exception");
		exit(1);
	}
	if(write(fifo,&subDirectoryNum,sizeof(int))<0)
		{
			perror("write error \n");
			exit(1);
		
		}

 	for(i=0;i<subDirectoryNum;i++){
		if(write(fifo,subDirectory[i],sizeof(subDirectory[i]))<0)
		{
			perror("write error \n");
			exit(1);
		
		}
		
		
	}	
	close(fifo);
	/*if(shmdt(myClient) != 0)
		fprintf(stderr, "Could not close memory segment.\n");
	if(shmdt(myServer) != 0)
		fprintf(stderr, "Could not close memory segment.\n");*/

	if(shmdt(tmp) != 0)
		fprintf(stderr, "Could not close memory segment.\n");
	if(shmdt(clientPoint) != 0)
		fprintf(stderr, "Could not close memory segment.\n");
	free(t2);

}

/*it takes main arguments */
int main2(char *myDirectory)
{
	/* 1. 2. argumanalr duzgun ve bos olmamali */
	/* calistirilabilir dosya ile file name arasinda . */
	
	

	unlink("tempLog.txt");
	
	clock_t start;
    double duration;

    start = clock();
	/* Printing Console al information about program*/
	if(countWords(myDirectory)!=-1){
	//printf("total read file %d\n",totalReadFile);
	
		int i=0,temp=0,status=0,fdx;
		int err;
		if (pthread_mutex_init(&lock, NULL) != 0)/*mutex init */
	    {
	        printf("\n mutex init failed\n");
	        return 0;
	    }
		for(i;i<totalReadFile;i++){
	            intThread_count++;/* Thread sayisini tutar. */
				if (pthread_create(&thread_id[i],NULL, threadOperation, &fileNames[i])!=0)
					fprintf(stderr, "Failed to create thread: %s\n", strerror(err));

				
		}
		for(i=0;i<totalReadFile;i++)
		{
			
			if (err = pthread_join(thread_id[i], NULL))
			    {
					fprintf(stderr, "Failed to join thread: %s\n", strerror(err));
					return -1;
				}
				

				
		}
		/*mutex destroyed*/
		pthread_mutex_destroy(&lock);

		
				
		/*printf("Total  words                          = %d\n",totalWords);
		printf("Total Threads       	              = %d\n",intThread_count+1);
		printf("Total File which is search            = %d\n",totalFile);
		printf("Total Directory  in given directory   = %d\n",totalDirectory);*/
		
		
		int k;
		wordFind = Make2DintArray(totalWords,BUFSIZE);
		wordsCount=(int *) malloc(sizeof(int)*(totalWords));
		memset(wordsCount,0,sizeof(wordsCount));
		int size = readWordsFile(wordFind,wordsCount);
		
		//printf("size %d\n",size );
		//kelime=words_create(wordFind,wordsCount,size);

		for(k=0;k<size;k++)
		{
				//perror("error1");
				//printf("%d\n",wordsCount[k]);
				strcpy(t2->words[k],wordFind[k]);
				//perror("error2");
				t2->wordsSize[k]=wordsCount[k];
				
				//perror("error3");

		}
		t2->size=size;
		//word_destroy(kelime);
		//printf("%s\n",kelime->words[0]);
		//tmp[globalSize]=words_create(wordFind,wordsCount,size);
		//globalSize++;
		//printf("%s\n",kelime->words[1]);
		



		//printf("\nTotal Union Words Num                = %d\n",size);

		freeArray(wordFind,totalWords);
		free(wordsCount);
		//
		
		duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
		printf("Total Duration time is                = %f\n",duration);
		return 1;
	}
	
	
	return -1;
}

/* Find Num of word given directory */
int countWords(char *fileDir)
{


	int boyutX,boyutY; /* max satir boyutulari */
	char **arr; /* grekli array*/

	int i=0,j=0; /* gerekli degiskenler. */
	int lineNum;
	int total=0;
	

	/*recursively find file inside file taking on internet */
    struct stat stDirInf;
    struct dirent * stAllFiles;
    DIR * stDirIn;
    
    char szFullName[MAXPATHLEN]; /* dosyalar icin */
    char szDirectory[MAXPATHLEN];
  	struct stat stFileInf;
    	/*directory asil directory e kopyalanir*/
	strncpy( szDirectory,fileDir, MAXPATHLEN - 1 );

	/* Error Handling */
	if (lstat( szDirectory, &stDirInf) < 0)
	{
    	perror (szDirectory);
    	return -1;
	}

	if (!S_ISDIR(stDirInf.st_mode))
    	return;
	if ((stDirIn = opendir( szDirectory)) == NULL) /* klasor open */
	{
    	perror( szDirectory );
    	return  -1;
	}

    /* klasorun icerigi okunur */
    while (( stAllFiles = readdir(stDirIn)) != NULL)
    {
	/*Dosya name yeni array a alinir*/
        sprintf(szFullName, "%s/%s", szDirectory, stAllFiles -> d_name );

        if (lstat(szFullName, &stFileInf) < 0)
           perror ( szFullName );

        /* klasor mu diye bakilir, klasor ise onunda icine girilir. */
        if (S_ISDIR(stFileInf.st_mode))
        {
            if ((strcmp(stAllFiles->d_name , "..")) && (strcmp(stAllFiles->d_name , ".")))
            {
                /* recursive olarak ic icedosyalara girilebilir. */
		 //total=*totalWords;
		/*directory icine girdigimiz icin file num ve directory num arttirilir */
		 		++totalDirectory;
		 		++totalFile;
                //countWords(szFullName);
                strcpy(subDirectory[subDirectoryNum],szFullName);
                subDirectoryNum++;
        
		 
            }
        }
        else /* dosya ise */
        {
	    /*file num yine arttilir*/
        	strcpy(fileNames[totalReadFile],szFullName);
	    	++totalFile;
	    	++totalReadFile;
                            		         	 						                  
        }

    }  /* end while */
    /* Dosyalar kapanir. */
    while ((closedir(stDirIn) == -1) && (errno == EINTR)) ;
 

    return total;
}


/* En uzun satirin kac karakterden olustugunu belirler */

int getLineNum(char *file)
{
    char c;
    int lineNum=0;
    FILE *inp= fopen(file,"r");
    /*file exits control*/
    if (inp == NULL)
	{
		perror("Error opening file");
	    exit(1);
	}

    int eof;

    eof = fscanf(inp,"%c",&c);

    while(eof!=EOF)
    {

        if(c=='\n')
            lineNum++;
       
        eof = fscanf(inp,"%c",&c);

    }
    fclose(inp);

    return lineNum;

}

int getBoyut(char *file)
{
	FILE * fp;

	/* Dosya acilir */
	fp=fopen(file,"r");

	/* Dosya acilmazsa yada yok ise hata verir ve program sonlanir */
	if (fp == NULL)
	{
		perror("Error opening file");
	    exit(1);
	}

	int max=0; /* Maximum satir uzunlugu */
	int say=1; /* gerekli sayic */
	int eoF; /*  while dongusu icin */
	char c;

	eoF = fscanf(fp,"%c",&c);

	/* dosya sonuna kadar okuma yapar */
	while(eoF != EOF)
	{
		eoF = fscanf(fp,"%c",&c);
		say++;

		/* Satir sonuna geldiginde satirdaki karakter sayisini degiskene atar ve sayaci sifirlar */
		if ((c == '\n')||(c == '\0'))
		{
			if (max < say)
			{
				max = say;
			}
			say = 0;
		}
	}

	/* Dosya kapatilir */
	fclose(fp);

	return max;
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

/*dosya dan okuma yapilir*/
void readData(char *file,char **arr,int arraySizeX)
{
    int i=0,j=0;
    char c='c';
    FILE *inp= fopen(file,"r");
/*Acilmazsa hata */
    if (inp == NULL)
	{
		perror("Error opening file");
	    exit(1);
	}
/*Onceden buldugumuz size a gore okuma yapilir */
    fscanf(inp,"%c",&c);

    for(i=0;i<arraySizeX;i++)
    {
        j=0;

        while(c!='\n')
        {
            arr[i][j] = c;
  
            fscanf(inp,"%c",&c);
 
            j++;

        }
        arr[i][j]='\0';
        
        fscanf(inp,"%c",&c);

    }
    fclose(inp);
}
// printing array two numbers

/* Turkce dil bilgisi kurallarina gore bir array da ki kelime sayisi bulunuyor*/
/* Ornegin ali, veli gibi kurallar */
/* Tab bir kelime degildir ve kelime icinde rakam varsa o da kelimeden sayilmaz*/
/*taking array and sizeX return total words in this array*/ 
int countWordsInArray(char **arr,int arraySizeX)
{
	int count=0,i,j;/*gerekli degiskenler*/
	int tempBool=-1;
	for(i=0;i<arraySizeX;i++)
	{
		
		j=0;
		char *tok;/*satir bosluga gore ayriliyor*/
		tok=strtok(arr[i]," .;:!?");
		while(tok!=NULL){	
			tempBool=0;
			j=0;
			/*ayirdigimiz kelime de bu karakterler varsa cikiliyor*/
			while(tok[j]!='\0' && tok[j] != '.' && tok[j] != ',' && tok[j]!='\t' 
							&& tok[j]!=':' && tok[j]!=';' && tok[j]!='?' && tok[j]!='!')
			{
				/*harf den baska bisey varsa hata kontrolu*/
				if( (tok[j]<'a' || tok[j]>'z') && (tok[j]<'A' || tok[j]>'Z' ) )
					tempBool=-1;
				j++;
			}
			/*Kelime hatali degilse counter arttirilir*/
			if(tempBool==0){
				count++;
				//printf("%s\n",tok);
				
			}
			/*eger kelime tab sa arttirilimaz*/
			if(strlen(tok)==1 && tok[0]=='\t')
				count--;
			
			tok=strtok(NULL," .;:!?");
			
		}
		
		
			
	
	}
	return count;

}


/* olusturulan thread e gonderilern void pointer fonksiyon */
void* threadOperation(void* data)
{
			

			pthread_mutex_lock(&lock);/*mutex locked*/
			clock_t start;
    		double duration;

    		start = clock();
			
			char *szFullName=(char*)data;
			int boyutX,boyutY,total;
			char **arr,**words;
			int fdx;
			
			
		/*total words sayisi bulunup toplama a eklenir*/
    		//printf("Thread_id: %lu\n", pthread_self());
		
			boyutX= getLineNum(data);
			//printf(" line %d\n",boyutX);
			boyutY = getBoyut(data);
    		arr=Make2DintArray(boyutX+1,boyutY+1);
			/*dosya okunur */
    		readData(data,arr,boyutX);

    		total=countWordsInArray(arr,boyutX);
			/* Thread resource lari geri birakilir */
			
			totalWords=totalWords+total;
			
			//fprintf(stderr,"%s  inside total words %d\n",(char*)data,total);
			readData(data,arr,boyutX);
			words = readWords(arr,boyutX,total);
				
	
			
			
			addWordsFile(words,total);

			freeArray(arr,boyutX+1);
			freeArray(words,total);
			
			//pthread_detach(pthread_self());
			duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
			//printf("duration time is %f\n",duration);
			pthread_mutex_unlock(&lock);/*mutex acilir*/
			
			//pthread_exit(NULL);
			return NULL;
 
			
}

/*verielen array da ki kelimeleri bulup array a ekleyip dondurur*/

char ** readWords(char **arr,int arraySizeX,int wordCount)
{

	int count=wordCount,i,j;/*gerekli degiskenler*/
	int tempBool=-1;
	
	char **words = Make2DintArray(count,1024);
	count=0 ;
	//printf("read worss %d\n",arraySizeX);
	for(i=0;i<arraySizeX;i++)
	{
		
		j=0;
		//printf("\n%s\n",arr[i]);
		char *tok;/*satir bosluga gore ayriliyor*/
		tok=strtok(arr[i]," \t.,;:?");
		while(tok!=NULL){	
			tempBool=0;
			j=0;
			/*ayirdigimiz kelime de bu karakterler varsa cikiliyor*/
			while(tok[j]!='\0' && tok[j] != '.' && tok[j] != ',' && tok[j]!='\t' 
							&& tok[j]!=':' && tok[j]!=';' && tok[j]!='?' && tok[j]!='!')
			{
				/*harf den baska bisey varsa hata kontrolu*/
				if( (tok[j]<'a' || tok[j]>'z') && (tok[j]<'A' || tok[j]>'Z' ) )
					tempBool=-1;
				j++;
			}
			/*Kelime hatali degilse counter arttirilir*/
			if(tempBool==0){
				
				strcpy(words[count],tok);
				//printf("%s\n",tok);

				//fprintf(stderr, "%s \n",words[count]);
				count++;
				//printf("%s\n",tok);
				
			}
			
			
			tok=strtok(NULL," \t.,;:?");
		
			
		}
		
		
			
	
	}
	
	return words;
	
		

}


int addWordsFile(char **words,int arraySizeX)
{

	int i=0;
	FILE *outp;
	outp=fopen("tempLog.txt","ab");
	for(i=0;i<arraySizeX;i++){
	
			
			
			fprintf(outp,"%s\n",words[i]);
			
			
	}
	fclose(outp);
}



/*temp file reading*/
int readWordsFile(char **words,int *wordsCount)
{
	int i=0,size=0,index=0;
	char buffer[BUFSIZE];
	char bufferWord[BUFSIZE];
	FILE* inp=fopen("tempLog.txt","r");
	if(inp==NULL)
	{
		perror("file doesnt opened");
		exit(1);
	}
	/*temp file reading line by line and adding array if isnt contain else ++ that words size*/
	while(fgets(buffer,BUFSIZE,inp)!=NULL)
	{
		/*for not to read \n*/
		char bufferWord[BUFSIZE];
		buffer[strlen(buffer)-1]='\0';
		strcpy(bufferWord,buffer);

		/*printf("%s\n",bufferWord);*/
		index=isContain(words,bufferWord,i);
		if(index==-1)
		{
			strcpy(words[i],bufferWord);
			wordsCount[i]=1;
			i++;
		}
		else
		{
			wordsCount[index]+=1;
		}
			
		

		
	}
	fclose(inp);
	return i;
}
/*is checking this word is in words if contain return index else return -1*/
int isContain(char **words,char *word,int arraySizeX)
{
	int i=0;
	for(i=0;i<arraySizeX;i++)
	{
		if(strcasecmp(words[i],word)==0)
		{
			return i;
		}
	}
	return -1;
}





void handle( int sig)
{
	int i;
	fprintf(logfp, "CTRL-C Handled \n");
	
	if(t2!=NULL)
		free(t2);
	fclose(logfp);
	if(wordFind!=NULL)
		freeArray(wordFind,totalWords);
	if(wordsCount!=NULL)
		free(wordsCount);
	

	for(i=0;i<intThread_count+1;i++)
	{
		pthread_detach(thread_id[i]);
	}

	
	killpg(getpgrp(),SIGINT);/*kill all zombi process*/
	
	
	exit(sig);
}