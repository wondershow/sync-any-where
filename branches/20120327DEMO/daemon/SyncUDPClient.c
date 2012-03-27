/***
  This function is used to query remote ip to get its tcp port number
**/

int udpsend_file_to_remote_peer4demo(char *filename,int port)
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
    char dest_ip[20];
    
    sprintf(dest_ip,"localhost");
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
    printf("In UDP Client Demo , the file to be opened is %s \n",filepath);
    sprintf(filepath,"%s/%s", get_sync_home_path(),filename);
    
    
     int inputFd, outputFd, openFlags;
     mode_t filePerms;
     ssize_t numRead;
     int buf_size = 1000;
     char send_buf[buf_size];
     int file_size = get_file_size(filepath);
     
     printf("The file to be read from is %d",filepath);
     inputFd = open(filepath, O_RDONLY);
     if (inputFd == -1)
         printf("opening file error");
 
     
     //time of transfer
      struct timeval tv;
      gettimeofday(&tv,NULL);
      int start = tv.tv_sec;
      
      
      
     /* Transfer data until we encounter end of input or an error */
     int byteSend = 0; 
     char *tmp_buf;
     int i = 0;
     while ((numRead = read(inputFd, send_buf, buf_size)) > 0)
     {
         if (write(sd, send_buf, numRead) != numRead)
             printf("couldn't write whole buffer");
	 else
	 {
	     byteSend += numRead;
	     //printf("Bytes sent: %d",byteSend);
	 }
	 
	 i++;
	 //send progress report to GUI
	 if( i%500 == 0)
	 {
	    tmp_buf = set_progress_notify_info(byteSend,file_size, filename, port, dest_ip);
	    send_msg_to_gui(tmp_buf);
	 }
	 usleep(500);
     }
     
     gettimeofday(&tv,NULL);
      int end = tv.tv_sec;
      gettimeofday(&tv,NULL);
      int duration = end-start;
     tmp_buf = set_progress_notify_info(byteSend,duration, filename, port, dest_ip);
     send_msg_to_gui(tmp_buf);
     //byteSend 
     close(inputFd);  
    removeItemFromSyncRepos();
    
    close(sd);
    return 0;
}













int get_udp_port(char *remote_ip)
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
      printf("ip is %s, port is %d\n",remote_ip,10021);
      //To connect to remote ip
      inet_aton(remote_ip,&ser.sin_addr); 
      //initiate connectiondest_ip
      con_res = connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
      
  
	if(con_res == 0)
	      printf("get_tcp_port: Socket Connection Success\n");
	else
	      printf("get_tcp_port:Failed to Connect to remote socket\n"); 
 
   
	char cmd_buf[50];
	
        sprintf(cmd_buf,"%s",MANAGE_CMD_REQ_UDP_PORT);
	
	
	write(sd,cmd_buf,strlen(cmd_buf));
	
	int count = 0;
	
	
	
	while( (count = read(sd,buf,1000) ) > 0)
	{
	  printf("The response from remote code is, which means, its udp port is %s,  \n",buf);
	}
	remote_host_tcp_port = atoi(buf);
	printf("The udp port is %d \n",remote_host_tcp_port);
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

int udpsend_file_to_remote_peer(char *filename,char *dest_ip,int port)
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
    
    
     int inputFd, outputFd, openFlags;
     mode_t filePerms;
     ssize_t numRead;
     int buf_size = 1000;
     char send_buf[buf_size];
 
     inputFd = open(filepath, O_RDONLY);
     if (inputFd == -1)
         printf("opening file error");
 
     /* Transfer data until we encounter end of input or an error */
     int byteSend = 0; 
     
      //time of transfer
      struct timeval tv;
      gettimeofday(&tv,NULL);
  
      int file_size = get_file_size(filepath);
      int start = tv.tv_sec;
      
      
      char *tmp_buf;
      int i=0;
      while ((numRead = read(inputFd, send_buf, buf_size)) > 0)
      {
         if (write(sd, send_buf, numRead) != numRead)
             printf("couldn't write whole buffer");
	 else
	 {
	     byteSend += numRead;
	     //printf("Bytes sent: %d",byteSend);
	 }
	 
	 i++;
	 //send progress report to GUI
	 if( i%500 == 0)
	 {
	  tmp_buf = set_progress_notify_info(byteSend,file_size, filename, port, dest_ip);
	  send_msg_to_gui(tmp_buf);
	 }
	 usleep(1000);
     }
      
      
      gettimeofday(&tv,NULL);
      int end = tv.tv_sec;
      gettimeofday(&tv,NULL);
      int duration = end-start;
      
      tmp_buf = set_progress_notify_info(byteSend,duration, filename, port, dest_ip);
      send_msg_to_gui(tmp_buf);
      printf("\nThe time duration is %d\n",duration);
     
     
     
      //printf("TIme used is %d",time_used);
      close(inputFd);
    
      
    
    
    
    /**The following part sends file in one shot, this 
    cant meet our req, as we need process bar 
    information but we make it a bak up**/
    
    /**bakup start**/
//     fd = open(filepath, O_RDONLY);
//     if (fd == -1) {
//       fprintf(stderr, "unable to open '%s': %s\n", filepath, strerror(errno));
//       exit(1);
//     }
// 
//     /* get the size of the file to be sent */
//     fstat(fd, &stat_buf);
// 
//     /* copy file using sendfile */
//     offset = 0;
//     
//     
//     rc = sendfile (sd, fd, &offset, stat_buf.st_size);
//     if (rc == -1) {
//       fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
//       exit(1);
//     } else
//     {
//       printf("file sending success 1\n");
//     }
//     if (rc != stat_buf.st_size) {
//       fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n",
//               rc, (int)stat_buf.st_size);
//       exit(1);
//     }
//     else
//       printf("file sending success 2\n");
// //       close(fd);/** Should be added, but not tested yet**/
    /**bakup ends**/
    
    
    removeItemFromSyncRepos();
    
    
    
    
    
    
    
    
    
    
    
    
    
    close(sd);
    return 0;
}


void *udp_sync_client()
{
  
    
    while(1)
    {
      if(SYAW_SYNC_MODE_TCP == getTransferMode())
      {//Now the application is put into UDP mode, which means this client thread should sleep
	  sleep(1); // sleep one second 
	  printf("udp_sync_client: not my duty, I am sleeping:\n");
      } else if(1 == hasMoreItemInSyncRepos() ) ////first check whether we have unsynchronized files 
      {//if we dont have items to sync, then we sleep
	sleep(1);
	printf("udp_sync_client: nothting to send, I am sleeping:\n");
	
      } else {
      
	  char dest_ip[30];
	  char file_path[200];
	  char file_name[32];
	  unsigned int file_len = 0;
	  int udp_port = 0;
	  
	  //getCurrentUnsyncedFile(file_name,&file_len,dest_ip);
	  getCurrentUnsyncedFile4DemoUDP(file_name,&file_len,dest_ip,&udp_port);
	  
	  
	  //test();
	  
	  //The first step is to get the TCP port of remote peer
	  int tcp_port = get_udp_port(dest_ip);
	  tcp_port = get_tcp_port(dest_ip);
	  printSyncRepos();
	  printf("The next file11111 is %s, ip is %s, length is %d, port is %d \n",file_name,dest_ip,file_len,udp_port);
	  udpsend_file_to_remote_peer4demo(file_name,udp_port);
	  sleep(5);
      }

    }
}