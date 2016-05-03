
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "cs402.h"
#include "my402list.h"
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/stat.h>

typedef struct packet
{
	int packet_index;
	int required_tokens;
	double service_time;
	double outtimeQ1;
	double outtimeQ2;
	double outtimeServer1;
	double outtimeServer2;
	double intimeQ2;
	double intimeQ1;
	double intimeServer1;
	double intimeServer2;
	double packet_arrival_time;//when packet arrives in a system
} packet;
char *str=NULL;
pthread_cond_t cv=PTHREAD_COND_INITIALIZER;
int packets_generated,q1_to_q2,q2_to_out;
int num_tokens,num_packets,dropped_packets,removed_packets;
int token_index=1;
long int P=3,B=10,num=10;
double arrival_time_previous_packet,arrival_time_previous_token;
double mu=0.35,r=1.5,lambda=2,emulation_time;
double dval=(double)0;
int intval=0;
int sys_mode;//0 for deterministic mode 1 for trace file
FILE *fp;
char filename[100];
pthread_t token,packet_arrival,s1,s2,handler;
pthread_mutex_t m;
sigset_t set;
My402List Q1,Q2;
int closefile;
int controlcflag,end,terminate;
int dropped_tokens;
double emulation_end_time;

double totaltimeq1,totaltimeq2,totaltimes1,totaltimes2,totalintervaltime,totalservicetime;//statistics parameters
double total_time_in_system;
double sum_of_squares;

void parseCommandLine(int argc,char *argv[])
{
	printf("\nArgc %d",argc);
	int cnt=0,i;
if(argc==1){
		sys_mode=0;//in a deterministic mode
	}
else if(argc>2){
	for(i=1;i<argc-1;i=i+2){
		if(strcmp(argv[i],"-lambda")==0){
			if(sscanf(argv[i+1],"%lf",&dval)!=1){
				printf("\nMalformed command");
				exit (1);
			}
			else{
				lambda=atof(argv[i+1]);
				
				if(lambda<0)
				{
					printf("lambda is not positive");
					exit (1);
				}
				if(lambda<0.1)
					lambda=0.1;
				cnt+=2;
			}
		}
		if(strcmp(argv[i],"-mu")==0){
			if(sscanf(argv[i+1],"%lf",&dval)!=1){
				printf("\nMalformed command");
				exit (1);
			}
			else{
				mu=atof(argv[i+1]);
				
				if(mu<0)
				{
					printf("mu is not positive");
					exit (1);

				}
				if(mu<0.1)
					mu=0.1;

				cnt+=2;
			}
		}
		if(strcmp(argv[i],"-r")==0){
			if(sscanf(argv[i+1],"%lf",&dval)!=1){
				printf("\nMalformed command");
				exit (1);
			}
			else{
				r=atof(argv[i+1]);
				if(r<0)
				{
					printf("r is not positive");
					exit (1);
				}
				if(r<0.1)
					r=0.1;
				cnt+=2;
			}
		}
		if(strcmp(argv[i],"-B")==0){
			if(sscanf(argv[i+1],"%d",&intval)!=1){
				printf("\nMalformed command");
				exit (1);
			}
			else{
				B=atol(argv[i+1]);
				if(B<0)
				{
					printf("B is not positive");
					exit (1);
				}
				if(B>2147483647){
					printf("Wrong Input file, check the value of B");
					exit (1);
				}
				cnt+=2;
			}
		}
		if(strcmp(argv[i],"-P")==0){
			if(sscanf(argv[i+1],"%d",&intval)!=1){
				printf("\nMalformed command");
				exit (1);
			}
			else{
				if(P<0){
					printf("P is not positive");
					exit (1);

				}
				if(P>2147483647){
					printf("Wrong Input file, check the value of P");
					exit (1);
				}
				P=atol(argv[i+1]);
				cnt+=2;
			}
		}
		if(strcmp(argv[i],"-n")==0){
			//printf("\n argv %s argv+1 %s",argv[i],argv[i+1]);
			if(sscanf(argv[i+1],"%d",&intval)!=1){
				printf("\nMalformed command");
				exit (1);
			}
			else{
				printf("\nn value %d",intval);
				num=atol(argv[i+1]);
				if(num<0)
				{
					printf("num is not positive");
					exit (1);

				}
				cnt+=2;
			}
		}
		if(strcmp(argv[i],"-t")==0){
			strcpy(filename,argv[i+1]);
			sys_mode=1;
			cnt+=2;
		}
    }
   
	if(cnt!=argc-1){
		printf("\nMalformed command \n");
		exit (1);
	}
}
else{
	printf("\nMalformed command");
	exit (1);
}
}
packet * CreatePacket(){
	double field2,field3;
	packet * p=(packet *)malloc(sizeof(packet));
	p->packet_index=++num_packets;
	if(sys_mode==1)//read from file
	{
        char delim[2]="\t ";
	    str=strtok(NULL,delim);

        field2=atoi(str);
        if(field2)
        {
        	p->required_tokens=field2;

        }
        else{
        	exit (1);
        }
        str=strtok(NULL,delim);
        field3=atoi(str);
        if(field3)
        {
        	p->service_time=field3;

        }
        else{
        	exit (1);
        }
    }
	    else{
	    p->required_tokens=P;
		p->service_time=(1/mu)*1000;
		}
	    return p;

}
double GetCurrentTime(){//return microseconds
	struct timeval tim;  
    gettimeofday(&tim, NULL);  
    double current_time=(double)(tim.tv_sec*1000000+(double)(tim.tv_usec));  
    return current_time;
}
double GetTimeStamp(){//return milliseconds
	double cur_time=GetCurrentTime();
	double diff=(cur_time-emulation_time)/1000;
	return diff;

}

