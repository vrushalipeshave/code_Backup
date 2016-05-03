/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 warmup programming assignments at USC.       */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: listtest.c,v 1.1 2015/08/21 03:52:20 william Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "cs402.h"
#include "my402list.h"
 #include <locale.h>
 #include <unistd.h>
 #include <sys/stat.h>
 
   
typedef struct record {
    char trans_type[25];
    char trans_time [25];
    char trans_amount [25];
    char trans_des[50];
} record;

int ReadInput(FILE *,My402List * list);
void SortInput(My402List * list,int );
void PrintStatement(My402List * list);
int checknumtabs(char buf[]);
int checktimestamp(char time[]);
int checktranstype(char type[]);
int checkamt(char trans_amount[]);
int checkdec(char trans_des[]);
record * InsertIntoStruct(char[],char[],char[],char[]);
char * insertcommas(char *);
char * insertcommas2(char *);
char * printvalue(char * val);
char * printvalue2(char * val);
char tempChar[50];
char tempChar1[50];   

void BubbleForward(My402List *, My402ListElem **, My402ListElem **);
char * insertcommas2(char * val){
int i=0;
char * temp=strchr(val,'.');
char array_temp[20];
int length=temp-val;

strcpy(array_temp,temp); 

    char arrayval[20];
    char newarray[20];
    strcpy(arrayval,val);

for(i=0;i<length;i++)
  newarray[i]=arrayval[i];
newarray[i]='\0';

char commaarray[25];
char *a=NULL;

if(length==7){
commaarray[0]=newarray[0];
commaarray[1]=',';
commaarray[2]=newarray[1];
 commaarray[3]=newarray[2];
 commaarray[4]=newarray[3];
 commaarray[5]=',';
 commaarray[6]=newarray[4];
 commaarray[7]=newarray[5];
 commaarray[8]=newarray[6];
 commaarray[9]=array_temp[0];
 commaarray[10]=array_temp[1];
 commaarray[11]=array_temp[2];
 commaarray[12]='\0';
 
a=commaarray;
 return a;
 }
  if(length==6){
   commaarray[0]=newarray[0];
  commaarray[1]=newarray[1];
commaarray[2]=newarray[2];
 commaarray[3]=',';
 commaarray[4]=newarray[3];
 commaarray[5]=newarray[4];
 commaarray[6]=newarray[5];
 commaarray[7]=array_temp[0];
 commaarray[8]=array_temp[1];
 commaarray[9]=array_temp[2];
 commaarray[10]='\0'; 
 
 a=commaarray;
 return a;
  }
  if(length==5){
    commaarray[0]=newarray[0];
commaarray[1]=newarray[1];
commaarray[2]=',';
 commaarray[3]=newarray[2];
 commaarray[4]=newarray[3];
 commaarray[5]=newarray[4];
 commaarray[6]=array_temp[0];
 commaarray[7]=array_temp[1];
 commaarray[8]=array_temp[2];
 commaarray[9]='\0'; 
 
 a=commaarray;
 return a;
}
if(length==4){
    commaarray[0]=newarray[0];
commaarray[1]=',';
commaarray[2]=newarray[1];
 commaarray[3]=newarray[2];
 commaarray[4]=newarray[3];
 commaarray[5]=array_temp[0];
 commaarray[6]=array_temp[1];
 commaarray[7]=array_temp[2];
 commaarray[8]='\0';
 
 
a=commaarray;
 return a;
}
return val;

}

