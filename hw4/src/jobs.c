#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#include "mush.h"
#include "debug.h"
#include "job.h"


//int fd[2];
//char *buf;
int leaderexit=0;
//pid_t bgpid=-1;
 //static int jobstatus;
void sigchld_handler(int sig){//deal with sign al
    pid_t pid;
    if((pid=waitpid(-1,NULL,WNOHANG))==0){
        //printf("0\n");
        return;
    }
    //printf("pid:%d\n",pid);
    for(int i = 0;i<MAX_JOBS;i++){
        if(jobs[i].pid==pid){
            jobs[i].status=completed;
            return;
        }
        
    }
    //printf("????\n");

    //jobs[1].status=completed;
}
/*
 * This is the "jobs" module for Mush.
 * It maintains a table of jobs in various stages of execution, and it
 * provides functions for manipulating jobs.
 * Each job contains a pipeline, which is used to initialize the processes,
 * pipelines, and redirections that make up the job.
 * Each job has a job ID, which is an integer value that is used to identify
 * that job when calling the various job manipulation functions.
 *
 * At any given time, a job will have one of the following status values:
 * "new", "running", "completed", "aborted", "canceled".
 * A newly created job starts out in with status "new".
 * It changes to status "running" when the processes that make up the pipeline
 * for that job have been created.
 * A running job becomes "completed" at such time as all the processes in its
 * pipeline have terminated successfully.
 * A running job becomes "aborted" if the last process in its pipeline terminates
 * with a signal that is not the result of the pipeline having been canceled.
 * A running job becomes "canceled" if the jobs_cancel() function was called
 * to cancel it and in addition the last process in the pipeline subsequently
 * terminated with signal SIGKILL.
 *
 * In general, there will be other state information stored for each job,
 * as required by the implementation of the various functions in this module.
 */

/**
 * @brief  Initialize the jobs module.
 * @details  This function is used to initialize the jobs module.
 * It must be called exactly once, before any other functions of this
 * module are called.
 *
 * @return 0 if initialization is successful, otherwise -1.
 */
int jobs_init(void) {
    // printf("init\n");
    //pipe(fd);
    for(int i = 0;i<MAX_JOBS;i++){
        jobs[i].jobid=-1;
        jobs[i].pid=-1;
        jobs[i].status=-1;
        jobs[i].pline=NULL;
    }
    return 0;
}

/**
 * @brief  Finalize the jobs module.
 * @details  This function is used to finalize the jobs module.
 * It must be called exactly once when job processing is to be terminated,
 * before the program exits.  It should cancel all jobs that have not
 * yet terminated, wait for jobs that have been cancelled to terminate,
 * and then expunge all jobs before returning.
 *
 * @return 0 if finalization is completely successful, otherwise -1.
 */
int jobs_fini(void) {
    for(int i = 0;i<MAX_JOBS;i++){
        if(jobs[i].status!=completed){
            // jobs_cancel(job[i].jobid);
            // jobs_expunge(job[i].jobid);
            if(jobs_cancel(jobs[i].jobid)==-1){
                return -1;
            }
            if(jobs_expunge(jobs[i].jobid)==-1){
                return -1;
            }
            
        }
        
    }
    return 1;
}

/**
 * @brief  Print the current jobs table.
 * @details  This function is used to print the current contents of the jobs
 * table to a specified output stream.  The output should consist of one line
 * per existing job.  Each line should have the following format:
 *
 *    <jobid>\t<pgid>\t<status>\t<pipeline>
 *
 * where <jobid> is the numeric job ID of the job, <status> is one of the
 * following strings: "new", "running", "completed", "aborted", or "canceled",
 * and <pipeline> is the job's pipeline, as printed by function show_pipeline()
 * in the syntax module.  The \t stand for TAB characters.
 *
 * @param file  The output stream to which the job table is to be printed.
 * @return 0  If the jobs table was successfully printed, -1 otherwise.
 */
