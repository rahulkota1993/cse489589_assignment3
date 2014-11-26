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


struct neighbr
{
 bool tf;
}a_neighbr[5];

struct ididcost
{
 uint16_t myid;
 uint16_t neighbourid;
 uint16_t costlink;
}a_ididcost[5];

uint16_t nexthop[5];



// Global variables
struct idiport *topology_idipport;
struct ididcost *topology_ididcost;
int ServerIDArray[5];
int CostTable[5][5];
int numofservers;
int numofneighbours;

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
	char command[4][32];
	bool bhasSpace = false;
	int i,j;
	int startindex=0;
	int endindex =0;
	int commandindex =0;
	int iFD =0;
	//end bhasspace initialisation
	
	//for counting packets
	int count=0;
	
	//for error messages
	char error_update[]="Wrong input, The first Id should be My ID";
	char error_update1[]="Wrong input, The second Id is not my neighbour";
	
	//at receiving end
	uint16_t recv_id;

	

	int opt;
	char filename[256];
	uint8_t IP[4];
	char myIP[INET_ADDRSTRLEN];
    char buf[BUFLEN];
	getMyIP(myIP);
	printf("IP address: %s\n",myIP );
	getIP(IP,myIP);//converting string IP address to uint_8t IP address
	printf("size of IP[0]=%d\n",sizeof(IP[0]));
	int test_tv;


	
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
			//interval= atoi(optarg);
			tv.tv_sec= atoi(optarg);
			printf("tv: %d\n",tv.tv_sec);
			break;
           case '?':   

           default:    printf("Usage: ./assignment3 -t <pathtotopologyfile> -i <routingupdateinterval>\n");
                       return -1;
       }
   }

		//for cost table 
	for(i=0;i<5;i++)
	{
		a_ididcost[i].costlink=65535;
		
		for(j=0;j<5;j++)
		{
      		CostTable[i][j]=65535;
			
			//printf("%d\t",CostTable[i][j]);
		}
	//printf("\n");    
	}
		
	// read the topology file and load the data in structures 
	ReadTopologyFile(filename);

	

	


	
	uint16_t updt=0;
	uint16_t padding=0;
	//for update
	char *bf;
	int a=(8+(numofservers*12));
	bf= (char*)malloc(8+(numofservers*12));
	//at receivng
	char *rcvbf;
	rcvbf= (char*)malloc(8+(numofservers*12));
	uint16_t cost[5];
	



	// Creating the sockets
	int z=0;
	int z1=0;
	uint16_t port_num=0;
	uint16_t my_id=0;
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
	for(i=0;i<numofservers;i++)
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
			my_id=a_idipport[i].topology_id;
			printf("my id is %d\n",my_id);
		}
		z=0;

	 }
	printf("port number=%d\n",port_num);
	CostTable[my_id-1][my_id-1]=0;
	a_ididcost[my_id-1].costlink=0;

	//nexthop initialisation

	for(i=0;i<numofservers;i++)
	{
		if(a_ididcost[i].costlink==65535)
		{
			nexthop[i]=-1;
		}
		else
		{
			nexthop[i]=my_id;
		}
			printf("hop is nexthop[%d]=%d\n",i,nexthop[i]);

	}

	



    my_addr.sin_family = AF_INET;
	// first serverid, ip & portnumber are current ip
	// so taking index '0'
    my_addr.sin_port = htons(port_num);
   my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	

	

    if (bind(ListeningSockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
      err("bind");
    else
      printf("Server : bind() successful\n");
	

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
			
			/*command[0][32]=strtok(command_line," ");
			printf( " %s\n", command[0] );
			i++;
			 while( command_line != NULL ) 
  			 {
     			 
    
     		 	command[i][32] = strtok(NULL," ");
				printf( " %s\n", command[i] );
   			 }*/
		

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

					if((strcmp(command[0],"DISABLE")== 0) || (strcmp(command[0],"disable")==0))
					{
					
						strcpy(command[commandindex],(command_line +i+1));
						printf("\n%d: %s\n",i,command[commandindex]);
						break;
					}
					else
					{

					

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
			//getchar();
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
				
				
				/*printf("The command is %s\n",command[0]);
				printf("the command[1] is %s\n",command[1]);
				printf("the atoi is %d\n",atoi(command[1]));
				printf("size of updt %d\n",sizeof(updt));
				printf("size of portnumber %d\n",sizeof(port_num));
				printf("size of padding %d\n",sizeof(padding));
				printf("size of costlink %d\n",sizeof(a_ididcost[2].costlink));*/

				//Update logic 
				if(my_id!=atoi(command[1]))
				{
					cse4589_print_and_log("%s:%s\n", command[0],error_update);
					//printf("wrong command, enter myid as 1st id\n");
					
				}
				else
				{
					
					if(a_neighbr[atoi(command[2])-1].tf== false)
					{
						cse4589_print_and_log("%s:%s\n", command[0],error_update1);
						//printf("not a neighbour\n");
					}

					else
					{
						cse4589_print_and_log("%s:SUCCESS\n", command[0]);
						CostTable[my_id-1][atoi(command[2])-1]=atoi(command[3]);
						a_ididcost[atoi(command[2])-1].costlink=atoi(command[3]);
						printf("a_ididcost updated = %d\n",a_ididcost[atoi(command[2])-1].costlink);
					}
				}	


			
   				
				}
//else
				

			//}
			if((strcmp(command[0],"STEP")== 0) || (strcmp(command[0],"step")==0))
			{
				printf("The command is %s\n",command[0]);
				cse4589_print_and_log("%s:SUCCESS\n", command[0]);
			//for(i=0;i<numofservers;i++)
			//{

				
				
					printf("entered Step send loop\n");
					int i_mem=0;
					int test_mem;
				 	//send update to all
					//creating a buffer and memcopying every parameter we need to send

					bzero(&serv_addr, sizeof(serv_addr));
    				serv_addr.sin_family = AF_INET;
					//new one
				
					updt=5;
					memcpy(bf,&updt,2);
					memcpy(bf+2,&port_num,2);
					memcpy(bf+4, &IP[0],sizeof(IP[0]));
					
					memcpy(bf+5, &IP[1],sizeof(IP[1]));
					memcpy(bf+6, &IP[2],sizeof(IP[2]));
					memcpy(bf+7, &IP[3],sizeof(IP[3]));
					uint16_t k;
					memcpy(&k,bf+2,2);
					printf("\n\n\n\n%d\n\n\n",k);
	
			        test_mem=8;
					printf("num of neighbours= %d line:%d\n",numofneighbours,__LINE__);
	
					for(i_mem=0;i_mem<numofservers;i_mem++)
					{
					
						//if(a_neighbr[i_mem].tf==true)
						//{
						
						memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[0],1);
						test_mem++;
						memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[1],1);	
						test_mem++;
						memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[2],1);
						test_mem++;
						memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[3],1);
						test_mem++;
						memcpy(bf+test_mem, &a_idipport[i_mem].topology_port,2);
						printf("port =%d\n",a_idipport[i_mem].topology_port);
						test_mem=test_mem+2;
						memcpy(bf+test_mem, &padding,2);
						test_mem=test_mem+2;
						memcpy(bf+test_mem, &a_idipport[i_mem].topology_id,2);
						printf("id=%d\n",a_idipport[i_mem].topology_id);
						test_mem=test_mem+2;
						memcpy(bf+test_mem, &a_ididcost[i_mem].costlink,2);
						printf("costlink=%d\n",a_ididcost[i_mem].costlink);
						test_mem=test_mem+2;
						//}
					}
				
			//}

			 for(i=0;i<numofservers;i++)
			 {
				//if((a_idipport[i].topology_id!=my_id) && (a_neighbr[i].tf== true))
				 //	{
		
					bzero(&serv_addr, sizeof(serv_addr));
					serv_addr.sin_family = AF_INET;
					//check this part
					char s_update[INET_ADDRSTRLEN];
					sprintf(s_update,"%d.%d.%d.%d",a_idipport[i].topology_ip[0],a_idipport[i].topology_ip[1],a_idipport[i].topology_ip[2],a_idipport[i].topology_ip[3]);
					printf("string is %s\n",s_update);
					serv_addr.sin_port = htons(a_idipport[i].topology_port);

    			  	  if (inet_aton(s_update, &serv_addr.sin_addr)==0)
   					 {
    			  		  fprintf(stderr, "inet_aton() failed\n");
    			  		  exit(1);
   					 }
					if (sendto(ConnectingSockfd, bf, a, 0, (struct sockaddr*)&serv_addr, slen)==-1)
    	      			  err("sendto()");
				//	}
			}	
			}
			if((strcmp(command[0],"PACKETS")== 0) || (strcmp(command[0],"packets")==0))
			{
				printf("The command is %s\n",command[0]);
				printf("count=%d\n",count);
				count=0;
			}
			if((strcmp(command[0],"DISPLAY")== 0) || (strcmp(command[0],"display")==0))
			{
				printf("The command is %s\n",command[0]);
				PrintCostTable();
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
				cse4589_print_and_log("%s:SUCCESS\n", command[0]);
				
				printf("\nI have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");
				

			}
				
			getchar();
		}// IF STDIN

        // run through the existing connections looking for data to read

		for(iFD = 0; iFD <= myMaxfd; iFD++)
		{
			if (FD_ISSET(iFD, &myread_fds) && iFD!=STDIN)
			{ // we got one!!
					count++;
				if (recvfrom(ListeningSockfd, rcvbf, a, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
		            err("recvfrom()");
       					 printf("Received packet from %s:%d\n\n\n",
               						inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

				

					uint16_t k1,k2,k11,k12,k13;
					uint8_t k3,k4,k5,k6,k7,k8,k9,k10;
					memcpy(&k1,rcvbf,2);
					memcpy(&k2,rcvbf+2,2);
					memcpy(&k3,rcvbf+4,1);
					memcpy(&k4,rcvbf+5,1);
					memcpy(&k5,rcvbf+6,1);
					memcpy(&k6,rcvbf+7,1);
					memcpy(&k7,rcvbf+8,1);
					memcpy(&k8,rcvbf+9,1);
					memcpy(&k9,rcvbf+10,1);
					memcpy(&k10,rcvbf+11,1);
					memcpy(&k11,rcvbf+12,2);
					memcpy(&k12,rcvbf+14,2);
					memcpy(&k13,rcvbf+16,2);
					printf("k1=%d k2=%d\n",k1,k2);
					printf("k3=%d k4=%d k5=%d k6=%d k7=%d k8=%d k9=%d k10=%d k11=%d k12=%d k13=%d\n",k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13);
					
					
					memcpy(&cost[0],rcvbf+18,2);
					
					
					printf("the cost[0] is %d\n",cost[0]);
					
					for(i=1;i<(numofservers);i++)
					{
						memcpy(&cost[i],rcvbf+18+(12*i),2);
						printf("the cost[%d] is %d\n",i,cost[i]);
					}
					//finding id of the server from which the packet is received

					for(i=0;i<numofservers;i++)
					{
	  					
	  						if((a_idipport[i].topology_ip[0]==k3) && (a_idipport[i].topology_ip[1]==k4)&& (a_idipport[i].topology_ip[2]==k5)&& (a_idipport[i].topology_ip[3]==k6))
							{
							z=1;
							}
							else
							{
							z=0;
							}
						
	   					 if(z==1)
						{
						
						recv_id=a_idipport[i].topology_id;
						printf("recv_id is %d\n",recv_id);
						}
					z=0;

				 }//id found, recv_id
					 

					if(recv_id!=my_id)
					{
						printf("its not my id\n");
						for(i=0;i<numofservers;i++)
						{   
							//updating the received costs from neighbour
							//a_ididcost[recv_id-1].costlink[i]=cost[i];
							CostTable[recv_id-1][i]=cost[i];

							//bellmanford for for my costs calculation
						}
						for(i=0;i<numofservers;i++)
						{
							
						if(a_ididcost[i].costlink > ( cost[i] + a_ididcost[recv_id-1].costlink))
					 	{
							CostTable[my_id-1][i]= (cost[i] + a_ididcost[recv_id-1].costlink);
							a_ididcost[i].costlink= (cost[i] + a_ididcost[recv_id-1].costlink);
							nexthop[i]=a_idipport[recv_id-1].topology_id;
						}
							
						}
						
					}	
					else
					{
						printf("its my id \n");
					}	
			

				
			}
		}
	}


	PrintCostTable();	
	return 0;
}

void PrintCostTable(void)
{
	int i,j;
	//CostTable[my_id][my_id]=0;
	for( i = 0; i < numofservers; i++)
	{
	
		for(j = 0; j < numofservers; j++)
		{
			printf("%d \t",CostTable[i][j]);
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
				a_neighbr[serverid-1].tf=false;
				a_idipport[serverid-1].topology_id=serverid;
				printf("id=%d\n",a_idipport[serverid-1].topology_id);
				
				 for(k_topology=0;k_topology<4;k_topology++)
				{
				 a_idipport[serverid-1].topology_ip[k_topology]=serverip[k_topology];
				 printf("ip=%d\n",a_idipport[serverid-1].topology_ip[k_topology]);
				}
				a_idipport[serverid-1].topology_port=serverport;
				printf("port=%d\n",a_idipport[serverid-1].topology_port);
				
  	 	 	
  			}
			if(i_topology>(2+numofservers))
			{
				r_id=atoi(strtok(buf," "));
				//printf("r_id=%d\n",r_id);
				r_nid=atoi(strtok(NULL," "));
				//printf("r_nid=%d\n",r_nid);
				r_cost=atoi(strtok(NULL," "));
				//printf("r_cost=%d\n",r_cost);
				
				
				a_neighbr[r_nid-1].tf=true;
				printf("a_neighbr[%d}=%d\n",r_nid,a_neighbr[r_nid-1].tf);
				a_ididcost[r_nid-1].myid=r_id;
				a_ididcost[r_nid-1].neighbourid=r_nid;
				a_ididcost[r_nid-1].costlink=r_cost;
				printf("id=%d,nid=%d,costlink=%d\n",a_ididcost[r_nid-1].myid,a_ididcost[r_nid-1].neighbourid,a_ididcost[r_nid-1].costlink);

                if ( r_id <= 5 && r_nid <= 5)	
				{
					CostTable[ r_id -1] [ r_nid-1 ] = r_cost;
					//CostTable[ r_nid-1 ] [ r_id -1] = r_cost;
					// implement the 2D array with cost	
					//a_ididcost[i].myid=r_id;
					//a_ididcost[i].neighbourid=r_nid;
					//a_ididcost[i].costlink=r_cost;
					//printf("id=%d,nid=%d,costlink=%d\n",a_ididcost[serverid].myid,a_ididcost[serverid].neighbourid,a_ididcost[serverid].costlink);
				}

				
				
			}
				
		i_topology++;
		//printf("%s", buf);
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
	for( i = 0 ; i < numofservers ; i++)
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