void *ControlCReciever(){
	//int sig;
	double time1;
	while(1){
		//sigwait(&set,&sig);
		sigwait(&set);
		pthread_mutex_lock(&m);
		controlcflag=1;
		pthread_cancel(token);
		pthread_cancel(packet_arrival);
		pthread_cond_broadcast(&cv);
		while(!My402ListEmpty(&Q1))
		{
		My402ListElem * elem=My402ListFirst(&Q1);
		packet *first=elem->obj;
		My402ListUnlink(&Q1,elem);
		time1=GetTimeStamp();
		printf("\n%012.3fms: p%d removed from Q1",time1,first->packet_index);
		free(first);	
		}
		
		while(!My402ListEmpty(&Q2))
		{
		My402ListElem * elem=My402ListFirst(&Q2);
		packet *first=elem->obj;
		My402ListUnlink(&Q2,elem);
		time1=GetTimeStamp();
		printf("\n%012.3fms: p%d removed from Q2",time1,first->packet_index);
		free(first);	
		}
		pthread_mutex_unlock(&m);
        pthread_exit(0);
	}
	return NULL;
}
void cleanup(void *arg){
	int * mutex_locked=(int *)arg;
	if(*mutex_locked){
		pthread_mutex_unlock(&m);
	}
	if(fp!=NULL && closefile==1){
		fclose(fp);
		closefile=0;

	}
}
void *DepositeToken(){
	int mutex_locked=0;
	pthread_cleanup_push(cleanup, &mutex_locked);
	double token_arrival_time,flag=0,delay=0.0,time1;
	while(1){
		time1=GetTimeStamp();
		delay=(1/r)*1000000-((time1-arrival_time_previous_token)*1000);
		if(delay<0.0)
			delay=0.0;
		usleep(delay);
		token_arrival_time=GetTimeStamp();
		pthread_mutex_lock(&m);
		mutex_locked=1;
       
		if(num_tokens<B){
				num_tokens++; 				
				printf("\n%012.3fms: token t%d arrives,token bucket now has %d token%s",token_arrival_time,token_index,num_tokens,num_tokens<2?"":"s");
				
			}
			else{
			printf("\n%012.3fms: token t%d arrives, dropped",token_arrival_time,token_index);
			dropped_tokens++;
			
		}
		 token_index++;
		arrival_time_previous_token=token_arrival_time;

	if(!My402ListEmpty(&Q1)){
					My402ListElem * elem=My402ListFirst(&Q1);
					packet *first=elem->obj;
        	        if(first->required_tokens<=num_tokens){
        	 	num_tokens-=first->required_tokens;
        	 	if(My402ListEmpty(&Q2))
        	 	{
        	 		flag=1;
        	 	}
        	 		My402ListUnlink(&Q1,elem);
        	 		q1_to_q2++;
        	 		first->outtimeQ1=GetTimeStamp();
        	 		totaltimeq1+=first->outtimeQ1-first->intimeQ1;
        	 		printf("\n%012.3fms: p%d leaves Q1, time in Q1=%.3f, token bucket now has %d token%s",first->outtimeQ1,first->packet_index,(first->outtimeQ1-first->intimeQ1),num_tokens,num_tokens<2?"":"s");
        	 		My402ListAppend(&Q2,first);
        	 		first->intimeQ2=GetTimeStamp();
				    printf("\n%012.3fms: p%d enters Q2",first->intimeQ2,first->packet_index);
				    if(flag==1){
				    	pthread_cond_broadcast(&cv);//signal servers
				    }     
			}
       	    }
	if((q1_to_q2+dropped_packets)==num){
		
     	mutex_locked=0;
        pthread_mutex_unlock(&m);
     	pthread_exit(0);
    }
    
     pthread_mutex_unlock(&m);
     mutex_locked=0;
	}
	pthread_cleanup_pop(0);
	return NULL;	
}
void *PacketArrival(){
	int mutex_locked=0;
	double packet_inter_arrival_time;
	pthread_cleanup_push(cleanup, &mutex_locked);
	char buf[1024];
    char delim[2]="\t ";
    int flag=0;
	double time1,delay=0.0,field1;
	
	while(1)
	{
		
		if(sys_mode==1)
		{
		if(!fgets(buf, sizeof(buf), fp)){
			printf("Error in reading input file %s",filename);
			exit (1);
		}
	    str=strtok(buf,delim);
        field1=atof(str);
        if(field1)
        {
        	packet_inter_arrival_time=(double)field1;

        }
        else{
        	printf("Invalid value");
        	exit (1);
        }
        time1=GetTimeStamp();
        delay=(packet_inter_arrival_time-(time1-arrival_time_previous_packet))*1000;
			if(delay<0)
				delay=0;
			
		}
		else
	   {
	   	time1=GetTimeStamp();
		delay=(1/lambda)*1000000-((time1-arrival_time_previous_packet)*1000);
		if(delay<0)
			delay=0;
		
	   }
       usleep(delay);
		packet *p=CreatePacket();
		packets_generated++;
        p->packet_arrival_time=GetTimeStamp();
		if(p->required_tokens<=B){
			pthread_mutex_lock(&m);
			mutex_locked=1;
		
		totalintervaltime+=p->packet_arrival_time-arrival_time_previous_packet;
        
		printf("\n%012.3fms: p%d arrives,needs %d tokens, inter-arrival time=%.3fms",p->packet_arrival_time,p->packet_index,p->required_tokens,(p->packet_arrival_time-arrival_time_previous_packet));
		arrival_time_previous_packet=p->packet_arrival_time;
		My402ListAppend(&Q1,p);
		
		p->intimeQ1=GetTimeStamp();
		printf("\n%012.3fms: p%d enters Q1",p->intimeQ1,p->packet_index);
		
		
		My402ListElem * elem=My402ListFirst(&Q1);
		packet *first=elem->obj;
		if(first->required_tokens<=num_tokens){
			num_tokens-=first->required_tokens;

			if(My402ListEmpty(&Q2))
        	 	{
        	 		flag=1;
        	 	}

        	 		My402ListUnlink(&Q1,elem);
        	 		first->outtimeQ1=GetTimeStamp();
        	 		totaltimeq1+=first->outtimeQ1-first->intimeQ1;
        	 		q1_to_q2++;
        	 		printf("\n%012.3fms: p%d leaves Q1, time in Q1=%.3f, token bucket now has %d token%s",first->outtimeQ1,first->packet_index,(first->outtimeQ1-first->intimeQ1),num_tokens,num_tokens<2?"":"s");
        	 		My402ListAppend(&Q2,first);
        	 		first->intimeQ2=GetTimeStamp();
				    printf("\n%012.3fms: p%d enters Q2",first->intimeQ2,first->packet_index);
				    if(flag==1){
				    	pthread_cond_broadcast(&cv);//signal servers
				    }
		}
		}
		else
		{
			p->packet_arrival_time=GetTimeStamp();
			printf("\n%012.3fms: p%d arrives, needs %d tokens, inter-arrival time=%.3fms,dropped",p->packet_arrival_time,p->packet_index,p->required_tokens,(p->packet_arrival_time-arrival_time_previous_packet));
			arrival_time_previous_packet=p->packet_arrival_time;
			dropped_packets++;
		}

		if(packets_generated==num){
			
			
			if(My402ListEmpty(&Q1)&&My402ListEmpty(&Q2)){
				end=1;
				pthread_cond_broadcast(&cv);
			}
			
			pthread_mutex_unlock(&m);
			mutex_locked=0;
			pthread_exit(0);
		}
		
		
		mutex_locked=0;
		pthread_mutex_unlock(&m);
		
	}
	pthread_cleanup_pop(0);
return NULL;
}
void *server1(){
	double timeinSystem;
	packet *first;
	while(1)
	{
		pthread_mutex_lock(&m);
		
		while(My402ListEmpty(&Q2)){
			pthread_cond_wait(&cv,&m);
			if(controlcflag==1||terminate==1||end==1 ){

				//printf("\nterminated due to flag");
				pthread_mutex_unlock(&m);
				pthread_exit(0);
			}
		}
		My402ListElem * elem=My402ListFirst(&Q2);
		first=elem->obj;
        My402ListUnlink(&Q2,elem);
        first->outtimeQ2=GetTimeStamp();
        totaltimeq2+=first->outtimeQ2-first->intimeQ2;
        printf("\n%012.3fms: p%d leaves Q2, time in Q2 = %.3f",first->outtimeQ2,first->packet_index,(first->outtimeQ2-first->intimeQ2));
        first->intimeServer1=GetTimeStamp();
		printf("\n%012.3fms: p%d begins service at S1 requesting %.3fms of service",first->intimeServer1,first->packet_index,first->service_time);
		pthread_mutex_unlock(&m);
	  	
	  	usleep(first->service_time*1000);
		
		first->outtimeServer1=GetTimeStamp();
		timeinSystem=first->outtimeServer1-first->packet_arrival_time;
		
		pthread_mutex_lock(&m);
		sum_of_squares+=(double)timeinSystem*timeinSystem;
		totaltimes1+=first->outtimeServer1-first->intimeServer1;
		total_time_in_system+=timeinSystem;
		printf("\n%012.3fms: p%d departs from S1, service time=%.3fms, time in system=%.3fms",first->outtimeServer1,first->packet_index,(first->outtimeServer1-first->intimeServer1),timeinSystem);
		q2_to_out++;
		free(first);
		
		
		if((q2_to_out+dropped_packets)==num||(controlcflag==1)){
			pthread_cond_signal(&cv);
	        terminate=1;
	        pthread_mutex_unlock(&m);
        	pthread_exit(0);
        }
		
		
		pthread_mutex_unlock(&m);
        
        }
	return NULL;

}

