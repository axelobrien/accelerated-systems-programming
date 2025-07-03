#include <unistd.h> // sbrk
#include <stdint.h> // size_t
#include <sys/mman.h> // mmap, munmamp
#include <stdlib.h>
#include <string.h> // memset
#include <stdio.h> // delete

struct allocation_prefix {
	size_t size;
	struct allocation_prefix* next;
	struct allocation_prefix* prev;
} __attribute__((packed));

struct page {
	struct allocation_prefix *start;
	struct page *next;
	struct page *prev;
}__attribute__((packed));

struct page *allocation_pages = NULL;
struct allocation_prefix *heap_top = NULL;
struct allocation_prefix *free_space = NULL;

static void printm(char *s) {
	write(STDOUT_FILENO, s, strlen(s));
}

static void print_num(intptr_t n, int base) {
	if (n == 0) {
		printm("0");
		return;
	}

	char reverse_buf[101];
	const int original_n = n;
	const int original_base = base;

	memset(reverse_buf, 0, 101);

	if (base == 0) {
		base = 16;
	}

	if (n < 0 && base != 16) {
		n *= -1;
	}


	for (int i = 0; i < 100; i++) {
		char digit = n % base;
		digit += '0';

		if (digit > '9') {
			digit += 'A' - '9' - 1;
		}

		reverse_buf[i] = digit;
		n /= base;
		
		if (n == 0) {
			break;
		}
	}

	char buf[102];
	int start_index = 0;

	memset(buf, 0, 102);

	if (original_n < 0 && base != 16) {
		buf[0] = '-';
		start_index = 1;
	}
	int length = strlen(reverse_buf);

	for (int i = 0; i < length; i++) {
		buf[i + start_index] = reverse_buf[length - 1 - i];
	}

	if (original_base == 16) {
		printm("0x");
	}
	printm(buf);
	printm("\n");
	memset(buf, 0, 102);
	memset(reverse_buf, 0, 101);
}

static void print_ptr(void *p) {
	print_num((intptr_t)p, 0);
}


