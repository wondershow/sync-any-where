void *tcp_sync_server4demo1()
{
  int port;           	/* port number to use */
  int *port_ptr;
  int sock;                  	/* socket desciptor */
  int desc;  	            	/* file descriptor for socket */
  int fd;                    /* file descriptor for file to send */
  struct sockaddr_in addr;   /* socket parameters for bind */
  struct sockaddr_in addr1;  /* socket parameters for accept */
  int    addrlen;            /* argument to accept */
  struct stat stat_buf;      /* argument to fstat */
  off_t offset = 0;          /* file offset */
  char filename[25];   		/* filename to rx/tx */
  int rc;                    /* holds return code of system calls */
 
 //port = getTCPPort();

  port = 55501;
 
   /* create Internet domain socke160t */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
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
   
  fprintf(stderr, "Just for fun\n");
  

  
  
  while (1) {
    char rcvd_buf[1000] = "";
    /* wait for a client to connect */
    desc = accept(sock, (struct sockaddr *)  &addr1, &addrlen);
    
    if (desc == -1) {
      fprintf(stderr, "accept failed: %s\n", strerror(errno));
      exit(1);
    }
     
    
    /** The first step is to get file name from remote client **/
    rc = recv(desc, filename, sizeof(filename), 0);
    if (rc == -1) {
      fprintf(stderr, "recv failed: %s\n", strerror(errno));
      exit(1);
    }

    /* null terminate and strip any \r and \n from filename */
	filename[rc] = '\0';
    if (filename[strlen(filename)-1] == '\n')
      filename[strlen(filename)-1] = '\0';
    if (filename[strlen(filename)-1] == '\r')
      filename[strlen(filename)-1] = '\0';
    char filepath[250];
    
    sprintf(filepath, "%s1/%s",get_sync_home_path(),filename);
    printf("\n The file name is %s, path is %s \n",filename,filepath);
    
    /**The second step is to reply an OK to client**/
    char buf[20];
    sprintf(buf,"%s","OK");
    write(desc,buf,strlen(buf));
    
    /**The 3rd step is to recefile from remote client**/
    fd = fopen(filepath, "w");
    if(fd<=0)
      printf("Failed to open the file \n");
    //while( count = read(sd,buf,1000) >0)
    
    int count =0;
    int i=0;
    bzero(rcvd_buf,1000);
    
    while( count = read(desc,rcvd_buf,1000) >0)
    {
	i = 0;
	printf("Count is %d \n",count);
	for(;i<1000;i++)
	{
	  if(rcvd_buf[i]!=0)
	      fprintf(fd,"%c",rcvd_buf[i]);
	}
	bzero(rcvd_buf,1000);
    }
    
    
    
    
    
    
    
    
    fclose(fd);
    /* close socket descriptor */
    close(desc);
  
  }
  
  /* close socket */
  close(sock);
}