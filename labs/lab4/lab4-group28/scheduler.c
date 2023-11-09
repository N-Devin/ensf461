#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>  // Include the limits.h header for INT_MAX
#include <stdbool.h> // Include for boolean type
#include <sys/types.h>

// Define the struct job with start_time and end_time
struct job
{
    int id;
    int arrival;
    int length;
    int start_time;
    int end_time;
    struct job *next;
};

int total_job_count = 0;
struct job *head = NULL;

/* Function to append a new job to the list */
void append(int id, int arrival, int length)
{
    struct job *tmp = (struct job *)malloc(sizeof(struct job));

    tmp->id = id;
    tmp->length = length;
    tmp->arrival = arrival;
    tmp->next = NULL;

    if (head == NULL)
    {
        head = tmp;
        return;
    }

    struct job *prev = head;
    while (prev->next != NULL)
    {
        prev = prev->next;
    }
    prev->next = tmp;
}

void free_job_list(struct job *head)
{
    struct job *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void read_workload_file(char *filename)
{
    int id = 0;
    FILE *fp;
    size_t len = 0;
    ssize_t read;
    char *line = NULL,
         *arrival = NULL,
         *length = NULL;

    if ((fp = fopen(filename, "r")) == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) > 1)
    {
        arrival = strtok(line, ",\n");
        length = strtok(NULL, ",\n");

        assert(arrival != NULL && length != NULL);
        append(id++, atoi(arrival), atoi(length));
        total_job_count++;
    }

    fclose(fp);
    assert(id > 0);
}

void policy_FIFO(struct job *head)
{
    int current_time = 0;
    struct job *current = head;

    while (current != NULL)
    {
        if (current_time < current->arrival)
        {
            current_time = current->arrival;
        }
        current->start_time = current_time;
        current->end_time = current_time + current->length;

        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", current_time, current->id, current->arrival, current->length);
        current_time += current->length;
        current = current->next;
    }
}

void policy_SJF(struct job *head)
{
    int current_time = 0;
    struct job *current = NULL, *prev = NULL, *selected = NULL, *selected_prev = NULL;

    while (head != NULL)
    {
        int shortest_time = INT_MAX;
        current = head;
        prev = NULL;

        while (current != NULL)
        {
            if (current->arrival <= current_time && current->length < shortest_time)
            {
                shortest_time = current->length;
                selected = current;
                selected_prev = prev;
            }
            prev = current;
            current = current->next;
        }

        if (selected == NULL)
        {
            current_time++;
            continue;
        }

        selected->start_time = current_time;
        selected->end_time = current_time + selected->length;

        printf("Job %d started at %d and finished at %d\n", selected->id, selected->start_time, selected->end_time);
        current_time += selected->length;

        if (selected_prev == NULL)
        {
            head = selected->next;
        }
        else
        {
            selected_prev->next = selected->next;
        }
        selected = NULL;
    }
}

void analyze(struct job *head)
{
    int current_time = 0, total_wait_time = 0, total_turnaround_time = 0;
    struct job *current = head;

    printf("Begin analyzing:\n");

    while (current != NULL)
    {
        int wait_time = (current->start_time - current->arrival);
        int turnaround_time = current->end_time - current->arrival;

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", current->id, current->start_time - current->arrival, turnaround_time, wait_time);

        total_wait_time += wait_time;
        total_turnaround_time += turnaround_time;

        current_time += current->length;
        current = current->next;
    }

    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", (float)total_wait_time / (float)total_job_count, (float)total_turnaround_time / (float)total_job_count, (float)total_wait_time / (float)total_job_count);
}
struct scheduled_job
{
    int id;
    int start_time;
    int end_time;
    struct scheduled_job *next;
};
void schedule_SJF(struct job *head, struct scheduled_job **scheduled_head)
{
    int current_time = 0;
    struct job *current = head;

    while (current != NULL)
    {
        struct job *selected = NULL;
        int shortest_time = INT_MAX;

        // Find the job with the shortest remaining time
        struct job *temp = head;
        while (temp != NULL)
        {
            if (temp->arrival <= current_time && temp->length < shortest_time)
            {
                selected = temp;
                shortest_time = temp->length;
            }
            temp = temp->next;
        }

        if (selected == NULL)
        {
            current_time++;
            continue;
        }

        // Create a new scheduled_job node
        struct scheduled_job *scheduled = (struct scheduled_job *)malloc(sizeof(struct scheduled_job));
        scheduled->id = selected->id;
        scheduled->start_time = current_time;
        scheduled->end_time = current_time + selected->length;
        scheduled->next = *scheduled_head;
        *scheduled_head = scheduled;

        current_time += selected->length;
    }
}

int get_arrival_time(int id)
{
    struct job *current = head;
    while (current != NULL)
    {
        if (current->id == id)
        {
            return current->arrival;
        }
        current = current->next;
    }
    return -1;
}

void analyze_SJF(struct scheduled_job *scheduled_head)
{
    int total_wait_time = 0, total_turnaround_time = 0;
    int job_count = 0;
    struct scheduled_job *current = scheduled_head;

    while (current != NULL)
    {
        int wait_time = (current->start_time - get_arrival_time(current->id));
        int turnaround_time = current->end_time - get_arrival_time(current->id);

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", current->id, current->start_time - get_arrival_time(current->id), turnaround_time, wait_time);

        total_wait_time += wait_time;
        total_turnaround_time += turnaround_time;

        job_count++;
        current = current->next;
    }

    printf("Average -- Response %.2f  Turnaround %.2f  Wait %.2f\n", (float)total_wait_time / (float)job_count, (float)total_turnaround_time / (float)job_count, (float)total_wait_time / (float)job_count);
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "Missing variables\n");
        fprintf(stderr, "Usage: %s analysis-flag policy workload-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int analysis = atoi(argv[1]);
    char *policy = argv[2], *workload = argv[3];
    read_workload_file(workload);

    if (strcmp(policy, "FIFO") == 0)
    {

        if (analysis)
        {
            printf("Execution trace with FIFO:\n");
            policy_FIFO(head);
            printf("End of execution with FIFO.\n");
            printf("Begin analyzing FIFO:\n");
            analyze(head);
            printf("End analyzing FIFO.\n");
        }
        else
        {
            printf("Begin analyzing FIFO:\n");
            policy_FIFO(head);
            printf("End analyzing FIFO.\n");
        }
    }
    else if (strcmp(policy, "SJF") == 0)
    {

        if (analysis)
        {
            struct scheduled_job *scheduled_head = NULL;

            struct policy_result *result_head = NULL;
            printf("Execution trace with SJF:\n");
            schedule_SJF(head, &scheduled_head);
            printf("End of execution with SJF.\n");
            printf("Begin analyzing SJF:\n");
            analyze_SJF(scheduled_head);
            printf("End analyzing SJF.\n");
        }
        else
        {
            printf("Execution trace with SJF:\n");
            policy_SJF(head);
            printf("End of execution with SJF.\n");
        }
    }
    else
    {
        fprintf(stderr, "Unknown policy: %s\n", policy);
        exit(EXIT_FAILURE);
    }

    free_job_list(head);
    exit(EXIT_SUCCESS);
}
