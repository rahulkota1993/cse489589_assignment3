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

// file descriptor for standard input
#define STDIN 0
#define BUFLEN 512


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
	//for bhasspace
	char command_line[150];
	char command[3][32];
	bool bhasSpace = false;
	int i;
	int startindex=0;
	int endindex =0;
	int commandindex =0;
	int iFD =0;
	//end bhasspace initialisation

	int opt;
	char filename[256];
	uint8_t IP[4];
	char myIP[INET_ADDRSTRLEN];
    char buf[BUFLEN];
	getMyIP(myIP);
	printf("IP address: %s\n",myIP );
	getIP(IP,myIP);//converting string IP address to uint_8t IP address
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
			printf("interval: %d\n",interval);
			break;
           case '?':   

           default:    printf("Usage: ./assignment3 -t <pathtotopologyfile> -i <routingupdateinterval>\n");
                       return -1;
       }
   }

	// read the topology file and load the data in structures 
	ReadTopologyFile(filename);



	// Creating the sockets
	int z=0;
	int z1=0;
	uint16_t port_num=0;
	struct sockaddr_in my_addr, cli_addr,serv_addr;
    int ListeningSockfd;
    socklen_t slen=sizeof(cli_addr);

    if ((ListeningSockfd = socket(AF_INET, SOCK_DGRAM,0))==-1)//IPPROTO_UDP
      err("socket");
    else
      printf("Server : Socket() successful\n");

	int ConnectingSockfd = socket(AF_INET, SOCK_DGRAM,0);

    bzero(&my_addr, sizeof(my_addr));
//extracting port number from topology file
	for(i=0;i<5;i++)
	{
	  	for(z1=0;z1<4;z1++)
		{
	  		if(a_idipport[i].topology_ip[z1]==IP[z1])
			{
			z=1;
			}
			else
			{
			z=0;
			}
		}
	    if(z==1)
		{
			port_num=a_idipport[i].topology_port;
		}
		z=0;

	 }
	printf("port number=%d\n",port_num);
int a_test=0;
	printf("enter 1 for bind or 2 to escape bind");
	scanf("%d",&a_test);

