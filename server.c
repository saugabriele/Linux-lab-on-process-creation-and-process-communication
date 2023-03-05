#include "./header.h"

void execute_command(int);

int main(int args, char **argv)
{
    int fd_npipe, fd_nppipe;
    request req;
    int fd_pipe[2];
    int pid;
    int num_bytes;
    char buffer[PIPE_BUF];

    //creates the public named pipe
    if(mkfifo(NAMED_PIPE, 0777) == -1)
    {
        if(errno != EEXIST)
        {
            perror("Error creating the pipe\n");
            exit(-1);        
        }    
    }    

    //Opening the public pipe
    if((fd_npipe = open(NAMED_PIPE, O_RDWR)) == -1)
    {
        perror("Error opening the public pipe\n");
        exit(-1);        
    }

    while(1)
    {
        printf("Waiting for requests\n");
    
        //reading an incoming request from the client
        if(read(fd_npipe, &req, sizeof(request)) == -1)
        {
            perror("Error reading from the public pipe\n");
            exit(-1);        
        }    

        printf("Request received\n");
        printf("\t command: %d\n", req.cmd);
        printf("\t private pipe: %s\n", req.private_pipe);

        //opening the private pipe for writing
        if((fd_nppipe = open(req.private_pipe, O_WRONLY))  == -1)
        {
            perror("Error opening the private pipe\n");
            exit(-1);
        }

        //creating the unnamed pipe
        if(pipe(fd_pipe) == -1)
        {
            perror("Unable to create the unnamed pipe\n");
            exit(-1);        
        }

        if((pid = fork()) == -1)
        {
            perror("Error creating a child process\n");

            //closing the private pipe
            if(close(fd_nppipe) == -1)
            {
                perror("Error closing the private pipe\n");
                exit(-1);            
            }

            //closing the public pipe
            if(close(fd_npipe) == -1)
            {
                perror("Error closing the public pipe\n");
                exit(-1);            
            }

            //unlinking the public pipe
            if(unlink(NAMED_PIPE) == -1)
            {
                perror("Unable to unlink the public pipe\n");
                exit(-1);            
            }
            exit(-1);        
        }

        switch(pid)
        {
        case 0:
            if(close(fd_pipe[0]) == -1)
            {
                perror("Error closing the read end of the unnamed pipe\n");
                exit(-1);                
            }

            //redirects child stdout to the unnamed pipe
            if(dup2(fd_pipe[1], STDOUT_FILENO) == -1)
            {
                perror("Dup2 error\n");
                exit(-1);                
            } 

            execute_command(req.cmd);
            
            if(close(fd_pipe[1]) == -1)
            {
                perror("Error closing the write end of the unnamed pipe\n");
                exit(-1);                
            }
            break;

        default:
            if(close(fd_pipe[1]) == -1)
            {
                perror("Error closing the write end of the unnamed pipe\n");
                exit(-1);                
            }

            //reading from the unnamed pipe and write to the private pipe
            while((num_bytes = read(fd_pipe[0], buffer, PIPE_BUF * sizeof(char)))>0)
            {
                if(write(fd_nppipe, buffer, num_bytes) == -1)
                {
                    perror("Error writing to the private pipe\n");
                    exit(-1);                    
                }
            }
                
            if(close(fd_pipe[0]) == -1)
            {
                perror("Error closing the read end of the unnamed pipe\n");
                exit(-1);                
            }   
            
            if(close(fd_nppipe) == -1)
            {
                perror("Error closing the private pipe\n");
                exit(-1);            
            }
            break; 
        }
    }
    return 0;
}

void execute_command(int cmd)
{
    switch(cmd)
    {
        case 1:
            if(execlp("/usr/bin/ls", "ls", "-al", (char *)NULL) == -1)
            {
                perror("Unable to execute the ls command\n");
                printf("Unable to satisfy the request\n");
                exit(-1);           
            }
            break; 
 
        case 2:
            if(execlp("df", "df", "-k", (char *)NULL) == -1)
            {
                printf("Exec error\n");
                exit(-1);           
            }
            break;

        case 3:
            if(execlp("du", "du", "-hk", "/usr/local/bin", (char *)NULL) == -1)
            {
                printf("Exec error\n");
                exit(-1);           
            }
            break;

        default:
            fprintf(stderr, "Not a valid command\n");
            printf("Unable to satisfy the request\n");
            exit(-1);
    }
}
