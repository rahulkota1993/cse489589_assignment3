


//Update logic 
if(my_id!=atoi(command[1])
{
	printf("wrong command, enter myid as 1st id");
}

if(a_neighbr[atoi(command[2])-1].tf== false)
{
	printf("not a neighbour");
}


if(a_ididcost[atoi(command[2])-1].cost != atoi(command[3])
{

 //send update to all

 for(i=0;i<5;i++)
 {
	if(a_idipport[i].id!=my_id)
 	{
		
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//check this part
	char s_update[INET_ADDRSTRLEN];
	sprintf(s_update,"%d.%d.%d.%d",a_ididpport[i].ip[0],a_ididpport[i].ip[1],a_ididpport[i].ip[2],a_ididpport[i].ip[3]);
	printf("string is %s\n",s_update);
	serv_addr.sin_port = htons(a_idipport[i].port);

      	  if (inet_aton(s_update, &serv_addr.sin_addr)==0)
   		 {
      		  fprintf(stderr, "inet_aton() failed\n");
      		  exit(1);
   		 }
		if (sendto(ConnectingSockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen)==-1)
          			  err("sendto()");
	}
}
	