switch(a_test)
	{
	 case 1:
    my_addr.sin_family = AF_INET;
	// first serverid, ip & portnumber are current ip
	// so taking index '0'
    my_addr.sin_port = htons(port_num);
   my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	

	

    if (bind(ListeningSockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
      err("bind");
    else
      printf("Server : bind() successful\n");
	break;
	case 2: break;
       }

    // Initialisation for select
    fd_set myread_fds;
    fd_set myinitialset;
	int myMaxfd = ListeningSockfd;
    // clear all entries in myread_fds
    FD_ZERO(&myinitialset);
    FD_ZERO(&myread_fds);
    // adding STDINPUT to myinitialset
    FD_SET(STDIN, &myinitialset);
    // adding the socket that is being listened to myinitialset
    FD_SET(ListeningSockfd, &myinitialset);
    

	while(1)
	{
    	printf("\nEnter Command:\n");
        myread_fds= myinitialset; // copying 

		if (select(myMaxfd+1, &myread_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
		}

		if (FD_ISSET(STDIN, &myread_fds))
		{
			i = 0;
			bhasSpace = false;
			memset(command[0],0, 32);
			//printf("Enter Command: ");
			scanf("%[^\n]",command_line);

			// split commands with space ' '
			i =startindex= 0;
			commandindex = 0;

			while( command_line[i] != '\0')
			{
				if(command_line[i] == ' ')
				{
					bhasSpace = true;
					endindex = i;
					strncpy(command[commandindex],(command_line+startindex), (endindex - startindex));
					command[commandindex][endindex-startindex] = '\0';
					printf("\n%d: %s\n",i,command[commandindex]);
					startindex = endindex + 1;
					commandindex++;

					if(commandindex == 3)
					{
						strcpy(command[commandindex], (command_line +i+1));
						printf("\n%d: %s\n",i,command[commandindex]);
						
						break;
					}
				}    
  				i++;
			
			//if(( strcmp(command[0], "REGISTER") == 0) || (strcmp(command[0],"register")==0))
			}
			getchar();
			if(bhasSpace== false)
			{
				strcpy(command[0] , command_line);
			}	 					
			// Execute commands
			if((strcmp(command[0],"EXIT")== 0) || (strcmp(command[0],"exit")==0))
			{	
				printf("The command is %s\n",command[0]);
				break;
			}
			if((strcmp(command[0],"UPDATE")== 0) || (strcmp(command[0],"update")==0))
			{
				printf("The command is %s\n",command[0]);

				bzero(&serv_addr, sizeof(serv_addr));
    				serv_addr.sin_family = AF_INET;
   				 serv_addr.sin_port = htons(port_num);
  				  if (inet_aton(command[1], &serv_addr.sin_addr)==0)
   					 {
      						  fprintf(stderr, "inet_aton() failed\n");
      						  exit(1);
   					 }
 
   		 while(1)
    		{
       				 printf("\nEnter data to send(Type exit and press enter to exit) : ");
       				 scanf("%[^\n]",buf);
       				 getchar();
        			if(strcmp(buf,"exit") == 0)
        			  exit(0);
 			
       				 if (sendto(ConnectingSockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen)==-1)
          			  err("sendto()");
   		 }

			}
			if((strcmp(command[0],"STEP")== 0) || (strcmp(command[0],"step")==0))
			{
				printf("The command is %s\n",command[0]);
			}
			if((strcmp(command[0],"PACKETS")== 0) || (strcmp(command[0],"packets")==0))
			{
				printf("The command is %s\n",command[0]);
			}
			if((strcmp(command[0],"DISPLAY")== 0) || (strcmp(command[0],"display")==0))
			{
				printf("The command is %s\n",command[0]);
			}
			if((strcmp(command[0],"DISABLE")== 0) || (strcmp(command[0],"disable")==0))
			{
				printf("The command is %s\n",command[0]);
			}
			if((strcmp(command[0],"CRASH")== 0) || (strcmp(command[0],"crash")==0))
			{
				printf("The command is %s\n",command[0]);
			}
			if((strcmp(command[0],"DUMP")== 0) || (strcmp(command[0],"dump")==0))
			{
				printf("The command is %s\n",command[0]);
			}
			if((strcmp(command[0],"ACADEMIC_INTEGRITY")== 0) || (strcmp(command[0],"academic_integrity")==0))
			{
				printf("The command is %s\n",command[0]);
				printf("\nI have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");
				

			}
		}// IF STDIN

        // run through the existing connections looking for data to read

		for(iFD = 0; iFD <= myMaxfd; iFD++)
		{
			if (FD_ISSET(iFD, &myread_fds) && iFD!=STDIN)
			{ // we got one!!
				if (recvfrom(ListeningSockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
		            err("recvfrom()");
       					 printf("Received packet from %s:%d\nData: %s\n\n",
               						inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);

				/*if (iFD == myListeningSkfd)
				{
				}*/
			}
		}
	}


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
				//topology_idipport=(struct idipport*)malloc(numofservers*sizeof(struct idipport));
			}

			if(i_topology==2)
			{
				numofneighbours=atoi(buf);
				printf("no. of neighbours=%d\n",numofneighbours);
				//topology_ididcost=(struct ididcost*)malloc(numofneighbours*sizeof(struct ididcost));
				
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
void getMyIP (char * IP)

{
	//Part Of this code has been taken from : http://stackoverflow.com/questions/6490759/local-ip-after-gethostbyname

	struct sockaddr_in test,test1;
	int socklen;
	char local[242];
	int r=socket(AF_INET, SOCK_DGRAM,0);

	if(r<0)
	{
		printf(" MYIP socket error");
	}

	test.sin_family = AF_INET;
	test.sin_port = htons(2345);
	test.sin_addr.s_addr = inet_addr("8.8.8.8");

	connect(r,(struct sockaddr*)&test,sizeof test);

	socklen = sizeof(test1);

	getsockname(r,(struct sockaddr*)&test1,&socklen);

	inet_ntop(AF_INET,&(test1.sin_addr),local, sizeof(local));

	strcpy(IP,local);  

}
