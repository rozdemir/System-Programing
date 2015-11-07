/*
/*				 111044051_Ramazan_Ozdemir.c 					*
/*																*
/*		   				Count							*
/*																*
/*	usage:$  ./hw04 directory		*
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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 255
#endif

/*Static degiskenler */
static int totalWords=0;
static int totalFile=0;
static int totalDirectory=0;

int totalReadFile=0;
pthread_attr_t tattr; /* Thread Attrebute */
pthread_t  thread_id[PIPE_BUF]; /* Thread ID */
int intThread_count=(-1);
char fileNames[PIPE_BUF][PIPE_BUF];
pthread_mutex_t lock;

/*Takes file directory and return integer */
int countWords(char *fileDir);

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

/* olusturulan thread e gonderilern void pointer fonksiyon */
void* threadOperation(void* data);

/*it takes main arguments */
int main(int argc, char **argv)
{
	/* 1. 2. argumanalr duzgun ve bos olmamali */
	/* calistirilabilir dosya ile file name arasinda . */
	if ((argv[1] == NULL) || argc!=2)
	{
		printf("Yanlis Kullanim\n");
		printf("Dogru yazim sekli\n");
		printf(" ./hw04 directory\n");
	    	return(-1);
	}

	clock_t start;
    double duration;

    start = clock();
	/* Printing Console al information about program*/
	countWords(argv[1]);/**/
	//printf("total read file %d\n",totalReadFile);
	
	int i=0,temp=0,status=0;
	int err;
	if (pthread_mutex_init(&lock, NULL) != 0)/*mutex init */
    {
        printf("\n mutex init failed\n");
        return 1;
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
				return 1;
			}
	}
	/*mutex destroyed*/
	pthread_mutex_destroy(&lock);

	duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
			
	printf("Total  words                          = %d\n",totalWords);
	printf("Total Threads       	              = %d\n",intThread_count+1);
	printf("Total File which is search            = %d\n",totalFile);
	printf("Total Directory  in given directory   = %d\n",totalDirectory);
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
                countWords(szFullName);
		 
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
			char **arr;
			
			
		/*total words sayisi bulunup toplama a eklenir*/
    		printf("Thread_id: %lu\n", pthread_self());
		
			boyutX= getLineNum(data);
			boyutY = getBoyut(data);
    		arr=Make2DintArray(boyutX+1,boyutY+1);
			/*dosya okunur */
    		readData(data,arr,boyutX);
    		total=countWordsInArray(arr,boyutX);
			/* Thread resource lari geri birakilir */
			freeArray(arr,boyutX+1);
			totalWords=totalWords+total;
			
			fprintf(stderr,"%s  inside total words %d\n",(char*)data,total);
			
			//pthread_detach(pthread_self());
			duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
			printf("duration time is %f\n",duration);
			pthread_mutex_unlock(&lock);/*mutex acilir*/
			//pthread_exit(NULL);
			return NULL;
 
			
}


