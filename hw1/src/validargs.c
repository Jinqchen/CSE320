#include <stdlib.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */

int validargs(int argc, char **argv) {
    // printf("what is it: %s",argv[6]);
    // printf("%d",argc);
    if (argc<=1){
        return -1;
    }
    if(argc==2){
        char *p= *(argv+1);
        char p1=*p; //suppose to be "-"
        char p2=*(p+1); //a letter
        char p3=*(p+2);//
        // printf("third lettet? %c",p3);
        if(p3!='\0'){
            return -1;
        }
        if (p1==ARGO_MINUS && p2=='c'){
            global_options =CANONICALIZE_OPTION;
            // printhelper(global_options);
            // printf("%d",global_options);
            // printf("%s","it is only c");
            return 0;
        }
        if(p1==ARGO_MINUS && p2=='v'){
            global_options=VALIDATE_OPTION;
            // printf("%d",global_options);
            // printf("%s","it is only v");
            return 0;
        }
        if(p1==ARGO_MINUS && p2=='h'){
            global_options=HELP_OPTION;
            // printf("%s","it is only h");
            return 0;
        }
        return -1;

    }
    if(argc>2){
        char *p= *(argv+1);
        char p1=*p; //suppose to be "-"
        char p2=*(p+1); //a letter
        char p3=*(p+2);
        if(p3!='\0'){
            return -1;
        }
        // if h provided
        if(p1==ARGO_MINUS && p2=='h'){
            global_options=HELP_OPTION;
            // printf("%s","it is only h");
            return 0;
        }
        // if h not provided
        // if c is first 
        else if (p1==ARGO_MINUS && p2=='c'){
            char *p= *(argv+2);
            char p1=*p; //suppose to be "-"
            char p2=*(p+1); //a letter
            char p3=*(p+2);
            if(p3!='\0'){
                return -1;
            }
            if (p1==ARGO_MINUS && p2=='p'){
                // printf("check second one %c\n",p2);
                if (argc==3){
                    // printf("%s","c and p");
                    global_options=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION+4;
                    // printf("%x",global_options);

                    return 0;

                }else if(argc==4){
                    char *p= *(argv+3);
                    char p1=*(p);
                    int num=0;
                    int i=1;
                    if( p1 == ARGO_MINUS){
                        return -1;
                    }
                    // if (*(p+2)=='\0'){printf("%s","null");}
                    // printf("%c,%c,%c,%c",*(p),*(p+1),*(p+2),*(p+3));
                    while(p1!='\0'){
                        // printf("%d\n",num);
                        num=num*10+(p1-ARGO_DIGIT0);
                        
                        p1= *(p+i);
                        
                        i+=1;
                        // printf("%d",num);



                    }
                    // printf("\n%d\n",num);
                    // printf("%s","c and p and a number\n");
                    global_options=CANONICALIZE_OPTION+PRETTY_PRINT_OPTION+num;
                    // printf("%d",global_options);
                    // printf("%c\n",p1);
                    // printf("%d\n",p1-'0');
                    // printhelper(global_options);
                    return 0;


                    
                    
                    
                    return -1;
                }
                return -1;
            }
            // if c with v
            // if (p1=='-' && p2=='v'){
            //     return -1;
            // }
            // if(p1=='-' && p2=='v')
            return -1;

        }
        // if v is first and with other 

        else if (p1==ARGO_MINUS && p2=='v'){
            // char *p= *(argv+2);
            // char p1=*p; //suppose to be "-"
            // char p2=*(p+1); //a letter
            // if (p1=='-' && p2=='c'){
            //     return -1
            // }
            return -1;
            
        }
        return -1;

    }
  

    return -1;

}
