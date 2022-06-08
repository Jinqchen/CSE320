#include <stdlib.h>
#include <stdio.h>

#include "argo.h"
#include "global.h"
#include "debug.h"
int argo_write_object(ARGO_VALUE *v, FILE *f);
int argo_write_array(ARGO_VALUE *v, FILE *f);
int argo_read_basic(ARGO_BASIC *s,FILE *f);
int argo_read_array(ARGO_VALUE **s, FILE *f);
int argo_read_object(ARGO_VALUE **s, FILE *f);
int recursive = 1;


/**
 * @brief  Read JSON input from a specified input stream, parse it,
 * and return a data structure representing the corresponding value.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON value,
 * according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.  See the assignment handout for
 * information on the JSON syntax standard and how parsing can be
 * accomplished.  As discussed in the assignment handout, the returned
 * pointer must be to one of the elements of the argo_value_storage
 * array that is defined in the const.h header file.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  A valid pointer if the operation is completely successful,
 * NULL if there is any error.
 */
ARGO_VALUE *argo_read_value(FILE *f) {
    
    
    int c=0;
    
    while(1){
        c=fgetc(f);
        if(feof(f)){// end if file is empty
            break;
        }
        if(argo_is_whitespace(c)){
            c=fgetc(f);
        }else if(c==ARGO_QUOTE){//string
            argo_value_storage->type=3;
           
            int valid=argo_read_string(&(argo_value_storage->content.string),f);
            if(valid==0){
                return argo_value_storage;
            }else return NULL;
            
            
        }else if(c==ARGO_LBRACK){//array
           

            argo_value_storage->type=5;
            
            if(argo_read_array(&(argo_value_storage->content.array.element_list),f)==0){
            return argo_value_storage;
            }else return NULL;


        }else if(c==ARGO_LBRACE){//object
            argo_value_storage->type=4;
            if(argo_read_object(&(argo_value_storage->content.object.member_list),f)==0){
                return argo_value_storage;
            }else return NULL;
            

        }else if(c==ARGO_T||c==ARGO_F||c==ARGO_N){
            argo_value_storage->type=1;
            
            ungetc(c,f);
            if(argo_read_basic(&(argo_value_storage->content.basic),f)==0){
                
                return argo_value_storage;
            }else{
                fprintf(stderr,"ERROR! Errors occur when reading basic type data");
                return NULL;
            }
            // return argo_value_storage;

        }else if(c==ARGO_MINUS||c==ARGO_COLON||c==ARGO_COMMA||c==ARGO_RBRACE||c==ARGO_RBRACK){
                fprintf(stderr,"ERROR! Errors occur because invalid character in the begining");
                return NULL;
        }
        else{
            argo_value_storage->type=2;
            ungetc(c,f);
            if(argo_read_number(&(argo_value_storage->content.number),f)==0){
                
                return argo_value_storage;
            }else return NULL;
            
        }

        
        
    }
    
    
    return NULL;
    
}
//return 0 if success, 1 if error 
int argo_read_basic(ARGO_BASIC *s,FILE *f){
    int c=fgetc(f);
    if(c==ARGO_N){
        int c1=fgetc(f);
        int c2=fgetc(f);
        int c3=fgetc(f);
        int c4=fgetc(f);
        if(c1==ARGO_U&&c2=='l'&&c3=='l'){
            if(c4==argo_is_whitespace(c)||c4=='\0'||c4==ARGO_COMMA||c4==ARGO_RBRACE||c4==ARGO_RBRACK){
                *s=ARGO_TRUE;
                ungetc(c4,f);
                return 0;
            } else
            return 1;


        }else{
            
            return 1;
        }
       
    }else if(c==ARGO_T){
        int c1=fgetc(f);
        int c2=fgetc(f);
        int c3=fgetc(f);
        int c4=fgetc(f);
       
        if(c1=='r'&&c2=='u'&&c3=='e'){
           
            if(c4==argo_is_whitespace(c)||c4=='\0'||c4==ARGO_COMMA||c4==ARGO_RBRACE||c4==ARGO_RBRACK){
                *s=ARGO_TRUE;
                ungetc(c4,f);
                return 0;
            } else
            return 1;


          
        }else{
            
            return 1;
        }
       
    }else if(c==ARGO_F){
        int c1=fgetc(f);
        int c2=fgetc(f);
        int c3=fgetc(f);
        int c4=fgetc(f);
        int c5=fgetc(f);
        if(c1=='a'&&c2=='l'&&c3=='s'&&c4=='e'){
           
            if(c5==argo_is_whitespace(c)||c5=='\0'||c5==ARGO_COMMA||c5==ARGO_RBRACE||c5==ARGO_RBRACK){
                *s=ARGO_TRUE;
                ungetc(c5,f);
                return 0;
            } else
            return 1;
        }else{
            
            return 1;
        }
        
       
    }
    return 1;
}

