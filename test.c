#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	printf("--- Starting Memory Allocation Tests --- \n");

	printf(" [Test 1: Simple malloc and free] \n");
	int* arr1 = malloc(10 * sizeof(int));
	if (arr1) {
		for (int i = 0; i < 10; i++) arr1[i] = i;
		printf("arr1[5] = %d \n", arr1[5]);
	}
	free(arr1);

	printf(" [Test 2: Simple calloc and free] \n");
	int* arr2 = calloc(10, sizeof(int));
	if (arr2) {
		printf("arr2[5] (after calloc) = %d \n", arr2[5]);
		for (int i = 0; i < 10; i++) arr2[i] = i * 2;
		printf("arr2[5] (after assignment) = %d \n", arr2[5]);
	}
	free(arr2);

	printf(" [Test 3: Realloc to a larger size] \n");
	char* str = malloc(10);
	if (str) {
		strcpy(str, "Hello");
		printf("Original string: %s \n", str);
		char* new_str = realloc(str, 50);
		if (new_str) {
			strcat(new_str, " World!");
			printf("Reallocated string: %s \n", new_str);
		}
	}
	free(str);

	printf(" [Test 4: Realloc to a smaller size] \n");
	int* arr3 = malloc(100 * sizeof(int));
	arr3 = realloc(arr3, 10 * sizeof(int));
	free(arr3);

	printf(" [Test 5: realloc(NULL, size)] \n");
	int* arr4 = realloc(NULL, 20 * sizeof(int));
	free(arr4);

	printf(" [Test 6: free(NULL)] \n");
	free(NULL);

	printf(" [Test 7: Double free] \n");
	int* double_free_ptr = malloc(sizeof(int));
	free(double_free_ptr);
	printf("Attempting to free the same pointer again: \n");
	free(double_free_ptr);

	printf(" [Test 8: Freeing stack memory] \n");
	int stack_var = 42;
	printf("Attempting to free a pointer to a stack variable: \n");
	free(&stack_var);

	printf(" [Test 9: Memory leak simulation] \n");
	int* leaked_mem = malloc(sizeof(int));
	if (leaked_mem != NULL) {
		*leaked_mem = 12345;
		printf("Allocated memory at %p that will not be freed. \n", (void*)leaked_mem);
	}

	printf(" --- Finished Memory Allocation Tests --- \n");

	return 0;
}
