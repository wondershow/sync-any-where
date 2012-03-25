void *tcp_sync_client()
{
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
    ser.sin_port=htons(22202); 
 
    inet_aton("localhost",&ser.sin_addr); 

    con_res = connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
      
    if(con_res == 0)
	    printf("Socket Connection Success\n");
    else
	    printf("Failed to connect to remote socket\n");
 
    
    // always running;
    while(1)
    {
	printf("ENTER THE MESSAGE"); 
	scanf("%s",buf);
	write(sd,buf,strlen(buf));
	int count = 0;
	rcvd_file = fopen("rcvd.txt","w");
	int pos = 0;
	while( count = read(sd,buf,1000) >0)
	{
	    int i=0;
	    for(;i<1000;i++)
	    {
	      if(buf[i]!=0)
		fprintf(rcvd_file,"%c",(char)buf[i]);
	    }
	    bzero(buf,1000);
	}
	fclose(rcvd_file);
      }
      close(sd); 
    }
    