int jobs_show(FILE *file) {
    // TO BE IMPLEMENTED
    signal(SIGCHLD,sigchld_handler);
    for(int i=0;i<MAX_JOBS;i++){
        if(jobs[i].jobid!=-1){
            fprintf(file,"<%d>\t<%u>\t<",jobs[i].jobid,jobs[i].pid);
            switch(jobs[i].status){
                case 0:
                    fprintf(file,"new>\t");
                    break;
                case 1:
                    fprintf(file,"running>\t");
                    break;
                case 2:
                    fprintf(file,"completed>\t");
                    break;
                case 3:
                    fprintf(file,"aborted>\t");
                    break;
                case 4:
                    fprintf(file,"canceled>\t");
                    break;
                default:
                    break;


            }
            //int i=0;
            fprintf(file,"<");
            struct arg *args=jobs[i].pline->commands->args;
            while(args!=NULL){
                //printf("sec%s\n",args->expr->members.variable);
                // if(args->expr->type==1){
                //         FILE *stream;
                //         char *buf;
                //         size_t len;
                //         stream=open_memstream(&buf,&len);
                //         fprintf(stream,"#");
                //         fprintf(stream,"%s",args->expr->members.variable);
                //         argument_list[i]=buf;
                //         args=args->next;
                //         i++;
                // }else{
                    fprintf(file,"%s ",args->expr->members.variable);
                //argument_list[i]=args->expr->members.variable;
                //printf("arg:%s\n",args->expr->members.variable);
                    args=args->next;
                    
                //}
            }
            //show_pipeline(file,jobs[i].pline);
            fprintf(file,">\n");
        }
    }
    return 0;
}

/**
 * @brief  Create a new job to run a pipeline.
 * @details  This function creates a new job and starts it running a specified
 * pipeline.  The pipeline will consist of a "leader" process, which is the direct
 * child of the process that calls this function, plus one child of the leader
 * process to run each command in the pipeline.  All processes in the pipeline
 * should have a process group ID that is equal to the process ID of the leader.
 * The leader process should wait for all of its children to terminate before
 * terminating itself.  The leader should return the exit status of the process
 * running the last command in the pipeline as its own exit status, if that
 * process terminated normally.  If the last process terminated with a signal,
 * then the leader should terminate via SIGABRT.
 *
 * If the "capture_output" flag is set for the pipeline, then the standard output
 * of the last process in the pipeline should be redirected to be the same as
 * the standard output of the pipeline leader, and this output should go via a
 * pipe to the main Mush process, where it should be read and saved in the data
 * store as the value of a variable, as described in the assignment handout.
 * If "capture_output" is not set for the pipeline, but "output_file" is non-NULL,
 * then the standard output of the last process in the pipeline should be redirected
 * to the specified output file.   If "input_file" is set for the pipeline, then
 * the standard input of the process running the first command in the pipeline should
 * be redirected from the specified input file.
 *
 * @param pline  The pipeline to be run.  The jobs module expects this object
 * to be valid for as long as it requires, and it expects to be able to free this
 * object when it is finished with it.  This means that the caller should not pass
 * a pipeline object that is shared with any other data structure, but rather should
 * make a copy to be passed to this function.
 * 
 * @return  -1 if the pipeline could not be initialized properly, otherwise the
 * value returned is the job ID assigned to the pipeline.
 */
