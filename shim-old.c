#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 256

typedef struct {
	void* pointer;
	size_t size;
} memory_t;
memory_t memory[BUF_SIZE];

typedef void* (*orig_malloc_t)(size_t);
typedef void* (*orig_calloc_t)(size_t, size_t);
typedef void* (*orig_realloc_t)(void*, size_t);
typedef void (*orig_free_t)(void*);

static unsigned int total = 0;
static unsigned int idx = 0;
static unsigned int allocs = 0;
static unsigned int frees = 0;

void print_log(const char* buf) {
	char* logmsg = "[LOG] ";
	write(1, logmsg, strlen(logmsg) + 1);
	write(1, buf, strlen(buf) + 1);
}

void print_memory_status() {
	char buffer[BUF_SIZE];
	print_log("Status:\n");
	for (int i = 0; i < idx; i++) {
		if (memory[i].pointer == NULL) continue;
		snprintf(buffer, BUF_SIZE, " -> %p of size %zu\n", memory[i].pointer, memory[i].size);
		print_log(buffer);
	}
	snprintf(buffer, BUF_SIZE, "--Total %u bytes of memory allocated--\n", total);
	print_log(buffer);
}

__attribute__((destructor)) void exit_log() {
	char buffer[BUF_SIZE];
	print_log("<<<<<<< Heap Summary >>>>>>>\n");
	int blocks_leaked = allocs - frees;
	snprintf(buffer, BUF_SIZE, "Total bytes of memory leaked: %u bytes\n", total);
	print_log(buffer);
	snprintf(buffer, BUF_SIZE, "   => Total allocs: %u\n", allocs);
	print_log(buffer);
	snprintf(buffer, BUF_SIZE, "   => Total frees: %u\n", frees);
	print_log(buffer);
	snprintf(buffer, BUF_SIZE, "   => Total blocks of memory leaked: %d\n", blocks_leaked);
	print_log(buffer);
	if (blocks_leaked > 0) print_memory_status();
}

int find_ptr(void* ptr) {
	int loc = -1;
	for (int i = 0; i < idx; i++) {
		if (memory[i].pointer == NULL)
			continue;
		else if (memory[i].pointer == ptr) {
			loc = i;
			break;
		}
	}
	return loc;
}

void* malloc(size_t size) {
	char buffer[BUF_SIZE];
	orig_malloc_t real_malloc = NULL;
	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (real_malloc == NULL) {
		write(1, "dlsym failed!\n", 15);
		return NULL;
	}
	void* ptr = real_malloc(size);
	memory[idx].pointer = ptr;
	memory[idx].size = size;
	idx++;
	allocs++;
	total += size;
	snprintf(buffer, BUF_SIZE, "Malloc: Allocate %zu bytes of memory at %p\n", size, ptr);
	print_log(buffer);
	return ptr;
}

void* calloc(size_t n, size_t size) {
	char buffer[BUF_SIZE];
	orig_calloc_t real_calloc = NULL;
	if ((real_calloc = dlsym(RTLD_NEXT, "calloc")) == NULL) {
		perror("dlsym");
		return NULL;
	}
	size_t memsize = n * size;
	void* ptr = real_calloc(n, size);
	memory[idx].pointer = ptr;
	memory[idx].size = memsize;
	idx++;
	allocs++;
	total += memsize;
	snprintf(buffer, BUF_SIZE, "Calloc: Allocate %zu bytes of memory at %p\n", memsize, ptr);
	print_log(buffer);
	return ptr;
}

void* realloc(void* ptr, size_t size) {
	char buffer[BUF_SIZE];
	orig_realloc_t real_realloc = NULL;
	if ((real_realloc = dlsym(RTLD_NEXT, "realloc")) == NULL) {
		perror("dlsym");
		return NULL;
	}
	void* new = real_realloc(ptr, size);
	int x = find_ptr(ptr);
	if (x == -1) {
		memory[idx].pointer = new;
		memory[idx].size = size;
		idx++;
		total += size;
		snprintf(buffer, BUF_SIZE, "Realloc: New memory allocated at %p of size %zu bytes\n", new, size);
		print_log(buffer);
		return new;
	}
	if (new == ptr) {
		size_t old = memory[x].size;
		memory[x].size = size;
		total = total + size - old;
		snprintf(buffer, BUF_SIZE, "Realloc: Realocated memory at %p from %zu to %zu bytes\n", ptr, old, size);
		print_log(buffer);
	} else {
		memory_t old = memory[x];
		memory[x].pointer = NULL;
		memory[idx].pointer = new;
		memory[idx].size = size;
		idx++;
		total = total + size - old.size;
		snprintf(buffer, BUF_SIZE, "Realloc: Realocated new memory at %p of %zu\n", new, size);
		print_log(buffer);
		snprintf(buffer, BUF_SIZE, "       : Deleted memory at %p of %zu\n", old.pointer, old.size);
		print_log(buffer);
	}
	return new;
}

void free(void* ptr) {
	if (ptr == NULL) {
		print_log("Free: Attempt to free NULL\n");
		return;
	}
	char buffer[BUF_SIZE];
	orig_free_t real_free = dlsym(RTLD_NEXT, "free");
	int x = find_ptr(ptr);
	if (x == -1) {
		print_log("Free: Unknown memory tried to free\n");
		return;
	} else {
        size_t size = memory[x].size;
        if (size > 0) frees++;
        total -= size;
		snprintf(buffer, BUF_SIZE, "Free: %p of size %zu freed\n", ptr, size);
		print_log(buffer);
		memory[x].pointer = NULL;
	}
	real_free(ptr);
}
