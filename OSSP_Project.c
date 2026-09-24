#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <dirent.h>
#include <pwd.h>

/* =========================================================
   GLOBAL VARIABLES
   ========================================================= */

int counter = 0;
pthread_mutex_t mutex;

/* =========================================================
   UTILITY FUNCTIONS
   ========================================================= */

void press_enter()
{
    printf("\nPress Enter to continue...");
    getchar();
}

void clear_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* =========================================================
   SIGNAL HANDLER
   ========================================================= */

void signal_handler(int signal_number)
{
    printf("\n\n✓ SIGUSR1 signal received successfully!\n");
    printf("✓ Signal handler executed.\n");
}

/* =========================================================
   1. SYSTEM OVERVIEW
   ========================================================= */

void system_overview()
{
    struct utsname system_info;
    char hostname[100];

    printf("\n");
    printf("============================================\n");
    printf("              SYSTEM OVERVIEW\n");
    printf("============================================\n");

    if (uname(&system_info) == 0)
    {
        printf("Operating System : %s\n", system_info.sysname);
        printf("Kernel Version   : %s\n", system_info.release);
        printf("Architecture     : %s\n", system_info.machine);
        printf("System Version   : %s\n", system_info.version);
    }

    if (gethostname(hostname, sizeof(hostname)) == 0)
        printf("Hostname         : %s\n", hostname);

    printf("Current User     : %s\n", getlogin());
    printf("Current Process  : %d\n", getpid());

    printf("--------------------------------------------\n");
    printf("✓ System information retrieved successfully.\n");
}

/* =========================================================
   PROCESS COMMAND EXECUTION
   ========================================================= */

void execute_process_command(int choice)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        switch (choice)
        {
            case 1:
                execlp("ps", "ps", NULL);
                break;

            case 2:
                execlp("ps", "ps", "-ef", NULL);
                break;

            case 3:
                execlp("ps", "ps", "-u", getlogin(), NULL);
                break;

            default:
                exit(1);
        }

        perror("Command execution failed");
        exit(1);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}

/* =========================================================
   2. PROCESS MANAGER
   ========================================================= */

void process_manager()
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("               PROCESS MANAGER\n");
        printf("============================================\n");
        printf("1. View Running Processes\n");
        printf("2. View All Processes\n");
        printf("3. View My Processes\n");
        printf("4. View Current Process ID\n");
        printf("5. Back to Main Menu\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clear_input();

        if (choice == 5)
            return;

        switch (choice)
        {
            case 1:
                printf("\nRunning Processes:\n\n");
                execute_process_command(1);
                break;

            case 2:
                printf("\nAll Processes:\n\n");
                execute_process_command(2);
                break;

            case 3:
                printf("\nYour Processes:\n\n");
                execute_process_command(3);
                break;

            case 4:
                printf("\nCurrent Process ID: %d\n", getpid());
                printf("Parent Process ID : %d\n", getppid());
                break;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }
}

/* =========================================================
   3. MEMORY MONITOR
   ========================================================= */

void memory_monitor()
{
    FILE *file;
    char line[256];

    printf("\n");
    printf("============================================\n");
    printf("                MEMORY MONITOR\n");
    printf("============================================\n");

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        perror("Unable to access memory information");
        return;
    }

    printf("\nSystem Memory Information:\n\n");

    int count = 0;

    while (fgets(line, sizeof(line), file) && count < 6)
    {
        printf("%s", line);
        count++;
    }

    fclose(file);

    printf("\n--------------------------------------------\n");

    file = fopen("/proc/self/status", "r");

    if (file != NULL)
    {
        while (fgets(line, sizeof(line), file))
        {
            if (strncmp(line, "VmSize:", 7) == 0 ||
                strncmp(line, "VmRSS:", 6) == 0)
            {
                printf("%s", line);
            }
        }

        fclose(file);
    }

    /* Demonstrate mmap */

    void *memory = mmap(
        NULL,
        4096,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (memory != MAP_FAILED)
    {
        printf("\n✓ Memory block allocated using mmap().\n");

        strcpy((char *)memory, "SysGuard Memory Test");

        printf("✓ Memory test: %s\n", (char *)memory);

        munmap(memory, 4096);

        printf("✓ Memory released using munmap().\n");
    }
}

