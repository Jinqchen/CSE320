#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "store.h"
//int insertTo(char *var, char *val);
/*
 * This is the "data store" module for Mush.
 * It maintains a mapping from variable names to values.
 * The values of variables are stored as strings.
 * However, the module provides functions for setting and retrieving
 * the value of a variable as an integer.  Setting a variable to
 * an integer value causes the value of the variable to be set to
 * a string representation of that integer.  Retrieving the value of
 * a variable as an integer is possible if the current value of the
 * variable is the string representation of an integer.
 */

/**
 * @brief  Get the current value of a variable as a string.
 * @details  This function retrieves the current value of a variable
 * as a string.  If the variable has no value, then NULL is returned.
 * Any string returned remains "owned" by the data store module;
 * the caller should not attempt to free the string or to use it
 * after any subsequent call that would modify the value of the variable
 * whose value was retrieved.  If the caller needs to use the string for
 * an indefinite period, a copy should be made immediately.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @return  A string that is the current value of the variable, if any,
 * otherwise NULL.
 */
char *store_get_string(char *var) {
    struct storeMap *temp=firststore;
    while(temp!=NULL){
        if(!strcmp(temp->var,var)){
            return temp->val.Sval;
        }
        temp=temp->next;
    }
    return NULL;
}

/**
 * @brief  Get the current value of a variable as an integer.
 * @details  This retrieves the current value of a variable and
 * attempts to interpret it as an integer.  If this is possible,
 * then the integer value is stored at the pointer provided by
 * the caller.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @param  valp  Pointer at which the returned value is to be stored.
 * @return  If the specified variable has no value or the value
 * cannot be interpreted as an integer, then -1 is returned,
 * otherwise 0 is returned.
 */
int store_get_int(char *var, long *valp) {
    struct storeMap *temp=firststore;
    //printf("1\n");
    int result=0;
    char x;
    int i=1;
    while(temp!=NULL){
        if(!strcmp(temp->var,var)){
            if(temp->type==0){
                // printf("get?%s\n",temp->val.Sval);
                // return -1;
                x = *(temp->val.Sval);
                //printf("xxx%d\n",x-'0');
                while(x>'\0'){
                    if(x<'0'||x>'9'){
                        return -1;
                    }
                    //printf("%d\n",result);
                    result=result*10+(x-'0');
                    x=*(temp->val.Sval+i);
                    i++;
                }
                *valp=result;
                //printf("result%d\n",result);
                return 0;
            }
            else{
                //printf("%d,\n",temp->val.Ival);
                *valp=(temp->val.Ival);
                return 0;
            }
            
            
        }
        temp=temp->next;
    }
    return -1;
}

/**
 * @brief  Set the value of a variable as a string.
 * @details  This function sets the current value of a specified
 * variable to be a specified string.  If the variable already
 * has a value, then that value is replaced.  If the specified
 * value is NULL, then any existing value of the variable is removed
 * and the variable becomes un-set.  Ownership of the variable and
 * the value strings is not transferred to the data store module as
 * a result of this call; the data store module makes such copies of
 * these strings as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set, or NULL if the variable is to become
 * un-set.
 */
int store_set_string(char *var, char *val) {
    struct storeMap *temp=firststore;
    //printf("1\n");
    while(temp!=NULL){
        if(!strcmp(temp->var,var)){
            //printf("1.1%s\n",temp->var);
            int size;
            for(size=0;val[size]!='\0';++size);
            //printf("1.2%d\n",size);
            //printf("1.3%s\n",val);
            //memcpy(temp->val.Sval,val,size);
            temp->val.Sval=val;
            //printf("1.3%s\n",temp->val.Sval);
            temp->type=0;
            return 0;
        }
        temp=temp->next;
    }
    //printf("2\n");
    struct storeMap *node=malloc(sizeof(struct storeMap));
    //node->var=var;
     //printf("2.1\n");
    node->var=var;
    //strcpy(node->var,var);
    // printf("2.2\n");
    node->val.Sval=val;
    node->type=0;
    //strcpy(node->val,val);
    // printf("3\n");
    if(firststore==NULL){
        firststore=node;
        tailstore=node;
        firststore=tailstore;
        tailstore->next=NULL;
    }else{
        tailstore->next=node;
        tailstore=tailstore->next;
    }

    // struct storeMap *tempx=firststore;
    // while(tempx!=NULL){
        
    //     printf("%s,%s\n",tempx->var,tempx->val.Sval);
    //     tempx=tempx->next;
    // }
    return 0;
}