int argo_read_array(ARGO_VALUE **s, FILE *f) {
    
    int c;
    argo_next_value=1;
    
    while(1){
        c=fgetc(f);
        
        if(argo_is_whitespace(c)||c==ARGO_COMMA){     
        }
        else if(feof(f)){
            fprintf(stderr,"Error! errors occur because there is no ']' in the end of array");
            return 1;
        }
        
        else if(c==ARGO_RBRACK){//end
            
            break;
        }
        else if(c==ARGO_QUOTE){
            argo_next_value+=1;
            (argo_value_storage+argo_next_value)->type=3;
            int valid=argo_read_string(&((argo_value_storage+argo_next_value)->content.string),f);
            if(valid !=0){
                return 1;
            }
            
        }
        
        else if(c==ARGO_T||c==ARGO_F||c==ARGO_N){
            
            ungetc(c,f);
            argo_next_value+=1;
            (argo_value_storage+argo_next_value)->type=1;
            
            int valid=argo_read_basic(&((argo_value_storage+argo_next_value)->content.basic),f);
            
            if(valid!=0){
                fprintf(stderr,"ERROR! Errors occur when reading basic type data");
                return 1;
            }
            
        }else if(c==ARGO_LBRACK){
            
            fprintf(stderr,"Error! errors occur inside the nested part");
            return 1;
            
            
        }else if(c==ARGO_LBRACE){//object
            fprintf(stderr,"Error! errors occur inside the nested part");
            return 1;
        }else{
            
            argo_next_value+=1;
            (argo_value_storage+argo_next_value)->type=2;
            
            ungetc(c,f);
            argo_read_number(&((argo_value_storage+argo_next_value)->content.number),f);
            
            
        }

    }
    
    ARGO_VALUE *newnode=NULL, *temp=NULL;
    
    
    for(int i=1;i<=argo_next_value;i++){ 
        newnode=(argo_value_storage+i); 
        if(i==1){
            (*s)= newnode; 
            newnode->prev=(*s);
            newnode->next=(*s);   
        }
        else{
            newnode=(argo_value_storage+i);
            temp=(*s)->prev;
            temp->next=newnode;
            newnode->next=(*s);
            newnode->prev=temp;
            temp=(*s);
            temp->prev=newnode; 
        }
    }

    return 0;

    
}
int argo_read_object(ARGO_VALUE **s,FILE *f){
    int c;//
    argo_next_value=1;
    
    while(1){
        c=fgetc(f);
        
        if(argo_is_whitespace(c)||c==ARGO_COMMA){   

        }
        else if(feof(f)){
            fprintf(stderr,"ERROR! Errors occur because there is no '}' in the end of object");
            return 1;  
        }
        else if(c==ARGO_RBRACE){//end
            
            break;
        }
        else if(c==ARGO_QUOTE){//get head
            argo_next_value+=1;
            
            int valid=argo_read_string(&((argo_value_storage+argo_next_value)->name),f);//get name
            if(valid!=0){
                return 1;
            }
            
            c=fgetc(f);
            if(c==ARGO_COLON){//VALUE
                
                //loop
                while(1){
                    c=fgetc(f);
                    if(argo_is_whitespace(c)){   

                    }else if(c==ARGO_COMMA){
                        break;
                    }
                    else if(c==ARGO_QUOTE){
                        (argo_value_storage+argo_next_value)->type=3;
                        int valid=argo_read_string(&((argo_value_storage+argo_next_value)->content.string),f);
                        if(valid!=0){
                            return 1;
                        }
                        break;
                    }
                    else if(c==ARGO_T||c==ARGO_F||c==ARGO_N){
                
                        ungetc(c,f);
                        
                        (argo_value_storage+argo_next_value)->type=1;
                        int valid=argo_read_basic(&((argo_value_storage+argo_next_value)->content.basic),f);
                        if(valid!=0){
                            
                            fprintf(stderr,"ERROR! Errors occur when reading basic type data");
                            return 1;
            
                        }


                        
                        break;
                        
                    }else if(c==ARGO_LBRACK){
                    
                    
                        fprintf(stderr,"Error! errors occur inside the nested part");
                        return 1;
                        
                    }else if(c==ARGO_LBRACE){//object
                     
                        fprintf(stderr,"Error! errors occur inside the nested part");
                        return 1;
                    }
                    else{
                        
                        
                        (argo_value_storage+argo_next_value)->type=2;
                        
                        ungetc(c,f);
                        argo_read_number(&((argo_value_storage+argo_next_value)->content.number),f);
                        break;
                        
                    }
                }


            }
            

            
        }
        
       

    }
    ARGO_VALUE *newnode=NULL, *temp=NULL;
    for(int i=1;i<=argo_next_value;i++){ 
        newnode=(argo_value_storage+i); 
        if(i==1){
            (*s)= newnode; 
            newnode->prev=(*s);
            newnode->next=(*s);   
        }else{
            newnode=(argo_value_storage+i);
            temp=(*s)->prev;
            temp->next=newnode;
            newnode->next=(*s);
            newnode->prev=temp;
            temp=(*s);
            temp->prev=newnode; 
        }
    }


    return 0;
}