/* =========================================================
   4. FILE EXPLORER
   ========================================================= */

void file_explorer()
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("                 FILE EXPLORER\n");
        printf("============================================\n");
        printf("Current Directory: ");

        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s\n", cwd);

        printf("\n");
        printf("1. View Files\n");
        printf("2. View Detailed File Information\n");
        printf("3. Create a File\n");
        printf("4. Write to a File\n");
        printf("5. Read a File\n");
        printf("6. Back to Main Menu\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clear_input();

        if (choice == 6)
            return;

        if (choice == 1)
        {
            DIR *directory;
            struct dirent *entry;

            directory = opendir(".");

            if (directory == NULL)
            {
                perror("Unable to open directory");
                continue;
            }

            printf("\nFiles and Folders:\n\n");

            while ((entry = readdir(directory)) != NULL)
            {
                if (entry->d_name[0] != '.')
                    printf("  %s\n", entry->d_name);
            }

            closedir(directory);
        }

        else if (choice == 2)
        {
            char filename[100];

            printf("Enter file name: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';

            struct stat file_info;

            if (stat(filename, &file_info) == 0)
            {
                printf("\nFile: %s\n", filename);
                printf("Size: %ld bytes\n", file_info.st_size);
                printf("Permissions: %o\n",
                       file_info.st_mode & 0777);
            }
            else
            {
                perror("File information");
            }
        }

        else if (choice == 3)
        {
            char filename[100];

            printf("Enter new file name: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';

            int fd = open(
                filename,
                O_WRONLY | O_CREAT | O_EXCL,
                0644
            );

            if (fd < 0)
            {
                perror("Unable to create file");
            }
            else
            {
                close(fd);
                printf("✓ File created successfully.\n");
            }
        }

        else if (choice == 4)
        {
            char filename[100];
            char message[500];

            printf("Enter file name: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';

            printf("Enter text: ");
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0';

            int fd = open(
                filename,
                O_WRONLY | O_APPEND
            );

            if (fd < 0)
            {
                perror("Unable to open file");
            }
            else
            {
                write(fd, message, strlen(message));
                write(fd, "\n", 1);

                close(fd);

                printf("✓ Data written successfully.\n");
            }
        }

        else if (choice == 5)
        {
            char filename[100];
            char buffer[501];

            printf("Enter file name: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';

            int fd = open(filename, O_RDONLY);

            if (fd < 0)
            {
                perror("Unable to open file");
            }
            else
            {
                int bytes;

                printf("\n========== FILE CONTENT ==========\n");

                while ((bytes = read(fd, buffer, 500)) > 0)
                {
                    buffer[bytes] = '\0';
                    printf("%s", buffer);
                }

                close(fd);

                printf("\n==================================\n");
            }
        }

        else
        {
            printf("Invalid choice.\n");
        }
    }
}

/* =========================================================
   5. IPC CENTER
   ========================================================= */

void ipc_center()
{
    int pipe_fd[2];
    char message[500];
    char received_message[500];

    printf("\n");
    printf("============================================\n");
    printf("                  IPC CENTER\n");
    printf("============================================\n");

    printf("\nEnter message to send to child process: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        close(pipe_fd[1]);

        read(pipe_fd[0],
             received_message,
             sizeof(received_message));

        printf("\n--------------------------------------------\n");
        printf("Child Process received:\n");
        printf("\"%s\"\n", received_message);
        printf("--------------------------------------------\n");

        close(pipe_fd[0]);

        exit(0);
    }
    else
    {
        close(pipe_fd[0]);

        write(
            pipe_fd[1],
            message,
            strlen(message) + 1
        );

        close(pipe_fd[1]);

        waitpid(pid, NULL, 0);

        printf("\n✓ IPC communication completed successfully.\n");
    }
}

/* =========================================================
   6. SIGNAL MONITOR
   ========================================================= */

void signal_monitor()
{
    signal(SIGUSR1, signal_handler);

    printf("\n");
    printf("============================================\n");
    printf("                SIGNAL MONITOR\n");
    printf("============================================\n");

    printf("\nCurrent Process ID: %d\n", getpid());

    printf("\n1. Send Test Signal\n");
    printf("2. Back\n");

    int choice;

    printf("\nEnter your choice: ");
    scanf("%d", &choice);
    clear_input();

    if (choice == 1)
    {
        printf("\nSending SIGUSR1...\n");

        kill(getpid(), SIGUSR1);
    }
}

/* =========================================================
   THREAD FUNCTION
   ========================================================= */

void *thread_function(void *argument)
{
    pthread_mutex_lock(&mutex);

    counter++;

    printf("Thread executed successfully.\n");
    printf("Shared Counter = %d\n", counter);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

/* =========================================================
   7. THREAD MONITOR
   ========================================================= */

void thread_monitor()
{
    pthread_t thread1;
    pthread_t thread2;

    counter = 0;

    printf("\n");
    printf("============================================\n");
    printf("               THREAD MONITOR\n");
    printf("============================================\n");

    pthread_mutex_init(&mutex, NULL);

    printf("\nCreating two threads...\n");

    pthread_create(
        &thread1,
        NULL,
        thread_function,
        NULL
    );

    pthread_create(
        &thread2,
        NULL,
        thread_function,
        NULL
    );

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutex);

    printf("\n--------------------------------------------\n");
    printf("✓ Threads completed successfully.\n");
    printf("✓ Mutex synchronization used.\n");
    printf("✓ Final Shared Counter: %d\n", counter);
}

/* =========================================================
   8. SYSTEM LOGS
   ========================================================= */

void add_log(const char *message)
{
    int fd = open(
        "sysguard_log.txt",
        O_WRONLY | O_CREAT | O_APPEND,
        0644
    );

    if (fd >= 0)
    {
        write(fd, message, strlen(message));
        write(fd, "\n", 1);
        close(fd);
    }
}

void system_logs()
{
    FILE *file;
    char line[500];

    printf("\n");
    printf("============================================\n");
    printf("                 SYSTEM LOGS\n");
    printf("============================================\n");

    file = fopen("sysguard_log.txt", "r");

    if (file == NULL)
    {
        printf("\nNo logs available yet.\n");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }

    fclose(file);
}

/* =========================================================
   MAIN MENU
   ========================================================= */

int main()
{
    int choice;

    /* Register signal handler */
    signal(SIGUSR1, signal_handler);

    add_log("SysGuard started.");

    while (1)
    {
        printf("\n\n");
        printf("╔══════════════════════════════════════════╗\n");
        printf("║                 SYSGUARD                 ║\n");
        printf("║     Linux System Monitoring System       ║\n");
        printf("╠══════════════════════════════════════════╣\n");
        printf("║                                          ║\n");
        printf("║  1. System Overview                     ║\n");
        printf("║  2. Process Manager                     ║\n");
        printf("║  3. Memory Monitor                      ║\n");
        printf("║  4. File Explorer                       ║\n");
        printf("║  5. IPC Center                          ║\n");
        printf("║  6. Signal Monitor                      ║\n");
        printf("║  7. Thread Monitor                      ║\n");
        printf("║  8. System Logs                         ║\n");
        printf("║  9. Exit                                ║\n");
        printf("║                                          ║\n");
        printf("╚══════════════════════════════════════════╝\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clear_input();

        switch (choice)
        {
            case 1:
                system_overview();
                add_log("System overview accessed.");
                break;

            case 2:
                process_manager();
                add_log("Process manager accessed.");
                break;

            case 3:
                memory_monitor();
                add_log("Memory monitor accessed.");
                break;

            case 4:
                file_explorer();
                add_log("File explorer accessed.");
                break;

            case 5:
                ipc_center();
                add_log("IPC communication performed.");
                break;

            case 6:
                signal_monitor();
                add_log("Signal monitor accessed.");
                break;

            case 7:
                thread_monitor();
                add_log("Thread demonstration completed.");
                break;

            case 8:
                system_logs();
                break;

            case 9:
                add_log("SysGuard closed.");
                printf("\nThank you for using SysGuard!\n");
                return 0;

            default:
                printf("\nInvalid choice. Please select 1-9.\n");
        }

        printf("\n");
        press_enter();
    }

    return 0;
}