char * insertcommas(char * val){
int i=0;
char * temp=strchr(val,'.');
char array_temp[20];
int length=temp-val;

strcpy(array_temp,temp); 

    char arrayval[20];
    char newarray[20];
    strcpy(arrayval,val);

for(i=0;i<length;i++)
  newarray[i]=arrayval[i];
newarray[i]='\0';

char commaarray[25];
char *a=NULL;

if(length==7){
commaarray[0]=newarray[0];
commaarray[1]=',';
commaarray[2]=newarray[1];
 commaarray[3]=newarray[2];
 commaarray[4]=newarray[3];
 commaarray[5]=',';
 commaarray[6]=newarray[4];
 commaarray[7]=newarray[5];
 commaarray[8]=newarray[6];
 commaarray[9]=array_temp[0];
 commaarray[10]=array_temp[1];
 commaarray[11]=array_temp[2];
 commaarray[12]=' ';
 commaarray[13]='\0';
a=commaarray;
 return a;
 }
  if(length==6){
   commaarray[0]=newarray[0];
  commaarray[1]=newarray[1];
commaarray[2]=newarray[2];
 commaarray[3]=',';
 commaarray[4]=newarray[3];
 commaarray[5]=newarray[4];
 commaarray[6]=newarray[5];
 commaarray[7]=array_temp[0];
 commaarray[8]=array_temp[1];
 commaarray[9]=array_temp[2];
 commaarray[10]=' '; 
 commaarray[11]='\0';
 a=commaarray;
 return a;
  }
  if(length==5){
    commaarray[0]=newarray[0];
commaarray[1]=newarray[1];
commaarray[2]=',';
 commaarray[3]=newarray[2];
 commaarray[4]=newarray[3];
 commaarray[5]=newarray[4];
 commaarray[6]=array_temp[0];
 commaarray[7]=array_temp[1];
 commaarray[8]=array_temp[2];
 commaarray[9]=' '; 
 commaarray[10]='\0';
 a=commaarray;
 return a;
}
if(length==4){
    commaarray[0]=newarray[0];
commaarray[1]=',';
commaarray[2]=newarray[1];
 commaarray[3]=newarray[2];
 commaarray[4]=newarray[3];
 commaarray[5]=array_temp[0];
 commaarray[6]=array_temp[1];
 commaarray[7]=array_temp[2];
 commaarray[8]=' ';
 commaarray[9]='\0'; 
 
a=commaarray;
 return a;
}
return strcat(val," ");

}
char * printvalue2(char *val){
  int num=atof(val);
   char string1[20]={"(?,"};
   char string2[20]={"???,???.?"};
   char string3[20]={"?)"};
   strcat(string1,string2);
   strcat(string1,string3);
   char * p=(char*)malloc(sizeof(string1));
    p = string1;
    if(num<0){
  if(abs(num)>=10000000){
    return(p);
  }
  else{
    strcpy(tempChar1,insertcommas2(val));
    return(tempChar1);
  }
}else if(num>=10000000){
    return("?,???,???.??");
  }
  else
  {
    strcpy(tempChar,insertcommas2(val));
    return(tempChar);
  } 

}


char* printvalue(char * val){
 
  int num=atof(val);
   char string1[20]={"(?,"};
   char string2[20]={"???,???.?"};
   char string3[20]={"?)"};
   strcat(string1,string2);
   strcat(string1,string3);
   char * p=(char*)malloc(sizeof(string1));
    p = string1;
    if(num<0){
  if(abs(num)>=10000000){
    return(p);
  }
  else{
    strcpy(tempChar1,insertcommas(val));
    return(tempChar1);
  }
}else if(num>=10000000){
    return("?,???,???.??");
  }
  else
  {
    strcpy(tempChar,insertcommas(val));
    return(tempChar);
  } 
}


void PrintStatement(My402List * list){
printf("+-----------------+--------------------------+----------------+----------------+\n");
printf("|       Date      | Description              |         Amount |        Balance |\n");
printf("+-----------------+--------------------------+----------------+----------------+\n");
My402ListElem *elem=NULL;
record *r=NULL;
float balance=0;
        for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem)) {
r=elem->obj;
//printf("\n%s	%s	%s	%s",r->trans_type,r->trans_time,r->trans_amount,r->trans_des);
int timestamp=atoi(r->trans_time);
time_t t=(time_t) timestamp;
//printf("\ntimestamp %s",ctime(&t));

  struct tm * timeinfo;
  char buffer [80];
   float positivebalance;
  timeinfo = localtime (&t);
  if(r->trans_type[0]=='+'){
    balance+=((atof)(r->trans_amount))*100;

  }
  else{ 
    balance-=((atof)(r->trans_amount))*100;
}

  char balarray[50];
//printf("Balance array %s\n",balarray);
  if(balance<0)
    positivebalance=-1*balance;
  else
    positivebalance=balance;

  snprintf(balarray,50,"%.2f",(positivebalance/100));
  
  strftime (buffer,16,"%a %b %e %Y",timeinfo);
  printf("| %s | %-24.24s |",buffer,r->trans_des);
  //printf(" %-24.24s |");
  
  if (r->trans_type[0]=='-')
  {
     printf(" (%12.12s) |",printvalue2(r->trans_amount));
    }  
  else{
  printf(" %14.14s |",printvalue(r->trans_amount));//original 14.14
}
  if(balance<0){
    
    
  printf(" (%12.12s) |",printvalue2(balarray));
}
else
  printf(" %14.14s |",printvalue(balarray));
printf("\n");
}
printf("+-----------------+--------------------------+----------------+----------------+\n");
}