/**
 * @brief  Read JSON input from a specified input stream, attempt to
 * parse it as a JSON string literal, and return a data structure
 * representing the corresponding string.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON string
 * literal, according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
int argo_read_string(ARGO_STRING *s, FILE *f) {
    int c= fgetc(f);
    
    
    while(1){
        
        if(feof(f)){
            fprintf(stderr,"Error! errors occur because there is no '\"' in the end of string");
            return 1;
            // break;
            
        }
        if(c==ARGO_QUOTE){
            c=fgetc(f);
            if(c==ARGO_COMMA||c=='\0'||argo_is_whitespace(c)||feof(f)||c==ARGO_COMMA||c==ARGO_RBRACK||c==ARGO_RBRACE||c==ARGO_COLON){
                ungetc(c,f);
                return 0;
            }
            
            
        }
        else if(c==ARGO_BSLASH){
            c=fgetc(f);
          
            
            if(c==ARGO_B){
                argo_append_char(s,ARGO_BS);
                c=fgetc(f);
            }
            else if(c==ARGO_QUOTE){
                c=fgetc(f);
                if(c!=ARGO_COMMA){
                    ungetc(c,f);
                    argo_append_char(s,'\"');
                    c=fgetc(f);
                }else{
                    if(c==ARGO_COMMA||c=='\0'||argo_is_whitespace(c)||feof(f)||c==ARGO_COMMA||c==ARGO_RBRACK||c==ARGO_RBRACE||c==ARGO_COLON){
                        ungetc(c,f);
                        return 0;
                    }
                }
            }
            
            else if(c==ARGO_BSLASH){
                
                argo_append_char(s,ARGO_BSLASH);
                c=fgetc(f);
            }
            else if(c==ARGO_F){
                argo_append_char(s,ARGO_FF);
                c=fgetc(f);
            }else if(c==ARGO_N){
                argo_append_char(s,ARGO_LF);
                c=fgetc(f);
            }else if(c==ARGO_R){
                argo_append_char(s,ARGO_CR);
                c=fgetc(f);
            }else if(c==ARGO_T){
                argo_append_char(s,ARGO_HT);
                c=fgetc(f);
            }else if(c==ARGO_U){
                int c1,c2,c3,c4;
                c1=fgetc(f);c2=fgetc(f);c3=fgetc(f);c4=fgetc(f);
                if(argo_is_hex(c1)&&argo_is_hex(c2)&&argo_is_hex(c3)&&argo_is_hex(c4)){
                    if(c1==ARGO_DIGIT0&&c2==ARGO_DIGIT0&&c3==ARGO_DIGIT0&&c4=='8'){
                        argo_append_char(s,ARGO_BS);
                        c=fgetc(f);
                    }else if(c1==ARGO_DIGIT0&&c2==ARGO_DIGIT0&&c3==ARGO_DIGIT0&&(c4=='c'||c4=='C')){
                        argo_append_char(s,ARGO_FF);
                        c=fgetc(f);
                    }else if(c1==ARGO_DIGIT0&&c2==ARGO_DIGIT0&&c3==ARGO_DIGIT0&&(c4=='a'||c3=='A')){
                        argo_append_char(s,ARGO_LF);
                        c=fgetc(f);
                    }else if(c1==ARGO_DIGIT0&&c2==ARGO_DIGIT0&&c3==ARGO_DIGIT0&&(c4=='d'||c4=='D')){
                        argo_append_char(s,ARGO_CR);
                        c=fgetc(f);
                    }else if(c1==ARGO_DIGIT0&&c2==ARGO_DIGIT0&&c3==ARGO_DIGIT0&&c4=='9'){
                        argo_append_char(s,ARGO_HT);
                        c=fgetc(f);
                    }
                    else{
                       
                        if(c1>=48 &&c1<=57){
                            c1-=48;
                        }else{
                            c1-=87;
                        }
                         if(c2>=48 &&c2<=57){
                            c2-=48;
                        }else{
                            c2-=87;
                        }
                         if(c3>=48 &&c3<=57){
                            c3-=48;
                        }else{
                            c3-=87;
                        }
                         if(c4>=48 &&c4<=57){
                            c4-=48;
                        }else{
                            c4-=87;
                        }
                        c1=(c1<<16)|(c2<<8)|(c3<<4)|c4;
                        argo_append_char(s,c1);
                        c=fgetc(f);
                    }
                    
                }else {
                    fprintf(stderr,"Error! errors occur because of invalid escape sequence");
                    return 1;//error
                }
                
            }else{
                argo_append_char(s,c);
            }
            
            
        }
        else{
            
            argo_append_char(s,c);
            c=fgetc(f);
        }
    }
    

    return 0;
}

/**
 * @brief  Read JSON input from a specified input stream, attempt to
 * parse it as a JSON number, and return a data structure representing
 * the corresponding number.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON numeric
 * literal, according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.  The returned value must contain
 * (1) a string consisting of the actual sequence of characters read from
 * the input stream; (2) a floating point representation of the corresponding
 * value; and (3) an integer representation of the corresponding value,
 * in case the input literal did not contain any fraction or exponent parts.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
int argo_read_number(ARGO_NUMBER *n, FILE *f) {
    
    int c;
    int num=0;
  
    long digit=0;
    double fl=0;
    int count=0;//len after period
    int pcount=0;//period
    int ecount=0;//e
    int mcount=0;//minus
    int neg=1;
    int cur=0;//curent char
    int exp=0;
    int expm=1;
    c=fgetc(f);
   
    if(c==ARGO_MINUS){
        neg=-1;
    }
    ungetc(c,f);
    while(1){
        c=fgetc(f);
        
        if(feof(f)){
            break;
            
        }
        if(c=='\0'||argo_is_whitespace(c)||c==ARGO_COMMA||c==ARGO_RBRACK||c==ARGO_RBRACE){
            ungetc(c,f);          
            break;
        }
        argo_append_char(&(n->string_value),c);
       if(argo_is_exponent(c)){
            ecount++;
            if(ecount>=2){
                fprintf(stderr,"Error! errors occur because invalid number");
                return 1;
            }
            cur=0;
        }else if(c==ARGO_PERIOD){           
            pcount++;
            if(count>=2||ecount!=0){
                fprintf(stderr,"Error! errors occur because invalid number");
                return 1;
            }
            cur++;
            
        }else if(argo_is_digit(c)){
            cur++;
            // integer
            if(pcount==0&&ecount==0){
                digit=(digit*10)+(c-ARGO_DIGIT0);
                fl=(fl*10)+(c-ARGO_DIGIT0);
                
            }else if(pcount==1&&ecount==0){//float
                count++;
                int move=1;
                for(int i=1;i<count;i++){
                    move*=10;
                }
                fl=fl+(c-ARGO_DIGIT0)/(10.0*move);
                
            }else if (ecount==1){
                
                exp=exp+(c-ARGO_DIGIT0);
                
            }

        }else if(c==ARGO_MINUS){
            mcount++;
            if(cur==0){

            }else{
                fprintf(stderr,"Error! errors occur because invalid number");
                return 1;
            }
            if(mcount>=3){
                fprintf(stderr,"Error! errors occur because invalid number");
                return 1;
            }
            if(ecount==1){
                expm=-1;
            }
        } 
      
        else{
             fprintf(stderr,"Error! errors occur because invalid number");
            

            return 1;
        }
       
    }
    digit*=neg;
    fl*=neg;
    
    int move=10;
    for(int i=1;i<exp;i++){
        move*=10;
    }
    exp=exp*expm;//get exp value
   
    if(exp>0){
        fl*=move;
    }else if(exp<0){
        fl/=move;
    }
    n->float_value=fl;
    n->int_value=digit;
   
    return 0;
}

/**
 * @brief  Write canonical JSON representing a specified value to
 * a specified output stream.
 * @details  Write canonical JSON representing a specified value
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.
 *
 * @param v  Data structure representing a value.
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
int argo_write_value(ARGO_VALUE *v, FILE *f) {
    //reminder: check global options.
    
    int vtype=v-> type;
    
    if(vtype==0){
        fprintf(stderr,"NULL type value");
        return 1;
    }else if(vtype==1){
        ARGO_BASIC n = v->content.basic;
  
        if(n==0){
            fprintf(stdout,"%s",ARGO_NULL_TOKEN);
        }else if(n==1){
            fprintf(stdout,"%s",ARGO_TRUE_TOKEN);
        }else if(n==2){
            fprintf(stdout,"%s",ARGO_FALSE_TOKEN);
        }
        return 0;  
            

    }else if(vtype==2){
        struct argo_number n = v->content.number;
        argo_write_number(&n,stdout);
        return 0;  
        
    }else if(vtype==3){
        struct argo_string n = (v->content).string;
        
        argo_write_string(&n,stdout);
        return 0;  
        
    }else if(vtype==4){//object
        argo_write_object(v,stdout); 
        return 0;      
    }else if(vtype==5){//array
        argo_write_array(v,stdout);   
        return 0;  
        
        
    }
    return 1;
}
int argo_write_object(ARGO_VALUE *v, FILE *f) {
    fprintf(stdout,"%c",ARGO_LBRACE);
    if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
        fprintf(stdout,"%c",ARGO_LF);
        indent_level=global_options-(CANONICALIZE_OPTION+PRETTY_PRINT_OPTION);
        
        for(int i=0;i<indent_level*recursive;i++){
            
            fprintf(stdout,"%c",ARGO_SPACE);
        }
    }
        

    struct argo_object t4=((v->content).object);
    //get the leghth of object;
    struct argo_value *lastv=t4.member_list -> prev;
    struct argo_value *nextv=t4.member_list -> next;
    int len=0;
    while(&lastv->name.content != &nextv->name.content){
        len++;
        nextv =nextv-> next;
    }
    
    struct argo_value *p =t4.member_list -> next;
    for(int i=0;i<=len;i++){
        
        if(len==0){
            break;
        }
        ARGO_STRING s =p->name;
        argo_write_string(&s,stdout);
        fprintf(stdout,"%c%c",ARGO_COLON,ARGO_SPACE);
        
        int valuetype = p->type;
        if(valuetype==0){
            
        }
        else if(valuetype==1){
            ARGO_BASIC n = p->content.basic;
            if(n==0){
                fprintf(stdout,"%s",ARGO_NULL_TOKEN);
            }else if(n==1){
                fprintf(stdout,"%s",ARGO_TRUE_TOKEN);
            }else if(n==2){
                fprintf(stdout,"%s",ARGO_FALSE_TOKEN);
            }
            
            
        }
        else if(valuetype==2){
            ARGO_NUMBER n = p->content.number;
            argo_write_number(&n, stdout);
        }
        else if(valuetype==3){
            ARGO_STRING n = p->content.string;
            argo_write_string(&n, stdout);
        }
        else if(valuetype==4){
            recursive++;
            argo_write_object(p,stdout);
            
        }
        else if(valuetype==5){
            recursive++;
            argo_write_array(v,stdout);
            
        }
        p=p->next;
        if(i!=len){
            fprintf(stdout,"%c",ARGO_COMMA);
            if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
                fprintf(stdout,"%c",ARGO_LF);
                indent_level=global_options-(CANONICALIZE_OPTION+PRETTY_PRINT_OPTION);
                for(int i=0;i<indent_level*recursive;i++){
                    fprintf(stdout,"%c",ARGO_SPACE);
                }
            }
        }
        

    }
    if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
        fprintf(stdout,"%c",ARGO_LF);
        indent_level=global_options-(CANONICALIZE_OPTION+PRETTY_PRINT_OPTION);
        
    }
    if(recursive==1){
        fprintf(stdout,"%c",ARGO_RBRACE);
        if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
            fprintf(stdout,"%c",ARGO_LF);
        }
    }
    else{
        for(int i=0;i<indent_level*(recursive-1);i++){

            fprintf(stdout,"%c",ARGO_SPACE);
        }
        recursive--;
        fprintf(stdout,"%c",ARGO_RBRACE);

    }
    
    return 0;
        
        
    
    
    
}
int argo_write_array(ARGO_VALUE *v, FILE *f) {
    fprintf(stdout,"%c",ARGO_LBRACK);
    if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
        fprintf(stdout,"%c",ARGO_LF);
        indent_level=global_options-(CANONICALIZE_OPTION+PRETTY_PRINT_OPTION);
        for(int i=0;i<indent_level*recursive;i++){
            fprintf(stdout,"%c",ARGO_SPACE);
        }
    }
    struct argo_array t5=((v->content).array);
    struct argo_value *lastv=t5.element_list -> prev;
    struct argo_value *nextv=t5.element_list -> next;
    int len=0;
    while(&lastv->name.content != &nextv->name.content){
        len++;
        nextv =nextv-> next;
    }
    struct argo_value *p =t5.element_list -> next;
    for(int i=0;i<=len;i++){
        int arraytype=p->type;
        if(arraytype==0){
            

        }else if(arraytype==1){
            ARGO_BASIC n = p->content.basic;
            if(n==0){
                fprintf(stdout,"%s",ARGO_NULL_TOKEN);
            }else if(n==1){
                fprintf(stdout,"%s",ARGO_TRUE_TOKEN);
            }else if(n==2){
                fprintf(stdout,"%s",ARGO_FALSE_TOKEN);
            }
        }else if(arraytype==2){
            ARGO_NUMBER n = p->content.number;
            argo_write_number(&n, stdout);
        }else if(arraytype==3){
            //printf("type =3");
            ARGO_STRING n = p->content.string;
            argo_write_string(&n, stdout);
        }else if(arraytype==4){
            recursive++;
            argo_write_object(p,stdout);
            
        }else if(arraytype==5){

            recursive++;
            argo_write_array(v,stdout);
        }
        p=p->next;
        if(i!=len){
            fprintf(stdout,"%c",ARGO_COMMA);
            if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
                fprintf(stdout,"%c",ARGO_LF);
                indent_level =global_options-(CANONICALIZE_OPTION+PRETTY_PRINT_OPTION);
                for(int i=0;i<indent_level*recursive;i++){
                    fprintf(stdout,"%c",ARGO_SPACE);
                }
            }
        }
            
            
        
    }
    if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
        fprintf(stdout,"%c",ARGO_LF);
        indent_level=global_options-(CANONICALIZE_OPTION+PRETTY_PRINT_OPTION);
        
    }
    if(recursive==1){
        fprintf(stdout,"%c",ARGO_RBRACK);
        if(global_options>=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION && global_options<VALIDATE_OPTION){
            fprintf(stdout,"%c",ARGO_LF);
        }
    }
    else{
        for(int i=0;i<indent_level*(recursive-1);i++){

            fprintf(stdout,"%c",ARGO_SPACE);
        }
        recursive--;
        fprintf(stdout,"%c",ARGO_RBRACK);

    }
    
    return 0;
}

/**
 * @brief  Write canonical JSON representing a specified string
 * to a specified output stream.
 * @details  Write canonical JSON representing a specified string
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.  The argument string may contain any sequence of
 * Unicode code points and the output is a JSON string literal,
 * represented using only 8-bit bytes.  Therefore, any Unicode code
 * with a value greater than or equal to U+00FF cannot appear directly
 * in the output and must be represented by an escape sequence.
 * There are other requirements on the use of escape sequences;
 * see the assignment handout for details.
 *
 * @param v  Data structure representing a string (a sequence of
 * Unicode code points).
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
int argo_write_string(ARGO_STRING *s, FILE *f) {
   
    fprintf(stdout,"%c",ARGO_QUOTE);
    ARGO_CHAR *str= s->content;
    ARGO_CHAR *p=str;
    int len=s->length;

    while(p!=NULL && *p!='\0'&&len>0 ){
        if(argo_is_control(*p)){
            if(*p==ARGO_BS){
                fprintf(stdout,"%c%c",ARGO_BSLASH,ARGO_B);
                
            }
            else if(*p==ARGO_FF){
                fprintf(stdout,"%c%c",ARGO_BSLASH,ARGO_F);

            }else if(*p==ARGO_LF){
                fprintf(stdout,"%c%c",ARGO_BSLASH,ARGO_N);

            }else if (*p==ARGO_CR){
                fprintf(stdout,"%c%c",ARGO_BSLASH,ARGO_R);

            }else if (*p==ARGO_HT){
                fprintf(stdout,"%c%c",ARGO_BSLASH,ARGO_T);

            }
            else{
                
                if(*p<=0x000F){
                    ;
                    fprintf(stdout,"\\u000%x",*p);
                }else{
                    fprintf(stdout,"\\u00%x",*p);
                }
                
            }

             
            
        }
        else if(*p==ARGO_BSLASH){

            fprintf(stdout,"%c%c",ARGO_BSLASH,ARGO_BSLASH);
                
        }
        else if (*p==ARGO_QUOTE){
            fprintf(stdout,"%c%c",ARGO_BSLASH,*p);
        }

        else {
            
            fprintf(stdout,"%c",*p);
        }
        p++;
        len--;
    }
   
    fprintf(stdout,"%c",ARGO_QUOTE);
    return 0;
}

/**
 * @brief  Write canonical JSON representing a specified number
 * to a specified output stream.
 * @details  Write canonical JSON representing a specified number
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.  The argument number may contain representations
 * of the number as any or all of: string conforming to the
 * specification for a JSON number (but not necessarily canonical),
 * integer value, or floating point value.  This function should
 * be able to work properly regardless of which subset of these
 * representations is present.
 *
 * @param v  Data structure representing a number.
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */
int argo_write_number(ARGO_NUMBER *n, FILE *f) {
    
    ARGO_CHAR *str= n->string_value.content;
    ARGO_CHAR *p=str;
    if(n->float_value==0.0){
        
        fprintf(stdout,"%ls",n->string_value.content);
        return 0;

    }

    while(p!=NULL && *p!='\0'){
        if(argo_is_exponent(*p)||*p==ARGO_PERIOD){
            double f=n->float_value;
            // printf("get number:%f\t",f);
            int exp=0;
            int digit=0;
            int count=0;
            int check=0;
            //positive
            if(f>0){
                while(f<0.1||f>=1){
                    if(f<0.1){
                        f*=10;
                        exp--;
                    }else{
                        f/=10;
                        exp++;
                    }
                }
                if(n->float_value==(int)n->float_value){
                    check=1;
                }
                while(count<ARGO_PRECISION){
                    digit=(int)f;
                    if(count==2&&check==1&&digit==0){
                        break;
                    }
                    fprintf(stdout,"%d",digit);
                    if(count==0){
                        fprintf(stdout,"%c",ARGO_PERIOD);
                    }
                    f-=digit;
                    f*=10;
                    count++;
                }

                fprintf(stdout,"%c%d",'e',exp);
                return 0;
            }
            else{
                
                while(f>-0.1||f<=-1){
                    if(f>-0.1){
                        f*=10;
                        exp--;
                    }else{
                        f/=10;
                        exp++;
                    }
                }
                if(n->float_value==(int)n->float_value){
                    check=1;
                }
                while(count<ARGO_PRECISION){
                    digit=(int)f;
                    if(count==2&&check==1&&digit==0){
                        break;
                    }
                    if(count==0){
                        fprintf(stdout,"%d%c",digit,ARGO_PERIOD);
                    }else{
                        fprintf(stdout,"%d",-digit);
                    }
                    
                    f-=digit;
                    f*=10;
                    count++;
                }
                fprintf(stdout,"%c%d",'e',exp);
                return 0;
            }
        }
        ++p;
    }

    fprintf(stdout,"%ld",n->int_value);
 
    return 0;  
}