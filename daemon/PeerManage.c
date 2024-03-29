
void peer_manange()
{
  int port;           	/* port number to use */
  int sock;                  	/* socket desciptor */
  int desc;  	            	/* file descriptor for socket */
  int fd;                    /* file descriptor for file to send */
  struct sockaddr_in addr;  /* socket parameters for bind */
  struct sockaddr_in addr1; /* socket parameters for accept */
  int    addrlen;            /* argument to accept */
  struct stat stat_buf;     /* argument to fstat */
  off_t offset = 0;          /* file offset */

  char request_code[100];    /* Request Code get from client file SyncIPCMsg.h for details*/
  int rc;
  
  port = 10021; 		// manage port of SyncAnywhere
  
  
  
  /* create Internet dotcp_sync_servermain socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    fprintf(stderr, "unable to create socket: %s\n", strerror(errno));
    exit(1);
  }

  /* fill in socket structure */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  /* bind socket to the port */
  rc =  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (rc == -1) {
    fprintf(stderr, "unable to bind to socket: %s\n", strerror(errno));
    exit(1);
  }

  /* listen for clients on the socket */
  rc = listen(sock, 1);
  if (rc == -1) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(1);
  }
   
  
  //listens for the connection from any peers
  while (1) {
	
    int udp_port = getUDPPort();
    int tcp_port = getTCPPort();
   // printf("11111111111Port number is %d \n",tcp_port);
    //char buf_to_send[200];
    /* wait for a client to connect */
    desc = accept(sock, (struct sockaddr *)  &addr1, &addrlen);
    
   // printf("2222222222222222Port number is %d \n",tcp_port);
    if (desc == -1) 
    {
      fprintf(stderr, "accept failed: %s\n", strerror(errno));
      exit(1);
    }
    //printf("333333333333333Port number is %d \n",tcp_port);
    /* get the file name from the client */
    rc = recv(desc, request_code, sizeof(request_code), 0);
    if (rc == -1) {
      fprintf(stderr, "recv failed: %s\n", strerror(errno));
      exit(1);
    }
   // printf("44444444444444444444444Port number is %d \n",tcp_port);
    /* null terminate and strip any \r and \n from filename */
    request_code[rc] = '\0';
    if (request_code[strlen(request_code)-1] == '\n')
      request_code[strlen(request_code)-1] = '\0';
    if (request_code[strlen(request_code)-1] == '\r')
      request_code[strlen(request_code)-1] = '\0';
  
    printf("Received reques from client: %s \n",request_code);
    
  //  printf("555555555555555555555Port number is %d \n",tcp_port);
    /**  The client request the UDP port of this server**/
    
    
    
    if ( 0 == strcmp(request_code, MANAGE_CMD_REQ_UDP_PORT) ) 
    {
      
      char buf_to_send[20];
      sprintf(buf_to_send,"%d",udp_port);
      printf("111Port number is %d \n",udp_port);
      write(desc,buf_to_send,strlen(buf_to_send));
      //send(sock, buf_to_send, strlen(buf_to_send), 0);
      
    }
    
    //printf("666666666666666666666666666Port number is %d \n",tcp_port);
    /** The client request the TCP port of this server**/
    //if (strcmp(request_code, MANAGE_CMD_REQ_TCP_PORT) == 0) 
    if (0 == strcmp(request_code, MANAGE_CMD_REQ_TCP_PORT)) 
    {
      char buf_to_send[20];
      sprintf(buf_to_send,"%d",tcp_port);
      printf("2222Port number is %d, buf_to_send is %s\n",tcp_port,buf_to_send);
      write(desc,buf_to_send,strlen(buf_to_send));
      //send(sock, buf_to_send, strlen(buf_to_send), 0);
    }

   // printf("777777777777777777776Port number is %d \n",tcp_port);

    /*
    //printf("File name is %s \n",filename);
    if(strcmp(filename,"get_file"))
    // open the file to be sent 
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
      exit(1);
    }

    // get the size of the file to be sent 
    fstat(fd, &stat_buf);

    // copy file using sendfile 
    offset = 0;
    rc = sendfile (desc, fd, &offset, stat_buf.st_size);
    if (rc == -1) {
      fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
      exit(1);
    }
    if (rc != stat_buf.st_size) {
      fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n",
              rc, (int)stat_buf.st_size);
      exit(1);
    }

    // close descriptor for file that was sent 
    close(fd); */

    // close socket descriptor 
    close(desc);
  }
  // close socket 
  close(sock);
  
  

}