void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
    /* (*pp_elem1) must be closer to First() than (*pp_elem2) */
{
    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
    void *obj1=elem1->obj, *obj2=elem2->obj;
    My402ListElem *elem1prev=My402ListPrev(pList, elem1);
/*  My402ListElem *elem1next=My402ListNext(pList, elem1); */
/*  My402ListElem *elem2prev=My402ListPrev(pList, elem2); */
    My402ListElem *elem2next=My402ListNext(pList, elem2);
	
    My402ListUnlink(pList, elem1);
    My402ListUnlink(pList, elem2);
    if (elem1prev == NULL) {
        (void)My402ListPrepend(pList, obj2);
        *pp_elem1 = My402ListFirst(pList);
    } else {
        (void)My402ListInsertAfter(pList, obj2, elem1prev);
        *pp_elem1 = My402ListNext(pList, elem1prev);
    }
    if (elem2next == NULL) {
        (void)My402ListAppend(pList, obj1);
        *pp_elem2 = My402ListLast(pList);
    } else {
        (void)My402ListInsertBefore(pList, obj1, elem2next);
        *pp_elem2 = My402ListPrev(pList, elem2next);
    }
}

void SortInput(My402List *pList, int num_items)
{
    My402ListElem *elem=NULL;
    int i=0;
    record *transaction=NULL;

    if (My402ListLength(pList) != num_items) {
        fprintf(stderr, "List length is not %1d in BubbleSortForwardList().\n", num_items);
        exit(1);
    }
    for (i=0; i < num_items; i++) {
        int j=0, something_swapped=FALSE;
        My402ListElem *next_elem=NULL;

        for (elem=My402ListFirst(pList), j=0; j < num_items-i-1; elem=next_elem, j++) {
            //int cur_val=(int)(elem->obj), 
transaction=elem->obj;
int next_val=0;
int cur_val=atoi(transaction->trans_time);
            next_elem=My402ListNext(pList, elem);
            //next_val = (int)(next_elem->obj);
             transaction=next_elem->obj;
 next_val=atoi(transaction->trans_time);
if(cur_val==next_val)
{
	printf("\nTimestamps are identical\n");
	exit(0);
}
            if (cur_val > next_val) {
                BubbleForward(pList, &elem, &next_elem);
                something_swapped = TRUE;
            }
        }
        if (!something_swapped) break;
    }
}

record * InsertIntoStruct(char trans_type[],char trans_time[],char trans_amount[],char trans_des[] ){
record *transaction=(record *)malloc(sizeof(record));
//printf("Hey in printf of insertInto fun");
strcpy(transaction->trans_type,trans_type);
strcpy(transaction->trans_time,trans_time);
strcpy(transaction->trans_amount,trans_amount);
strcpy(transaction->trans_des,trans_des);

//transaction->trans_des[25]='\0';
/*
printf("\nType %s",transaction->trans_type);
printf("\nTime %s",transaction->trans_time);
printf("\nAmount %s",transaction->trans_amount);
printf("\ndes %s",transaction->trans_des);
*/
return transaction;
}


int checkdec(char trans_des[]){
if(trans_des==NULL)
return 0;
else
return 1;
}


int checkamt(char trans_amount[])
{
if(strlen(trans_amount)<=10){
	char * str1=strchr(trans_amount,'.');
	char *str2=NULL;
	if(str1==NULL){
       
	return 0;
        }
	int length=str1-trans_amount;
	if(length>7){
       
	return 0;
        }
	
	str2=strchr(trans_amount,'\0');
	if(str2==NULL){
       
	return 0;
        }
	length=str2-str1-1;
	if(length>2){
        
	return 0;
        }
	
	return 1;
}
else
return 0;
}

