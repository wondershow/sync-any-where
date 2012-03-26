/***
  This function is used to query remote ip to get its tcp port number
**/

int test()
{

int sd,cd; 
 char buf[1000]="",buf1[1000]=""; 
 struct sockaddr_in ser;
 FILE *rcvd_file;
 int con_res = 3;

 sd=socket(AF_INET,SOCK_STREAM,0); 
 if(sd<0)
 printf("SOCKET NOT CREATED\n"); 
 bzero(&ser,sizeof(struct sockaddr_in)); 
 ser.sin_family=AF_INET; 
 ser.sin_port=htons(10021); 
 
 inet_aton("131.96.49.204",&ser.sin_addr); 

 con_res = connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
  
 if(con_res == 0)
 	printf("Socket Connection Success\n");
 else
	printf("Failed to connect to remote socket\n");
 
 for(;;) 
 {
    printf("ENTER THE MESSAGE"); 
    scanf("%s",buf);
    
    write(sd,buf,strlen(buf));
    
    int count = 0;
    rcvd_file = fopen("rcvd.txt","w");
    //fprintf(rcvd_file,"waaaaaa");
    int pos = 0;
    while( count = read(sd,buf,1000) >0)
    {
//       if(count==0)
	
	//printf("1RECEIVED %d FROM SERVER%s\n",count,buf);
	//write(rcvd_file,buf,1000,&pos);
	int i=0;
	for(;i<1000;i++)
	{
	  if(buf[i]!=0)
	    fprintf(rcvd_file,"%c",(char)buf[i]);
	}
	bzero(buf,1000);
// 	else if(count != 1000 && count >0)
//       {
// 	char buf1[count];
// 	memcpy(buf1,buf,count);
// 	printf("1RECEIVED FROM SERVER%s\n",buf1);
//       } 
      //read(sd,buf,1000);
      //printf("2RECEIVED FROM SERVER%s\n",buf);
      //read(sd,buf,1000);
     // printf("3RECEIVED FROM SERVER%s\n",buf);
    }
    fclose(rcvd_file);
 } 
 close(sd); 

}



int get_tcp_port(char *remote_ip)
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
      int remote_host_tcp_port = 0;
      
      
      ser.sin_family=AF_INET;
      
      //To use the manage port 
      /**todo MANAGE_PORT**/
      ser.sin_port=htons(10021);
      printf("get_tcp_port: ip is %s, port is %d\n",remote_ip,10021);
      //To connect to remote ip
      inet_aton(remote_ip,&ser.sin_addr); 
      //initiate connectiondest_ip
      con_res = connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
      
  
	if(con_res == 0)
	      printf("get_tcp_port: Socket Connection Success\n");
	else
	      printf("get_tcp_port:Failed to Connect to remote socket\n"); 
 
   
	char cmd_buf[50];
	
        sprintf(cmd_buf,"%s",MANAGE_CMD_REQ_TCP_PORT);
	
	
	write(sd,cmd_buf,strlen(cmd_buf));
	
	int count = 0;
	
	
	
	while( (count = read(sd,buf,1000) ) > 0)
	{
	  printf("The response from remote code is %s \n",buf);
	}
	remote_host_tcp_port = atoi(buf);
	printf("The tcp port is %d \n",remote_host_tcp_port);
	close(sd);
	
	return remote_host_tcp_port;
	/*dest_ip
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
		sleep(1); // sleep one second dest_ip
		break;
	    }
	    int sd,cd; 
	    char buf[1000]=""; dclose(sd);est_ip
	    struct sockaddr_in ser;
	    FILE *rcvd_file;
	    int con_res = 3;
	    
	  
	  }fprintf(rcvd_file,"%c",(char)buf[i]);
		  }
	    bzero(buf,1000);
	}
	fclose(rcvd_file);
	*/
	
}

int tcpsend_file_to_remote_peer(char *filename,char *dest_ip,int port)
{
    int sd,cd; 
    char buf[1000]="",buf1[1000]=""; 
    struct sockaddr_in ser;
    FILE *fd;
    int con_res = 3;
    struct stat stat_buf; 
    int offset;
    int rc;
    int desc;
    
    sd=socket(AF_INET,SOCK_STREAM,0); 
    if(sd<0)
    printf("SOCKET NOT CREATED\n"); 
    bzero(&ser,sizeof(struct sockaddr_in)); 
    ser.sin_family=AF_INET; 
    ser.sin_port=htons(port); 
    
    inet_aton(dest_ip,&ser.sin_addr); 

    con_res = connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
 
    if(con_res == 0)
 	printf("11111:Socket Connection Success\n");
    else
	printf("1111:Failed to connect to remote socket\n");
    
    
    /**The first step is to send file name **/
    sprintf(buf,"%s",filename);
    write(sd,buf,strlen(buf));
    
    
    /**The second step is wait for an OK from remote peer**/
    int cnt = recv(sd,buf,sizeof(buf),0);
    
    /* null terminate and strip any \r and \n from filename */
	buf[cnt] = '\0';
    if (buf[strlen(buf)-1] == '\n')
      buf[strlen(buf)-1] = '\0';
    if (buf[strlen(buf)-1] == '\r')
      buf[strlen(buf)-1] = '\0';
    
    if(strcmp(buf,"OK"))
	  printf("Good,\n");
    
    /**The 3rd step is to send file to remote peer**/
    /* open the file to be sent */
    char filepath[200];
    sprintf(filepath,"%s/%s", get_sync_home_path(),filename);
    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "unable to open '%s': %s\n", filepath, strerror(errno));
      exit(1);
    }

    /* get the size of the file to be sent */
    fstat(fd, &stat_buf);

    /* copy file using sendfile */
    offset = 0;
    rc = sendfile (sd, fd, &offset, stat_buf.st_size);
    if (rc == -1) {
      fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
      exit(1);
    } else
    {
      printf("file sending success 1\n");
    }
    if (rc != stat_buf.st_size) {
      fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n",
              rc, (int)stat_buf.st_size);
      exit(1);
    }
    else
      printf("file sending success 2\n");
    
    
    
    removeItemFromSyncRepos();
    
    
    
    
    
    
    
    
    
    
    
    
    
    close(sd);
    return 0;
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
	  
	  //test();
	  
	  //The first step is to get the TCP port of remote peer
	  int tcp_port = get_tcp_port(dest_ip);
	  
	  
	  printf("The next file is %s, ip is %s, length is %d, port is %d \n",file_name,dest_ip,file_len,tcp_port);
	  tcpsend_file_to_remote_peer(file_name,dest_ip,tcp_port);
	  sleep(5);
      }

    }
}