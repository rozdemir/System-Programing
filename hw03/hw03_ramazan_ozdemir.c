/*
/*				 111044051_Ramazan_Ozdemir.c 					*
/*																*
/*		   				Count							*
/*																*
/*	usage:$  ./hw03 directory		*
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
#include <fcntl.h>


#ifndef MAXPATHLEN
#define MAXPATHLEN 255
#endif

#ifndef PIPE_BUF
#define PIPE_BUF 1024
#endif
#define BUFSIZE 1024



#define LOGFILE "logFile.txt"

/*Static degiskenler */
static int *totalWords=0;
static int *totalChildProcess=0;
static int totalFile=0;
static int totalDirectory=0;
static int sizeOfMyWords=0;

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

/*it takes array and return all words in it */
char ** readWords(char **arr,int arraySizeX,int wordCount);

/*it is takes array and fifo arguments write to fifo file  return error codes */
int addWords(int fd,char **words,int arraySizeX);


/*it is read fifo file  and writing temp file all words taking fifo argument*/
int getWords(int fd);


/*it is reading temp file and adding words array after that delete temp file return set words size*/
int readWordsFile(char **words,int *wordsCount);


/*it is checking a word is in words array if is contain return its index else return -1*/
int isContain(char **words,char *word,int arraySizeX);


/*taking words and int array of how many of them and write after that write to logFile */
int addLogFile(char **words,int *wordsCount,int arraySizeX);


/*it takes main arguments */
int main(int argc, char **argv)
{
	/* 1. 2. argumanalr duzgun ve bos olmamali */
	/* calistirilabilir dosya ile file name arasinda . */
	int fd[2];
	int boyutX,boyutY,i;
	char buf[PIPE_BUF];
	char **words;
	int *wordsCount;
	if ((argv[1] == NULL) || argc!=2)
	{
		printf("Yanlis Kullanim\n");
		printf("Dogru yazim sekli\n");
		printf(" ./hw03 directory\n");
	    	return(-1);
	}


	/*   Taking on internet stackoverflow   for changing data value in processes */
   	totalWords = mmap(NULL, sizeof *totalWords, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	totalChildProcess = mmap(NULL, sizeof *totalChildProcess, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	/*same file if exits it is removes */
	unlink("tempLog.txt");
	unlink("myfifo");

	/* Printing Console al information about program*/
	countWords(argv[1]);
	printf("Total  words                          = %d\n",*totalWords);
	printf("Total child process        	      = %d\n",*totalChildProcess);
	printf("Total File which is search            = %d\n",totalFile);
	printf("Total Directory  in given directory   = %d\n",totalDirectory);


	unlink("myfifo");
	words = Make2DintArray(*totalWords,BUFSIZE);
	wordsCount=(int *) malloc(sizeof(int)*(*totalWords));
	int size = readWordsFile(words,wordsCount);
	addLogFile(words,wordsCount,size);
	unlink("tempLog.txt");
	printf("\nTotal Union Words Num                = %d\n",size);
	freeArray(words,*totalWords);
	free(wordsCount);


	return 0;
}

/* Find Num of word given directory */
int countWords(char *fileDir)
{



	
		int fdx;

		int boyutX,boyutY; /* max satir boyutulari */
		char **arr,**words; /* grekli array*/

		int i=0,j=0; /* gerekli degiskenler. */
		int lineNum;
		int total=0;
	
	
		char buf[BUFSIZE];
	/*recursively find file inside file taking on internet */
    	struct stat stDirInf;
    	struct dirent * stAllFiles;
    	DIR * stDirIn;
    	pid_t  pid;
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
    	if ((stDirIn = opendir( szDirectory)) == NULL) /* klasor acilir */
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

        /* klasor mu diye bakilir, kalsor ise onunda icine girilir. */
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
                /*printf("pid:%ld , %s\n",(long)getpid(),szFullName);*/
		/*child process sayisi arttirilir*/
          	 	*totalChildProcess=*totalChildProcess+1;
		

				boyutX= getLineNum(szFullName);
				boyutY = getBoyut(szFullName);
		    	arr=Make2DintArray(boyutX+1,boyutY+1);
				/*dosya okunur */
		    	readData(szFullName,arr,boyutX);
				/*total words sayisi bulunup toplama a eklenir*/
				total=countWordsInArray(arr,boyutX);

				*totalWords=*totalWords+total;
				/*array a tekrar datalar okunur*/
				readData(szFullName,arr,boyutX);

				/*bu datalar words arrayine eklenir */
				words = readWords(arr,boyutX,total);
				
				/*fifo la parent a gonderlilir*/
				addWords(fdx,words,total);
				
				
				//printf(" %s inside total words %d\n",szFullName,total);
				
				
				/*aldigimiz yer geri verilir*/
		 		
				freeArray(words,total);
				freeArray(arr,boyutX+1);	
                exit(1); /* exit ile prosesler oldurulur. */
            }
            else
            {
            	/*making fifo */
            	if (mkfifo("myfifo", 0666) == -1) 
					perror("Failed to create myfifo");
				/*reading fifo file */
            	getWords(fdx);

                wait(NULL);/* parent child lari bekler. */
                unlink("myfifo");
            }
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
		tok=strtok(arr[i]," .,:;\t?");
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
			
			tok=strtok(NULL," .,:;\t?");
			
		}
		
		
			
	
	}
	return count;

}

/*verielen array da ki kelimeleri bulup array a ekleyip dondurur*/

char ** readWords(char **arr,int arraySizeX,int wordCount)
{

	int count=wordCount,i,j;/*gerekli degiskenler*/
	int tempBool=-1;
	
	char **words = Make2DintArray(count,1024);
	count=0 ;
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
				//fprintf(stderr, "%s \n",words[count]);
				count++;
				//printf("%s\n",tok);
				
			}
			
			
			tok=strtok(NULL," \t.,;:?");
		
			
		}
		
		
			
	
	}
	
	return words;
	
		

}

/*adding words to fifo file */
int addWords(int fd,char **words,int arraySizeX)
{

	
	 int i;


	
	if ((fd = open("myfifo",O_WRONLY)) == -1) 
	{ 
		perror("Client failed to open log fifo for writing"); 

		return 1; 
	}
	/*first write  size and then write  words which wwere come  */
	write(fd, &arraySizeX, sizeof(int));
	
	for(i=0;i<arraySizeX;i++){
		char buf[PIPE_BUF];
		
		//sprintf(buf,"%s",words[i]);
		strcpy(buf,words[i]);

		write(fd, buf, sizeof(buf));
	}
	
	close(fd);
	

}

/*taking parents words from childs with fifo and writing on temp file  */
int getWords(int fd)
{
	char buf[PIPE_BUF];
	int count=0;
	
	
	/*opening fifo file */
	if ((fd = open("myfifo", O_RDONLY)) == -1) 
	{ 
		perror("Client failed to open log fifo for reading"); 
		return 1; 
	}
	/*x size of local words in file */
	int x,i,kelime=0;
	read(fd, &x, sizeof(int));
	sizeOfMyWords+=x;
	for(i=0;i<x;i++){
		while(read(fd, buf, sizeof(buf))>0){
			FILE *outp;
			outp=fopen("tempLog.txt","ab");
			
			fprintf(outp,"%s\n",buf);
			fclose(outp);
			
		}
	
	}
	
	close(fd);
	
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