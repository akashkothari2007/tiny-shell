#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <strings.h> 
#include <sys/wait.h>
#include <termios.h>
#include <dirent.h>
#include <ctype.h>

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

//autocomplete

char **autocomplete(char *prefix, int pos) {
    DIR *d = opendir("."); // open current directory
    struct dirent *dir;    // structure to hold one entry

    if (d == NULL) {
        printf("Could not open current directory\n");
        return NULL;
    }
    char **file_names = malloc(100 * sizeof(char*));
    int num_of_matching_files = 0;
    while ((dir = readdir(d)) != NULL) {
        char *file_name = dir->d_name;  // print each file name
        int match = 1;
        for (int i = 3; i < pos; i++) {
 
            if (toupper(file_name[i-3]) != toupper(prefix[i])) {
                match = 0;
                break;
            }
        }
        if (match) {
            file_names[num_of_matching_files] = file_name;
            num_of_matching_files += 1;
        }
        

    }
    file_names[num_of_matching_files] = NULL;
    closedir(d);
    return file_names;
    
}

int takeCommands = 1; //should take commands on launch
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
    //size of space to allocate for the line
    
    int bufSize = 1024;
    int position = 0;

    char *buffer = malloc(bufSize * sizeof(char)); //allocate some space
    int c;
    //fail if allocation error
    if (!buffer) {
        printf("allocation error\n");;
        exit(EXIT_FAILURE);
    }
    while(takeCommands) {
    //keep looping getting next characters and deciding what to do with it, until user presses enter
    while (1) {
        //gets character
        c = getchar();
        if (c == '\033') { // Escape sequence (arrows or tab or delete key)
            getchar(); // skip the next character '['
            switch (getchar()) {
                case 'A': // up arrow previous command
                    //if it isnt at the start, clear and reprent line,
                    //subtract one from history index and copy the line from that index to buffer + update position
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
                case 'B': // down arrow opposite of up arrow, index goes up if not at limit
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
            char **file_names = autocomplete(buffer, position);
            int searching = 1;
            int count = 0;
            while (searching) {
                if (file_names[count] == NULL) {
                    searching = 0;
                } else {
                    count += 1;
                }
            }
            if (count == 1) {
                printf("\r\033[K");
                char cwd3[1024];
                if (getcwd(cwd3, sizeof(cwd3)) != NULL) {
                    printf(BLUE"TinyShell" RESET":" GREEN"%s" RESET "> ", cwd3); // green directory path
                } else {
                    printf("$ ");
                }

                sprintf(buffer, "cd %s", file_names[0]);
                position = (int)strlen(buffer);
                printf("cd %s", file_names[0]);
            } else if (count > 1) {
                printf("\n");
                printf("\r\033[K");
                int num = 0;
                while (file_names[num] != NULL) {
                    printf("%s  ", file_names[num]);
                    num += 1;
                
                }
                printf("\033[A");
                printf("\r\033[K");
                char cwd4[1024];
                if (getcwd(cwd4, sizeof(cwd4)) != NULL) {
                    printf(BLUE"TinyShell" RESET":" GREEN"%s" RESET "> ", cwd4); // green directory path
                } else {
                    printf("$ ");
                }

                
                printf("%s", buffer);
            }

        } else if (c == 127 || c == 8) { //delete key, move cursor back and 
            if (position > 0) {
                position--;              // move cursor back
                buffer[position] = '\0'; // erase last char in buffer
                // erase visually
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == EOF || c == '\n') { //if end of line set it to \0 and return the line
            printf("\n");
            printf("\r\033[K");
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


    //if it fails to allocate
    if (!tokens) {
        printf("allocation error\n");
        exit(EXIT_FAILURE);
    }

    //splits line with the delimiters and first arg is token
    token = strtok(line, LSH_TOK_DELIM); 

    while (token != NULL) { //split line with delimiters and add each token to tokens
        //checking for quotes (should not split spaces in quotes)
        if (token[0] == '"' || token[0] == '\'') {
            char quote = token[0];
            token++; // skip opening quote

            char merged[1024];
            strcpy(merged, token);

            // keep adding tokens until closing quote
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

        //adds one to position (referring to index of tokens)
        position++;

        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                printf("allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        //repeat (NULL means continue splitting the same string)
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL; //last one to null (exec requires it)
    return tokens;
}

//FORK AND RUN A PROGRAM W THE CHILD
int shell_launch(char **args) {
    pid_t pid, wpid;
    int status;
    takeCommands = 0;
    pid = fork();
    if (pid == 0) { //child
        if (execvp(args[0], args) == -1) {
            printf(MAGENTA "Please enter a real command, or use help to see built in commands\n" RESET);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) { //if error
        printf("error forking \n");

    } else { //parent waits until it finished or it was suspended 
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    enable_raw_mode();
    takeCommands = 1;
    return 1;
}

int shell_execute(char **args) {
    int i;
    
    //if empty return
    if (args[0] == NULL) { 
        if (hiState) {
            printf(MAGENTA "Fine don't respond. \n" RESET);
            hiState = 0;
        }
        printf(RED"Wheres the command??\n"RESET);
        
        return 1;
    }
    //dont even worry abt this
    if (hiState) {
        printf(MAGENTA "No hablo ingles. \n" RESET);
        hiState = 0;
    }
    //checks the string of builtins if any matches **ADD AUTOCORRECT HERE??
    for (i = 0; i < num_builtins(); i++) {
        if (strcasecmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    //if none of the above, then it must create another process to run (fork)
    return shell_launch(args);
}

//LOOPS: READS LINE --> PARSES LINE --> EXECUTES
void shell_loop(void) {
    int status = 1;
    char cwd[1024];
    while (status) {
        //green directory path
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf(BLUE"TinyShell" RESET":" GREEN"%s" RESET "> ", cwd); 
        } else {
            printf("$ ");
        }

        //reads line, adds it to history, splits, and executes command
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

    chdir(getenv("HOME")); //set starting directory
    enable_raw_mode(); //allow for tab and arrow key handling
    shell_loop(); //begin the loop

    return 0;
}