void *server2(){
	double timeinSystem;
	packet *first;
	while(1)
	{
		pthread_mutex_lock(&m);
		
		while(My402ListEmpty(&Q2)){
			pthread_cond_wait(&cv,&m);
			if(controlcflag==1||terminate==1 ||end==1){
				//printf("\nterminated due to flag");
				pthread_mutex_unlock(&m);
				pthread_exit(0);
			}
		}
		My402ListElem * elem=My402ListFirst(&Q2);
		first=elem->obj;
        My402ListUnlink(&Q2,elem);
        first->outtimeQ2=GetTimeStamp();
        totaltimeq2+=first->outtimeQ2-first->intimeQ2;
        printf("\n%012.3fms: p%d leaves Q2, time in Q2 = %.3f",first->outtimeQ2,first->packet_index,(first->outtimeQ2-first->intimeQ2));
        first->intimeServer2=GetTimeStamp();
		printf("\n%012.3fms: p%d begins service at S2 requesting %.3fms of service",first->intimeServer2,first->packet_index,first->service_time);
		pthread_mutex_unlock(&m);
	  	
	  	usleep(first->service_time*1000);
		
		first->outtimeServer2=GetTimeStamp();
		timeinSystem=first->outtimeServer2-first->packet_arrival_time;

		
		pthread_mutex_lock(&m);
		sum_of_squares+=(double)timeinSystem*timeinSystem;
		totaltimes2+=first->outtimeServer2-first->intimeServer2;
		total_time_in_system+=timeinSystem;
		printf("\n%012.3fms: p%d departs from S2, service time=%.3fms, time in system=%.3fms",first->outtimeServer2,first->packet_index,(first->outtimeServer2-first->intimeServer2),timeinSystem);
		q2_to_out++;
		free(first);
		
		
		if((q2_to_out+dropped_packets)==num||(controlcflag==1)){
			pthread_cond_signal(&cv);
	        terminate=1;
	        pthread_mutex_unlock(&m);
        	pthread_exit(0);
        }
		
		
		pthread_mutex_unlock(&m);
        
        }
	return NULL;

}


