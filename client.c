#include "./header.h"

int main(int args, char **argv)
{
    int fd_npipe, fd_nppipe;
    request req;
    char next_command[1];
    int num_bytes; 
    char buffer[PIPE_BUF];

    int i = NUMBER_TRIES;
    
    //check if the server is running
    while(access(NAMED_PIPE, F_OK) && i>0)
    {
        printf("Trying %d.... \n", i);
        sleep(SLEEP_TIME);
        i--;
    }
        
    if(i==0)
    {
        printf("Server is not running\n");    
        exit(-1);
    }
    
    //Opening the public pipe
    if((fd_npipe = open(NAMED_PIPE, O_WRONLY)) == -1)
    {
        perror("Error opening the public pipe\n");
        exit(-1); 
    }
    
    while(1)
    {
        printf("\nDo you want to run a command? [y/n]\n");
        scanf("%s", next_command);

        if(strcmp(next_command, "n") == 0)
        {
            printf("Bye\n");
            exit(0);        
        }
        else if(strcmp(next_command, "y") == 0)
        {
            printf("Select the next command:\n \t1. file_list\n \t2. disk_space\n \t3. disk_usage\n");
            scanf("%d", &req.cmd);
            
            //creating the filename of the private pipe
            sprintf(req.private_pipe, "%s", "mkfifo2");

            //creating the private named pipe
            if(mkfifo(req.private_pipe, 0777) == -1)
            {
                if(errno != EEXIST)
                {
                    perror("Error creating the pipe\n");
                    exit(-1);                
                }
            }
        
            //writing on the public named pipe
            if(write(fd_npipe, &req, sizeof(request)) == -1)
            {
                perror("Error writing to the public pipe\n");
                exit(-1);
            }

            //opening the private named pipe
            if((fd_nppipe = open(req.private_pipe, O_RDONLY)) == -1)
            {
                perror("Error opening the private pipe\n");
                exit(-1);            
            }

            //reading from the private pipe
            memset(buffer, '\0', PIPE_BUF);
            while((num_bytes = read(fd_nppipe, buffer, sizeof(PIPE_BUF))) >0)
            {
                printf("%s", buffer);  
                memset(buffer, '\0', PIPE_BUF); 
            }

            if(close(fd_nppipe) == -1)
            {
                perror("Error closing the private pipe\n");
                exit(-1);            
            }

            if(unlink(req.private_pipe) == -1)
            {
                perror("Error closing the private pipe\n");
                exit(-1);            
            }
        }
        else
        {
            printf("What?\n");     
        }
        
        while(getchar() != '\n');
    }
}
