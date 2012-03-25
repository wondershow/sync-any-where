void *tcp_sync_client()
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
 ser.sin_port=htons(22202); 
 
 inet_aton("localhost",&ser.sin_addr); 

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