#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
void getIP(uint8_t *ip, char * logline);
struct idipport
{
 uint16_t topology_id;
 uint8_t topology_ip[4];
 uint16_t topology_port;
};

struct ididcost
{
 uint16_t myid;
 uint16_t neighbourid;
 uint16_t costlink;
};

int main (void)
{
	struct idiport *topology_idipport;
	struct ididcost *topology_ididcost;
	char buf[100];
	char *check;
	int i=1;
	uint16_t serverid,serverport;
	uint8_t serverip[4];
	int numofservers=0;
	int numofneighbours=0;
	int lines=0;
	int k;
	
	int store[20][100];
	FILE *p= fopen("topology.txt","r");
  	
	if(p!=NULL)
	{
		
		
  		while (fgets (buf, sizeof(buf), p))
  		{
    		 	if(i==1)
		 	{
				numofservers=atoi(buf);
				printf("no. of servers=%d\n",numofservers);
				topology_idipport=(struct idipport*)malloc(numofservers*sizeof(struct idipport));
				

			}
			if(i==2)
			{
				numofneighbours=atoi(buf);
				printf("no. of neighbours=%d\n",numofneighbours);
				topology_ididcost=(struct ididcost*)malloc(numofneighbours*sizeof(struct ididcost));
				
				lines=numofservers+numofneighbours;
				
			}
			//if((i>2) && (i<=(2+numofservers)))
			if(i==3)		
			{
				serverid=atoi(strtok(buf," "));
				printf("server id =%d\n",serverid);
				check=strtok(NULL," ");
				printf("check=%s\n",check);
					
				
				serverport=atoi(strtok(NULL," "));
				printf("server port =%d\n",serverport);
				getIP(serverip,check);
				for(k=0;k<4;k++)
				{
				 printf("serverip[%d]=%d\n",k,serverip[k]);
				}
				
  	 	 	
  			}
		i++;
		//printf("%s", buf);
 		}
	}
	else
	{
	  printf("error in opening file");
	}
 	fclose(p);
  	return 0;
}
void getIP(uint8_t *ip, char * logline)
{
    ip[0] = atoi(strtok(logline, "."));
    ip[1] = atoi(strtok(NULL, "."));
    ip[2] = atoi(strtok(NULL, "."));
    ip[3] = atoi(strtok(NULL, " "));
    
    return;
}