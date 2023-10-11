#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFLEN 1024

// Function to search for a command in the directories listed in the PATH variable
char *findCommandInPath(const char *cmd)
{
    char *path = getenv("PATH");
    if (path == NULL)
    {
        return NULL;
    }

    char *path_copy = strdup(path);
    if (path_copy == NULL)
    {
        perror("Error duplicating PATH");
        return NULL;
    }

    char *token = strtok(path_copy, ":");
    while (token != NULL)
    {
        // Construct the full path to the command
        char full_path[BUFLEN];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);

        // Check if the command exists and is executable
        if (access(full_path, X_OK) == 0)
        {
            free(path_copy);
            return strdup(full_path);
        }

        token = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

int main()
{
    char buffer[BUFLEN];

    printf("Welcome to the group 28 Shell! Enter commands, enter 'quit' to exit\n");

    do
    {
        // Print the terminal prompt and get input
        printf("$ ");
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input)
        {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }

        // Remove newline character if present
        size_t input_length = strlen(input);
        if (input_length > 0 && input[input_length - 1] == '\n')
        {
            input[input_length - 1] = '\0';
        }

        // Tokenize the input using spaces as delimiters
        char *token;
        char *tokens[BUFLEN];
        int token_count = 0;

        token = strtok(input, " ");
        while (token != NULL)
        {
            tokens[token_count++] = token;
            token = strtok(NULL, " ");
        }

        if (token_count == 0)
        {
            // No command entered, continue to the next iteration
            continue;
        }

        // Handle 'quit' command to exit the shell
        if (strcmp(tokens[0], "quit") == 0)
        {
            printf("Bye!!\n");
            break;
        }
        for (int i = 0; i < token_count; i++)
        {
            printf("Token %d: %s\n", i, tokens[i]);
        }

        // Check if the command is an absolute path or in the current directory
        char *cmd = tokens[0];
        char cmd_path[BUFLEN];

        if (cmd[0] == '/')
        {
            // Absolute path, use it as is
            snprintf(cmd_path, sizeof(cmd_path), "%s", cmd);
        }
        else if (strchr(cmd, '/') != NULL)
        {
            // Relative path, use it in the current working directory
            snprintf(cmd_path, sizeof(cmd_path), "./%s", cmd);
        }
        else
        {
            // Search for the command in the directories listed in the PATH
            char *found_cmd_path = findCommandInPath(cmd);
            if (found_cmd_path == NULL)
            {
                printf("Command not found: %s\n", cmd);
                continue;
            }
            snprintf(cmd_path, sizeof(cmd_path), "%s", found_cmd_path);
            free(found_cmd_path);
        }

        // Create a child process to execute the command
        pid_t fork_pid = fork();

        if (fork_pid == 0)
        {
            // Child process
            char **cmd_args = &tokens[0];

            if (execv(cmd_path, cmd_args) == -1)
            {
                perror("Error executing command");
                exit(EXIT_FAILURE);
            }
        }
        else if (fork_pid < 0)
        {
            perror("Error forking");
        }
        else
        {
            // Parent process
            wait(NULL);
        }
    } while (1);

    return 0;
}
