#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
char *read_line(void)
 {
    int bufSize = 1024;
    int position = 0;

    char *buffer = malloc(bufSize * sizeof(char));
    int c;
    if (!buffer) {
        printf("lsh: allocation error\n");;
        exit(EXIT_FAILURE);
    }
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position += 1;

        

        if (position >= bufSize) {
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
#define LSH_TOK_DELIM " \t\r\n\a"
char **split_line(char *line) {
    int bufsize = 64; 
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        printf("lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
        bufsize += 64;
        tokens = realloc(tokens, bufsize * sizeof(char*));
        if (!tokens) {
            printf("lsh: allocation error\n");
            exit(EXIT_FAILURE);
        }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
void shell_launch(char **args) {
    
}

void shell_loop(void) {
    char *line;
    char **args;
    line = read_line();
    args = split_line(line);


    free(line);
    free(args);
}



int main(int argc, char **argv) {
    
    shell_loop();

    return 0;
}



