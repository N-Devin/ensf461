#include <stddef.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include "myalloc.h"

static void *_arena_start = NULL;
static size_t _arena_size = 0;
int statusno = 0;
static node_t *_head = NULL;

int myinit(size_t size)
{
    if (size > MAX_ARENA_SIZE)
    {
        statusno = ERR_BAD_ARGUMENTS;
        return ERR_BAD_ARGUMENTS;
    }
    printf("Initializing arena:\n");
    printf("...requested size %lu bytes\n", size);
    long page_size = getpagesize();
    printf("...pagesize is %ld bytes\n", page_size);
    size_t adjusted_size = ((size + page_size - 1) / page_size) * page_size;
    printf("...adjusting size with page boundaries\n");
    printf("...adjusted size is %lu bytes\n", adjusted_size);

    _arena_start = mmap(NULL, adjusted_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (_arena_start == MAP_FAILED)
    {
        statusno = ERR_SYSCALL_FAILED;
        return ERR_SYSCALL_FAILED;
    }

    _arena_size = adjusted_size;
    _head = (node_t *)_arena_start;
    _head->size = adjusted_size - sizeof(node_t);
    _head->is_free = 1;
    _head->fwd = NULL;
    _head->bwd = NULL;

    printf("...mapping arena with mmap()\n");
    printf("...arena starts at %p\n", _arena_start);
    printf("...arena ends at %p\n", _arena_start + _arena_size);

    return adjusted_size;
}

int mydestroy()
{
    printf("Destroying Arena:\n");

    if (_arena_start != NULL)
    {
        munmap(_arena_start, _arena_size);
        _arena_start = NULL;
        _arena_size = 0;
        printf("...unmapping arena with munmap()\n");
        return 0; // Success
    }
    if (_arena_start == NULL)
    {
        printf("...arena was not initialized\n");
        return ERR_UNINITIALIZED;
    }
    return -1;
}

void *myalloc(size_t size)
{
    if (_arena_start == NULL)
    {
        statusno = ERR_UNINITIALIZED;
        return NULL;
    }
    if (size == 0 || size > MAX_ARENA_SIZE)
    {
        statusno = ERR_BAD_ARGUMENTS;
        return NULL;
    }

    node_t *current_chunk = _head;

    while (current_chunk)
    {
        if (current_chunk->is_free && current_chunk->size >= size)
        {
            if (current_chunk->size >= size + sizeof(node_t))
            {
                node_t *new_chunk = (node_t *)((char *)current_chunk + sizeof(node_t) + size);
                new_chunk->size = current_chunk->size - size - sizeof(node_t);
                new_chunk->is_free = 1;
                new_chunk->fwd = current_chunk->fwd;
                new_chunk->bwd = current_chunk;
                if (current_chunk->fwd)
                {
                    current_chunk->fwd->bwd = new_chunk;
                }
                current_chunk->fwd = new_chunk;
                current_chunk->size = size;
            }

            current_chunk->is_free = 0;
            void *user_ptr = (void *)((char *)current_chunk + sizeof(node_t));
            return user_ptr;
        }

        current_chunk = current_chunk->fwd;
    }

    statusno = ERR_OUT_OF_MEMORY;
    return NULL;
}

void myfree(void *ptr)
{
    if (_arena_start == NULL || ptr == NULL)
    {

        return;
    }

    node_t *header = (node_t *)((char *)ptr - sizeof(node_t));
    header->is_free = 1;

    node_t *prev = header->bwd;
    node_t *next = header->fwd;

    if (prev && prev->is_free)
    {
        prev->size += sizeof(node_t) + header->size;
        prev->fwd = next;
        if (next)
        {
            next->bwd = prev;
        }
        header = prev;
    }

    if (next && next->is_free)
    {
        header->size += sizeof(node_t) + next->size;
        header->fwd = next->fwd;
        if (next->fwd)
        {
            next->fwd->bwd = header;
        }
    }
}