int jobs_run(PIPELINE *pline) {
    // TO BE IMPLEMENTED
    //create leader first
    //int fd[2];
    //int val;
    //pipe(fd);
    int position=-1;
    pid_t pid=fork();
    if(pid==-1){
        return -1;
    }
    if(pid==0){
        //leader
        //printf("group%u\n",getpid());
        struct command *cmd=pline->commands;
        while(cmd!=NULL){
            if(fork()==0){
                //setpgid(getpid(),getpid());
                exit(0);
            }else{
                cmd=cmd->next;
            }
        }
        // dup2(fd[WRITE_END],STDOUT_FILENO);
        // close(fd[READ_END]);
        // CLOSE(fd[WRITE_END]);
        char* argument_list[10];
        struct arg *args=pline->commands->args;
        int i=0;
        while(args!=NULL){
            //printf("sec%s\n",args->expr->members.variable);
            // if(args->expr->type==1){
            //         FILE *stream;
            //         char *buf;
            //         size_t len;
            //         stream=open_memstream(&buf,&len);
            //         fprintf(stream,"#");
            //         fprintf(stream,"%s",args->expr->members.variable);
            //         argument_list[i]=buf;
            //         args=args->next;
            //         i++;
            // }else{
            argument_list[i]=args->expr->members.variable;
            //printf("arg:%s\n",args->expr->members.variable);
            args=args->next;
            i++;
            //}
        }
        argument_list[i]=NULL;
        //bgpid=getpid();
        execvp(pline->commands->args->expr->members.variable,argument_list);
        //printf("if end???\n");
        //completed
        // char *s;
        // sprint(s,"%d",getpid);
        // close(fd[0]);
        // write(fd[1],s,sizeof(s));
        exit(0);
        
    }else{
        //parent
        //printf("store%u\n",pid);
        for(int i=0;i<MAX_JOBS;i++){
            if(jobs[i].pid ==-1&& jobs[i].status==-1){
                position=i;
                jobs[i].pid=pid;
                jobs[i].jobid=i;
                jobs[i].status=running;
                jobs[i].pline=pline;
               //show_pipeline(stdout,jobs[i].pline);
                break;
            }
        }
        // signal(SIGCHLD,sigchld_handler);
        
        // int jobstatus;
        // //int result=-1;
        // //printf("this pid wait:%u\n",getpid());
        // if(waitpid(pid,&jobstatus,WNOHANG)){
        //     perror("waitpid");
            
        // }
        // if(WIFEXITED(jobstatus)){
        // // jobs[jobid].pline->capture_output=1;
        //     for(int i = 0;i<MAX_JOBS;i++){
        //         if(jobs[i].pid==pid){
        //             jobs[i].status=completed;
                    
        //         }
            
        //     }
        //     //jobs[position].status=completed;
        //     //show_pipeline(stdout,jobs[position].pline);
        //     printf("main process------child exit with %d.pid%d\n",jobstatus,pid);
        // }else{
        //     //printf("????????????????\n");
        //     for(int i = 0;i<MAX_JOBS;i++){
        //         if(jobs[i].pid==pid){
        //             jobs[i].status=aborted;
                    
        //         }
            
        //     }
        // }
        
    }
    return position;
    /*
    if(!leaderexit){
        pid_t leader=fork();
        leaderexit=1;
        if(leader==0){
            //leader
            setpgid(getpid(),getpid());
            leaderexit=getpid();
            printf("leader:%u\n",getpid());
        }else{
            //parent
            jobs_wait(leader);
            
        }
    }
    //printf("pid:%u",getpid());
    //pid_t leaderpid=getpid();
    int position=-1;
    for(int i=0;i<MAX_JOBS;i++){
        if(jobs[i].pid ==-1&& jobs[i].status==-1){
            position=i;
            jobs[i].pid=getpid();
            jobs[i].jobid=i;
            jobs[i].status=new;
            jobs[i].pline=pline;
            //printf("find empty\n");
            break;
        }
    }
    if(position==-1) return -1;
    //printf("%u pid\n",getpid());
    pid_t pid=fork();
    if(pid==-1){
        return -1;
    }
    if(pid==0){
        printf("position%d\n",position);
        setpgid(getpid(),leaderexit);
        jobs[position].jobid=position;
        jobs[position].status=new;
        jobs[position].pline=pline;

        jobs[position].pid=getpid();
        //setpgid(getpid(),leaderpid);
        printf("group pid:%u\n",getpgid(0));
        printf("this pid:%u\n",getpid());
        jobs[position].status=running;
        //printf("child%u\n",getpid());
        char* argument_list[10];
        struct arg *args=pline->commands->args;
        int i=0;
        while(args!=NULL){
            //printf("sec%s\n",args->expr->members.variable);
            argument_list[i]=args->expr->members.variable;
            args=args->next;
            i++;
        }
        argument_list[i]=NULL;
        
        jobs[position].status=running;
        //printf("first%s\n",pline->commands->args->expr->members.variable);
        execvp(pline->commands->args->expr->members.variable,argument_list);
        //printf("error2?\n");
        jobs[position].status=completed;
        //exit(0);
        
    }
    else{
         printf("this pid in parent:%u\n",getpid());
        //printf("father pid:%u\n",getpid());
        //jobs_wait(pid);
        // int status;
        // waitpid(pid,&status,WUNTRACED|WCONTINUED);
        // close(0);
        // close(f[1]);
        // dup(fd[0]);
        // int array[1];
        // read(fd[0],array,sizeof(array));
        // jobs[position].pid=array[0];
        // jobs[position].status=array[1];
        //jobs_wait(pid);
        
    }

    return position;*/
    //return 0;
}

/**
 * @brief  Wait for a job to terminate.
 * @details  This function is used to wait for the job with a specified job ID
 * to terminate.  A job has terminated when it has entered the COMPLETED, ABORTED,
 * or CANCELED state.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(),
 * or -1 if any error occurs that makes it impossible to wait for the specified job.
 */
int jobs_wait(int jobid) {
    int jobstatus;
    //int result=-1;
     //printf("this pid wait:%u\n",getpid());
    if(!waitpid(jobs[jobid].pid,&jobstatus,/*WUNTRACED|WCONTINUED*/WUNTRACED)){
        perror("waitpid");
        return SIGABRT;
    }
    /*sigset_t signal_n;
    sigfillset(&signal_n);
    sigdelset(&signal_n,2);
    //signal(SIGCHLD,sigchld_handler);
    
    //sigprocmask(SIG_BLOCK,&signal_n,&signal_o);
    
    
    sigprocmask(SIG_BLOCK,&signal_n,NULL);*/
    if(WIFEXITED(jobstatus)){
        // jobs[jobid].pline->capture_output=1;
        jobs[jobid].status=completed;
        //show_pipeline(stdout,jobs[jobid].pline);
        //printf("------child exit with %d.jobid%d\n",jobstatus,jobid);
    }else{
        // printf("????????????????\n");
        jobs[jobid].status=aborted;
    }
    //jobs[jobid].status=completed;
    
    //printf("result%d,pid%u\n",jobstatus,getpid());
    return jobstatus;
    // TO BE IMPLEMENTED
    
}

