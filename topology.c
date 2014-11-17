#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>

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
	char check[100];
	int i=1;
	int numofservers=0;
	int numofneighbours=0;
	int lines=0;
	
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
				i++;

			}
			if(i==2)
			{
				numofneighbours=atoi(buf);
				printf("no. of neighbours=%d\n",numofneighbours);
				topology_ididcost=(struct ididcost*)malloc(numofneighbours*sizeof(struct ididcost));
				i++;
				lines=numofservers+numofneighbours;
				
			}
			if((i>2) && (i<=(2+numofservers)))
			{
				
				
  	 	 	//printf("%s", buf);
  			}
 		}
	}
	else
	{
	  printf("error in opening file");
	}
 	fclose(p);
  	return 0;
}
