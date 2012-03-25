/**
This file defines the format for messages used to exchange data between SyncAnywhere GUI an daemon
**/

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SYAW_MSG_ADD_SYNC_FILE 1
#define SYAW_MSG_NOTIFY_PROTOCOL 2
#define SYAW_MSG_NOTIFY_BYTES_FINISHED 3
#define SYAW_MSG_NOTIFI_NEW_SYNC_FILE 4


#define SYAW_SYNC_MODE_UDP 1
#define SYAW_SYNC_MODE_TCP 2

#define SKADDRESS   "/tmp/tsck8"


char *set_add_sync_file_msg(char *filepath,int file_len)
{
	char *res,buffer[9],file_len_hex[10];
	int msg_len = 10 + strlen(filepath);
	int i =0;
	
	//to convert file length to be a hex value 
	sprintf(buffer,"%8x",file_len);
	buffer[9] = 0;
	
	// to set the format of msg_buf
	res = (char *)malloc(msg_len);
	res[0] = (unsigned int)SYAW_MSG_ADD_SYNC_FILE;
	res[1] = (unsigned int)msg_len;
	for (i=0;i<8;i++)
	{
	  res[i+2] = buffer[i];
	}
	memcpy(res+10, filepath, strlen(filepath));
	//printf("The res is %s, filepath is %s, length is %d \n",res,filepath,strlen(res));
	//*buf_len = msg_len;
	return res;
}


char *get_file_path_from_msg_buf(char *buf)
{
      char *res;
      unsigned int buf_len = (unsigned int)buf[1];
      printf("The msg length is supposed to be: %d",buf_len);
      res = malloc(buf_len-10);
      memcpy(res,buf+10,buf_len-10);
      return res;
}

int get_file_size(char *filepath)
{
  struct stat buffer;
  int  status,fildes;
  
  
  fildes = open(filepath, O_RDWR);
  status = fstat(fildes, &buffer);
  
  return buffer.st_size;
}