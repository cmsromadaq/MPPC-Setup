#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>
//#include "HVPowerSupply.h"
#include "SocketConnection.h"
#include <string.h>
#include <math.h>

#define	msleep(x) usleep((x)*1000)
#define Nch 12
#define SLOT 3
#define VOLT 1
#define TEMP 2
#define HUM 3
#define CLOSE 0
#define BUFFLENGTH 1024

char buffer[BUFFLENGTH];
double t0,tm;
int i,j,k,Nmed=5;
struct timeval t;
char name[50];
char *pstr;
std::ofstream tempfile;

int main(int argc, char* argv[])
{
    
  if (argc>3) {
    printf("Error, solo se aceptan dos paraetros\n\
		 %s [<nombre archivo>] [<numero de mediciones>]",argv[0]);		
    return 1;
  }
  else if (argc > 2){
    sprintf(name,"%s_temp.dat",argv[1]);
    tempfile.open(name);
    Nmed=atoi(argv[2]);
  }
  else if (argc > 1){
    sprintf(name,"%s_temp.dat",argv[1]);
    tempfile.open(name);
  }
  else{
    tempfile.open("datos_temp.dat");
  }
 
  SocketConnection *sock=new SocketConnection(CLIENT);
  if (sock->connectClient()) return 1;
  printf("connected to atlas3:win01\n");
  printf("measuring sensors\n");
  
  tempfile<<"#sensor\tmean\trms\ttime"<<std::endl;
  gettimeofday(&t,NULL);
  t0=(double)t.tv_sec+(double)t.tv_usec/1000000;
  for (i=0;i<Nmed;i++){
   
    for (int sensor=1;sensor<33;sensor++){			
      memset(buffer,0,strlen(buffer));
      if ((sensor<2) || (sensor>28)) continue;
      if((sensor<29) && (sensor >25)) 
      	sprintf(buffer,"option %d,%d!",HUM,sensor);
      else
      	sprintf(buffer,"option %d,%d!",TEMP,sensor);
      sock->writeData(buffer);
      sock->readUntil('!');
      gettimeofday(&t,NULL);
      tm=((double)t.tv_sec+(double)t.tv_usec/1000000) - t0;
      pstr = strtok(sock->buffer,", ");
      double mean=0,var=0,count=0,val=0;
      while (pstr != NULL){
	val = ((atof(pstr)/1000) < 100.0)?(atof(pstr)/1000):-1;
	mean+=val;
	var+=val*val;
	count++;
	pstr = strtok(NULL,", ");
      }
      if(count>1){
        mean=mean/count;
        var=var/(count-1)-count/(count-1)*mean*mean;
      }
      else
        var=-1;
      if ((sensor<29) && (sensor>25) ){
	printf("mean: %f\tvar: %f\n",((mean>0)?(mean*1000*100.0/5.0):-1),((mean>0)?(sqrt(var)*1000*100.0/5.0):-1));
      }
      else
      	printf("mean: %f\tvar: %f\n",((mean>0)?( 1.0/(1.0/298.0 + 1.0/3970.0*log(mean/30.0)) - 273.0 ):-1),((mean>0)?sqrt(var):-1));
      tempfile<<sensor<<"\t"<<((mean>0)?mean:-1)<<"\t"<<((mean>0)?sqrt(var):-1)<<"\t"<<tm<<std::endl;
    }
  }
  memset(buffer,0,strlen(buffer));
  sprintf(buffer,"option %d!",CLOSE);
  sock->writeData(buffer);
  printf("win01 response: ");
  sock->readUntil('!');
  sock->printBuffer();
  tempfile.close();	
  sock->closeClient();
  printf("done!\n");
  return 0;
}

