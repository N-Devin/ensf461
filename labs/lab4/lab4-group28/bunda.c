#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

struct job
{
    int id;
    int arrival;
    int length;
    struct job *next;
};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length)
{
    // create a new struct and initialize it with the input data
    struct job *tmp = (struct job *)malloc(sizeof(struct job));

    // tmp->id = numofjobs++;
    tmp->id = id;
    tmp->length = length;
    tmp->arrival = arrival;

    // the new job is the last job
    tmp->next = NULL;

    // Case: job is first to be added, linked list is empty
    if (head == NULL)
    {
        head = tmp;
        return;
    }

    struct job *prev = head;

    // Find end of list
    while (prev->next != NULL)
    {
        prev = prev->next;
    }

    // Add job to end of list
    prev->next = tmp;
    return;
}

/*Function to read in the workload file and create job list*/
void read_workload_file(char *filename)
{
    int id = 0;
    FILE *fp;
    size_t len = 0;
    ssize_t read;
    char *line = NULL,
         *arrival = NULL,
         *length = NULL;

    struct job **head_ptr = malloc(sizeof(struct job *));

    if ((fp = fopen(filename, "r")) == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) > 1)
    {
        arrival = strtok(line, ",\n");
        length = strtok(NULL, ",\n");

        // Make sure neither arrival nor length are null.
        assert(arrival != NULL && length != NULL);

        append(id++, atoi(arrival), atoi(length));
    }

    fclose(fp);

    // Make sure we read in at least one job
    assert(id > 0);
    free(head_ptr);

    return;
}

void policy_FIFO(struct job *head)
{
    int currentTime = 0;
    int jobCount = 0;
    struct job *currentJob = head;

    printf("Execution trace with FIFO:\n");

    while (currentJob != NULL)
    {
        if (currentJob->arrival > currentTime)
        {
            currentTime = currentJob->arrival;
        }

        // Print job details
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", currentTime, currentJob->id, currentJob->arrival, currentJob->length);

        currentTime += currentJob->length;

        // Remove the completed job
        struct job *temp = currentJob;
        currentJob = currentJob->next;
        free(temp);

        jobCount++;
    }

    printf("End of execution with FIFO.\n");

    return;
}

void analyze_FIFO(struct job *head)
{
    int totalTurnaroundTime = 0;
    int jobCount = 0;

    struct job *currentJob = head;
    while (currentJob != NULL)
    {
        int turnaroundTime = currentJob->arrival - 0 + currentJob->length;
        totalTurnaroundTime += turnaroundTime;
        jobCount++;

        currentJob = currentJob->next;
    }

    if (jobCount > 0)
    {
        double averageTurnaroundTime = (double)totalTurnaroundTime / jobCount;
        printf("Average Turnaround Time: %.2f\n", averageTurnaroundTime);
    }
    else
    {
        printf("No jobs were executed.\n");
    }

    return;
}

void policy_SJF(struct job *head)
{
    int currentTime = 0;
    int jobCount = 0;

    printf("Execution trace with SJF:\n");

    while (head != NULL)
    {
        struct job *currentJob = head;
        struct job *shortestJob = NULL;
        struct job *prevShortestJob = NULL;
        int shortestTime = INT_MAX;

        while (currentJob != NULL)
        {
            if (currentJob->arrival <= currentTime && currentJob->length < shortestTime)
            {
                shortestTime = currentJob->length;
                shortestJob = currentJob;
                prevShortestJob = prevShortestJob;
            }
            currentJob = currentJob->next;
        }

        if (shortestJob == NULL)
        {
            // No job available at this time, wait for the next job to arrive
            currentTime++;
        }
        else
        {
            if (prevShortestJob == NULL)
            {
                head = shortestJob->next;
            }
            else
            {
                prevShortestJob->next = shortestJob->next;
            }

            // Print job details
            printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", currentTime, shortestJob->id, shortestJob->arrival, shortestJob->length);

            currentTime += shortestJob->length;

            // Remove the completed job
            free(shortestJob);
            jobCount++;
        }
    }

    printf("End of execution with SJF.\n");

    return;
}

void analyze_SJF(struct job *head)
{
    int totalResponseTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitTime = 0;
    int jobCount = 0;

    struct job *currentJob = head;
    int currentTime = 0;

    printf("Begin analyzing SJF:\n");

    while (currentJob != NULL)
    {
        int turnaroundTime = currentJob->arrival - 0 + currentJob->length;
        int responseTime = currentTime - currentJob->arrival;
        int waitTime = responseTime;

        totalResponseTime += responseTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitTime += waitTime;

        // Print job analysis details
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", currentJob->id, responseTime, turnaroundTime, waitTime);

        currentTime += currentJob->length;
        jobCount++;

        currentJob = currentJob->next;
    }

    if (jobCount > 0)
    {
        double averageResponseTime = (double)totalResponseTime / jobCount;
        double averageTurnaroundTime = (double)totalTurnaroundTime / jobCount;
        double averageWaitTime = (double)totalWaitTime / jobCount;

        printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", averageResponseTime, averageTurnaroundTime, averageWaitTime);
    }
    else
    {
        printf("No jobs were executed.\n");
    }

    printf("End analyzing SJF.\n");

    return;
}

int main(int argc, char **argv)
{

    if (argc < 4)
    {
        fprintf(stderr, "missing variables\n");
        fprintf(stderr, "usage: %s analysis-flag policy workload-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int analysis = atoi(argv[1]);
    char *policy = argv[2],
         *workload = argv[3];

    // Note: we use a global variable to point to
    // the start of a linked-list of jobs, i.e., the job list
    read_workload_file(workload);

    if (strcmp(policy, "FIFO") == 0)
    {
        policy_FIFO(head);
        if (analysis)
        {
            printf("Begin analyzing FIFO:\n");
            analyze_FIFO(head);
            printf("End analyzing FIFO.\n");
        }

        exit(EXIT_SUCCESS);
    }

    // TODO: Add other policies
    else if (strcmp(policy, "SJF") == 0)
    {
        policy_SJF(head);
        if (analysis)
        {
            printf("Begin analyzing SJF:\n");
            analyze_SJF(head);
            printf("End analyzing SJF.\n");
        }

        exit(EXIT_SUCCESS);
    }

    else
    {
        fprintf(stderr, "Invalid policy: %s\n", policy);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
