/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"
#include <errno.h>
static sf_block *prologue;
static sf_block *epilogue;
sf_size_t payload;
double totalPayload=0;
int growTime=0;
int inQuick=0;
void initFreeList();
void initPrologue();
void initEpilogue();
void addToFreeList(sf_size_t size,sf_block* block);
int getIndex(sf_size_t size);
void removeFromFree(sf_block* block);
sf_block *searchFree(int size);
void split(sf_block *block,int left,int needSize);
void setFooter(sf_block *block,int size);
void extandMem(sf_block *block,void* start);
void coalesce(sf_block*cur);
sf_size_t getBlockSize(sf_block* block);
void freeABlock(sf_block* block);
int canInQuick(int a);
void addToQuickList(sf_size_t size,sf_block* block);
int getQuickIndex(int a);
void setPayLoadSize(sf_block* block,unsigned long pls);
void removePayloadSize(sf_block* block);
void RemoveNextPre(sf_block* block);
void AddNextPre(sf_block* block);
sf_size_t getPayloadSize(sf_block *block);
void flushQuick(int index);
void removeFromFree(sf_block* block){
    block->body.links.prev->body.links.next=block->body.links.next;
    block->body.links.next->body.links.prev=block->body.links.prev;

}
void removeFromQuick(sf_block* block,int index){
    sf_quick_lists[index].first=block->body.links.next;   
    sf_quick_lists[index].length-=1;
}
void flushQuick(int index){
    sf_block *block=NULL;
    for(int i=0;i<QUICK_LIST_MAX;i++){
        block=sf_quick_lists[index].first;
        removeFromQuick(block,index);
        freeABlock(block);
        //coalesce(block);
        addToFreeList(getBlockSize(block),block);
        
    }
    for(int i=0;i<4;i++){
        coalesce(block);
    }
}
void *sf_malloc(sf_size_t size) {
    totalPayload+=size;
    if(size<=0){
        return NULL;
    }
    
    if(sf_mem_start()==sf_mem_end()){//if first use, start == end
        if(sf_mem_grow()==NULL){
            return NULL;
        }
        growTime+=1;
        //initialize everything
        initFreeList();//good
        initPrologue();//good
        sf_block* initBlock=(sf_block*)((char *)prologue+32);//prologue 32 
        initBlock->header=(((PAGE_SZ-48))|PREV_BLOCK_ALLOCATED)^MAGIC;
        setFooter(initBlock,(PAGE_SZ-48));
        

        initEpilogue();       
        addToFreeList((initBlock->header^MAGIC),(sf_block*)((initBlock)));
    }
    sf_size_t tsize= size+sizeof(sf_header);
    sf_size_t needSize;
    
    if(tsize%16==0){//get suitable block size
        needSize=tsize;
    }else{
        needSize=tsize+16-(tsize%16);
    }
    if(needSize<32){//minimum size is 32;
        needSize=32;
    }
    
    payload=size;
    sf_block *findBlock= searchFree(needSize);
   
    if(findBlock==NULL){
        //extand memory
       
        while(findBlock==NULL){
            void *newMem=sf_mem_grow();
            growTime+=1;
            if(newMem==NULL){
                sf_errno=ENOMEM;
                return NULL;
            }
            extandMem(findBlock,newMem);
            findBlock= searchFree(needSize);
        }
        if(inQuick==1){
            int index=getQuickIndex(getBlockSize(findBlock));
            removeFromQuick(findBlock,index);
            inQuick=0;
        }else{
            removeFromFree(findBlock);
            }
        int btyeleft=getBlockSize(findBlock)-needSize;
        findBlock->header=((findBlock->header^MAGIC)|THIS_BLOCK_ALLOCATED)^MAGIC;
       
        setPayLoadSize(findBlock,(unsigned long)payload);
      
        if(btyeleft>32){
            
            split(findBlock,btyeleft,needSize);
        }
        AddNextPre(findBlock);
        return findBlock->body.payload;

    }else{
        
        if(inQuick==1){
            int index=getQuickIndex(getBlockSize(findBlock));
            removeFromQuick(findBlock,index);
            inQuick=0;
        }else{
            removeFromFree(findBlock);
            }
       
        int btyeleft=getBlockSize(findBlock)-needSize;
        findBlock->header=((findBlock->header^MAGIC)|THIS_BLOCK_ALLOCATED)^MAGIC;
        
        setPayLoadSize(findBlock,(unsigned long)payload);
       
        if(btyeleft>32){
            split(findBlock,btyeleft,needSize);
        }
        AddNextPre(findBlock);
    }
  
    return findBlock->body.payload;
}

