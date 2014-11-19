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
void getMyIP (char * IP);
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
void getIP(uint8_t * ip, char * logline);

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
	fclose(fopen(LOGFILE, "w"));
	fclose(fopen(DUMPFILE, "wb"));

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
	printf("Usage: %s ./assignment3 t <pathtotopologyfile> i <routingupdateinterval> \n", argv[0]);
   	return -1;
   }

   /* 
    * Parse the arguments 
    * http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html 
    */
   while((opt = getopt(argc, argv,"t:i")) != -1){
       switch (opt){
           case 't':   printf("file %s\n",optarg);
			strcpy(filename,optarg);
			printf("filename %s\n",filename);

                      // seed = atoi(optarg);
                       break;
	   case 'i':   /*if(!isNumber(optarg)){
                           fprintf(stderr, "Invalid value for -i\n");
                           return -1;
                       }*/
			printf("%s\n",optarg);
			interval= atoi(optarg);
			break;
           case '?':   

           default:    printf("Usage: ./assignment3 t <pathtotopologyfile> i <routingupdateinterval>\n", argv[0]);
                       return -1;
       }
   }


#if 0
	

	struct sockaddr_in my_addr;
	struct sockaaddr_in clientaddress;
	socklen_t addrlen;
	int sockfd;
	//creating socket
	sockfd=socket(AF_INET, SOCK_DGRAM,0);
	if(sockfd<0)
	{
	  perror("listener: socket");
	}

	// Setting socket option, so it can be reused
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));
	
	//clearing the structure before use
	memset((char*)&my_addr, 0, sizeof(my_addr));

	//initializing my address
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons((unsigned short)port_num);

	/*binding server socket*/
	if (bind(sockfd, (struct sockaddr *) &my_addr,sizeof(my_addr)) < 0)
	{
		perror("ERROR in binding\n");
	}
#endif
	return 0;
}

	#if 0			//Functions definitions start from here

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
#endif
//parsing IP address using strtok has been referenced from http://www.friedspace.com/cprogramming/parsing.php
void getIP(uint8_t * ip, char * logline)
{
    ip[0] = atoi(strtok(logline, "."));
    ip[1] = atoi(strtok(NULL, "."));
    ip[2] = atoi(strtok(NULL, "."));
    ip[3] = atoi(strtok(NULL, " "));
    
    return;
}