void PrintParameters(){
	printf("\nEmulation Parameters:");
    printf("\n\tnumber to arrive = %ld",num);
    if(sys_mode==0){
    	printf("\n\tlambda = %f",lambda);
    	printf("\n\tmu = %f",mu);
    }
    printf("\n\tr = %f",r);
    printf("\n\tB = %ld",B);
    if(sys_mode==0){
    	printf("\n\tP = %ld\n",P);
    }
    if(sys_mode==1){
    	printf("\n\ttsfile = %s\n",filename);
    }

}
int CheckForFileError(){
	struct stat st_buf;
	lstat(filename, &st_buf);
	
	if(S_ISDIR(st_buf.st_mode)){
        printf("\n%s is a directory.\n", filename);
        return 1;
	}
	if(fopen(filename, "r")){
    fp=fopen(filename, "r");
	}else if(!(access(filename, R_OK) == 0)){
  printf("\n The file %s can not be opened. Access Denied\n",filename);
  return 1;
}else{
	//if(lstat(filename, &st_buf)==-1){
    	printf("\n%s file does not exist",filename);
	        return 1;

    }

 return 0;
}
void PrintStatistics()
{
	double Ex2=0.0,Ex=0.0,std_deviation=0.0;
	Ex2=(double)sum_of_squares/q2_to_out;
	Ex=pow((double)(total_time_in_system/q2_to_out),2);
	std_deviation=sqrt(Ex2-Ex);
    if(packets_generated){
    	printf("\naverage packet inter-arrival time = %.6g",totalintervaltime/(packets_generated*1000));

    }
    else{
    	printf("\naverage packet inter-arrival time = NA");

    }
    if(q2_to_out){
    	printf("\naverage packet service time = %.6g",(totaltimes1+totaltimes2)/(q2_to_out*1000));

    }
    else{
    	printf("\naverage packet service time = NA");
    }
	printf("\navg number of packets in q1 = %.6g",totaltimeq1/(emulation_end_time));
	printf("\navg number of packets in q2 = %.6g",totaltimeq2/(emulation_end_time));
    printf("\navg number of packets in s1  = %.6g",totaltimes1/(emulation_end_time));
    printf("\navg number of packets in s2 = %.6g",totaltimes2/(emulation_end_time));
    if(q2_to_out){
    	printf("\naverage time a packet spent in system = %.6g",total_time_in_system/(q2_to_out*1000));
    	printf("\nstandard deviation for time spent in system = %.6g",std_deviation/1000);
    }
    else{
    	printf("\naverage time a packet spent in system = NA");
        printf("\nstandard deviation for time spent in system = NA");
    }
	if(token_index){
    	printf("\ntoken drop probability = %.6g",(double)dropped_tokens/token_index);
    }
    else{
    	printf("\ntoken drop probability = NA");
    }
    
    if(packets_generated){
    	printf("\npacket drop probability = %.6g",(double)dropped_packets/packets_generated);
    }
    else {
    	printf("\npacket drop probability = NA\n");
    }
    
}

