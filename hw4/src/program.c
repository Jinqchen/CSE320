#include <stdlib.h>
#include <stdio.h>

#include "mush.h"
#include "debug.h"

#include "program.h"

//static int index=0;
static double counter=-1;
//struct stmtList *counter=NULL;;
void ifList(struct stmtList *temp);
void foregroundList(struct stmtList *temp);
void backgroundList(struct stmtList *temp);
void jobctlList(struct stmtList *temp);
void setList(struct stmtList *temp );
/*
 * This is the "program store" module for Mush.
 * It maintains a set of numbered statements, along with a "program counter"
 * that indicates the current point of execution, which is either before all
 * statements, after all statements, or in between two statements.
 * There should be no fixed limit on the number of statements that the program
 * store can hold.
 */

/**
 * @brief  Output a listing of the current contents of the program store.
 * @details  This function outputs a listing of the current contents of the
 * program store.  Statements are listed in increasing order of their line
 * number.  The current position of the program counter is indicated by
 * a line containing only the string "-->" at the current program counter
 * position.
 *
 * @param out  The stream to which to output the listing.
 * @return  0 if successful, -1 if any error occurred.
 */
int prog_list(FILE *out) {
    //printf("line 33\n");
    struct stmtList *temp=first;
    STMT *stmt;
     //printf("line 35\n");
     //printf("counter:%d",counter);
    /*if(counter==-1){
        fprintf(out,"-->\n");
    }*/
    stmt=prog_fetch();
    //printf("%d\n",counter);
    if(stmt==NULL){
        //printf("???x\n");
        fprintf(out,"-->\n");
        return 0;
    }
    prog_next();
    while(temp!=NULL){
        
        //printf("class%d",temp->statement->class);
        // if(temp->statement->lineno<counter && temp->next->statement->lineno>counter){
        //     fprintf(out,"-->\n");
        // }
        //printf("%d\n",counter);
        //printf("line 37\n");
        //fprintf(out,"%d",temp->statement->lineno);
        switch(temp->statement->class){
            // case 0:
            //     fprintf(out,"%d",temp->statement->lineno);
            //     break;
            // case 1:
            //     fprintf(out,"%d",temp->statement->lineno);
            //     break;
            case 2:
                fprintf(out,"%d delete %d,%d\n",temp->statement->lineno,temp->statement->members.delete_stmt.from,temp->statement->members.delete_stmt.to);
                break;
            // case 3:
            //     fprintf(out,"%d",temp->statement->lineno);
            //     break;
            // case 4:
            //     fprintf(out,"%d",temp->statement->lineno);
            //     break;
            case 5:
                fprintf(out,"%d ",temp->statement->lineno);
                fprintf(out,"stop\n");
                break;
            case 6:
                fprintf(out,"%d wait ",temp->statement->lineno);
                jobctlList(temp);
                break;
            case 7:
                fprintf(out,"%d poll ",temp->statement->lineno);
                jobctlList(temp);
                break;
            case 8:
                fprintf(out,"%d cancel ",temp->statement->lineno);
                jobctlList(temp);
                break;
            case 9:
                fprintf(out,"%d pause\n",temp->statement->lineno);
                break;
            case 10:
                fprintf(out,"%d set %s = ",temp->statement->lineno,temp->statement->members.set_stmt.name);
                setList(temp );
                break;
            case 11:
                fprintf(out,"%d unset %s\n",temp->statement->lineno,temp->statement->members.unset_stmt.name);
                break;
            case 12:
                //unfinished
                fprintf(out,"%d if ",temp->statement->lineno);
                ifList(temp);
                fprintf(out," goto %d\n",temp->statement->members.if_stmt.lineno);
                break;
            case 13:
                fprintf(out,"%d goto ",temp->statement->lineno); 

                fprintf(out,"%d\n",temp->statement->members.goto_stmt.lineno);
                break;
            case 14:
                fprintf(out,"%d source \"%s\"\n",temp->statement->lineno,temp->statement->members.source_stmt.file);
                break;
            case 15:
                
                fprintf(out,"%d ",temp->statement->lineno);     
                foregroundList(temp);
                break;
            case 16:
                fprintf(out,"%d ",temp->statement->lineno);     
                backgroundList(temp);
                //fprintf(out,"%d %s %s\n",temp->statement->lineno,temp->statement->members.sys_stmt.pipeline->commands->args->expr->members.variable,temp->statement->members.sys_stmt.pipeline->commands->args->next->expr->members.value);
                break;
            default:
                break;
        }
        stmt=prog_fetch();
        //printf("counter%f\n",counter);
        if(stmt==NULL){
            //printf("???x\n");
            break;
        }
        prog_next();
        //printf("%d\n",counter);
        //printf("%d\n",temp->statement->lineno);
        temp=temp->next;
        //printf("%d\n",temp->statement->lineno);
    }
    //printf("???\n");
    if(counter>temp->statement->lineno){
        //printf("1???\n");
        fprintf(out,"-->\n");
    }
    
    //fprintf(out,"list\n",temp->statement->lineno,temp->statement->lineno);
    return 0;
}
void ifList(struct stmtList *temp){
    //printf("error? 131\n");
    struct expr *expr=temp->statement->members.if_stmt.expr;
    show_expr(stdout,expr,0);
    //fprintf(stdout,"\n");
    /*if(expr->members.binary_expr.arg1->type==1){
            fprintf(stdout,"#");
    }
    //printf("error? 137\n");
    fprintf(stdout,"%s ",expr->members.binary_expr.arg1->members.variable);
    switch(expr->members.binary_expr.oprtr){
        case 2:
            fprintf(stdout,"&& ");
            break;
        case 3:
            fprintf(stdout,"|| ");
            break;
        case 4:
            fprintf(stdout,"== ");
            break;
        case 5:
            fprintf(stdout,"< ");
            break;
        case 6:
            fprintf(stdout,"> ");
            break;
        case 7:
            fprintf(stdout,"<= ");
            break;
        case 8:
            fprintf(stdout,">= ");
            break;
        // case 9:
        //     break;
        // case 10:
        //     break;
        // case 11:
        //     break;
        // case 12:
        //     break;
        default: break;*/
                      
    //}
    
    // if(expr->members.binary_expr.arg2->type==1){
    //         fprintf(stdout,"#");
    // }
    // fprintf(stdout,"%s ",expr->members.binary_expr.arg2->members.variable);
    //printf("error? 139\n");

}
void setList(struct stmtList *temp ){
    struct expr *expr=temp->statement->members.set_stmt.expr;
    if(expr->type==1){
            fprintf(stdout,"#");
    }
    fprintf(stdout,"%s \n",expr->members.variable);
}
void jobctlList(struct stmtList *temp){
    struct expr *expr=temp->statement->members.jobctl_stmt.expr;
    if(expr->type==1){
            fprintf(stdout,"#");
    }
    fprintf(stdout,"%s \n",expr->members.variable);
    
//    show_expr(stdout,expr,1);
//     fprintf(stdout,"done\n:");

    
}
void backgroundList(struct stmtList *temp){
    //struct arg *args=temp->statement->members.sys_stmt.pipeline->commands->args;
    // while(args!=NULL){
    //     //printf("value type%d-",args->expr->type);
    //     if(args->expr->type==1){
    //         fprintf(stdout,"#");
    //     }
    //     fprintf(stdout,"%s ",args->expr->members.variable);
    //     args=args->next;
    // }
    // fprintf(stdout,"&\n");

    show_pipeline(stdout,temp->statement->members.sys_stmt.pipeline);
    fprintf(stdout," &\n");
}

