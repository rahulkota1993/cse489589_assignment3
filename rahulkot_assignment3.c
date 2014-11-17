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
void getMyIP (char * IP);
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
	char myIP[INET_ADDRSTRLEN];
	getMyIP(myIP);

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
	return 0;
}

				//Functions definitions start from here

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
