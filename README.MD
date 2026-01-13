# Pipex

A Unix pipe implementation in C that mimics shell pipe behavior, allowing the output of one command to become the input of another.

## 📖 Overview

Pipex is a 42 school project that recreates the shell's pipe mechanism (`|`). The program takes two commands and executes them sequentially, passing the output of the first command as input to the second command, similar to how pipes work in the shell.

**Shell equivalent:**
```bash
< infile cmd1 | cmd2 > outfile
```

**Pipex implementation:**
```bash
./pipex infile cmd1 cmd2 outfile
```

## 🎯 Project Purpose

This project teaches fundamental Unix concepts:
- Process creation with `fork()`
- Inter-process communication with `pipe()`
- File descriptor manipulation with `dup2()`
- Program execution with `execve()`
- Error handling in system calls

## 🔧 Installation

### Prerequisites
- GCC compiler
- Make
- Unix/Linux environment

### Build

```bash
# Clone the repository
git clone https://github.com/Motaz9999/Pipex.git
cd Pipex

# Compile the project
make

# Clean object files
make clean

# Clean everything
make fclean

# Recompile
make re
```

## 💻 Usage

### Basic Syntax

```bash
./pipex <infile> <cmd1> <cmd2> <outfile>
```

### Examples

**Example 1: Count lines**
```bash
./pipex input.txt "grep hello" "wc -l" output.txt
# Equivalent to: < input.txt grep hello | wc -l > output.txt
```

**Example 2: Sort and filter**
```bash
./pipex data.txt "cat" "sort" sorted.txt
# Equivalent to: < data.txt cat | sort > sorted.txt
```

**Example 3: Process text**
```bash
./pipex file.txt "grep error" "wc -w" result.txt
# Counts words containing "error" from file.txt
```

## 🏗️ Implementation Details

### Process Flow

```
Parent Process
    │
    ├─── fork() ──> Child 1 (Process 1)
    │                   │
    │                   ├─ Opens infile
    │                   ├─ Redirects stdin from infile (dup2)
    │                   ├─ Redirects stdout to pipe[1] (dup2)
    │                   ├─ Executes cmd1
    │                   └─ Writes output to pipe
    │
    └─── fork() ──> Child 2 (Process 2)
                        │
                        ├─ Opens outfile
                        ├─ Redirects stdin from pipe[0] (dup2)
                        ├─ Redirects stdout to outfile (dup2)
                        ├─ Executes cmd2
                        └─ Writes result to outfile
```

### Key Components

#### 1. Pipe Creation (`main.c`)
```c
int my_pipe[2];

if (pipe(my_pipe) == -1)
    error_handle("Pipe failed", NULL, 1);

// my_pipe[0] = read end
// my_pipe[1] = write end
```

#### 2. Process Management
- **Parent process**: Creates pipe, forks two children, waits for completion
- **Child 1 (process_1)**: Reads from infile, executes cmd1, writes to pipe
- **Child 2 (process_2)**: Reads from pipe, executes cmd2, writes to outfile

#### 3. Path Resolution (`find_path.c`)

The implementation handles three types of command paths:

**Absolute paths:** `/bin/ls`
```c
if (cmd[0] == '/')
    return path_rel_abs_not_name(cmd);
```

**Relative paths:** `./script.sh`
```c
if (cmd[0] == '.')
    return path_rel_abs_not_name(cmd);
```

**Command names:** `ls`, `grep`
```c
// Searches through PATH environment variable
char **envp_path = find_path(envp);
char *full_path = check_path(cmd, envp_path);
```

#### 4. I/O Redirection (`process.c`)

**Process 1 (First command):**
```c
// Open input file
int fd = open(argv[1], O_RDONLY);

// Redirect stdin from infile
dup2(fd, 0);

// Redirect stdout to pipe
dup2(pipex->my_pipe[1], 1);

// Close unused file descriptors
close(fd);
close(pipex->my_pipe[0]);
close(pipex->my_pipe[1]);

// Execute command
execve(path, cmd_args, envp);
```

**Process 2 (Second command):**
```c
// Open output file (create if doesn't exist, truncate if exists)
int fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);

// Redirect stdin from pipe
dup2(pipex->my_pipe[0], 0);

// Redirect stdout to outfile
dup2(fd, 1);

// Close unused file descriptors
close(fd);
close(pipex->my_pipe[0]);
close(pipex->my_pipe[1]);

// Execute command
execve(path, cmd_args, envp);
```

## 📁 Project Structure

```
Pipex/
├── main.c              # Entry point, pipe creation, process forking
├── process.c           # Process 1 and 2 implementations
├── find_path.c         # PATH resolution and command finding
├── pipex.h             # Header with structures and prototypes
├── libft/              # Custom C library
├── Makefile            # Build configuration
└── Pipex/              # Additional test files
```

## 🔍 Data Structures

### Main Pipex Structure
```c
typedef struct s_pipex
{
    pid_t   pid1;           // Process ID for child 1
    pid_t   pid2;           // Process ID for child 2
    int     my_pipe[2];     // Pipe file descriptors
} t_pipex;
```

### Process Data Structure
```c
typedef struct s_process_data
{
    int     fd;                     // File descriptor for input/output
    char    **cmd_have_flag;        // Command split into args
    char    *path_that_have_cmd;    // Full path to executable
    char    **envp_path;            // Array of PATH directories
    int     there_is_error;         // Error flag
    int     casee;                  // Which command (2 or 3)
} t_process;
```