void foregroundList(struct stmtList *temp){
    //fprintf(out,"%d ",stmt->lineno);
    // struct arg *args=temp->statement->members.sys_stmt.pipeline->commands->args;
    // while(args!=NULL){
    //     //printf("value type%d-",args->expr->type);
    //     if(args->expr->type==1){
    //         fprintf(stdout,"#");
    //     }
    //     fprintf(stdout,"%s ",args->expr->members.variable);
    //     args=args->next;
    // }
    // fprintf(stdout,"\n");
    show_pipeline(stdout,temp->statement->members.sys_stmt.pipeline);
    fprintf(stdout,"\n");
    
    // if(counter>=temp->statement->lineno){
    //     fprintf(stdout,"-->\n");
    // }
    
}

/**
 * @brief  Insert a new statement into the program store.
 * @details  This function inserts a new statement into the program store.
 * The statement must have a line number.  If the line number is the same as
 * that of an existing statement, that statement is replaced.
 * The program store assumes the responsibility for ultimately freeing any
 * statement that is inserted using this function.
 * Insertion of new statements preserves the value of the program counter:
 * if the position of the program counter was just before a particular statement
 * before insertion of a new statement, it will still be before that statement
 * after insertion, and if the position of the program counter was after all
 * statements before insertion of a new statement, then it will still be after
 * all statements after insertion.
 *
 * @param stmt  The statement to be inserted.
 * @return  0 if successful, -1 if any error occurred.
 */
