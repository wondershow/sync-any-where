/**
  This is the file for background servcie for SyncAnywhere.
  to compile:
  rm -rf SendfileServer;gcc -o SendfileServer SendfileServer.c -lpthread;./SendfileServer 
**/
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "SyncAnywhereDaemonTypes.h"
#include "../SyncIPCMsg.h"
#include <string.h>
#include "SyncTCPServer.c"
#include "SyncTCPClient.c"
#include "SyncListener.c"



int main(int argc, char **argv)
{
  
  
  
  initializeSyncRepos(); // to initialize the file repository
  
  setTransferMode(SYAW_SYNC_MODE_TCP);
  
  global_transfer_mode = SYAW_SYNC_MODE_TCP; // to set default tranfer mode
  
  setTCPPort(22202);
  
  /**  sync_thread synchronize files between peers while listen_thread dealwith request from the GUI **/
  pthread_t sync_tcp_server_thread, listen_thread, sync_tcp_client_thread,sync_udp_server_thread, sync_udp_client_thread           ;
  int  iret1, iret2;

  iret1 = pthread_create(&listen_thread,NULL,sync_listen,NULL);
  iret2 = pthread_create(&sync_tcp_server_thread,NULL,tcp_sync_server,NULL);
  
  
  //sync_listen();
  
  
  pthread_join( listen_thread, NULL);
  pthread_join( sync_tcp_server_thread, NULL);
  
  printf("zcd is a pig \n");
  
  //tcp_sync();
  
  return 0;
}
