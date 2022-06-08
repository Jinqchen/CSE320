#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>


#include "pbx.h"
#include "server.h"
#include "debug.h"
#include "csapp.h"

static void terminate(int status);
void sighup_handler(int sig){
    debug("exit success");
    terminate(EXIT_SUCCESS);
}
//open and return a listening socket
//from text book
/*int open_listenfd(int port){
    int listenfd,optval=1;
    struct sockaddr_in serveraddr;

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0) return -1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int))<0)return -1;
    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serveraddr.sin_port=htons((unsigned short)port);
    if(bind(listenfd,(SA *)&serveraddr,sizeof(serveraddr))<0)return -1;
    if(listen(listenfd,LISTENQ)<0)return -1;
    return listenfd;


}*/
/*
 * "PBX" telephone exchange simulation.
 *
 * Usage: pbx <port>
 */
int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.

    // int position=0;
    // int findP=0;
    // printf("argv:%s\n",*argv);
    // printf("argv:%s\n",*(argv+1));
    // printf("argv:%s\n",*(argv+2));
    // printf("argv:%s\n",*(argv+3));
    /*while(position<argc){
        if(strcmp(argv[position],"-p")==0){
            printf("got it%s,%d\n",argv[position],position);
            findP=1;
            break;
        }
        printf("got it%s,%d\n",argv[position],position);
        position++;
        printf("???\n");
    }*/
    if(argc!=3){
        fprintf(stderr,"Usage:bin/pbx -p <prot>\n");
        exit(EXIT_SUCCESS);
    }
    if(strcmp(argv[1],"-p")){
        fprintf(stderr,"Usage:bin/pbx -p <prot>\n");
        exit(EXIT_SUCCESS);
    }
    int port=atoi(argv[2]);
    //printf("%d\n",port);
    if(port<=0){
        fprintf(stderr,"Usage:bin/pbx -p <prot>\n");
        exit(EXIT_SUCCESS);
    }

    /*if(!findP){
        debug("handle -p");
        fprintf(stderr,"invalid option -- '%s'\n",argv[position]);
        //printf("???\n");
        terminate(EXIT_FAILURE);
    }*/
    //printf("success\n");



    // Perform required initialization of the PBX module.
    debug("Initializing PBX...");
    pbx = pbx_init();
    //debug("done?");
    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function pbx_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    struct sigaction new_act;
    new_act.sa_handler=sighup_handler;
    sigemptyset(&new_act.sa_mask);
    //sigaddset(&new_act.sa_mask,SIGHUP);
    int success;
    success=sigaction(SIGHUP,&new_act,NULL);
    if(success==-1){
        terminate(EXIT_FAILURE);
    }

    
    int listenfd,*connfd;
    socklen_t clientLen;
    pthread_t TID;
    struct sockaddr_storage clientAddress;
    //debug("lestening");
    listenfd=Open_listenfd(argv[2]);
    if(listenfd<0){
        //debug("exit");
        exit(EXIT_FAILURE);
    }
    while(1){
        //debug("lestening");
        clientLen=sizeof(struct sockaddr_storage);
        connfd=malloc(sizeof(int));
        //debug("accept");
        *connfd=Accept(listenfd,(SA *) &clientAddress,&clientLen);
        //debug("server part...");
        Pthread_create(&TID,NULL,pbx_client_service,connfd);

    }



    fprintf(stderr, "You have to finish implementing main() "
	    "before the PBX server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status) {
    debug("Shutting down PBX...");
    pbx_shutdown(pbx);
    debug("PBX server terminating");
    exit(status);
}
