#include <gtest/gtest.h>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

// ==========================================
//           INCLUDE C HEADERS
// ==========================================
extern "C" {
    // 1. Hack: Rename 'new' to avoid collision with C++ keyword
    #define new _new_var 
    #include "pipex.h"
    #undef new 

    // 2. DEFINE MISSING ERROR_HANDLE (Since main.c is excluded)
    // We replicate the logic but THROW an exception instead of exiting.
    void error_handle(char *ar, pipex_t *pipex1)
    {
        // Cleanup logic from your main.c
        if (pipex1 != NULL)
        {
            if (pipex1->my_pipe[0] > 0) close(pipex1->my_pipe[0]);
            if (pipex1->my_pipe[1] > 0) close(pipex1->my_pipe[1]);
        }
        // Throw exception to be caught by the test
        throw std::runtime_error("EXIT_CALLED");
    }

    // ==========================================
    //           THE MOCKS (INTERCEPTORS)
    // ==========================================

    // --- MALLOC WRAPPER ---
    extern void *__real_malloc(size_t size);
    int g_fail_malloc_at = -1;
    int g_malloc_counter = 0;

    void *__wrap_malloc(size_t size) {
        if (g_fail_malloc_at != -1 && g_malloc_counter++ == g_fail_malloc_at)
            return NULL;
        return __real_malloc(size);
    }

    // --- EXIT WRAPPER ---
    void __wrap_exit(int status) {
        throw std::runtime_error("EXIT_CALLED");
    }

    // --- EXECVE WRAPPER ---
    // If code reaches here, it succeeded!
    int __wrap_execve(const char *pathname, char *const argv[], char *const envp[]) {
        throw std::runtime_error("EXECVE_SUCCESS"); 
        return 0; 
    }

    // --- FORK WRAPPER ---
    // Always return 0 to run logic linearly (we call process_1/2 directly anyway)
    pid_t __wrap_fork(void) {
        return 0; 
    }
    
    // --- WAITPID WRAPPER ---
    // Just in case parent() logic is tested
    pid_t __wrap_waitpid(pid_t pid, int *status, int options) {
        if (status) *status = 0;
        return pid;
    }
}

// ==========================================
//           THE TORTURE TEST SUITE
// ==========================================

class PipexTorture : public ::testing::Test {
protected:
    int saved_stdin;
    int saved_stdout;

    void SetUp() override {
        // Save real STDIN/STDOUT because process_1/2 will mess them up with dup2
        saved_stdin = dup(STDIN_FILENO);
        saved_stdout = dup(STDOUT_FILENO);
        
        g_malloc_counter = 0;
        g_fail_malloc_at = -1;
    }

    void TearDown() override {
        // Restore STDIN/STDOUT so GTest can print results
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdin);
        close(saved_stdout);
    }
};

// -------------------------------------------------------------------------
// TEST 1: Torture Test Process 1 (The Input Handler)
// -------------------------------------------------------------------------
TEST_F(PipexTorture, Process1_MemoryCheck) {
    char *envp[] = { (char*)"PATH=/bin:/usr/bin", NULL };
    
    // Create dummy infile
    int tmp_fd = open("infile", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(tmp_fd);

    char *argv[] = { (char*)"./pipex", (char*)"infile", (char*)"ls", (char*)"wc", (char*)"outfile", NULL };
    pipex_t pipex_struct;

    // Run loop to fail malloc 0, 1, 2, ...
    for (int i = 0; i < 200; i++) {
        g_fail_malloc_at = i;
        g_malloc_counter = 0;

        // Reset Struct
        memset(&pipex_struct, 0, sizeof(pipex_struct));
        
        // Open fresh pipe for this iteration
        if (pipe(pipex_struct.my_pipe) == -1) break;

        // Restore FDs inside loop (because process_1 clobbers them every time)
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);

        try {
            process_1(&pipex_struct, argv, envp);
        } catch (const std::runtime_error& e) {
            std::string msg = e.what();
            // It is only allowed to fail via exit() or succeed via execve()
            if (msg != "EXIT_CALLED" && msg != "EXECVE_SUCCESS") {
                FAIL() << "Unexpected exception at malloc " << i << ": " << msg;
            }
        }

        // Cleanup pipe FDs if they weren't closed by error_handle/process_1
        // (We use fcntl to check if they are still open)
        if (fcntl(pipex_struct.my_pipe[0], F_GETFD) != -1) close(pipex_struct.my_pipe[0]);
        if (fcntl(pipex_struct.my_pipe[1], F_GETFD) != -1) close(pipex_struct.my_pipe[1]);
        
        // Break early if we didn't hit the malloc limit (optimization)
        if (g_malloc_counter < i) break; 
    }
    unlink("infile");
}

// -------------------------------------------------------------------------
// TEST 2: Torture Test Process 2 (The Output Handler)
// -------------------------------------------------------------------------
TEST_F(PipexTorture, Process2_MemoryCheck) {
    char *envp[] = { (char*)"PATH=/bin:/usr/bin", NULL };
    char *argv[] = { (char*)"./pipex", (char*)"infile", (char*)"ls", (char*)"wc", (char*)"outfile", NULL };
    pipex_t pipex_struct;

    for (int i = 0; i < 200; i++) {
        g_fail_malloc_at = i;
        g_malloc_counter = 0;

        memset(&pipex_struct, 0, sizeof(pipex_struct));
        if (pipe(pipex_struct.my_pipe) == -1) break;

        // Restore FDs
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);

        try {
            process_2(&pipex_struct, argv, envp);
        } catch (const std::runtime_error& e) {
            std::string msg = e.what();
            if (msg != "EXIT_CALLED" && msg != "EXECVE_SUCCESS") {
                FAIL() << "Unexpected exception at malloc " << i << ": " << msg;
            }
        }

        if (fcntl(pipex_struct.my_pipe[0], F_GETFD) != -1) close(pipex_struct.my_pipe[0]);
        if (fcntl(pipex_struct.my_pipe[1], F_GETFD) != -1) close(pipex_struct.my_pipe[1]);
        
        if (g_malloc_counter < i) break;
    }
    unlink("outfile");
}