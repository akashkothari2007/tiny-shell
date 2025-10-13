# Tiny Shell 
A simple command-line shell written in C for learning process management, memory handling, and system calls.

## Features (in progress)
- Read and execute user commands  
- Fork and execute child processes  
- Wait for child completion  
- Basic error handling  
- Dynamically allocated input buffer (`malloc` + `realloc`)

## Future Features (planned)
- Command history  
- Autocorrect / fuzzy command matching  
- Built-in commands (`cd`, `exit`, `help`)  
- Pipelining and redirection (`|`, `>` , `<`)  
- Colorized prompt 
-  

## Build & Run
```bash
# Compile
gcc src/main.c -o build/tiny-shell

# Run
./build/tiny-shell

## Repo Structure
src/        → source code
build/      → compiled outputs
README.md   → documentation