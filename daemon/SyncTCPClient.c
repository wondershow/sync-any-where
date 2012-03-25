/***
  This function is used to query remote ip to get its tcp port number
**/



char *get_tcp_port(char *remote_ip)
{
      char query_res[10]; 
      
  
      int sd,cd;
      char buf[1000]=""; 
      struct sockaddr_in ser;
      FILE *rcvd_file;
      int con_res = 3;
      sd=socket(AF_INET,SOCK_STREAM,0); 
      if(sd<0)
      printf("SOCKET NOT CREATED\n"); 
      bzero(&ser,sizeof(struct sockaddr_in));  
      
      
      ser.sin_family=AF_INET; 
      
      //To use the manage port 
      /**todo MANAGE_PORT**/
      ser.sin_port=htons(10020);
 
      //To connect to remote ip
      inet_aton(remote_ip,&ser.sin_addr); 

      //initiate connection
      con_res = connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
      
	if(con_res == 0)
	      printf("Socket Connection Success\n");
	else
	      printf("Failed to Connect to remote socket\n");
 
   
	char cmd_buf[50];
	
        sprintf(cmd_buf,"%s",MANAGE_CMD_REQ_UDP_PORT);
	
	
	write(sd,cmd_buf,strlen(cmd_buf));
	
	int count = 0;
	while(count = read(sd,buf,1000) > 0)
	{
	  printf("The response from remote code is %s \n",buf);
	}
	
	/*
	rcvd_file = fopen("rcvd.txt","w");
	int pos = 0;
	while( count = read(sd,buf,1000) >0)
	{
	    int i=0;
	    for(;i<1000;i++)
		  {
		    if(buf[i]!=0)
		      {
	    if(SYAW_SYNC_MODE_UDP == getTransferMode())
	    {//Now the application is put into UDP mode, which means this client thread should sleep
		sleep(1); // sleep one second 
		break;
	    }
	    int sd,cd; 
	    char buf[1000]=""; 
	    struct sockaddr_in ser;
	    FILE *rcvd_file;
	    int con_res = 3;
	    
	  
	  }fprintf(rcvd_file,"%c",(char)buf[i]);
		  }
	    bzero(buf,1000);
	}
	fclose(rcvd_file);
	*/
	close(sd);
}




void *tcp_sync_client()
{
  
    
    while(1)
    {
      if(SYAW_SYNC_MODE_UDP == getTransferMode())
      {//Now the application is put into UDP mode, which means this client thread should sleep
	  sleep(1); // sleep one second 
	  printf("tcp_sync_client: I am sleeping:\n");
      } else if(1 == hasMoreItemInSyncRepos() ) ////first check whether we have unsynchronized files 
      {//if we dont have items to sync, then we sleep
	sleep(1);
	printf("tcp_sync_client: I am sleeping:\n");
	
      } else {
      
      char dest_ip[30];
      char file_path[200];
      char file_name[32];
      unsigned int file_len = 0;
      
      getCurrentUnsyncedFile(file_name,&file_len,dest_ip);

      printf("The next file is %s, ip is %s, length is %d \n",file_name,dest_ip,file_len);
      
      
      //The first step is to get the TCP port of remote peer
      
      
      
      
     
      }

    }
}