void setPayLoadSize(sf_block* block,unsigned long pls){
    unsigned long payloadSize=pls<<32;
    block->header=((block->header^MAGIC)|payloadSize)^MAGIC;

}
void removePayloadSize(sf_block* block){
    unsigned long remove=(unsigned long )(block->header ^ MAGIC) >> 32<<32;
    block->header=((block->header^MAGIC)-remove)^MAGIC;
}






void sf_free(void *pp) {
    //check pp 
    if(pp==NULL) abort();
    sf_block* temp=(sf_block*)((char*)pp-16);
    int size=getBlockSize(temp);
    if(size%16!=0) abort();
    if(size<32) abort();
    int cur_Alloc=(temp->header^MAGIC)&THIS_BLOCK_ALLOCATED;
    if(cur_Alloc==0){
        abort();

    }
     int pre_Alloc=(temp->header^MAGIC)&PREV_BLOCK_ALLOCATED;
    if(pre_Alloc==0){//not allocted
         sf_size_t prev_size=((temp->prev_footer^MAGIC)&(~(THIS_BLOCK_ALLOCATED)));
        sf_block* prev_block=(sf_block*)((void*)temp -prev_size);
        int pre_Alloc=(prev_block->header^MAGIC)&THIS_BLOCK_ALLOCATED;
        if(pre_Alloc!=0)abort();
    }
    if((temp<prologue)||(temp>epilogue))abort();
    if(canInQuick(size)==-1){
        freeABlock(temp);
        
        addToFreeList(size,temp);
        removePayloadSize(temp);
        setFooter(temp,getBlockSize(temp));
        coalesce(temp);
       
    }else{
        addToQuickList(size,temp);
        //freeABlock(temp);
        temp->header=((temp->header^MAGIC)|IN_QUICK_LIST)^MAGIC;
        removePayloadSize(temp);
        //setFooter(temp,getBlockSize(temp));



       
       
    }
   
}
void freeABlock(sf_block* block){
    
    block->header=((block->header^MAGIC)-(THIS_BLOCK_ALLOCATED))^MAGIC;
    


   
    setFooter(block,getBlockSize(block));
    RemoveNextPre(block);
}
void addToQuickList(sf_size_t size,sf_block* block){
    int index=getQuickIndex(size);
    
    int length=sf_quick_lists[index].length;
 
    if(length<QUICK_LIST_MAX){
    
        block->body.links.next=sf_quick_lists[index].first;
        sf_quick_lists[index].first=block;
        sf_quick_lists[index].length+=1;

    }else{
        //flushing
        flushQuick(index);
        block->body.links.next=sf_quick_lists[index].first;
        sf_quick_lists[index].first=block;
        sf_quick_lists[index].length+=1;
    }

}
int canInQuick(int a){
    if(a>0&&a<=(32+(9*16))){
        return getQuickIndex(a);
    }else return -1;
}
int getQuickIndex(int a){
    if(a==32)return 0;
    else if(a==48) return 1;
    else if(a==64) return 2;
    else if(a==80) return 3;
    else if(a==96) return 4;
    else if(a==112) return 5;
    else if(a==128) return 6;
    else if(a==144) return 7;
    else if(a==160) return 8;
    else if(a==176) return 9;
    else{return -1;}

}
void *sf_realloc(void *pp, sf_size_t rsize) {
    if(pp==NULL){
        sf_errno=EINVAL;
        return NULL;

        } 
    sf_block* temp=(sf_block*)((char*)pp-16);
    int size=getBlockSize(temp);
    if(size%16!=0){
        sf_errno=EINVAL;
        return NULL;
    } 
    if(size<32){
        sf_errno=EINVAL;
        return NULL;
    } 
    int cur_Alloc=(temp->header^MAGIC)&THIS_BLOCK_ALLOCATED;
    if(cur_Alloc==0){
        sf_errno=EINVAL;
        return NULL;
    } 
     int pre_Alloc=(temp->header^MAGIC)&PREV_BLOCK_ALLOCATED;
     if(pre_Alloc==0){//not allocted
         sf_size_t prev_size=((temp->prev_footer^MAGIC)&(~(THIS_BLOCK_ALLOCATED)));
        sf_block* prev_block=(sf_block*)((void*)temp -prev_size);
        int pre_Alloc=(prev_block->header^MAGIC)&THIS_BLOCK_ALLOCATED;
        if(pre_Alloc!=0){
        sf_errno=EINVAL;
        return NULL;
    } 
     }
    if((temp<prologue)||(temp>epilogue))abort();
    if(rsize==0){
        sf_free(pp);
        return NULL;
    }
    rsize+=8;
    int oldSize=getBlockSize(temp);
    if(rsize>oldSize){//larger
        
        void* largerBlock=sf_malloc(rsize-8);//payload
        
        memcpy(largerBlock,temp,oldSize-8);
        sf_free(pp);
        return largerBlock;


    }else if(rsize<oldSize){
        int needSize=0;
        if(rsize%16==0){//get suitable block size
            needSize=rsize;
        }else{
            needSize=rsize+16-(rsize%16);
        }
        if(needSize<32){//minimum size is 32;
            needSize=32;
        }
        payload=rsize-8;
        sf_block* smallerBlock=(sf_block*)temp;
       
        int btyeleft=((smallerBlock->header^MAGIC)&(~(THIS_BLOCK_ALLOCATED|PREV_BLOCK_ALLOCATED|IN_QUICK_LIST))) -needSize;
        smallerBlock->header=((smallerBlock->header^MAGIC)|THIS_BLOCK_ALLOCATED)^MAGIC;
        
        removePayloadSize(smallerBlock);
        setPayLoadSize(smallerBlock,(unsigned long)payload);
       
        if(btyeleft>32){
            split(smallerBlock,btyeleft,needSize);
            sf_block *next=(sf_block *)((char*)smallerBlock+getBlockSize(smallerBlock));
            RemoveNextPre(next);
            
            coalesce(next);
            
        }
       
        coalesce(smallerBlock);


    }
    return temp->body.payload;

    
}

