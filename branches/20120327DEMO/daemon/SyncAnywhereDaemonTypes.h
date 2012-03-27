/**
This file provides method and data structures used in SyncAnywhere Daemon.
Including thread-safe access functions.
**/

#include <pthread.h>
#define MAX_REPOS_NUM 30//the max number of unsynced file in the repository
#define MAX_PEER_NUM 10// the max number of peers

typedef struct sync_file_items
{
  char filename[32];
  char filepath[150];
  unsigned int file_size;
  char dest_ip[20];
  
  
  
  int is_used; 
} SyncReposItem;

typedef struct peer_ip
{
  char ip[30];
  
  /**indicate whpeer_listether this item is valqueue_headid, 0 means valied, 1 means invalud**/
  int is_used;
} PeerIp;
//mutex for safe access to SyncReposItem
pthread_mutex_t mutex_repos = PTHREAD_MUTEX_INITIALIZER;
SyncReposItem syncfile_repos[MAX_REPOS_NUM];
PeerIp peer_list[MAX_PEER_NUM];
int queue_head = 0;
int queue_tail = 0;


//
pthread_mutex_t mutex_trasmode = PTHREAD_MUTEX_INITIALIZER;
int global_transfer_mode; // TCP or UDP
int global_tcp_port;
int global_udp_port;

void initializeSyncRepos()
{
  queue_head = 0;
  queue_tail = 0;
}

/**
  to add an item into sync repository, it should be a thread safe way
**/
void addItem2SyncRepos(char *filename, char *filepath, unsigned int file_len)
{
  pthread_mutex_lock( &mutex_repos );
  
  
  int i;
  for(i=0;i<MAX_PEER_NUM;i++)
  {
    if(peer_list[i].is_used==0 && is_local_ip(peer_list[i].ip) == 0)
    { 	// when this peer_list item is valid and its correponding ip is not the local machine
	strcpy((char *)(syncfile_repos[queue_head].filename),filename );
	strcpy((char *)(syncfile_repos[queue_head].filepath),filepath );
	sprintf(syncfile_repos[queue_head].dest_ip,"%s",peer_list[i].ip);
	syncfile_repos[queue_head].file_size = file_len;
	queue_head++;
	if(queue_head==MAX_REPOS_NUM)
	{
	  queue_head = 0;
	}
    }
  }
  
  pthread_mutex_unlock( &mutex_repos );
}


void getCurrentUnsyncedFile(char *filename, unsigned int *file_len, char *ip)
{
  sprintf(filename,"%s", syncfile_repos[queue_tail].filename);
  *file_len = syncfile_repos[queue_tail].file_size;
  sprintf(ip,"%s", syncfile_repos[queue_tail].dest_ip);
}

/**
  to remove an item into sync repository, it should be a thread safe way
**/
void removeItemFromSyncRepos()
{
  pthread_mutex_lock( &mutex_repos );
  queue_tail++;
  if(queue_tail == MAX_REPOS_NUM)
      queue_tail = 0;
  pthread_mutex_unlock( &mutex_repos );
}

/**
  To find out whether we have unsynced items
  @return 0 yes we have
	   1 no we dont
*/
int hasMoreItemInSyncRepos()
{
  int res = -1;
  pthread_mutex_lock( &mutex_repos );
  if(queue_head == queue_tail)
  {
    printf("No the task queue is empty \n");
    res = 1;
  }
  else
  {
    printf("Yes there is items int the task queue \n");
    res = 0;
  }
  pthread_mutex_unlock( &mutex_repos );  
  return res;
}

void printSyncRepos()
{
  int num = queue_head-queue_tail;
  printf("In SyncRepos, there are %d items \n", num);
}

void setTransferMode(int mode_code)
{
  pthread_mutex_lock( &mutex_trasmode );
  global_transfer_mode = mode_code;
  pthread_mutex_unlock( &mutex_trasmode ); 
}

int getTransferMode()
{
  return global_transfer_mode;
}

void setTCPPort(int port)
{
  pthread_mutex_lock( &mutex_trasmode );
  global_tcp_port = port;
  pthread_mutex_unlock( &mutex_trasmode ); 
}

int getTCPPort()
{
  printf("In function getTCPPort(): gsdfasdfcp_port:%d",global_tcp_port);
  int res = global_tcp_port;
  return res;
}

void setUDPPort(int port)
{
  pthread_mutex_lock( &mutex_trasmode );
  global_udp_port = port;
  pthread_mutex_unlock( &mutex_trasmode ); 
}

int getUDPPort()
{
  return global_udp_port;
}

int initializePeerList()
{
  int i;
  for(i=0;i<MAX_PEER_NUM;i++)
  {
    peer_list[i].is_used = 1;
  }
  sprintf(peer_list[0].ip,"131.96.49.139"); //ip of master node
  peer_list[0].is_used = 0;
  sprintf(peer_list[1].ip,"131.96.49.204"); //ip of current node
  peer_list[1].is_used = 0;
}


/**
To send a msg to GUI via unix domain socket
**/
int send_msg_to_gui(char *buf)
{
  struct sockaddr_un dest_addr;
  int bytes_sent;
  int   sock,errno;
  
  printf("I am in the test_send_msg_to_gui, the buff is%s \n",  buf);
  
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sun_family = AF_UNIX;
  strcpy(dest_addr.sun_path, SK_GUI_ADDRESS1);
  
  sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  
  if (sock < 0) 
  {
    printf("socket failure %d\n", errno);
    exit(1);
  }

  
  bytes_sent = sendto(sock, buf, strlen(buf),0, &dest_addr, sizeof(struct sockaddr_un));
  
  if (bytes_sent < 0) {
    printf("sendto failure %d\n", errno);
    exit(1);
  }
}