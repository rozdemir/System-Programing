/*
/*				 111044051_Ramazan_Ozdemir.c 					*
/*																*
/*		   				More Or Less							*
/*																*
/*	usage:$  ./111044051_Ramazan_Ozdemir a.txt linenum		*
/*																*/

/*libraries*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>


/* return a chracter from console without press entering */
int getUserChar();

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


/* it takes char **array and arrays sizex and takes two interval size 1 and size2  */
/* iki boyut arasinda array i print yapar */
int printBetweenSize(char **arr,int arraySizeX,int arraySize1,int arraySize2,int givingNum);


/*it takes char** array and size x of array and givingnmber of user */
/* moreOrLess function */
void moreOrLess(char **arr,int arraySizeX,int givinglineNum);


/*it takes main arguments */
int main(int argc, char **argv)
{
	/* 1. 2. argumanalr duzgun ve bos olmamali */
	/* calistirilabilir dosya ile file name arasinda . */
	if ((argv[1] == NULL) || (argv[2] == NULL) || argc!=3)
	{
		printf("Yanlis Kullanim\n");
		printf("Dogru yazim sekli\n");
		printf(" ./111044051Ramazan a.txt 11\n");
	    return(-1);
	}


	int boyutX,boyutY; /* max satir boyutu */
	char **arr;

        boyutX= getLineNum(argv[1]);
	boyutY = getBoyut(argv[1]);

	if(atoi(argv[2])<1){
		printf("Yanlis Kullanim linenumber birden kucuk olamaz\n");
		return(-1);

	}



	int i=0,j=0; /* Donguler icin gerekli degiskenler. */
	int lineNum;

	lineNum=atoi(argv[2]);
	//printf("lineNum = % d\n",lineNum);


	/* Hafizadan yer alinir. */

    	arr=Make2DintArray(boyutX+1,boyutY+1);

	/*dosya okunur */
    	readData(argv[1],arr,boyutX);

	/*more or less calisir*/
    	moreOrLess(arr,boyutX,lineNum);



	/*array yeri geri verilir*/
	freeArray(arr,boyutX+1);


	//printf("\b");
	//printf("\b\n");
	return 0;
}
/* getting key listener */
/*get a character without press enter for linux system */
/*like gectch() found on web and prepare*/
int getUserChar() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set terminal  */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
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

int printBetweenSize(char **arr,int arraySizeX,int arraySize1,int arraySize2,int givingNum)
{
    int i=0;
    int yuzde=0;
    int bufferSize=0;

/*controls size asilmismi veya 0 dan asagi inilmismi bu tur durumlarda otomatik duzeltme*/
    if(arraySize1<0){
        arraySize1=0;

    }
    if(arraySize2<0)
    {
        arraySize2=0;
    }

    if(arraySize1>=arraySizeX)
        arraySize1=arraySizeX-givingNum;

    if (arraySize2<arraySize1)
        {
            arraySize2=arraySize1;
        }

    if(arraySize1>arraySize2 || arraySize1>arraySizeX-1)
    {
	arraySize1=arraySize2;
    }
    if(arraySize2>=arraySizeX){
	arraySize2=arraySizeX-1;
    }

    if(arraySize1>arraySize2-givingNum)
    {
	arraySize1=arraySize2-givingNum;

    }
    if(arraySize1<=arraySize2 && arraySize1<arraySizeX && arraySize2<arraySizeX)
    	for(i=arraySize1;i<=arraySize2;i++)
    	{

        	printf("%s\n",arr[i]);
		bufferSize+=strlen(arr[i])+1;


    	}
	if(arraySizeX-arraySize2==1)
		printf("end\n");
	/*yuzde=((double)arraySize2/arraySizeX)*100;
	/printf("(%d)loaded\n",yuzde+1);*/
   return bufferSize;

}



void moreOrLess(char **arr,int arraySizeX,int givinglineNum)
{

    int i=0;
    int arraySize1=0;
    int arraySize2=givinglineNum;
    int key = getUserChar();
    system("clear");
/*kullanici calitridiginda once verdigi satir kadar basar */
    printBetweenSize(arr,arraySizeX,arraySize1,arraySize2,givinglineNum);


    //printf("%d\n",times);
    char c;
    //scanf("%c",&c);
/*q girerse program biter*/

    while(key!='q')
    {

	/*eger esc up or down tusuna basarsa bu kosula girer*/

	if(key=='\e'){
		/* up ve down tuslari uc karakter oldguu icin iki key daha alinir*/
		int key2=getUserChar();
		int key3=getUserChar();
	       if(key2=='[' && key3=='B' )
		{

		    arraySize1+=1;
		    arraySize2+=1;
		    system("clear");
		    system("clear");

		    printBetweenSize(arr,arraySizeX,arraySize1,arraySize2,givinglineNum);

		}

		else if(key2=='['&& key3=='A')
		{


		    arraySize1-=1;
		    arraySize2-=1;
	            system("clear");
		    system("clear");
		    if(arraySize2<givinglineNum)
			arraySize2=givinglineNum;

		    printBetweenSize(arr,arraySizeX,arraySize1,arraySize2,givinglineNum);

		}
		/*escape tusu icin */
		else
		{



		    if(arraySizeX>arraySize1+givinglineNum) arraySize1+=givinglineNum;
		    if(arraySizeX>arraySize2+1) arraySize2+=givinglineNum;
		    system("clear");
		    system("clear");
		    printBetweenSize(arr,arraySizeX,arraySize1,arraySize2,givinglineNum);

		}


	}



	/*Enter tusu icin*/
        else if(key=='\n')
        {
            //

      	    arraySize2+=1;
            system("clear");
	    system("clear");

            printBetweenSize(arr,arraySizeX,arraySize1,arraySize2,givinglineNum);

        }
	/*Yanlis tusa bastiginde bisey yok */
	else
	{
	}
        //scanf("%c",&c);
	key = getUserChar();


    }

    system("clear");

}