int checktimestamp(char time1[])
{
int result=0;
if(strlen(time1)>10||strlen(time1)==0)
{
printf("\nInvalid Timestamp\n");
return 0;
}
else{
  time_t t1 = atoi(time1);
  time_t t2= time(NULL);
if(t1>t2){
  printf("\nTimestamp is greater than current time\n");
 return 0;
}
else{ 
result=atoi(time1);
return result;
}
}

}


int checktranstype(char type[]){
if(strlen(type)==1){
if(type[0]=='+'|| type[0]=='-')
return 1;
}
return 0;
}

int checknumtabs(char buf[]){
char *str=NULL;
int num_tabs=0;

/*str=strtok(buf,"\t");
while(str!=NULL)
{
num_tabs++;
printf("%s\n",str);
if(num_tabs<5)
str=strtok(NULL,"\t");

}*/
str=strchr(buf,'\t');
while(str!=NULL){
num_tabs++;
str++;
str=strchr(str,'\t');
}
//printf("\nNo of tabs are%d",num_tabs);
return num_tabs;
}


int ReadInput(FILE *fp,My402List * list){
char buf[1026]="";

buf[sizeof(buf)-1]='\0';
char trans_des[25]=" ";
char trans_type[25]="";
char trans_amt[25]=" ";
char trans_time[25]=" ";

while(fgets(buf, sizeof(buf), fp) != NULL) { /* buf may contain '\n' */
if(strlen(buf)<1024)//line is not beyond 1024 characters
{

record * transaction=NULL;
if((checknumtabs(buf))==3)//it means no of tabs=3
{
char *str=NULL;
str=strtok(buf,"\t");
if(checktranstype(str)){
strcpy(trans_type,str);
str=strtok(NULL,"\t");
if(checktimestamp(str)){
strcpy(trans_time,str);
str=strtok(NULL,"\t");
if(checkamt(str)){
strcpy(trans_amt,str);
str=strtok(NULL,"\n");
if(checkdec(str)){
strcpy(trans_des,str);  //valid line
transaction=InsertIntoStruct(trans_type,trans_time,trans_amt,trans_des );
My402ListAppend(list,transaction);



}
else
{
printf("\nMissing Description");
exit (0);
}
}
else
{
printf("\nAmount is not in correct format\n");
exit (0);
}
}
else{
printf("\nTime stamp is invalid\n");
exit (0);
}
}
else{
printf("\nTransaction type is invalid\n");
exit (0);
}
}
else{
printf("\nMalformed Input.Check the format of line\n");
exit(0);}
}

else{
printf("\nNumber of characters in a line more than 1024.Malformed Input\n");
exit(0);
}
        }

return 1;
}

int main(int argc,char * argv[]){
My402List list;
FILE *fp=NULL;
struct stat st_buf;

int reading_from_file=0;

if (!My402ListInit(&list)) {
/* error */
  printf("\nList Initialisation failed");
  exit(0);
}
if(argc<2)
{
 printf("\nMalformed command\n");
 exit (0);

}
if(strcmp(argv[1],"sort")){
              printf("\nMalformed command\n");
              exit (0);

            }
if (argc == 3) {
            reading_from_file = 1;
            lstat(argv[2], &st_buf);
        }
        
if (reading_from_file) {

  if (S_ISDIR(st_buf.st_mode)){
        printf("\n%s is a directory.\n", argv[2]);
        exit (0);
}
   /*  else if (access(argv[2], R_OK) == 0)
{*/
  else if(fopen(argv[2], "r")){
    fp=fopen(argv[2], "r");
  }
  else if(!(access(argv[2], R_OK) == 0)){
      printf("\n The file %s does not exist or access Denied\n",argv[2]);
      //printf("\nAccess Denied\n");
      exit(1);
    }else{
    //printf("\n The file %s does not exist",argv[2]);
      printf("\nAccess Denied\n");
      exit(1);
    }    /* It's readable by the current user. */
           
    if (!ReadInput(fp, &list)) {//read file line by line
/* error */
             // printf("\nNumber of characters in a line more than 1024.Malformed Input");
fclose(fp);
exit (0);
}
}
else{
if(!ReadInput(stdin,&list)){//error if input from keyboard is incorrect
printf("Error in reading from keyboard");
exit (0);
}
}
            

//if (fp != stdin) 
//fclose(fp);
SortInput(&list,list.num_members);

PrintStatement(&list);
return 0;

}
