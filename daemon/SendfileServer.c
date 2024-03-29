/**
  This is the file for background servcie for SyncAnywhere.
  to compile:
  rm -rf SendfileServer;gcc -o SendfileServer SendfileServer.c -lpthread;./SendfileServer 
**/
#include <arpa/inet.h>

#include <errno.h>
#include <fcntl.h>

#include <ifaddrs.h>

#include <netinet/in.h> 
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>    
#include <unistd.h>
#include <ctype.h>

#include "../SyncIPCMsg.h"
#include "SyncAnywhereDaemonTypes.h"
#include "SyncTCPServer.c"
#include "SyncTCPClient.c"
#include "SyncListener.c"
#include "PeerManage.c"
#include "SyncUDPClient.c"
#include "SyncUDPServer.c"
#include <sys/time.h>

/**  sync_thread synchronize files between peers while listen_thread dealwith request from the GUI **/
pthread_t sync_tcp_server_thread, listen_thread, sync_tcp_client_thread,sync_udp_server_thread, sync_udp_client_thread, peer_mgr_thread;

void restart_tcp_server()
{
    printf("Tring to restart tcp daemon, new port is %d \n", getTCPPort());
    pthread_kill(sync_tcp_server_thread);
    pthread_create(&sync_tcp_server_thread,NULL,tcp_sync_server,NULL);
    pthread_join( sync_tcp_server_thread, NULL);
}

void restart_udp_server()
{
    printf("Tring to restart udp daemon, new port is %d \n", getTCPPort());
    pthread_kill(sync_udp_server_thread);
    pthread_create(&sync_udp_server_thread,NULL,udp_sync_server,NULL);
    pthread_join( sync_udp_server_thread, NULL);
}

void startDaemon()
{
      
 
  int  iret1, iret2,iret3,iret4,iret5,iret6;

  //char *user = getlogin();
  
  //struct passwd *pw = getpwuid(getuid());

  
  setTransferMode(SYAW_SYNC_MODE_UDP);
  setTCPPort(20000);
  setUDPPort(30000);
  set_sync_home();
  
  initializePeerList();
  
  
  
  //copy_file_to_apphome("/home/aniu/GoogleCode_Repos/CoursePoroject6620/sync-any-where/daemon/SimpleTCPTransferClient.c");
  
  //test_md5(test);
  
  /*
  if(is_master_node()==1) // yes, this machnie is a masternode
  {
    
  }*/
  printf("2222222222222222\n");
  iret1 = pthread_create(&listen_thread,NULL,sync_listen,NULL);
  iret2 = pthread_create(&sync_tcp_server_thread,NULL,tcp_sync_server,NULL);
  iret3 = pthread_create(&sync_tcp_client_thread,NULL,tcp_sync_client,NULL);
  iret4 = pthread_create(&peer_mgr_thread,NULL,peer_manange,NULL);
  iret5 = pthread_create(&sync_udp_client_thread,NULL,udp_sync_client,NULL);
  iret5 = pthread_create(&sync_udp_server_thread,NULL,udp_sync_server,NULL);
  
  
  pthread_join( listen_thread, NULL);
  pthread_join( sync_tcp_server_thread, NULL);
  pthread_join( tcp_sync_client, NULL);
  pthread_join( peer_mgr_thread, NULL);
  pthread_join( sync_udp_client_thread, NULL);
  pthread_join( sync_udp_server_thread, NULL);
}

int main(int argc, char **argv)
{
  
  
  
  initializeSyncRepos(); // to initialize the file repository
  
  setTransferMode(SYAW_SYNC_MODE_TCP);
  
  global_transfer_mode = SYAW_SYNC_MODE_TCP; // to set default tranfer mode
  startDaemon();
  //tcp_sync();
  
  return 0;
}