double sf_internal_fragmentation() {
    double pld=0.0;
    double bSize=0.0;
    sf_block *start=prologue;
    sf_block *end=epilogue;
    sf_block *next=(sf_block *)((char*)start+getBlockSize(start));
    while(next!=end){
        int alloc=(next->header^MAGIC)&THIS_BLOCK_ALLOCATED;
        if(alloc!=0){
            pld+=getPayloadSize(next);
            bSize=getBlockSize(next);
        }
        next=(sf_block *)((char*)next+getBlockSize(next));
    }
    if(bSize==0||pld==0){
        return 0.0;
    }
    return pld/bSize;
}

double sf_peak_utilization() {
    if(sf_mem_start()==sf_mem_end()){
        return 0.0;
    }
    return totalPayload/((double)growTime*PAGE_SZ);
    
}






void initFreeList(){
    for(int i=0;i<NUM_FREE_LISTS;i++){
        sf_free_list_heads[i].body.links.next=&sf_free_list_heads[i];
        sf_free_list_heads[i].body.links.prev=&sf_free_list_heads[i]; 
    }
}

void initPrologue(){
    prologue=(sf_block*)(sf_mem_start());
    prologue->header=(32|THIS_BLOCK_ALLOCATED)^MAGIC;
   
    
}
void initEpilogue(){
    epilogue=(sf_block *)(sf_mem_end()-16);
    epilogue->header=(0|THIS_BLOCK_ALLOCATED)^MAGIC;;
    setFooter(epilogue,8);
    
    
    
}
void addToFreeList(sf_size_t size,sf_block* block){
    int index=getIndex(size);
    sf_block *temp=&sf_free_list_heads[index];
    
    block->body.links.next=temp->body.links.next;
    
    block->body.links.prev=temp;
    temp->body.links.next->body.links.prev=block;
    temp->body.links.next=block;
    
    

   

}