intptr_t count = 0;
void *malloc(size_t len) {
	count++;
	printm("Count: ");
	print_num(count, 10);
	print_ptr((void *)0x0123456789abcdef);
	if (len < sizeof heap_top[0]) {
		len = sizeof heap_top[0] + sizeof len;
	} else {
		len += sizeof len;
		if (len % 8 != 0) {
			len += 8 - (len % 8); // is this needed?
		}
	}

	if (allocation_pages != NULL) {
		struct page *clone = allocation_pages;
		printm("ALLOCATION_PAGES\n");
		do {
			print_ptr(clone->start);
			clone = clone->next;
		} while (clone != NULL);
		printm("\n");
	}

	if (heap_top != NULL) {
		printm("HEAP_TOP\n");
		print_ptr(heap_top);
		printm("Size: ");
		print_num(heap_top->size, 10);
		printm("\n");
	}

	if (free_space != NULL) {
		struct allocation_prefix *clone = free_space;
		printm("FREE_SPACE\n");

		print_ptr(clone);
			printm("Bytes at address above: ");
			print_num(clone->size, 10);
		while (clone != NULL && clone->next != NULL) {
			clone = clone->next;
			print_ptr(clone);
			printm("Bytes at address above: ");
			print_num(clone->size, 10);
		} 
	}

	printm("len: ");
	print_num(len, 10);

	size_t page_size = getpagesize();

	if (len > page_size) {
		struct allocation_prefix *const result = mmap(
			NULL,
			len,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1,
			0
		);

		if (result == MAP_FAILED) {
			return NULL;
		}

		result->size = len;
		return (void *)((uintptr_t)(result) + sizeof len);
	}


	if (allocation_pages == NULL) {

		void *sbrk_result = sbrk(0);

		if ((uintptr_t)sbrk_result == -1) {
			return NULL;
		}

		sbrk_result = (void *)((uintptr_t)sbrk_result + 0x2000); // allow space for pages

		struct allocation_prefix *const result = mmap(
			sbrk_result,
			len,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1,
			0
		);

		if (result == MAP_FAILED) {
			return NULL;
		}

		struct allocation_prefix beginning_node;
		beginning_node.size = len;
		beginning_node.next = NULL;
		beginning_node.prev = NULL;

		struct page *const pages_memory = mmap(
			sbrk_result - 0x2000,
			page_size,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
			-1,
			0
		);
		
		allocation_pages = pages_memory;

		allocation_pages->start = result;
		allocation_pages->next = NULL;
		allocation_pages->prev = NULL;

		*result = beginning_node;
		free_space = (struct allocation_prefix *)((uintptr_t)(result) + beginning_node.size);
		free_space->next = NULL;
		free_space->prev = NULL;

		if (page_size - len < sizeof beginning_node) {
			struct allocation_prefix *const extra_page = mmap(
				sbrk_result + page_size,
				page_size,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS,
				-1,
				0
			);

			if (result == MAP_FAILED) {
				return NULL;
			}

			free_space->size = (2 * page_size) - len;

			allocation_pages[1].start = extra_page;
			allocation_pages[1].next = NULL;
			allocation_pages[1].prev = allocation_pages;
			allocation_pages[0].next = allocation_pages + 1;
		} else {
			free_space->size = page_size - len;
		}

		heap_top = free_space;
		printm("HEAP TOP SIZE: ");
		print_num(heap_top->size, 10);

		return (void *)((uintptr_t)(result) + sizeof beginning_node.size);
	}

	struct allocation_prefix *node = heap_top;

	if (free_space != NULL) {
		struct allocation_prefix *free_node = free_space;
		while (free_node->size < len + sizeof *free_node && free_node->next != NULL) {
			free_node = free_node->next;
		}
		
		
		if (free_node->size >= len + sizeof *free_node) {
			node = free_node;
			size_t new_size = free_node->size - len;
			struct allocation_prefix *new_free_space = (struct allocation_prefix *)((uintptr_t)free_node + len);

			if (free_node->prev != NULL) {
				free_node->prev->next = new_free_space;
			}

			if (free_node->next != NULL) {
				free_node->next->prev = new_free_space;
			}
			
			print_ptr(free_node->next);
			printm("\n");
			print_ptr(free_node->prev);
			printm("\n");
			new_free_space->next = free_node->next;
			new_free_space->prev = free_node->prev;
			new_free_space->size = new_size;
			printm("new_free_space (size, next, prev, addr)\n");
			print_num(new_free_space->size, 10);
			print_ptr(new_free_space->next);
			print_ptr(new_free_space->prev);
			print_ptr(new_free_space);

			if (free_node == heap_top) {
				heap_top = new_free_space;

				if (free_node->next != NULL) {
			printm("this shouldn't print\n");
					free_node->next->prev = new_free_space->prev;
				}

				if (free_node->prev != NULL) {
			printm("this shouldn't print\n");
					free_node->prev->next = new_free_space->next;
				}

				if (free_node == free_space) {
					free_space = heap_top;
				}
			}

			node->size = len;

			printm("Heap top size in free: ");
			print_num(heap_top->size, 10);
			print_ptr(heap_top->next);
			print_ptr(heap_top->prev);

			printm("ruoiewp\n");
			print_ptr(node);
			print_ptr(heap_top);
			print_num(node->size, 10);

			return (void *)((uintptr_t)(node) + sizeof node->size);
		} else if (free_node != NULL && free_node->size > len && free_node != heap_top) { // if free_node->size isn't big enough to accomodate a struct allocation_prefix, there's no way to use it as free space
			node = free_node;
			
			if (free_node->prev != NULL) {
				free_node->prev->next = free_node->next;
			}

			if (free_node->next != NULL) {
				free_node->next->prev = free_node->prev;
			}

			if (free_node == free_space) {
				free_space = free_node->next;
			}			



			return (void *)((uintptr_t)(node) + sizeof node->size);
		} 
	}


//	while (node->next != NULL) {
//		node = node->next;
//	}
//
//	node = (struct allocation_prefix *)((uintptr_t)(node) + node->size);

	if (node == heap_top) {
		// Check if allocation crosses the page size
		// If so, allocate a new page
		if (heap_top->size < len + sizeof *heap_top) {
			struct allocation_prefix *const extra_page = mmap(
				(void *)(((uintptr_t)heap_top) + page_size - ((uintptr_t)heap_top % page_size)),
				// heap_top,
				page_size,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS,
				-1,
				0
			);

			if (extra_page == MAP_FAILED) {
				return NULL;
			}

			heap_top->size += page_size;

			struct page *last_page = allocation_pages; // This has to not be NULL because this only ever triggers when at least one page has been allocated, which means it will bein allocation_pages.

			while (last_page->next != NULL) {
				last_page = last_page->next;
			}
			
			struct page *new_page = last_page + 1;

			last_page->next = new_page;
			new_page->start = extra_page;
			new_page->next = NULL;
			new_page->prev = last_page;
			printm("a\n");
			print_ptr(heap_top);
			print_ptr(last_page->start);
			print_ptr((void *)((uintptr_t)(heap_top) + len));
		}

		struct allocation_prefix *old_heap_top = heap_top;
		heap_top = (struct allocation_prefix *)((uintptr_t)(heap_top) + len);
		heap_top->size = old_heap_top->size - len;
		
		struct allocation_prefix *free_node = free_space;
		while (free_node != NULL && free_node != old_heap_top && free_node->next != NULL) {
			free_node->next;
		}

		if (free_node == old_heap_top) {
			if (free_node->next != NULL) {
				free_node->next->prev = free_node->prev;
			}

			if (free_node->prev != NULL) {
				free_node->prev->next = free_node->next;
			}

			if (free_node == free_space) {
				free_space = heap_top;
			}
		}

//		heap_top->next = old_heap_top->next;
//		heap_top->prev = old_heap_top->prev;
//
//		if (heap_top->next != NULL) { // This should never run, but just in case
//		printm("4\n");
//			heap_top->next->prev = heap_top; // is heap->next just whatever random data from a time when it used to be allocated?
//		}
//
//		if (heap_top->prev != NULL) {
//			heap_top->prev->next = heap_top;
//		}

	}


	node->size = len;  // Remember: This overwrites the free space at *node!!!!
	node->next = NULL; //TODO: Should these be set? Should next and prev even exist on allocated memory?
	node->prev = NULL;

	printm("aksdjh\n");
	print_ptr(node);
	print_num(node->size, 10);
	print_ptr((void *)((uintptr_t)(node) + sizeof len));
	print_ptr(allocation_pages);
	return (void *)((uintptr_t)(node) + sizeof len);
}

