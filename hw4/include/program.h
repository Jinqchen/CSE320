//extern int counter=0;


// typedef struct stmtList {
//     int counter;
//     STMT *s;
//     union {
        
//         struct {
//             struct stmtList *next;
//             struct stmtList *prev;
//         } links;
       
        
//     } body;
// } stmtList;
struct stmtList{
    int index;
    //int lineNumber;
    STMT *statement;
    struct stmtList *next;

};
struct stmtList *first = NULL;
struct stmtList *tail = NULL;



// typedef enum {
//     new = 0, 
//     running, 
//     completed,
//     aborted, 
//     canceled
// }status;
// struct jobs{
//     int jobid;
//     pid_t pid;
//     status status;
    
// };
// struct jobs jobs[MAX_JOBS];



