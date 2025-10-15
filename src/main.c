#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <strings.h> 
#include <sys/wait.h>
#include <termios.h>

//colors for text
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW   "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA   "\033[1;35m"
#define CYAN   "\033[1;36m"
#define WHITE    "\033[1;37m"
#define RESET   "\033[0m"

//history
char *history[1000];
int history_index = 0;
int history_cur_index = 0;
void manageHistory(char *line) {
    if (line[0] != '\0') {
        history[history_index] = strdup(line);
        history_index+=1;
        history_cur_index = history_index;
    }
}

//allow terminal to get real time handling for arrow key presses or tabs (so their not just chars)
void enable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);  // disable canonical mode & echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
//states
int hiState = 0;

//SHELL BUILT IN COMMANDS LIST
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_greet(char **args);
//list of built in commands strings (to match)
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "hi",
  "hello",
  "hola"
};
int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit,
  &shell_greet,
  &shell_greet,
  &shell_greet
};
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}


//BUILT IN FUNCTIONS
int shell_cd(char **args) {
    if (args[1] != NULL) {
        if (chdir(args[1]) != 0) {
            printf(MAGENTA "That doesnt exist stupid. \n" RESET);
        }
    } else {
        chdir("..");
    }
    return 1;
}
int shell_help(char **args) {
    printf(YELLOW "Welcome to tiny shell!! \n");
    printf("Created by Akash Kothari based on Stephen Brennan's shell \n");
    printf("Use the usual program names, but here are some built in ones!: \n" RESET);
    for (int i = 0; i < num_builtins(); i++) {
        printf(WHITE "%s \n" RESET , builtin_str[i]);
    }
    return 1;
}
int shell_exit(char **args) {
    return 0;
}
int shell_greet(char **args) {
    printf(MAGENTA "Hi there. Why are you talking to me. \n" RESET);
    hiState = 1;
    return 1;
}



//READ LINE
char *read_line(void)
 {
    int bufSize = 1024;
    int position = 0;

    char *buffer = malloc(bufSize * sizeof(char)); //allocate some space
    int c;
    if (!buffer) {
        printf("allocation error\n");;
        exit(EXIT_FAILURE);
    }
    while (1) {
        c = getchar();
        if (c == '\033') { // Escape sequence
            getchar(); // skip the bracket '['
            switch (getchar()) {
                case 'A': // up arrow previous command
                    if (history_cur_index == 0) break;
                    printf("\r\033[K");
                    char cwd[1024];
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf(BLUE"TinyShell" RESET":" GREEN"%s" RESET "> ", cwd); // green directory path
                    } else {
                        printf("$ ");
                    }
                    if (history_cur_index > 0) {
                        history_cur_index -= 1;
                        strcpy(buffer, history[history_cur_index]);
                        position = (int)strlen(buffer);
                        printf("%s", history[history_cur_index]);
                        

                    } else {
                        position = 0;
                    }
                    break;
                case 'B': // down arrow (dont know yet)
                    if (history_cur_index >= history_index) break;
                    printf("\r\033[K");
                    char cwd2[1024];
                    if (getcwd(cwd2, sizeof(cwd2)) != NULL) {
                        printf(BLUE"TinyShell" RESET":" GREEN"%s" RESET "> ", cwd2); // green directory path
                    } else {
                        printf("$ ");
                    }
                    
                    history_cur_index += 1;
                    if (history_cur_index != history_index) {
                        strcpy(buffer, history[history_cur_index]);
                        position = (int)strlen(buffer);
                        printf("%s", history[history_cur_index]);
                    } else {
                        position = 0;
                    }
                        

                    
                    break;
            }
        } else if (c == '\t') { //tab to autocomplete
            printf("tab");
        } else if (c == 127 || c == 8) {
            if (position > 0) {
                position--;              // move cursor back
                buffer[position] = '\0'; // erase last char in buffer
                // erase visually
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == EOF || c == '\n') { //if end of line set it to \0 and return the line
            printf("\n");
            buffer[position] = '\0';
            
            return buffer; 
        } else {
            printf("%c", c);
           
            buffer[position] = c; 
            position += 1;
                             //else next char add it to the line
        }

        

        
        //reallocate some more space if exceeds buffer size
        if (position >= bufSize) {
            bufSize += 1024;
            buffer = realloc(buffer, bufSize * sizeof(char));
            if (!buffer) {
                printf("reallocation error\n");
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
        printf("allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM); 

    while (token != NULL) { //split line with delimiters and add each token to tokens
        if (token[0] == '"' || token[0] == '\'') {
            char quote = token[0];
            token++; // skip opening quote

            char merged[1024];
            strcpy(merged, token);

            // keep adding tokens until we find the closing quote
            while (merged[strlen(merged) - 1] != quote) {
                char *next = strtok(NULL, LSH_TOK_DELIM);
                if (!next) break;
                strcat(merged, " ");
                strcat(merged, next);
            }
 
            // remove trailing quote
            size_t len = strlen(merged);
            if (len > 0 && merged[len - 1] == quote)
                merged[len - 1] = '\0';

            tokens[position] = strdup(merged);
        } else {
            tokens[position] = token;
        }

        position++;

        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                printf("allocation error\n");
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
            printf(MAGENTA "Please enter a real command, or use help to see built in commands\n" RESET);
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
        if (hiState) {
            printf(MAGENTA "Fine don't respond. \n" RESET);
            hiState = 0;
        }
        printf(RED"Wheres the command??\n"RESET);
        
        return 1;
    }
    if (hiState) {
        printf(MAGENTA "No hablo ingles. \n" RESET);
        hiState = 0;
    }
    for (i = 0; i < num_builtins(); i++) {
        if (strcasecmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return shell_launch(args);
}

//LOOPS: READS LINE --> PARSES LINE --> EXECUTES
void shell_loop(void) {
    int status = 1;
    char cwd[1024];
    while (status) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf(BLUE"TinyShell" RESET":" GREEN"%s" RESET "> ", cwd); // green directory path
        } else {
            printf("$ ");
        }
        char *line;
        char **args;
        line = read_line();
        manageHistory(line);
        args = split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);
    }
}


//START THE SHELL!!!!
int main(int argc, char **argv) {

    chdir("/Users/akashkothari/Desktop"); //set starting directory
    enable_raw_mode(); //allow for tab and arrow key handling
    shell_loop(); //being the loop

    return 0;
}



