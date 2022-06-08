/*
 * PBX: simulates a Private Branch Exchange.
 */
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "csapp.h"
#include "pbx.h"
#include "debug.h"
#include "tu.h"
typedef struct pbx{
    TU *clientMap[PBX_MAX_EXTENSIONS];
    // int cientExt;
    // int clientFd;

}PBX;

//pthread_mutex_t mutex;
sem_t mutex;
/*
 * Initialize a new PBX.
 *
 * @return the newly initialized PBX, or NULL if initialization fails.
 */

PBX *pbx_init() {
    pbx=malloc(sizeof(PBX));
    for(int i = 0;i<PBX_MAX_EXTENSIONS;i++){
        pbx->clientMap[i]=NULL;
    }
    //pthread_mutex_init(&mutex,0);
    sem_init(&mutex,0,1);
    return pbx;
    //return NULL;
}



/*
 * Shut down a pbx, shutting down all network connections, waiting for all server
 * threads to terminate, and freeing all associated resources.
 * If there are any registered extensions, the associated network connections are
 * shut down, which will cause the server threads to terminate.
 * Once all the server threads have terminated, any remaining resources associated
 * with the PBX are freed.  The PBX object itself is freed, and should not be used again.
 *
 * @param pbx  The PBX to be shut down.
 */

void pbx_shutdown(PBX *pbx) {
    P(&mutex);
    for(int i =0;i<PBX_MAX_EXTENSIONS;i++){
        if(pbx->clientMap[i]!=NULL){
            TU *temp=pbx->clientMap[i];
            shutdown(tu_fileno(temp),SHUT_RDWR);
            free(temp);
        }else{
            free(pbx->clientMap[i]);
        }
    }
    free(pbx);
    V(&mutex);
    
    
}




/*
 * Register a telephone unit with a PBX at a specified extension number.
 * This amounts to "plugging a telephone unit into the PBX".
 * The TU is initialized to the TU_ON_HOOK state.
 * The reference count of the TU is increased and the PBX retains this reference
 *for as long as the TU remains registered.
 * A notification of the assigned extension number is sent to the underlying network
 * client.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU to be registered.
 * @param ext  The extension number on which the TU is to be registered.
 * @return 0 if registration succeeds, otherwise -1.
 */

int pbx_register(PBX *pbx, TU *tu, int ext) {
    //p(&mutex);
    // debug("register");
    //printf("register\n");
    //from tu to ext
    //pthread_mutex_lock(&mutex);
    P(&mutex);
    //tu = tu_init(ext);
    tu_set_extension(tu, ext);
    
    // for(int i =0;i<PBX_MAX_EXTENSIONS;i++){
    //     if(pbx->clientMap[i]!=NULL){
    //         pbx->clientMap[i]=tu;
    //         break;
    //     }
    // }
    if(pbx->clientMap[ext]!=NULL){
        V(&mutex);
        return -1;
    }
    pbx->clientMap[ext]=tu;
    V(&mutex);
    //pbx->clientMap[ext]=tu;
    
    dprintf(tu_fileno(tu),"%s %d\n",tu_state_names[TU_ON_HOOK],ext);
    // write(tu_fileno(tu),TU_ON_HOOK,10);
    // write(tu_fileno(tu),' ',1);
    // write(tu_fileno(tu),' ',1);
    


    tu_ref(tu, "add to pbx");
    //write(ext,TU_ON_HOOK,10);
    //pthread_mutex_unlock(&mutex);
    return 0;
}


/*
 * Unregister a TU from a PBX.
 * This amounts to "unplugging a telephone unit from the PBX".
 * The TU is disassociated from its extension number.
 * Then a hangup operation is performed on the TU to cancel any
 * call that might be in progress.
 * Finally, the reference held by the PBX to the TU is released.
 *
 * @param pbx  The PBX.
 * @param tu  The TU to be unregistered.
 * @return 0 if unregistration succeeds, otherwise -1.
 */

int pbx_unregister(PBX *pbx, TU *tu) {
    
    P(&mutex);
    // for(int i =0;i<PBX_MAX_EXTENSIONS;i++){
    //     if(pbx->clientMap[i]==tu){
    //         pbx->clientMap[i]=NULL;
    //         free(tu);
    //         pthread_mutex_unlock(&mutex);
    //         return 0;
    //     }
    // }
    tu_unref(tu, "unregister to pbx");
    if(tu==NULL||pbx==NULL){
        return -1;
    }
    tu=NULL;
    pbx->clientMap[tu_fileno(tu)]=NULL;
    free(tu);
    // tu_fileno(tu)
    
    V(&mutex);
    return 1;
    

    
}


/*
 * Use the PBX to initiate a call from a specified TU to a specified extension.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU that is initiating the call.
 * @param ext  The extension number to be called.
 * @return 0 if dialing succeeds, otherwise -1.
 */

int pbx_dial(PBX *pbx, TU *tu, int ext) {
    P(&mutex);
    if(tu==NULL){
        V(&mutex);
        return -1;
    }
    TU *target = pbx->clientMap[ext];
    if(target==NULL){
        V(&mutex);
        return -1;
    }
    // if(tu_dial(tu, target)==-1){
    //     V(&mutex);
    //     return -1;
    // }
    
     V(&mutex);
    return tu_dial(tu, target);
    //return 0;
        
    
    
}