int main(int argc, char *argv[])
{
	char buf[50];
	sigemptyset(&set);
	sigaddset(&set,SIGINT);
	sigprocmask(SIG_BLOCK,&set,0);
    
    parseCommandLine(argc,argv);
	if(sys_mode==1){
		if(!CheckForFileError()){
			printf("reading from file");
			closefile=1;
		if(!fgets(buf, sizeof(buf), fp)){
			printf("Error in reading input file %s",filename);
			fclose(fp);
			closefile=0;
			exit (1);
		}
		num=atoi(buf);
		if(num<0){
			printf("\nInput file %s is not in a right format, check the value of number to arrive",filename);
			exit(1);
		}
		if(num>2147483647){
			printf("\nInput file %s is not in a right format, check the value of number to arrive",filename);
			exit(1);
		}
		if(!num){
			printf("\nInput file %s is not in a right format",filename);
			fclose(fp);
			closefile=0;
			exit (1);
		}

		}
		else
			exit (1);
		}
	PrintParameters();

	My402ListInit(&Q1);
	My402ListInit(&Q2);
	
	emulation_time=GetCurrentTime();
	printf("\n00000000.000ms: emulation begins");
	
	pthread_mutex_init(&m,0);

    pthread_create(&token,NULL,DepositeToken,0);
	pthread_create(&packet_arrival,NULL,PacketArrival,0);
	pthread_create(&s1,NULL,server1,0);
	pthread_create(&s2,NULL,server2,0);
	pthread_create(&handler,NULL,ControlCReciever,0);

	pthread_join(token,NULL);
	pthread_join(packet_arrival,NULL);
	pthread_join(s1,NULL);
	pthread_join(s2,NULL);
	
	emulation_end_time=GetTimeStamp();
	printf("\n%012.3fms: emulation ends\n",emulation_end_time);
   
    PrintStatistics();
	return 0;

}