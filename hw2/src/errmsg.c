/*********************/
/* errmsg.c          */
/* for Par 3.20      */
/* Copyright 1993 by */
/* Adam M. Costello  */
/*********************/

/* This is ANSI C code. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errmsg.h"  /* Makes sure we're consistent with the declarations. */
//char *errmsg=NULL;
char *errmsg;
void set_error(char *msg){
    errmsg=(char *)malloc(strlen(msg)+1);
    strcpy(errmsg,msg);
    
}
int is_error(){
    if(errmsg){
        return 1;
    }else 
    return 0;
}
int report_error(FILE*file){
    if(!errmsg){
        return 0;
    }else{
        fprintf(file,"%s",errmsg);
        return 1;
    }
}
void clear_error(){
    if(errmsg){
        free(errmsg);
        errmsg=NULL;
    }
    

}