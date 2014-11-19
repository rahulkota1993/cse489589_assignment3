/**
 * @rahulkot_assignment3
 * @author  RAHUL KOTA <rahulkot@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>

#include "../include/global.h"
#include "../include/logger.h"

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

// Data Sturctures
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


// Global variables
struct idiport *topology_idipport;
struct ididcost *topology_ididcost;
int ServerIDArray[5];
int CostTable[5][5];

// Pre declarations
void getMyIP (char * IP);
int ReadTopologyFile (char* chFileName);
int getServerIDIndex ( int serverid);
void getIP(uint8_t * ip, char * logline);
void PrintCostTable(void);
/**
 * Checks if the array pointed to by input holds a valid number.
 *
 * @param  input char* to the array holding the value.
 * @return TRUE or FALSE
 */
int isNumber(char *input)
{
    while (*input){
        if (!isdigit(*input))
            return 0;
        else
            input += 1;
    }

    return 1;
}

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log();

	/*Clear LOGFILE and DUMPFILE*/
	//fclose(fopen(LOGFILE, "w"));
	//fclose(fopen(DUMPFILE, "wb"));

	/*Start Here*/
	int opt;
	char filename[256];
	uint8_t IP[4];
	char myIP[INET_ADDRSTRLEN];
	//getMyIP(myIP);
	//getIP(IP,myIP);//converting string IP address to uint_8t IP address
	int interval;
	 //Check for number of arguments
   if(argc < 5){
   	fprintf(stderr, "Missing arguments\n");
	printf("Usage: %s ./assignment3 -t <pathtotopologyfile> -i <routingupdateinterval> \n", argv[0]);
   	return -1;
   }

   /* 
    * Parse the arguments 
    * http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html 
    */
   while((opt = getopt(argc, argv,"t:i:")) != -1){
       switch (opt){
           case 't':   
			strcpy(filename,optarg);
			printf("filename %s\n",filename);

                       break;
	   case 'i':   if(!isNumber(optarg)){
                           fprintf(stderr, "Invalid value for -i\n");
                           return -1;
                       }
			//printf("%s\n",optarg);
			interval= atoi(optarg);
			printf("interval: %d",interval);
			break;
           case '?':   

           default:    printf("Usage: ./assignment3 -t <pathtotopologyfile> -i <routingupdateinterval>\n");
                       return -1;
       }
   }

	// read the topology file and load the data in structures 
	ReadTopologyFile(filename);


	PrintCostTable();	
	return 0;
}

void PrintCostTable(void)
{
	int i,j;
	for( i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			printf("%d ",CostTable[i][j]);
		}
		printf("\n");
	}
}

int ReadTopologyFile (char* chFileName)
{
	char buf[100];
	char *check;
	int i_topology=1;
	uint16_t serverid,serverport;
	uint8_t serverip[4];
	uint16_t r_id,r_nid,r_cost;
	int numofservers=0;
	int numofneighbours=0;
	int lines=0;
	int k_topology;
	int iServerIndex = 0;

	int imyindex = 0;
	int ineighbourindex = 0;
	
	//int store[20][100];
	FILE *fpTopology= fopen(chFileName,"r");
  	
	if(fpTopology!=NULL)
	{
  		while (fgets (buf, sizeof(buf), fpTopology))
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
			{
					
				serverid=atoi(strtok(buf," "));
				//printf("server id =%d\n",serverid);
				ServerIDArray[iServerIndex] = serverid;
				check=strtok(NULL," ");
				//printf("check=%s\n",check);
					
								
				serverport=atoi(strtok(NULL," "));
				//printf("server port =%d\n",serverport);
				getIP(serverip,check);
				/*for(k=0;k<4;k++)
				{
				 printf("serverip[%d]=%d\n",k,serverip[k]);
				}*/
				
				a_idipport[iServerIndex].topology_id=serverid;
				printf("id=%d\n",a_idipport[iServerIndex].topology_id);
				
				 for(k_topology=0;k_topology<4;k_topology++)
				{
				 a_idipport[iServerIndex].topology_ip[k_topology]=serverip[k_topology];
				 printf("ip=%d\n",a_idipport[iServerIndex].topology_ip[k_topology]);
				}
				a_idipport[iServerIndex].topology_port=serverport;
				printf("port=%d\n",a_idipport[iServerIndex].topology_port);
			
				iServerIndex++;	
  	 	 	
  			}
			if(i_topology>(2+numofservers))
			{
				r_id=atoi(strtok(buf," "));
				//printf("r_id=%d\n",r_id);
				r_nid=atoi(strtok(NULL," "));
				//printf("r_nid=%d\n",r_nid);
				r_cost=atoi(strtok(NULL," "));
				//printf("r_cost=%d\n",r_cost);

				// find the serverid index
				imyindex = getServerIDIndex(r_id);
				ineighbourindex = getServerIDIndex( r_nid );
				if ( imyindex < 5 && ineighbourindex < 5)	
				{
					CostTable[ imyindex ] [ ineighbourindex ] = r_cost;
					CostTable[ ineighbourindex ] [ imyindex ] = r_cost;
					// implement the 2D array with cost	
					//a_ididcost[i].myid=r_id;
					//a_ididcost[i].neighbourid=r_nid;
					//a_ididcost[i].costlink=r_cost;
					//printf("id=%d,nid=%d,costlink=%d\n",a_ididcost[serverid].myid,a_ididcost[serverid].neighbourid,a_ididcost[serverid].costlink);
				}
				
			}
				
			i_topology++;
			//printf("%s", buf);
			memset(buf,0,100);
 		}
	}
	else
	{
	  printf("error in opening file");
	}
 	fclose(fpTopology);
  	return 0;
}
int getServerIDIndex ( int serverid)
{
	int i = 0;
	for( i = 0 ; i < 5 ; i++)
	{
		if( ServerIDArray[i] == serverid )
			return i;
	}
	return i;
}
void getIP(uint8_t *ip, char * logline)
{
    ip[0] = atoi(strtok(logline, "."));
    ip[1] = atoi(strtok(NULL, "."));
    ip[2] = atoi(strtok(NULL, "."));
    ip[3] = atoi(strtok(NULL, " "));
    
    return;
}
