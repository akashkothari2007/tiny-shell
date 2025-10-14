#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/wait.h>

//SHELL BUILT IN COMMANDS LIST
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
//list of built in commands strings (to match)
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};
int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}


//BUILT IN FUNCTIONS
int shell_cd(char **args) {
    if (args[1] != NULL) {
        if (chdir(args[1]) != 0) {
            printf("errorr \n");
        }
    } else {
        printf("wrong cd command \n");
    }
    return 1;
}
int shell_help(char **args) {
    printf("Welcome to tiny shell!! \n");
    printf("Created by Akash Kothari based on Stephen Brennan's shell \n");
    printf("Use the usual program names, but here are some built in ones!:\n");
    for (int i = 0; i < num_builtins(); i++) {
        printf("%s\n", builtin_str[i]);
    }
    return 1;
}
int shell_exit(char **args) {
    return 0;
}



//READ LINE
char *read_line(void)
 {
    int bufSize = 1024;
    int position = 0;

    char *buffer = malloc(bufSize * sizeof(char)); //allocate some space
    int c;
    if (!buffer) {
        printf("lsh: allocation error\n");;
        exit(EXIT_FAILURE);
    }
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') { //if end of line set it to \0 and return the line
            buffer[position] = '\0';
            return buffer; 
        } else {
            buffer[position] = c; //else next char add it to the line
        }

        position += 1;

        

        if (position >= bufSize) { //reallocate some more space if exceeds buffer size
            bufSize += 1024;
            buffer = realloc(buffer, bufSize * sizeof(char));
            if (!buffer) {
                printf("lsh: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
 }
 //SPLIT LINE INTO ARGUMENTS
#define LSH_TOK_DELIM " \t\r\n\a" //where to split
char **split_line(char *line) {
    int bufsize = 64; 
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*)); //allocate some size for tokens
    char *token;

    if (!tokens) {
        printf("lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM); 

    while (token != NULL) { //split line with delimiters and add each token to tokens
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*)); //reallocate size if needed
            if (!tokens) {
                printf("lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL; //last one to null (exec requires it)
    return tokens;
}

//FORK AND RUN A PROGRAM W THE CHILD
int shell_launch(char **args) {
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) { //child
        if (execvp(args[0], args) == -1) {
            printf("Please enter a real command \n");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        printf("error forking \n");

    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int shell_execute(char **args) {
    int i;

    if (args[0] == NULL) { //empty command
        printf("Wheres the command?? \n");
        return 1;
    }

    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return shell_launch(args);
}

//LOOPS: READS LINE --> PARSES LINE --> EXECUTES
void shell_loop(void) {
    int status = 1;
    while (status) {
        char *line;
        char **args;
        line = read_line();
        args = split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);
    }
}


//START THE SHELL!!!!
int main(int argc, char **argv) {
    
    shell_loop();

    return 0;
}