void *calloc(size_t n, size_t size) {
	if (n != 0 && size != 0) {
		size_t overflow_test = n * size;
		if (overflow_test / n != size) {
			return NULL;
		}
	}

		printm("8\n");
	void *result = malloc(n * size);
	
	if (result == MAP_FAILED) {
		return NULL;
	}

	void *memset_result = memset(result, 0, n * size);

	if (memset_result == NULL) {
		return NULL;
	}

	return result;
}


//ASSUMPTION: Something in free is messing with the heap_top and changing its
//size, causing the rest of the program to explode

void free(void *const p) {
		printm("5\n");
	if (p == NULL) {
		return;
	}

	size_t page_size = getpagesize();

	struct allocation_prefix *allocation = (struct allocation_prefix *)((size_t *)p - 1);

	if (allocation->size + sizeof allocation->size > page_size) {
		munmap(allocation, allocation->size);
		return;
	}

	if (free_space == NULL) {
		free_space = p;
		return;
	}

	struct allocation_prefix *head = free_space;

	// How to detect neighboring freed memory allocations
	// Maintain a "forward pointer" that is immediately following p
	// If any free_space node is at that "forward pointer" address, it's adjacent
	// If any free_space node + its size == p, it's adjacent
	// Inheriently, no need to check for adjacents beyond this, because before
	// free is called, all current free_space nodes are non-adjacent.
	
	struct allocation_prefix *forward_ptr = NULL;
	struct allocation_prefix *backward_ptr = NULL;

	while (free_space->size < allocation->size && free_space->next != NULL) {
		if (free_space == (struct allocation_prefix *)((uintptr_t)allocation + allocation->size)) {
			forward_ptr = free_space;
		} else if ((struct allocation_prefix *)((uintptr_t)free_space + free_space->size) == allocation) {
			backward_ptr = allocation;
		}

		free_space = free_space->next;
	}

	if (free_space->next != NULL) {
		free_space->next->prev = allocation;
	}

	if (free_space->prev != NULL) {
		free_space->prev->next = allocation;
	}

	allocation->next = free_space->next;
	allocation->prev = free_space->prev;


	while (free_space->next != NULL) { // finish looking for adjacents
		if (free_space == (struct allocation_prefix *)((uintptr_t)allocation + allocation->size)) {
			forward_ptr = free_space;
		} else if ((struct allocation_prefix *)((uintptr_t)free_space + free_space->size) == allocation) {
			backward_ptr = allocation;
		}

		free_space = free_space->next;
	}

	free_space = head;

	if (forward_ptr != NULL) {
		allocation->size += forward_ptr->size;
		allocation->next = forward_ptr->next;

		if (forward_ptr->next != NULL) {
			forward_ptr->next->prev = allocation;
		}

		forward_ptr->size = 0; // are these necessary? I don't think so
		forward_ptr->next = NULL;
		forward_ptr->prev = NULL;
	}

	if (backward_ptr != NULL) {
//		allocation->size += backward_ptr->size;
//		allocation->prev = backward_ptr->prev;
//		backward_ptr->prev->next = allocation;

		backward_ptr->size += allocation->size;
		backward_ptr->next = allocation->next;

		if (allocation->next != NULL) {
			allocation->next->prev = backward_ptr;
		}

		allocation = backward_ptr;
	}

	// Here I assume the following:
	// The only condition that would allow a chunk of freed memory to stretch
	// from one page alignment to another is when they merge with adjacent
	// freed spaces.
	//
	// I assume this because if a chunk of memory is >= page_size in and of
	// itself, it's not in the main heap, and the only way for it to grow is
	// when merging with adjacents.

	if ((forward_ptr != NULL || backward_ptr != NULL) && allocation->size > page_size && allocation_pages != NULL) { // allocation_pages should never be NULL
		struct page *page_list = allocation_pages;
		uintptr_t allocation_base = (uintptr_t)allocation;
		uintptr_t allocation_top = allocation_base + allocation->size;

		while (page_list->next != NULL) {
			uintptr_t page_base = (uintptr_t)page_list;
			uintptr_t page_top = page_base + page_size;

			if (page_base >= allocation_base && page_base < allocation_top && page_top > allocation_base && page_top <= allocation_top) {
				if (page_list->prev != NULL) {
					page_list->prev->next = page_list->next;
					page_list->next->prev = page_list->prev;
				} else {
					page_list->next->prev = NULL;
				}

				munmap(page_list, page_size);
			}
		}
	}

		printm("3\n");
	return;
}

void *realloc(void *p, size_t size) {
	if (p == NULL) {
		return malloc(size);
	}

	if (p != NULL && size == 0) {
		free(p);
		return NULL;
	}

	printm("6\n");
	void *result = malloc(size);
	size_t old_size = (size_t)((size_t *)p - 1);

	if (old_size < size) {
		memcpy(result, p, old_size);
	} else {
		memcpy(result, p, size);
	}

	free(p);
	return result;
}

void *reallocarray(void *p, size_t n, size_t size) {
    return realloc(p, n * size);
}

