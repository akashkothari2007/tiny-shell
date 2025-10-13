#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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

void shell_loop(void) {
    char *line;
    line = read_line();
    printf("%c", *line);

}

int main(int argc, char **argv) {
    
    shell_loop();

    return 0;
}



