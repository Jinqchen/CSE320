
typedef enum {
    none = -1,
    new = 0, 
    running, 
    completed,
    aborted, 
    canceled
}status;
struct jobs{
    int jobid;
    pid_t pid;
    status status;
    PIPELINE *pline;
    
};
struct jobs jobs[MAX_JOBS];