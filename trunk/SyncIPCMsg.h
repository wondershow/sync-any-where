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

// The IP address of the master node, if a new node join the sync group, it will acquire information from that node.
#define MASTER_IP "169.254.7.160"

//#define MASTER_IP "131.96.49.139"

#define REPOS_DIR_NAME "/SyncAnywhere" // the directory name of SyncAnywhere, it is put directly under $HOME dir
#define PEER_LIST_FILE_NAME "/.peer_list"
#define RSRC_LIST_FILE_NAME "/.resrc_tbl"

#define MANAGE_PORT 10020; // this port is used for management, for peers to exchange application level information

#define MANAGE_CMD_JOIN_GRP "join" // the command code used for a peer to join this sync group
#define MANAGE_CMD_GET_RSTBL "get_tbl" //the command code used for a peer to request resource table from master node


#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define PATH_LEN 256
#define MD5_LEN 32


char *global_sync_home; // THE Home repository of SyncAnywhere at current machine
char *global_peer_file_path; // The peer list file path for SyncAnywhere
char *global_resource_list_path; //The resource list file path for SyncAnywhere


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


/**
  to find out whether the running machine is a master node.
  @return 0 yes it is
	   1 no 
**/
int is_master_node()
{
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;
  int res = 0;
  
  getifaddrs(&ifAddrStruct);
  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
	    
	    if(strcmp(addressBuffer,MASTER_IP)==0)
	      {	
		  res = 1;
	      }
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
	    if(strcmp(addressBuffer,MASTER_IP)==0)
	    {	
		  res = 1;
	    }
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    return res;
}

/**
  When SyncAnywhere is started on a machine, this function sets SyncAnywhere's file repository on that machine,
  if it does not has that dir, create a new one.
**/
void set_sync_home()
{
  char *home_env = getenv("HOME");
  global_sync_home = strcat(home_env, REPOS_DIR_NAME);
  printf("SYNC_HOME is %s\n",global_sync_home);
   
  struct stat st;
  if(stat(global_sync_home,&st) != 0) // if home dir does not exist, creating some application level dir and manage use files
  {
    int status;
    char *control_file_path;
    status = mkdir(global_sync_home, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    // to create the peer list file
    global_peer_file_path = strcat(global_sync_home,PEER_LIST_FILE_NAME);
    FILE *fd = fopen(global_peer_file_path,"w+"); 
    fclose(fd);
    
    //to create the resource table file
    global_resource_list_path = strcat(global_sync_home,RSRC_LIST_FILE_NAME);
    fd = fopen(global_resource_list_path,"w+"); 
    fclose(fd);
  }
}

char *get_peer_file_path()
{
  return global_peer_file_path;
}

char *get_resource_list_file_path()
{
  return global_resource_list_path;
}

char *get_sync_home_path()
{
  return global_sync_home;
}


/**
When one file has been distributed to every peer, add it to the resource file;
**/
void *add_file_into_resource(char *file_path)
{
    struct stat st;
    stat(file_path, &st);
    int file_size = st.st_size;
    //FILE *fd = fopen(global_resource_list_path,"a+");
    printf("File path is %s \n", file_path);
    printf("%s: mtime = %lld.%.9ld\n", file_path, (long long)st.st_mtim.tv_sec, st.st_mtim.tv_nsec);
    
    char line_buf[200];
    char md5[MD5_LEN + 1];
    get_md5(file_path,md5);
    
    sprintf(line_buf,"%s,%lld,%s \n",file_path, (long long)st.st_mtim.tv_sec, md5);
    //printf("The result is %s \n",line_buf);
}


int test_md5(char *filepath)
{
  char md5[MD5_LEN + 1];
  if (!get_md5(filepath, md5)) {
        puts("Error occured!");
  } else {
        printf("Success! MD5 sum is: %s\n", md5);
    }
}


/**
  To get the md5 sum of a given file
**/
int get_md5(char *file_name, char *md5_sum)
{
    #define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
    char cmd[PATH_LEN + sizeof (MD5SUM_CMD_FMT)];
    sprintf(cmd, MD5SUM_CMD_FMT, file_name);
    #undef MD5SUM_CMD_FMT

    FILE *p = popen(cmd, "r");
    if (p == NULL) return 0;

    int i, ch;
    for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++) {
        *md5_sum++ = ch;
    }

    *md5_sum = '\0';
    pclose(p);
    return i == MD5_LEN;
}