/**
 * @brief  Poll to find out if a job has terminated.
 * @details  This function is used to poll whether the job with the specified ID
 * has terminated.  This is similar to jobs_wait(), except that this function returns
 * immediately without waiting if the job has not yet terminated.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(), if the job
 * has terminated, or -1 if the job has not yet terminated or if any other error occurs.
 */
int jobs_poll(int jobid) {
    for(int i = 0;i<MAX_JOBS;i++){
        if(jobs[i].jobid==jobid){
            if(jobs[i].status==completed){
                return 0;
            }else{
                return -1;
            }
            
        }
        
    }
    return -1;
}

/**
 * @brief  Expunge a terminated job from the jobs table.
 * @details  This function is used to expunge (remove) a job that has terminated from
 * the jobs table, so that space in the table can be used to start some new job.
 * In order to be expunged, a job must have terminated; if an attempt is made to expunge
 * a job that has not yet terminated, it is an error.  Any resources (exit status,
 * open pipes, captured output, etc.) that were being used by the job are finalized
 * and/or freed and will no longer be available.
 *
 * @param  jobid  The job ID of the job to expunge.
 * @return  0 if the job was successfully expunged, -1 if the job could not be expunged.
 */
int jobs_expunge(int jobid) {
    // TO BE IMPLEMENTED
    if(jobs[jobid].status==aborted||jobs[jobid].status==completed){
        jobs[jobid].pid=-1;
        jobs[jobid].jobid=-1;
        jobs[jobid].status=-1;
        
        return 0;
    }
    return -1;
}

/**
 * @brief  Attempt to cancel a job.
 * @details  This function is used to attempt to cancel a running job.
 * In order to be canceled, the job must not yet have terminated and there
 * must not have been any previous attempt to cancel the job.
 * Cancellation is attempted by sending SIGKILL to the process group associated
 * with the job.  Cancellation becomes successful, and the job actually enters the canceled
 * state, at such subsequent time as the job leader terminates as a result of SIGKILL.
 * If after attempting cancellation, the job leader terminates other than as a result
 * of SIGKILL, then cancellation is not successful and the state of the job is either
 * COMPLETED or ABORTED, depending on how the job leader terminated.
 *
 * @param  jobid  The job ID of the job to cancel.
 * @return  0 if cancellation was successfully initiated, -1 if the job was already
 * terminated, a previous attempt had been made to cancel the job, or any other
 * error occurred.
 */
int jobs_cancel(int jobid) {
    // TO BE IMPLEMENTED
    for(int i=0;i<MAX_JOBS;i++){
        if(jobs[i].jobid==jobid && jobs[i].status==running){
            kill(jobs[i].pid,SIGKILL);
            jobs[i].status=canceled;
            return 0;
        }
    }
    return -1;
}

/**
 * @brief  Get the captured output of a job.
 * @details  This function is used to retrieve output that was captured from a job
 * that has terminated, but that has not yet been expunged.  Output is captured for a job
 * when the "capture_output" flag is set for its pipeline.
 *
 * @param  jobid  The job ID of the job for which captured output is to be retrieved.
 * @return  The captured output, if the job has terminated and there is captured
 * output available, otherwise NULL.
 */
char *jobs_get_output(int jobid) {
   for(int i=0;i<MAX_JOBS;i++){
        if(jobs[i].jobid==jobid && jobs[i].status==completed && jobs[i].pline->capture_output){
            
            return " ";
        }
    }
    return NULL;
}

/**
 * @brief  Pause waiting for a signal indicating a potential job status change.
 * @details  When this function is called it blocks until some signal has been
 * received, at which point the function returns.  It is used to wait for a
 * potential job status change without consuming excessive amounts of CPU time.
 *
 * @return -1 if any error occurred, 0 otherwise.
 */
int jobs_pause(void) {
    pid_t pid;
    if((pid=waitpid(-1,NULL,WUNTRACED))==0){
        //printf("0\n");
        return -1;
    }
    for(int i = 0;i<MAX_JOBS;i++){
        if(jobs[i].pid==pid){
            jobs[i].status=completed;
            
        }
        
    }

    return 0;
}
