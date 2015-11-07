/*
/*				 hw06_ramazan_Ozdemir.c 					*
/*																*
/*		   				Count							*
/*																*
/*	usage:$  ./hw06 directory		*
/*																*/

/*libraries*/
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
#include <semaphore.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 255
#endif

#define LOGFILE "logFile.txt"
#define FILENAMEFILE "file.txt"
#define BUFSIZE 1024
/*Static degiskenler */
static int totalWords=0;
static int totalFile=0;
static int totalDirectory=0;
static int sizeOfMyWords=0;

static int *totalReadFile=0;
pthread_attr_t tattr; /* Thread Attrebute */
pthread_t  thread_id[PIPE_BUF]; /* Thread ID */
int intThread_count=(-1);
static char fileNames[PIPE_BUF][PIPE_BUF];
pthread_mutex_t lock;
sem_t sem;

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


/*taking words and int array of how many of them and write after that write to logFile */
int addLogFile(char **words,int *wordsCount,int arraySizeX);
/* olusturulan thread e gonderilern void pointer fonksiyon */

void* threadOperation(void* data);

/*read static array file names from temp file */
void readFileNamesTempFile();

/*it takes main arguments */
int main(int argc, char **argv)
{
	/* 1. 2. argumanalr duzgun ve bos olmamali */
	/* calistirilabilir dosya ile file name arasinda . */
	if ((argv[1] == NULL) || argc!=2)
	{
		printf("Yanlis Kullanim\n");
		printf("Dogru yazim sekli\n");
		printf(" ./hw05 directory\n");
	    	return(-1);
	}
	totalReadFile = mmap(NULL, sizeof *totalReadFile, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	unlink("tempLog.txt");
	unlink(FILENAMEFILE);
	
	clock_t start;
    double duration;

    start = clock();
	/* Printing Console al information about program*/
	countWords(argv[1]);/**/
	readFileNamesTempFile();
	//printf("total read file %d\n",*totalReadFile);
	
	int i=0,temp=0,status=0,fdx;
	int err;
	if (sem_init(&sem, 0,1) != 0)/*semapohew init */
    {
        printf("\n semaphore failed\n");
        return 1;
    }

	for(i=0;i<*totalReadFile;i++){
            intThread_count++;/* Thread sayisini tutar. */
			if (pthread_create(&thread_id[i],NULL, threadOperation, &fileNames[i])!=0)
				fprintf(stderr, "Failed to create thread: %s\n", strerror(err));

			
	}
	for(i=0;i<*totalReadFile;i++)
	{
		
		if (err = pthread_join(thread_id[i], NULL))
		    {
				fprintf(stderr, "Failed to join thread: %s\n", strerror(err));
				return 1;
			}
			

			
	}
	/*semaphore destroyed*/
	sem_destroy(&sem);

	
			
	printf("Total  words                          = %d\n",totalWords);
	printf("Total Threads       	              = %d\n",intThread_count+1);
	printf("Total File which is search            = %d\n",totalFile);
	printf("Total Directory  in given directory   = %d\n",totalDirectory);
	
	char **words;
	int *wordsCount;
	words = Make2DintArray(totalWords,BUFSIZE);
	wordsCount=(int *) malloc(sizeof(int)*(totalWords));
	int size = readWordsFile(words,wordsCount);
	addLogFile(words,wordsCount,size);

	unlink("tempLog.txt");
	unlink(FILENAMEFILE);

	printf("\nTotal Union Words Num                = %d\n",size);
	freeArray(words,totalWords);
	free(wordsCount);
	duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
	printf("Total Duration time is                = %f\n",duration);
	
	return 0;
}

/* Find Num of word given directory */
int countWords(char *fileDir)
{

	int boyutX,boyutY; /* max satir boyutulari */
	char **arr; /* grekli array*/

	int i=0,j=0; /* gerekli degiskenler. */
	int lineNum;
	int total=0;
	int pid;
	

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
    	return;
	}

	if (!S_ISDIR(stDirInf.st_mode))
    	return;
	if ((stDirIn = opendir( szDirectory)) == NULL) /* klasor open */
	{
    	perror( szDirectory );
    	return;
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
	            	printf("child process id=%d  for %s \n",getpid(),szFullName);
	              	countWords(szFullName);
	                exit(1); /* exit ile prosesler oldurulur. */
	            }
	            else
	            {
	     	            	

	                wait(NULL);/* parent child lari bekler. */
	                
	            }
                
		 
            }
        }
        else /* dosya ise */
        {
	    /*file num yine arttilir*/
        	FILE *inp;
        	inp=fopen(FILENAMEFILE,"ab");
        	if(inp==NULL)
        	{
        		perror("exit file");
        		exit(1);
        	}
        	fprintf(inp, "%s\n",szFullName);
        	fclose(inp);
        	//strcpy(fileNames[*totalReadFile],szFullName);
	    	++totalFile;
	    	*totalReadFile=*totalReadFile+1;
                            		         	 						                  
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
			

			sem_wait(&sem);/*mutex locked*/
			clock_t start;
    		double duration;

    		start = clock();
			
			char *szFullName=(char*)data;
			int boyutX,boyutY,total;
			char **arr,**words;
			int fdx;
			
			
		/*total words sayisi bulunup toplama a eklenir*/
    		printf("Thread_id: %lu\n", pthread_self());
		
			boyutX= getLineNum(data);
			//printf(" line %d\n",boyutX);
			boyutY = getBoyut(data);
    		arr=Make2DintArray(boyutX+1,boyutY+1);
			/*dosya okunur */
    		readData(data,arr,boyutX);

    		total=countWordsInArray(arr,boyutX);
			/* Thread resource lari geri birakilir */
			
			totalWords=totalWords+total;
			
			fprintf(stderr,"%s  inside total words %d\n",(char*)data,total);
			readData(data,arr,boyutX);
			words = readWords(arr,boyutX,total);
				
	
			
			
			addWordsFile(words,total);

			freeArray(arr,boyutX+1);
			freeArray(words,total);
			
			//pthread_detach(pthread_self());
			duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
			printf("duration time is %f\n",duration);
			sem_post(&sem);/*mutex acilir*/
			
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

/*write words temp file */
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
/*given set words writing log file */
int addLogFile(char **words,int *wordsCount,int arraySizeX)
{
	FILE *log=fopen(LOGFILE,"w");
	int i;

	if(log==NULL)
	{
		perror("Couldnt open");
		exit(1);
	}
	for(i=0;i<arraySizeX;i++)
	{
		fprintf(log, "%s : %d\n",words[i],wordsCount[i]+1);
	}
	fclose(log);
}


/*read file names which writes by child*/
void readFileNamesTempFile()
{
	int i=0;
	char buf[PIPE_BUF];
	FILE *out;
	out=fopen(FILENAMEFILE,"r");
	//printf("read %d\n",*totalReadFile);
	for(i=0;i<*totalReadFile;i++)
	{
		fscanf(out,"%s",buf);
		//printf("%s",buf);
		//buf[strlen(buf)-1]='\0';
		
		strcpy(fileNames[i],buf);
	}
	fclose(out);
}