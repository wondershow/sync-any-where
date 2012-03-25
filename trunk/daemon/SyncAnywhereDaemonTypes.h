/**
This file provides method and data structures used in SyncAnywhere Daemon.
Including thread-safe access functions.
**/

#include <pthread.h>
#define MAX_REPOS_NUM 10//the max number of unsynced file in the repository

typedef struct sync_file_items
{
  char filename[32];
  char filepath[150];
  unsigned int file_size;
} SyncReposItem;

//mutex for safe access to SyncReposItem
pthread_mutex_t mutex_repos = PTHREAD_MUTEX_INITIALIZER;
SyncReposItem syncfile_repos[MAX_REPOS_NUM];
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
  strcpy((char *)(syncfile_repos[queue_head].filename),filename );
  strcpy((char *)(syncfile_repos[queue_head].filepath),filepath );
  syncfile_repos[queue_head].file_size = file_len;
  queue_head++;
  
  if(queue_head==MAX_REPOS_NUM)
  {
     queue_head = 0;
  }
  pthread_mutex_unlock( &mutex_repos );
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
    res = 1;
  else
    res = 0;
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
  return global_tcp_port;
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