### Path Finding Helper
```c
typedef struct s_names
{
    char    *full_path;     // Complete path being tested
    char    *cmd_slash;     // Command with leading slash
    int     i;              // Iterator for PATH directories
} t_names;
```

## ✅ Input Validation & Error Handling

### Argument Validation
```c
if (argc != 5)
    error_handle_custom("invalid num of argument", NULL, 1);
```

### File Access Errors
- **Infile doesn't exist:** Error printed, process exits with code 1
- **Outfile can't be created:** Error printed, process exits with code 1
- **Permission denied:** Handled with proper error messages

### Command Errors
- **Command not found:** Exit code 127 (standard shell behavior)
- **Command not executable:** Proper error message and exit
- **Invalid PATH:** Handled gracefully

### System Call Errors
- **pipe() fails:** Error message and exit
- **fork() fails:** Error message and exit
- **dup2() fails:** Error message and exit
- **execve() fails:** Error message and exit

### Error Message Examples
```bash
# Invalid number of arguments
$ ./pipex file.txt "cat"
ERROR : invalid num of argument

# File not found
$ ./pipex nonexistent.txt "cat" "wc -l" out.txt
ERROR : nonexistent.txt: No such file or directory

# Command not found
$ ./pipex input.txt "invalidcmd" "wc -l" output.txt
ERROR : command not found
```

## 🧪 Testing

### Basic Functionality Tests

```bash
# Test 1: Simple pipe
./pipex input.txt "cat" "wc -l" output.txt
cat input.txt | wc -l

# Test 2: Grep and count
./pipex file.txt "grep 42" "wc -l" result.txt
< file.txt grep 42 | wc -l > result.txt

# Test 3: Multiple flags
./pipex data.txt "ls -la" "grep test" output.txt
< data.txt ls -la | grep test > output.txt
```

### Edge Cases

```bash
# Empty file
./pipex empty.txt "cat" "wc -l" out.txt

# Large file
./pipex large.txt "cat" "wc -l" out.txt

# Special characters in filename
./pipex "file with spaces.txt" "cat" "wc -l" out.txt

# Command with multiple arguments
./pipex input.txt "grep -n hello" "wc -l" out.txt
```

### Error Testing

```bash
# Non-existent infile
./pipex nofile.txt "cat" "wc -l" output.txt

# Invalid command
./pipex input.txt "invalidcmd" "wc -l" output.txt

# Permission denied
chmod 000 restricted.txt
./pipex restricted.txt "cat" "wc -l" output.txt

# No arguments
./pipex

# Too few arguments
./pipex input.txt "cat" output.txt
```

### Comparison with Shell

```bash
# Create test file
echo -e "line1\nline2\nline3" > test.txt

# Run with pipex
./pipex test.txt "cat" "wc -l" out_pipex.txt

# Run with shell
< test.txt cat | wc -l > out_shell.txt

# Compare results
diff out_pipex.txt out_shell.txt
```

## 🔑 Key Implementation Features

### 1. PATH Environment Variable Parsing
- Extracts `PATH` from environment variables
- Splits by `:` delimiter into array of directories
- Searches each directory for executable

### 2. Command Path Resolution Priority
1. **Check if absolute path** (`/bin/ls`)
2. **Check if relative path** (`./script.sh`)
3. **Search in PATH directories** (`ls`, `grep`)

### 3. File Descriptor Management
- Proper closing of unused pipe ends
- No file descriptor leaks
- Clean resource management

### 4. Exit Code Handling
- Returns exit status of last command (cmd2)
- Properly uses `waitpid()` to get child status
- Uses `WIFEXITED()` and `WEXITSTATUS()` macros

### 5. Memory Management
- All allocated memory is freed
- `ft_split()` results are freed with `ft_free_all2()`
- Path strings are freed after use
- No memory leaks (verified with valgrind)

## 🎓 What I Learned

### System Calls Mastered
- `fork()` - Process creation
- `pipe()` - Inter-process communication
- `dup2()` - File descriptor duplication
- `execve()` - Program execution
- `waitpid()` - Process synchronization
- `access()` - File permission checking
- `open()` / `close()` - File operations

### Concepts Understood
- **Process forking**: Creating child processes
- **File descriptors**: Understanding stdin (0), stdout (1), stderr (2)
- **Pipes**: Creating communication channels between processes
- **I/O redirection**: Changing where input comes from and output goes to
- **Environment variables**: Accessing and parsing PATH
- **Exit codes**: Proper error code handling (127 for command not found)

### Best Practices Applied
- Comprehensive error handling for all system calls
- Proper resource cleanup (closing fds, freeing memory)
- Clear code structure with separate functions
- Meaningful variable and function names

## 👤 Author

**Motaz** (moodeh)
- GitHub: [@Motaz9999](https://github.com/Motaz9999)
- 42 Intra: moodeh

## 📚 Resources

- [Linux man pages](https://man7.org/linux/man-pages/)
  - `man 2 fork`
  - `man 2 pipe`
  - `man 2 dup2`
  - `man 2 execve`
  - `man 2 waitpid`
  - `man 2 access`
- [Beej's Guide to Unix IPC](https://beej.us/guide/bgipc/)
- [Advanced Programming in the UNIX Environment](https://www.apuebook.com/)
- also i use ai to understand deeply
---

**Note:** This is a 42 school project. Use this as a learning reference to understand Unix process management and IPC concepts.