int prog_insert(STMT *stmt) {
    // TO BE IMPLEMENTED
    //printf("insert\n");
    if(stmt->lineno<0){
        return -1;
    }
    //TODO: check for line number existing?
    struct stmtList *temp=first;
     //printf("insert1\n");
    while(/*temp!=NULL&&temp->statement!=NULL*/temp!=NULL){
        //printf("insert1.1\n");
        if(temp->statement->lineno==stmt->lineno){
            //printf("insert1.2\n");
            temp->statement=stmt;
            //printf("insert1.3\n");
            return 0;
        }
         //printf("insert1.4\n");
        //  if(temp->next==NULL){
        //      break;
        //  }
        temp=temp->next;
        //printf("insert1.5\n");
        
        //printf("end%d\n",temp->statement->lineno);
        
    }
     //printf("insert2\n");


    struct stmtList *node=malloc(sizeof(struct stmtList));
    node->statement=stmt;
    // node->index=index;
    // index++;
    // node->lineNumber=stmt->lineno;
    // node->next=first;
    // first=node;
    //printf("x\n");
    //counter=node;
    //printf("y\n");
    if(first==NULL){
        first=node;
        tail=node;
        first=tail;
        tail->next=NULL;
    }else{
        tail->next=node;
        tail=tail->next;
        tail->next=NULL;
    }
     //printf("insert3\n");
    

    // struct stmtList *temp=first;
    // while(temp!=NULL){
    //     //printf("%d\n",temp->statement->lineno);
    //     printf("%d\n",temp->index);
    //     temp=temp->next;
    // }
    return 0;
}

/**
 * @brief  Delete statements from the program store.
 * @details  This function deletes from the program store statements whose
 * line numbers fall in a specified range.  Any deleted statements are freed.
 * Deletion of statements preserves the value of the program counter:
 * if before deletion the program counter pointed to a position just before
 * a statement that was not among those to be deleted, then after deletion the
 * program counter will still point the position just before that same statement.
 * If before deletion the program counter pointed to a position just before
 * a statement that was among those to be deleted, then after deletion the
 * program counter will point to the first statement beyond those deleted,
 * if such a statement exists, otherwise the program counter will point to
 * the end of the program.
 *
 * @param min  Lower end of the range of line numbers to be deleted.
 * @param max  Upper end of the range of line numbers to be deleted.
 */
