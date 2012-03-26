/**
	This file defines functions for SyncAnywhere to send/receive commands to daemons.
**/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

//used for unix domain socket addressing
#define SKADDRESS     "/tmp/tsck8"

static gboolean send_msg_to_daemon(gchar *buf)
{
  struct sockaddr_un dest_addr;
  gint bytes_sent;
  int   sock,errno;
  
  printf("I am in the send_msg_to_daemon, the buff is%s \n",  buf);
  
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sun_family = AF_UNIX;
  strcpy(dest_addr.sun_path, SKADDRESS);
  
  sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  
  if (sock < 0) 
  {
    printf("socket failure %d\n", errno);
    exit(1);
  }
  
  /*
  if (bind(sock, &dest_addr,strlen(dest_addr.sa_data) + sizeof(dest_addr.sa_family)) < 0){
    printf("bind failure %d\n", errno);
    exit(1);
  }*/
  //strcpy(buf, "test data line");
  
  bytes_sent = sendto(sock, buf, strlen(buf),0, &dest_addr, sizeof(struct sockaddr_un));
  
  if (bytes_sent < 0) {
    printf("sendto failure %d\n", errno);
    exit(1);
  }
  
  
}

static gboolean add_file_to_sync_repos(gchar *filepath)
{
	gint buf_len;
	gchar *msg_buf;
	msg_buf= set_add_sync_file_msg(filepath,20000);
	printf("In function add_file_to_sync_repos, filepath is%s\n, msg_buf is %s, msg_len is %d \n", filepath,msg_buf,(guint)msg_buf[1]);
	send_msg_to_daemon(msg_buf);
}

static gboolean change_tcp_udp_port(int mode, char *port)
{
      gchar *msg_buf;
      printf("In the function of change_tcp_udp_port \n" );
      msg_buf= set_set_trans_mode_msg(mode, port);
      send_msg_to_daemon(msg_buf);
}










