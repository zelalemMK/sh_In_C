#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void lsh_loop(void);
char *lsh_read_line(void);
int lsh_launch(char **args);
int lsh_excute(char **args);
char **lsh_split_line(char *line);

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);


int main(void)
{
    //Load config files, if any.

    //Run command 
    lsh_loop();

    //Perform shutdown clean up/

}

void lsh_loop(void) 
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_excute(args);

        free(line);
        free(args);
    } while (status);
    
}


#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) *bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    while(1) {
        c = getchar();
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize)
        {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr,"lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define LSH_TOK_SIZE_BUFF 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line) 
{
    int bufsize = LSH_TOK_SIZE_BUFF, position = 0;
    char **tokens = malloc(bufsize *sizeof(char*));
    char *token;

    if (!token) {
        fprintf(stderr, "lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while(token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        {
            bufsize += LSH_TOK_SIZE_BUFF;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens)
            {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);

    }
    tokens[position] = NULL;
    return tokens;
}

/*
    fork(): creats a parallal process next to the main process, returns pid as int
    execvp(): system call, takes process name and where to find the bin file. 
    perror(): print system error. 
    waitpid:
    WIFEXITED:
    WUNTRACED:
    WIFSIGNALED:

*/
int lsh_launch(char **args) 
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        //child process
        if (execvp(args[0], args) == -1)
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0)
    {
        perror("lsh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;

}

char *builtin_str[] = {
    "cd",
    "help", 
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help, 
    &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


int lsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "lsh: expected args to \"cd\" \n");
    } else {
        if (chdir(args[1]) != 0)
        {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
    int i;
    printf("The Built ins");
    for (int i = 0; i < lsh_num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }
    printf("Use the man command for info.");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

int lsh_excute(char **args)
{
    int i;
    if (args[0] == NULL) {
        return 1;
    }
    for (i = 0; i < lsh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}