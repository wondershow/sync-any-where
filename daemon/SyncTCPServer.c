void *tcp_sync_server()
{
  int port;           	/* port number to use */
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
 
  port = getTCPPort();
  
  
  printf("The tcp port is %d \n",port);
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
    
    sprintf(filepath, "%s/%s",get_sync_home_path(),filename);
    printf("The file name is %s, path is %s",filename,filepath);
    
    
    
    
    
    
    
    
    
    
    
    /* exit server if filename is "quit" */
    if (strcmp(filename, "quit") == 0) {
      fprintf(stderr, "quit command received, shutting down server\n");
      break;
    }

    fprintf(stderr, "received request to send file %s\n", filename);
    printf("File name is %s \n",filename);
    /* open the file to be sent */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
      exit(1);
    }

    /* get the size of the file to be sent */
    fstat(fd, &stat_buf);

    /* copy file using sendfile */
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

    /* close descriptor for file that was sent */
    close(fd);

    /* close socket descriptor */
    close(desc);
  
  }
  
  /* close socket */
  close(sock);
}