int prog_delete(int min, int max) {
    struct stmtList *temp=first,*prev;
    // if(temp->statement->lineno>=min && temp->statement->lineno<=max){
    //     first=temp->next;
    //     free(temp);
    //     temp=first;
    // }
    while(temp!=NULL){
        // printf("endless?\n");
        if(temp==first){
            if(temp->statement->lineno>=min && temp->statement->lineno<=max){
                //printf("endless?\n");
                first=temp->next;
                free(temp);
                temp=first;
            }else{
                //printf("error?\n");
                prev=temp;
                //printf("error?\n");
                temp=temp->next;
            }
        }
        else if(temp->statement->lineno>=min && temp->statement->lineno<=max){
            //  printf("middleendless?\n");
            prev->next=temp->next;
            free(temp);
            temp=prev;
            temp=temp->next;
        }else{
            //printf("go next?\n");
            prev=temp;
            temp=temp->next;
        }
        //printf("%d\n",temp->statement->lineno);
        
    }
    
    // struct stmtList *tempx=first;
    // while(tempx!=NULL){
    //     printf("%d\n",tempx->statement->lineno);
    //     //printf("%d\n",tempx->index);
    //     tempx=tempx->next;
    // }
    return 1;
}

/**
 * @brief  Reset the program counter to the beginning of the program.
 * @details  This function resets the program counter to point just
 * before the first statement in the program.
 */
void prog_reset(void) {
    //counter=first;
    counter=-1;
    //printf("reset%d\n",counter->statement->lineno);
}

/**
 * @brief  Fetch the next program statement.
 * @details  This function fetches and returns the first program
 * statement after the current program counter position.  The program
 * counter position is not modified.  The returned pointer should not
 * be used after any subsequent call to prog_delete that deletes the
 * statement from the program store.
 *
 * @return  The first program statement after the current program
 * counter position, if any, otherwise NULL.
 */
STMT *prog_fetch(void) {

    struct stmtList *temp=first;
    //printf("counter%d\n",counter);
    while(temp!=NULL){
        //printf("%d\n",temp->statement->lineno);
       if(temp->statement->lineno>counter){
           return temp->statement;
       }
        temp=temp->next;
    }
    // counter=counter->next;
    //printf("%d\n",temp->statement->lineno);
    return NULL;
    // if(temp==NULL){
    //     return NULL;
    // }
    // return temp->statement;
}

/**
 * @brief  Advance the program counter to the next existing statement.
 * @details  This function advances the program counter by one statement
 * from its original position and returns the statement just after the
 * new position.  The returned pointer should not be used after any
 * subsequent call to prog_delete that deletes the statement from the
 * program store.
 *
 * @return The first program statement after the new program counter
 * position, if any, otherwise NULL.
 */
STMT *prog_next() {
    struct stmtList *temp=first;
    while(temp->statement->lineno<counter&&temp!=NULL){
        //printf("%d\n",temp->statement->lineno);
       
        temp=temp->next;
    }
    if(temp==NULL){
        return NULL;
    }
    counter=temp->statement->lineno+0.5;
    // counter=counter->next;
    //printf("%d\n",counter);
    
    return temp->statement;
    
}

/**
 * @brief  Perform a "go to" operation on the program store.
 * @details  This function performs a "go to" operation on the program
 * store, by resetting the program counter to point to the position just
 * before the statement with the specified line number.
 * The statement pointed at by the new program counter is returned.
 * If there is no statement with the specified line number, then no
 * change is made to the program counter and NULL is returned.
 * Any returned statement should only be regarded as valid as long
 * as no calls to prog_delete are made that delete that statement from
 * the program store.
 *
 * @return  The statement having the specified line number, if such a
 * statement exists, otherwise NULL.
 */
STMT *prog_goto(int lineno) {
    struct stmtList *temp=first;
    while(temp!=NULL){
        if(temp->statement->lineno==lineno){
            counter=lineno-1;
            //printf("goto%d\n",counter);
            return temp->statement;
        }
        temp=temp->next;
    }
    return NULL;
}
