# 🔗 pipex - Unix Pipes Implementation

<div align="center">

![42 School Badge](https://img.shields.io/badge/42-pipex-blue)
![Language](https://img.shields.io/badge/language-C-orange)
![Grade](https://img.shields.io/badge/grade-100%2F100-brightgreen)
![System](https://img.shields.io/badge/system-Unix-lightgrey)

</div>

## 📋 About The Project

**pipex** is a project from the 42 school curriculum that teaches you about **Unix pipes** and **process management** in C. You'll recreate the shell's pipe mechanism (`|`) that allows the output of one command to become the input of another.

This project introduces fundamental Unix concepts:
- Process creation with `fork()`
- Inter-process communication with `pipe()`
- File descriptor manipulation with `dup2()`
- Program execution with `execve()`

## 🎯 Project Purpose

The purpose of pipex is to:
- Replicate the behavior of shell pipes in C
- Understand how processes communicate
- Learn about file descriptors and I/O redirection
- Master process creation and synchronization
- Prepare for more complex projects like **minishell**

## 📝 What I Learned

Through this project, I gained expertise in:

### Core Unix Concepts
- **Processes**: Creating child processes with `fork()`
- **Pipes**: Inter-process communication mechanisms
- **File Descriptors**: Understanding fd 0 (stdin), 1 (stdout), 2 (stderr)
- **I/O Redirection**: Redirecting input/output with `dup2()`
- **Process Execution**: Replacing process image with `execve()`

### System Calls
- `fork()` - Create new process
- `pipe()` - Create pipe for IPC
- `dup2()` - Duplicate file descriptors
- `execve()` - Execute program
- `wait()` / `waitpid()` - Wait for child processes
- `access()` - Check file permissions
- `open()` / `close()` - File operations

### Advanced Topics
- **Process Tree**: Parent-child relationships
- **Zombie Processes**: Understanding and preventing them
- **File Permissions**: Checking and handling file access
- **PATH Variable**: Finding executables in system paths
- **Error Handling**: Proper error management in system calls

## 🎮 How It Works

### Shell Equivalent

The program mimics this shell behavior:

```bash
< infile cmd1 | cmd2 > outfile
```

### Your Implementation

```bash
./pipex infile "cmd1" "cmd2" outfile
```

### Example

**Shell command:**
```bash
< input.txt grep "hello" | wc -l > output.txt
```

**pipex equivalent:**
```bash
./pipex input.txt "grep hello" "wc -l" output.txt
```

Both produce the same result: count lines containing "hello" in input.txt and write the count to output.txt.

## 🚀 How to Use

### Compilation

```bash
git clone https://github.com/Motaz9999/Pipex.git
cd Pipex
make
```

This creates the `pipex` executable.

### Make Commands

```bash
make        # Compile pipex
make bonus  # Compile bonus (multiple pipes, here_doc)
make clean  # Remove object files
make fclean # Remove object files and executables
make re     # Recompile everything
```

### Basic Usage

```bash
./pipex <infile> <cmd1> <cmd2> <outfile>
```

### Examples

**Example 1: Word count**
```bash
./pipex input.txt "cat" "wc -w" output.txt
# Equivalent to: < input.txt cat | wc -w > output.txt
```

**Example 2: Grep and sort**
```bash
./pipex file.txt "grep error" "sort" errors.txt
# Equivalent to: < file.txt grep error | sort > errors.txt
```

**Example 3: Multiple transformations**
```bash
./pipex data.txt "grep 42" "wc -l" result.txt
# Counts lines containing "42"
```

## 🎁 Bonus Features

### Multiple Pipes

Handle unlimited number of commands:

```bash
./pipex infile "cmd1" "cmd2" "cmd3" "cmd4" outfile
# Equivalent to: < infile cmd1 | cmd2 | cmd3 | cmd4 > outfile
```

**Example:**
```bash
./pipex input.txt "cat" "grep a" "wc -l" output.txt
```

### Here_doc

Support for here_doc (`<<`) with append mode (`>>`):

```bash
./pipex here_doc LIMITER "cmd1" "cmd2" outfile
# Equivalent to: << LIMITER cmd1 | cmd2 >> outfile
```

**Example:**
```bash
./pipex here_doc EOF "grep hello" "wc -l" output.txt
```

Then type your input:
```
hello world
this is a test
hello 42
EOF
```

The commands process what you typed and **append** to output.txt.

## 🔧 Implementation Details

### The Process Flow

```
Parent Process
    │
    ├─── fork() ───> Child 1 (executes cmd1)
    │                   │
    │                   └── reads from infile
    │                   └── writes to pipe
    │
    └─── fork() ───> Child 2 (executes cmd2)
                        │
                        └── reads from pipe
                        └── writes to outfile
```

### Key Functions

#### 1. Creating a Pipe

```c
int pipe_fd[2];

// pipe_fd[0] = read end
// pipe_fd[1] = write end

if (pipe(pipe_fd) == -1)
    error_exit("Pipe failed");
```

#### 2. Forking Processes

```c
pid_t pid;

pid = fork();
if (pid == -1)
    error_exit("Fork failed");
    
if (pid == 0)
{
    // Child process
    execute_command();
}
else
{
    // Parent process
    wait(&pid);
}
```

#### 3. Redirecting I/O

```c
// Redirect stdin from file
int fd_in = open("input.txt", O_RDONLY);
dup2(fd_in, STDIN_FILENO);
close(fd_in);

// Redirect stdout to pipe
dup2(pipe_fd[1], STDOUT_FILENO);
close(pipe_fd[0]);
close(pipe_fd[1]);

// Redirect stdin from pipe  
dup2(pipe_fd[0], STDIN_FILENO);
close(pipe_fd[0]);
close(pipe_fd[1]);

// Redirect stdout to file
int fd_out = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd_out, STDOUT_FILENO);
close(fd_out);
```

#### 4. Executing Commands

```c
// Find command in PATH
char *cmd_path = find_command_path(cmd, envp);

// Parse command and arguments
char **args = ft_split(cmd, ' ');

// Execute
execve(cmd_path, args, envp);

// If execve returns, there was an error
perror("Command execution failed");
exit(127);
```

### Finding Commands in PATH

```c
char *find_command_path(char *cmd, char **envp)
{
    char **paths;
    char *path;
    char *full_path;
    int i;
    
    // Check if cmd is already a path
    if (access(cmd, F_OK | X_OK) == 0)
        return (ft_strdup(cmd));
    
    // Get PATH environment variable
    path = get_env_value("PATH", envp);
    if (!path)
        return (NULL);
        
    // Split PATH by ':'
    paths = ft_split(path, ':');
    
    // Try each directory
    i = 0;
    while (paths[i])
    {
        full_path = create_path(paths[i], cmd);
        if (access(full_path, F_OK | X_OK) == 0)
        {
            free_split(paths);
            return (full_path);
        }
        free(full_path);
        i++;
    }
    
    free_split(paths);
    return (NULL);
}
```

## 📁 Project Structure

```
pipex/
├── Makefile
├── pipex.h              # Header file
├── pipex.c              # Main program
├── pipex_utils.c        # Helper functions
├── path_utils.c         # PATH handling
├── process.c            # Process management
├── error.c              # Error handling
├── pipex_bonus.c        # Bonus: multiple pipes
├── here_doc_bonus.c     # Bonus: here_doc
└── README.md
```

## 🧪 Testing

### Basic Tests

```bash
# Test 1: Simple pipe
./pipex input.txt "cat" "wc -l" output.txt
cat input.txt | wc -l

# Test 2: Grep and sort
./pipex file.txt "grep 42" "sort" result.txt
< file.txt grep 42 | sort > result.txt

# Test 3: Complex command
./pipex input.txt "grep a" "wc -l" output.txt
< input.txt grep a | wc -l > output.txt
```

### Error Cases

```bash
# Non-existent infile
./pipex nofile.txt "cat" "wc -l" output.txt

# Invalid command
./pipex input.txt "invalidcmd" "wc -l" output.txt

# Permission denied
./pipex /etc/shadow "cat" "wc -l" output.txt

# No arguments
./pipex
```

### Bonus Tests

```bash
# Multiple pipes
./pipex infile "cat" "grep a" "wc -l" outfile

# Here_doc
./pipex here_doc EOF "grep hello" "wc -w" output.txt
```

### Memory Leak Testing

```bash
valgrind --leak-check=full --show-leak-kinds=all ./pipex input.txt "cat" "wc -l" output.txt
```

### File Descriptor Leak Check

```bash
valgrind --track-fds=yes ./pipex input.txt "cat" "wc -l" output.txt
```

## 💡 Key Insights

### 1. Understanding File Descriptors

```
0 = STDIN_FILENO   (standard input)
1 = STDOUT_FILENO  (standard output)
2 = STDERR_FILENO  (standard error)
```

Every open file gets a fd ≥ 3.

### 2. The Power of dup2()

`dup2(oldfd, newfd)` makes `newfd` a copy of `oldfd`:

```c
dup2(file_fd, STDIN_FILENO);  // Now reading from stdin reads from file
dup2(pipe_fd[1], STDOUT_FILENO); // Now writing to stdout writes to pipe
```

### 3. Fork Creates a Copy

After `fork()`:
- Child gets a copy of all file descriptors
- Changes in child don't affect parent
- Both processes run simultaneously

### 4. Close What You Don't Need

Always close unused pipe ends:
```c
// In child 1 (writer)
close(pipe_fd[0]);  // Don't need read end

// In child 2 (reader)  
close(pipe_fd[1]);  // Don't need write end

// In parent
close(pipe_fd[0]);  // Don't need either end
close(pipe_fd[1]);
```

### 5. Wait for Children

Parent must wait for children to prevent zombie processes:
```c
waitpid(pid1, NULL, 0);
waitpid(pid2, NULL, 0);
```

## ⚠️ Common Pitfalls

1. **Forgetting to close file descriptors**
   - Causes fd leaks
   - Can prevent EOF from being sent

2. **Not waiting for child processes**
   - Creates zombie processes
   - Parent may exit before children finish

3. **Wrong order of operations**
   - Redirect before executing
   - Close after duplicating

4. **Not checking return values**
   - fork() can fail
   - execve() only returns on error
   - pipe() can fail

5. **PATH handling**
   - Command might not be in PATH
   - Need to check access permissions

## 🎓 What This Project Teaches

- **Process Management**: How Unix handles multiple processes
- **IPC**: Inter-process communication fundamentals
- **I/O Redirection**: How shells redirect input/output
- **System Programming**: Low-level Unix programming
- **Resource Management**: Properly closing fds and waiting for processes

## 📚 Resources

- [Linux man pages](https://man7.org/linux/man-pages/)
  - `man fork`
  - `man pipe`
  - `man dup2`
  - `man execve`
  - `man waitpid`
- [Unix Pipes Tutorial](https://www.geeksforgeeks.org/pipe-system-call/)
- [Process Creation](https://www.cs.odu.edu/~cs471w/spring11/lectures/fork-exec.htm)

## 👤 Author

**Motaz**
- GitHub: [@Motaz9999](https://github.com/Motaz9999)
- Intra: @moodeh

---

<div align="center">

**⭐ Connecting processes, one pipe at a time**

Made at 42 School

</div>