/**
 * @brief  Set the value of a variable as an integer.
 * @details  This function sets the current value of a specified
 * variable to be a specified integer.  If the variable already
 * has a value, then that value is replaced.  Ownership of the variable
 * string is not transferred to the data store module as a result of
 * this call; the data store module makes such copies of this string
 * as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set.
 */
int store_set_int(char *var, long val) {
    struct storeMap *temp=firststore;
    //printf("1\n");
    while(temp!=NULL){
        if(!strcmp(temp->var,var)){
            //printf("exist??\n");
            temp->val.Ival=val;
            temp->type=1;
            return 0;
        }
        //printf(" no exist??\n");
        temp=temp->next;
    }
    //printf("2\n");
    struct storeMap *node=malloc(sizeof(struct storeMap));
    //node->var=var;
     //printf("2.1\n");
    //
    node->var=var;
    //strcpy(node->var,var);
    // printf("2.2\n");
    node->val.Ival=val;
    //printf("2.3\n");
    node->type=1;
    //strcpy(node->val,val);
    // printf("3\n");
    if(firststore==NULL){
        firststore=node;
        tailstore=node;
        firststore=tailstore;
        tailstore->next=NULL;
    }else{
        tailstore->next=node;
        tailstore=tailstore->next;
    }
     //printf("??\n");
    // struct storeMap *tempx=firststore;
    // while(tempx!=NULL){
        
    //     printf("%s,%d\n",tempx->var,tempx->val.Ival);
    //     tempx=tempx->next;
    // }
    return 0;
}
/*int insertTo(char *var, char *val){
    // check var for existing? replace
    struct storeMap *temp=firststore;
    //printf("1\n");
    while(temp!=NULL){
        if(!strcmp(temp->var,var)){
            int size;
            for(size=0;val[size]!='\0';++size);
            memcpy(temp->val.Sval,val,size);
            return 0;
        }
        temp=temp->next;
    }
    //printf("2\n");
    struct storeMap *node=malloc(sizeof(struct storeMap));
    //node->var=var;
    // printf("2.1\n");
    node->var=var;
    //strcpy(node->var,var);
    // printf("2.2\n");
    node->val.Sval=val;
    //strcpy(node->val,val);
    // printf("3\n");
    if(firststore==NULL){
        firststore=node;
        tailstore=node;
        firststore=tailstore;
        tailstore->next=NULL;
    }else{
        tailstore->next=node;
        tailstore=tailstore->next;
    }

    struct storeMap *tempx=firststore;
    while(tempx!=NULL){
        
        printf("%s,%s\n",tempx->var,tempx->val.Sval);
        tempx=tempx->next;
    }
    return 0;
}*/

/**
 * @brief  Print the current contents of the data store.
 * @details  This function prints the current contents of the data store
 * to the specified output stream.  The format is not specified; this
 * function is intended to be used for debugging purposes.
 *
 * @param f  The stream to which the store contents are to be printed.
 */
void store_show(FILE *f) {
    struct storeMap *tempx=firststore;
     //printf("??\n");
    while(tempx!=NULL){
        //printf("??1\n");
        if(tempx->type==0){
            //printf("??\n");
            fprintf(f,"%s = %s, ",tempx->var,tempx->val.Sval);
            //fprintf(f,"x");
        }else{
            //fprintf(f,"x");
            fprintf(f,"%s = %d, ",tempx->var,tempx->val.Ival);
        }
        //printf("%s,%s\n",tempx->var,tempx->val.Sval);
        tempx=tempx->next;
    }
}
