



void *gui_listener_thread()
{
  
   int   sock,errno;
   int   fromlen, cnt;
   struct sockaddr_un listen_addr;
   struct sockaddr_un from_name;
   sock = socket(AF_UNIX, SOCK_DGRAM, 0);
   char buf[300];
   unsigned int req_type; // the GUI request type
   
   
   fprintf(stderr, "Just for fun, in sync_ioctl\n");
   
   // printf("TEst");
   if (sock < 0) {
      printf("socket failure %d\n", errno);
      close(sock);
      exit(1);
   }
   
   listen_addr.sun_family = AF_UNIX;
   strcpy(listen_addr.sun_path, SK_GUI_ADDRESS1);
   
   
   unlink(SK_GUI_ADDRESS1);
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
    printf("Get msg from Daemon %s \n",buf);
    
    switch((unsigned int) buf[0])
    {
//       char *filepath_tmp,*file_name;
//       int f_size_tmp;
//       char **plugins = NULL;
//       char delims[] = "/";
//       
//       char *port;
//       
      int i =0;
      int port_num;
      int file_len=0;
      int bytes_sent=0;
      int msg_len;
      int protocol;
      char dest_ip[30];
      char file_name[50];
      char byte_sent_hex[20];
      char file_len_hex[20];
      for(i=0;i<20;i++)
      {
	byte_sent_hex[i] = 0;
	file_len_hex[i] = 0;
      }

      
      /**
      msg format:
      bytes                   description
      0          msg_type(can be GUI_MSG_PROGRESS_NOTIFY or else)
      1          msg_length (the total length of this msg)
      2-3          port (2 is high, 3 is low)
      4-19          dest_ip (char)
      20-27        bytes sent
      28-35        file length
      36-end       file name
      **/ 
      case 1:  //GUI_MSG_PROGRESS_NOTIFY, to update progress bar
		port_num = 256*((unsigned char)buf[2]) + (unsigned char)buf[3];
		
		
		msg_len = (unsigned int)buf[1];
		printf("msg_len = %d bytes_sent= %d file_len = %d,port_num = %d \n",msg_len,bytes_sent,file_len,port_num);
// 		sprintf(file_name,"%s","");
		for(i=36;i<msg_len;i++)
		{
		  //if( (buf[i]>'A'&&buf[i]<'Z') || (buf[i]>'a'&& buf[i]<'z') || (buf[i]>'0' &&buf[i]<'9') ||  buf[i]=='.')
		    file_name[i-36] = buf[i];
		  //else
		    //break;
// 		  strcat(file_name,(char)buf[i]);
		}
		file_name[msg_len-36]=0;
// 		strncpy(file_name,buf+36,msg_len-36);
// 		
		for(i=0;i<8;i++)
		{
		  byte_sent_hex[i] = buf[i+20];
		  file_len_hex[i] = buf[i+28];
		}
		bytes_sent = strtol(byte_sent_hex, NULL, 16);
		file_len = strtol(file_len_hex, NULL, 16);
		
		
		printf("byte_sent_hex is %s,decimal value is %d\n",byte_sent_hex,bytes_sent);
		printf("file_len_hex is %s,decimal value is %d\n",file_len_hex,file_len);
		printf("file name is %s \n",file_name);
		
// // 		sprintf(dest_ip,"%s","");
// // 		for(i=4;i<=19;i++)
// // 		{
// // 		  dest_ip[i-4] = buf[i];//strcat(dest_ip,buf[i]);
// // 		}
// 		dest_ip[15] =0;
// 		printf("dest_ip %s \n",dest_ip);
		set_progress_bar(bytes_sent,file_name,file_len,port_num); 
		break;
      case 2:  // tell daemon which mode to use(tcp/udp)
		
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
