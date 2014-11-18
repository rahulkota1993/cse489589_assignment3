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
}a_idipport[5];

struct ididcost
{
 uint16_t myid;
 uint16_t neighbourid;
 uint16_t costlink;
}a_ididcost[5];

int main (void)
{
	struct idiport *topology_idipport;
	struct ididcost *topology_ididcost;
	char buf[100];
	char *check;
	int i_topology=1;
	uint16_t serverid,serverport;
	uint8_t serverip[4];
	uint16_t r_id,r_nid,r_cost;
	int numofservers=0;
	int numofneighbours=0;
	int lines=0;
	int k;
	int k_topology;
	
	int store[20][100];
	FILE *p= fopen("topology.txt","r");
  	
	if(p!=NULL)
	{
		
		
  		while (fgets (buf, sizeof(buf), p))
  		{
    		 	if(i_topology==1)
		 	{
				numofservers=atoi(buf);
				printf("no. of servers=%d\n",numofservers);
				topology_idipport=(struct idipport*)malloc(numofservers*sizeof(struct idipport));
				

			}
			if(i_topology==2)
			{
				numofneighbours=atoi(buf);
				printf("no. of neighbours=%d\n",numofneighbours);
				topology_ididcost=(struct ididcost*)malloc(numofneighbours*sizeof(struct ididcost));
				
				lines=numofservers+numofneighbours;
				
			}
			if((i_topology>2) && (i_topology<=(2+numofservers)))
			//if(i_topology==3)		
			{
				serverid=atoi(strtok(buf," "));
				//printf("server id =%d\n",serverid);
				check=strtok(NULL," ");
				//printf("check=%s\n",check);
					
				
				serverport=atoi(strtok(NULL," "));
				//printf("server port =%d\n",serverport);
				getIP(serverip,check);
				/*for(k=0;k<4;k++)
				{
				 printf("serverip[%d]=%d\n",k,serverip[k]);
				}*/
				
				a_idipport[serverid].topology_id=serverid;
				printf("id=%d\n",a_idipport[serverid].topology_id);
				
				 for(k_topology=0;k_topology<4;k_topology++)
				{
				 a_idipport[serverid].topology_ip[k_topology]=serverip[k_topology];
				 printf("ip=%d\n",a_idipport[serverid].topology_ip[k_topology]);
				}
				a_idipport[serverid].topology_port=serverport;
				printf("port=%d\n",a_idipport[serverid].topology_port);
				
  	 	 	
  			}
			if(i_topology>(2+numofservers))
			{
				r_id=atoi(strtok(buf," "));
				//printf("r_id=%d\n",r_id);
				r_nid=atoi(strtok(NULL," "));
				//printf("r_nid=%d\n",r_nid);
				r_cost=atoi(strtok(NULL," "));
				//printf("r_cost=%d\n",r_cost);

				a_ididcost[serverid].myid=r_id;
				a_ididcost[serverid].neighbourid=r_nid;
				a_ididcost[serverid].costlink=r_cost;
				printf("id=%d,nid=%d,costlink=%d\n",a_ididcost[serverid].myid,a_ididcost[serverid].neighbourid,a_ididcost[serverid].costlink);

				
				
			}
				
		i_topology++;
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