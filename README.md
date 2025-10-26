# 🐚 TinyShell — A Custom Lightweight Linux Shell

TinyShell is a **custom-built interactive shell** written in **C**, inspired by Stephen Brennan’s tutorial shell but expanded with a bunch of cool quality-of-life features and a fun personality.  
It’s designed to be simple, colorful, and surprisingly capable — a mini version of bash with custom behaviors and real-time input handling.

---

## ✨ Features

- **Live Input Handling:**  
  Raw terminal mode for real-time arrow key and tab handling.

- **Command History Navigation:**  
  Use ↑ and ↓ arrows to scroll through previous commands.

- **Autocorrect Suggestions:**  
  Levenshtein distance-based fuzzy matching for built-in commands.  
  → Mistype `hlp`? It’ll politely suggest “Did you mean help?”

- **Autocomplete (Tab):**  
  Press `Tab` to autocomplete directory names or list matching files.

- **Built-in Commands:**
  - `cd` — Change directory  
  - `help` — Display available built-in commands  
  - `exit` — Quit TinyShell  
  - `hi`, `hello`, `hola` — A little personality from your shell 😎  

- **Custom Colored Prompt:**  
  Display current working directory in green with a blue `TinyShell:` prefix.

- **Quoted Arguments:**  
  Supports multi-word strings in quotes (e.g., `"Hello World"`).

---

## 🧠 How It Works

TinyShell follows the same basic loop as a standard command-line interpreter (read → parse → execute → repeat):

1. **read_line()** — captures raw keyboard input (with live handling of special keys).  
2. **split_line()** — tokenizes input into arguments, respecting quotes and spaces.  
3. **shell_execute()** — checks for built-ins or forks a new process to run commands.  
4. **autocorrect()** — suggests corrections using the Levenshtein algorithm.  
5. **manageHistory()** — stores and navigates up to 1000 past commands.

---

## ⚙️ Compilation & Usage

gcc -o tinyshell main.c
./tinyshell

## Future Improvements
1. Piping (|) and redirection support
2. Background processes (&)
3. Command aliases and scripting support
4. Enhanced autocomplete and directory traversal