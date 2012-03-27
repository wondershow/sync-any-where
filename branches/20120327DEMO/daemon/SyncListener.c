//used for unix domain socket addressing
#define SKADDRESS     "/tmp/tsck8"



/**
This function listens the socks from GUI, and distribute the
different GUI request to daemon function
*/
void *sync_listen(void *args)
{
   int   sock,errno;
   int   fromlen, cnt;
   struct sockaddr_un listen_addr;
   struct sockaddr_un from_name;
   sock = socket(AF_UNIX, SOCK_DGRAM, 0);
   char buf[300];
   unsigned int req_type; // the GUI request type
   
   
   fprintf(stderr, "Just for fun, in sync_ioctl\n");
   
   
   if (sock < 0) {
      printf("socket failure %d\n", errno);
      close(sock);
      exit(1);
   }
   
   listen_addr.sun_family = AF_UNIX;
   strcpy(listen_addr.sun_path, SKADDRESS);
   
   
   unlink(SKADDRESS);
   if (bind(sock, (struct sockaddr *) &listen_addr,
	      sizeof(struct sockaddr_un)) < 0) {
    printf("bind failure %d\n", errno);
    close(sock);
    exit(1);
   }
   
   while(1)
   {
    cnt = recvfrom(sock, buf, sizeof(buf),
      0, &from_name, &fromlen);
    if (cnt < 0) {
      printf("recvfrom failure %d\n", errno);
      close(sock);
      exit(1);
    }
    
    buf[cnt] = '\0';  /* assure null byte */
    from_name.sun_path[fromlen] = '\0';
    printf("Get msg from GUI %s \n",buf);
    //continue;
    req_type = (unsigned int) buf[0];
    switch((unsigned int) buf[0])
    {
      char *filepath_tmp,*file_name;
      int f_size_tmp;
      char **plugins = NULL;
      char delims[] = "/";
      
      char *port;
      int port_num;
      int msg_len;
      int protocol;
      
      case 1:  //add a new file into sync repos
		filepath_tmp = get_file_path_from_msg_buf(buf);
		file_name = basename (filepath_tmp);
		f_size_tmp = get_file_size(filepath_tmp);
		printf("haha,file name is %s, the file path is %s,its length is %d\n",file_name,filepath_tmp,f_size_tmp);
		addItem2SyncRepos(file_name,filepath_tmp,f_size_tmp);
		//printSyncRepos();
		break;
      case 2:  // tell daemon which mode to use(tcp/udp)
		printf("Type is 2 \n");
		msg_len = (unsigned int)buf[1]; // the second byte is length of msg
		//sprintf(port,%s,)
		port = 256*((unsigned char)buf[3]) + (unsigned char)buf[4];
		printf("Here it is, they want to set tcp/udp,%d, %d\n",protocol,port);
		//strncpy(port,buf+3,msg_len-2);
		protocol = (unsigned int)buf[2];
		if(buf[2] == TRANS_PROTOCOL_TCP)
		{
		   setTCPPort(port);
		   setTransferMode(SYAW_SYNC_MODE_TCP);
		   //restart_tcp_server();
		}
		else
		{
		   setUDPPort(port);
		   setTransferMode(SYAW_SYNC_MODE_UDP);
		   //restart_udp_server();
		}
		//printf("Here it is, they want to set tcp/udp,%d, %d\n",protocol,port);
		break;
      case 3:  // reserved
		break;
      case 4:  // reservced
		break;
    }
    //printf("Request received from GUI, its string is %s, its type is %d\n",buf, (unsigned int) buf[0]);
   }
   close(sock);
}
