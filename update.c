



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
//creating a buffer and memcopying every parameter we need to send
char *bf;
memcpy(bf,&updt,sizeof(updt));
memcpy(bf+ 2,&port_num,sizeof(port_num));
memcpy(bf+ 4, &IP[0],sizeof(IP[0]);
memcpy(bf+ 5, &IP[1],sizeof(IP[1]);
memcpy(bf+ 6, &IP[2],sizeof(IP[2]);
memcpy(bf+ 7, &IP[3],sizeof(IP[3]);

int test_mem=8;

for(i_mem=0;i_mem<=numofneighbours;i_mem++)
{
	if(a_neighbr[i_mem].tf==true)
	{
		memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[0],1);
		test_mem++;
		memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[1],1);	
		test_mem++;
		memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[2],1);
		test_mem++;
		memcpy(bf+test_mem, &a_idipport[i_mem].topology_ip[3],1);
		test_mem++;
		memcpy(bf+test_mem, &a_idipport[i_mem].topology_port,2);
		test_mem=test_mem+2;
		memcpy(bf+test_mem, &padding,2);
		test_mem=test_mem+2;
		memcpy(bf+test_mem, &a_idipport[i_mem].topology_id,2);
		test_mem=test_mem+2;
		memcpy(bf+test_mem, &a_ididcost[i_mem].costlink,2);
		test_mem=test_mem+2;
	}
}

//sending update to each neighbour

 for(i=0;i<5;i++)
 {
	if((a_idipport[i].id!=my_id) && (a_neighbr[i]== true))
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
		if (sendto(ConnectingSockfd, bf, strlen(bf), 0, (struct sockaddr*)&serv_addr, slen)==-1)
          			  err("sendto()");
	}
}

	