int getIndex(sf_size_t size){
    int Min=32;
    if(size<=Min) return 0;
    else if (size<=2*Min) return 1;
    else if (size<=4*Min) return 2;
    else if (size<=8*Min) return 3;
    else if (size<=16*Min) return 4;
    else if (size<=32*Min) return 5;
    else if (size<=64*Min) return 6;
    else if (size<=128*Min) return 7;
    else if (size<=256*Min) return 8;
    else if (size>256*Min) return 9;
    else return -1;

}    
sf_block *searchFree(int size){
    int index=canInQuick(size);
    //search in quick list first;
    if(index!=-1){
        if(sf_quick_lists[index].first==NULL){
            
        }else{
                inQuick=1;
                return sf_quick_lists[index].first;
        }
    }
    
   

  
    //search in main free list ;
    for(int i = 0;i<NUM_FREE_LISTS;i++){
        sf_block* temp=sf_free_list_heads[i].body.links.next;
        sf_block* temp2=sf_free_list_heads[i].body.links.next->body.links.next;
        while(temp!=temp2){
            if(size<=getBlockSize(sf_free_list_heads[i].body.links.next)){
                
                return sf_free_list_heads[i].body.links.next;

            }
            temp2=temp2->body.links.next;
        }
    }
    return NULL;
}
void split(sf_block *block,int left,int needSize){
    block->header=(((block->header^MAGIC)|THIS_BLOCK_ALLOCATED)-left)^MAGIC;
    setPayLoadSize(block,(unsigned long)payload);

    sf_block *split=(sf_block *)((char*)block+needSize);
    split->header=(left|PREV_BLOCK_ALLOCATED)^MAGIC;
    
    setFooter(split,left);
    
    
    
    addToFreeList(getBlockSize(split),split);
    
    

}
void RemoveNextPre(sf_block* block){
    sf_block *next=(sf_block *)((char*)block+getBlockSize(block));
    next->header=((next->header^MAGIC)-PREV_BLOCK_ALLOCATED)^MAGIC;
    setFooter(next,getBlockSize(next));
}
void AddNextPre(sf_block* block){
    sf_block *next=(sf_block *)((char*)block+getBlockSize(block));
    next->header=((next->header^MAGIC)|PREV_BLOCK_ALLOCATED)^MAGIC;
    setFooter(next,getBlockSize(next));
}
void setFooter(sf_block *block,int size){
    sf_block* nextblock=(sf_block*)((char*)block+size);
    nextblock->prev_footer=block->header;
}

void extandMem(sf_block *block,void* start){
    sf_block *newPage=epilogue;
   
    sf_size_t pagesize=PAGE_SZ;
    
    newPage->header=(pagesize|getBlockSize(epilogue)|(PREV_BLOCK_ALLOCATED&((epilogue->header)^MAGIC)))^MAGIC;
    setFooter(newPage,pagesize);
   
    epilogue=(sf_block *)(sf_mem_end()-16);
    epilogue->header=(0|THIS_BLOCK_ALLOCATED)^MAGIC;;
    setFooter(epilogue,8);
    
    addToFreeList(getBlockSize(newPage),newPage);
    coalesce(newPage);
}



sf_size_t getBlockSize(sf_block* block){
    return (block->header ^ MAGIC) & 0xfffffff0;
}
sf_size_t getPayloadSize(sf_block *block){
    sf_size_t pld=(((block->header ^ MAGIC) >> 32) & 0xffffffff);
    return pld;
}

void coalesce(sf_block*block){
    
    sf_size_t size=getBlockSize(block);
    sf_block* next_block=(sf_block*)((void*)block +size);
    
    int next_Alloc=(next_block->header^MAGIC)&THIS_BLOCK_ALLOCATED;
    int cur_Alloc=(block->header^MAGIC)&THIS_BLOCK_ALLOCATED;
    int pre_Alloc=(block->header^MAGIC)&PREV_BLOCK_ALLOCATED;
   //printf("????%d\n",pre_Alloc);
    if(cur_Alloc==0){
        if(next_Alloc==0){
            removeFromFree(block);
            removeFromFree(next_block);
            sf_size_t total=getBlockSize(next_block)+size;
            block->header=(total|((block->header^MAGIC)&PREV_BLOCK_ALLOCATED))^MAGIC;
            setFooter(block,getBlockSize(block));
            removeFromFree(block);
            addToFreeList(getBlockSize(block),block);
        }
        if(pre_Alloc==0){

            sf_block* prev_block=NULL;
            sf_size_t prev_size=(block->prev_footer^MAGIC)& 0xfffffff0;
            //printf("size%d\n",prev_size);
            prev_block=(sf_block*)((void*)block -prev_size);
            removeFromFree(block);
            removeFromFree(prev_block);
            sf_size_t total=getBlockSize(prev_block)+size;
            prev_block->header=(total|((prev_block->header^MAGIC)&PREV_BLOCK_ALLOCATED))^MAGIC;
            
            setFooter(prev_block,getBlockSize(prev_block));
            
            removeFromFree(prev_block);
            addToFreeList(getBlockSize(prev_block),prev_block);

        }
    }
    

}