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

//used for unix domain socket addressing
#define SKADDRESS     "/tmp/tsck8"


int global_transfer_mode; // TCP or UDP
int global_tcp_port;
int global_udp_port;
  

/**
This function listens the socks from GUI, and distribute the
different GUI request to daemon function
*/
void *sync_listen()
{
   int   sock,errno;
   int   fromlen, cnt;
   struct sockaddr_un listen_addr;
   struct sockaddr_un from_name;
   sock = socket(AF_UNIX, SOCK_DGRAM, 0);
   char buf[160];
   unsigned int req_type; // the GUI request type
   
   
   
   fprintf(stderr, "Just for fun, in sync_ioctl\n");
   if (sock < 0) {
      printf("socket failure %d\n", errno);
      close(sock);
      exit(1);
   }
   
   listen_addr.sun_family = AF_UNIX;
   strcpy(listen_addr.sun_path, SKADDRESS);
   
   
   unlink(SKADDRESS);
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
    
    req_type = (unsigned int) buf[0];
    switch((unsigned int) buf[0])
    {
      char *filepath_tmp;
      int f_size_tmp;
      case 1:  //add a new file into sync repos
		filepath_tmp = get_file_name_from_msg_buf(buf);
		f_size_tmp = get_file_size(filepath_tmp);
		printf("haha,the file path is %s,its length is %d\n",filepath_tmp,f_size_tmp);
		break;
      case 2:  // tell daemon which mode to use(tcp/udp)
		break;
      case 3:  // reserved
		break;
      case 4:  // reservced
		break;
    }
    
    printf("Request received from GUI, its string is %s, its type is %d\n",buf, (unsigned int) buf[0]);
   }
   close(sock);
}


void sync_transfer()
{
  

}


int main(int argc, char **argv)
{
  int port = 22202;           /* port number to use */
  int sock;                  /* socket desciptor */
  int desc;  	            /* file descriptor for socket */
  int fd;                    /* file descriptor for file to send */
  struct sockaddr_in addr;   /* socket parameters for bind */
  struct sockaddr_in addr1;  /* socket parameters for accept */
  int    addrlen;            /* argument to accept */
  struct stat stat_buf;      /* argument to fstat */
  off_t offset = 0;          /* file offset */
  char filename[25];   		/* filename to send */
  int rc;                    /* holds return code of system calls */
  
  /**  sync_thread synchronize files between peers while listen_thread dealwith request from the GUI **/
  pthread_t sync_thread, listen_thread;
  int  iret1, iret2;

  iret1 = pthread_create(&listen_thread,NULL,sync_listen,NULL);
  
  //sync_listen();
  pthread_join( listen_thread, NULL);

  
  /* check command line arguments, handling an optional port number */
  if (argc == 2) {
    port = atoi(argv[1]);
    if (port <= 0) {
      fprintf(stderr, "invalid port: %s\n", argv[1]);
      exit(1);
    }
  } else if (argc != 1) {
    fprintf(stderr, "usage: %s [port]\n", argv[0]);
    exit(1);
  }

  /* create Internet domain socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    fprintf(stderr, "unable to create socket: %s\n", strerror(errno));
    exit(1);
  }

  /* fill in socket structure */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  /* bind socket to the port */
  rc =  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (rc == -1) {
    fprintf(stderr, "unable to bind to socket: %s\n", strerror(errno));
    exit(1);
  }

  /* listen for clients on the socket */
  rc = listen(sock, 1);
  if (rc == -1) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(1);
  }
   
  fprintf(stderr, "Just for fun\n");

  while (1) {

    /* wait for a client to connect */
    desc = accept(sock, (struct sockaddr *)  &addr1, &addrlen);
    if (desc == -1) {
      fprintf(stderr, "accept failed: %s\n", strerror(errno));
      exit(1);
    }

    /* get the file name from the client */
    rc = recv(desc, filename, sizeof(filename), 0);
    if (rc == -1) {
      fprintf(stderr, "recv failed: %s\n", strerror(errno));
      exit(1);
    }

    /* null terminate and strip any \r and \n from filename */
	filename[rc] = '\0';
    if (filename[strlen(filename)-1] == '\n')
      filename[strlen(filename)-1] = '\0';
    if (filename[strlen(filename)-1] == '\r')
      filename[strlen(filename)-1] = '\0';

    /* exit server if filename is "quit" */
    if (strcmp(filename, "quit") == 0) {
      fprintf(stderr, "quit command received, shutting down server\n");
      break;
    }

    fprintf(stderr, "received request to send file %s\n", filename);
    printf("File name is %s \n",filename);
    /* open the file to be sent */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
      exit(1);
    }

    /* get the size of the file to be sent */
    fstat(fd, &stat_buf);

    /* copy file using sendfile */
    offset = 0;
    rc = sendfile (desc, fd, &offset, stat_buf.st_size);
    if (rc == -1) {
      fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
      exit(1);
    }
    if (rc != stat_buf.st_size) {
      fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n",
              rc, (int)stat_buf.st_size);
      exit(1);
    }   

    /* close descriptor for file that was sent */
    close(fd);

    /* close socket descriptor */
    close(desc);
  }
  
  /* close socket */
  close(sock);
  return 0;
}
