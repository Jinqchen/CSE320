/*
 * "PBX" server module.
 * Manages interaction with a client telephone unit (TU).
 */
#include <stdlib.h>

#include "debug.h"
#include "pbx.h"
#include "server.h"
#include "tu.h"
#include "csapp.h"

char *readMsg(FILE *file);
int getNum(FILE *file);
char* getMsg(FILE *file);
/*
 * Thread function for the thread that handles interaction with a client TU.
 * This is called after a network connection has been made via the main server
 * thread and a new thread has been created to handle the connection.
 */

void *pbx_client_service(void *arg) {
    int connectfd=*(int *)arg;
    free(arg);
    //debug("detach");

    // if(!pthread_detach(pthread_self())){
    //     exit(EXIT_FAILURE);
    // }
    //pthread_t p;
    //Pthread_create(&p,NULL,)
    Pthread_detach(pthread_self());
    //debug("tu ready init");
    TU *clientTU=tu_init(connectfd);

    //debug("ty init%d",connectfd);
    pbx_register(pbx,clientTU,connectfd);
    //debug("success?%d\n",check);
    FILE *clientP=fdopen(connectfd,"r");
    char *msg;
    while(1){
        //int count =0;
        //debug("msgbuf?");
        msg=readMsg(clientP);
        //debug("msg%s.",msg);
        //message in msg
        //pickup case
        /*int compare =strncmp(msg,"hangup",6);
        debug("msg is %s,compare to hangup%d",msg,compare);*/
        if(strncmp(msg,"pickup",6)==0){
            //debug("pickup msg");
            tu_pickup(clientTU);
        }else if(strncmp(msg,"hangup",6)==0){
            //debug("hangup msg");
            tu_hangup(clientTU);
        }else{
            
            if(strncmp(msg,"dial",4)==0){
                int n=getNum(clientP);
                //debug("dial n=%d",n);
                //tu_dial(clientTU,NULL);
                pbx_dial(pbx,clientTU,n);
                //tu_dial(clientTU,NULL);
                //debug("return=%d",re);
            }else if(strncmp(msg,"chat",4)==0){
                //int n=getNum(clientP);
                //debug("chat?");
                char *m=getMsg(clientP);
                //debug("chat msg%s",m);
                tu_chat(clientTU,m);
                
            }

            //need to deal with rest
        }
        //free(msg);

    }
    
    //server_loop()
    //if()
    free(msg);
    pbx_unregister(pbx,clientTU);
    return NULL;
}
char *readMsg(FILE *file){
    char* result=malloc(sizeof(char));
    //char* helper = result;
    int count = 0;
    int c=fgetc(file);
    
    //debug("loop?");
    while(c!='\r'&&c!=' '&&c!='\n'){
        if(c==EOF){
            //exit(EXIT_FAILURE);
            break;
        }
        *(result+count)=c;
        //debug("c%c",c);
        count++;
        //debug("realloc");
        result=realloc(result,count+1);
        //debug("realloc done");
        // if(c==EOF){
        //     exit(EXIT_FAILURE);
        // }
        //debug("add to buf");
        //*(result+count)=c;
        //debug("%s",result);
        //debug("print?");
        //result++;
        c=fgetc(file);
    }
    //char *helper=result;
    //free(result);
    //debug("out?");
    // if(fgetc(file)==EOF){
    //     exit(EXIT_FAILURE);
    // }
    // rest if it has

    //helper=result;
    return result;

}

int getNum(FILE *file){
    char* result=malloc(sizeof(char));
    //char* helper = result;
    int count = 0;
    int c=fgetc(file);
    
    //debug("loop?");
    while(c!='\r'&&c!=' '&&c!='\n'){
        if(c==EOF){
            //exit(EXIT_FAILURE);
            break;
        }
        *(result+count)=c;
        //debug("num get c%c",c);
        count++;
        
        result=realloc(result,count+1);
        
        // if(c==EOF){
        //     exit(EXIT_FAILURE);
        // }
        //debug("num add to buf");
        //*(result+count)=c;
        //debug("%s",result);
       // debug("print?");
        //result++;
        c=fgetc(file);
    }
    //debug("number%s",result);
    return atoi(result);
}
char* getMsg(FILE *file){
    char* result=malloc(sizeof(char));
    //char* helper = result;
    int count = 0;
    int c=fgetc(file);
    
    //debug("loop?");
    while(c!='\r'&&c!=' '&&c!='\n'){
        if(c==EOF){
            //exit(EXIT_FAILURE);
            break;
        }
        *(result+count)=c;
        //debug("num get c%c",c);
        count++;
        
        result=realloc(result,count+1);
        
        // if(c==EOF){
        //     exit(EXIT_FAILURE);
        // }
        //debug("num add to buf");
        //*(result+count)=c;
        //debug("%s",result);
       // debug("print?");
        //result++;
        c=fgetc(file);
    }
    
    return result;
}

