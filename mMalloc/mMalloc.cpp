#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCK_SIZE 1024
#define NODE_SIZE 12

struct Node {
  size_t size;
  Node *prev;
  Node *next;
};

struct Node *head;
long unsigned heapStart;


void printFreeList(){
  printf("=== FREE LIST ===\n");
  struct Node *current = (struct Node *) head;
  while(current != NULL){
    printf("%lu: (", (long unsigned)current - heapStart);
    (current->prev == NULL) ? printf("%d, ", -1) : printf("%lu, ", (long unsigned)current->prev - heapStart);
    (current->next == NULL) ? printf("%d, ", -1) : printf("%lu, ", (long unsigned)current->next - heapStart);
    printf("%d)\n",current->size);
    current = current->next;
  }
  printf("=== END       ===\n");
}

void addHeapSpace(){

  printf("mmalloc: calling sbrk...\n");
  void *startOfNewHeapSace = sbrk(BLOCK_SIZE);
  //void *test = sbrk();
  //perror("Seg Fault 69");
  if(head == NULL){
    heapStart = (unsigned long) startOfNewHeapSace;
    head = (struct Node *)startOfNewHeapSace;
    head->size = BLOCK_SIZE - NODE_SIZE;
    head->prev = NULL;
    head->next = NULL;
    printf("New Heap space starting at %lu\n", (unsigned long) startOfNewHeapSace - heapStart);
    return;
  }
  printf("New Heap space starting at %lu\n", (unsigned long) startOfNewHeapSace - heapStart);

  struct Node *n = head;
  struct Node *prev;

  /* Append Free List with newly allocated heap space */
  while(n != NULL){
    prev = n;
    n = n->next;
    if(n == NULL){
      n = (struct Node *) startOfNewHeapSace;
      n->size = BLOCK_SIZE - NODE_SIZE;
      n->prev = prev;
      n->prev->next = n;
      n->next = NULL;
      return;
    }
  }
}

void *mmalloc(size_t size){
  printf("mmalloc: requesting %d bytes\n", size);
  struct Node *temp = head;
  while(temp != NULL){
    //printf("tmpSize: %d Size: %d\n", temp->size, size);
    if(temp->size > size){
        printf("mmalloc: node at %lu has enough space (%d)\n", (long unsigned)temp - heapStart, temp->size);

        int tempSize = temp->size - size;
        // Split memory & append to freeList if able
        if(temp->size > NODE_SIZE){
          long unsigned extraMem =(long unsigned )temp + size + NODE_SIZE; // + 1 ?
          struct Node *split = (struct Node *) extraMem;
          if(split == NULL){
            perror("Tail is NULL");
            exit(0);
          }

          /* Connect Node */
          split->size = tempSize - NODE_SIZE;
          split->prev = temp->prev;
          split->next = temp->next;

          if(temp->prev != NULL){
            temp->prev->next = split;
          }else{
            head = split;
            head->prev = NULL;
            head->next = split->next;
            head->size = split->size;
          }
          if(temp->next != NULL){
            temp->next->prev = split;
          }

          printf("mmalloc: splitting %lu (%d) into %lu (%d) and %lu (%d)\n",
                  (long unsigned) temp - heapStart, temp->size,
                  (long unsigned) temp - heapStart, size,
                  (long unsigned) split - heapStart, split->size);

          temp->size = size; //save for future mfree calls
          return (void *)((long unsigned) temp + NODE_SIZE);
        }

    /* Perfect Fit */
    }else if(temp->size == size){
      /* Disconnect Node */
      printf("mmalloc: node at %lu has enough space (%d)\n", (long unsigned)temp - heapStart, temp->size);
      int status = 0; //test for head or tail conditions
      //perror("Seg Fault 97");
      if(temp->prev != NULL){
        temp->prev->next = temp->next;
      }else{
        status ++;
      }
      if(temp->next != NULL){
        temp->next->prev = temp->prev;
      }else{
        status --;
      }
      printf("mmalloc: releasing %lu (size = %d) from free list\n", (long unsigned)temp - heapStart, temp->size);

      if(status == 1){ //temp is head
        head = head->next;
        head->prev = NULL;
      }else if(status == -1){ //temp is tail
        temp->prev->next = NULL;
      }
      printf("mmalloc: returning %lu\n", (long unsigned)temp - heapStart + NODE_SIZE);
      return (void *)((long unsigned) temp + NODE_SIZE);

    }
    temp = temp->next;
  }

  /* No nodes satisfy size */
  addHeapSpace();
  return mmalloc(size);
}


void mfree(void * n){
  struct Node *node = (struct Node *) ((long unsigned)n - NODE_SIZE);
  node->prev = NULL;
  node->next = NULL;
  //node->size = (size_t)((void *)((long unsigned) node));
  printf("Freeing %lu size: %d\n", (long unsigned)node - heapStart, node->size );

  struct Node *current = head;
  struct Node *prev;

  while(current != NULL){
    prev = current;
    current = current->next;
    if(current == NULL){
        prev->next = node;
        node->prev = prev;
        node->next = NULL;
        return;
    }
  }
  /* Head == NULL */
  head = node;
  return;
}

int main(int argc, char *argv[]){
  printf("main: starting main()\n");
  printFreeList();

  printf("main: testing imperfect match...\n");
  printf("main: mmalloc return address : %lu\n", (long unsigned) mmalloc(10) - heapStart);
  printFreeList();

  printf("main: testing perfect match...\n");
  printf("main: mmalloc return address : %lu\n", (long unsigned) mmalloc(1012) - heapStart);
  printFreeList();

  printf("main: attempting mfree(1036)\n");
  void *freeThis = (void *)((long unsigned) 1036 + heapStart);
  mfree(freeThis);
  printFreeList();

  printf("main: testing imperfect match...\n");
  printf("main: mmalloc return address : %lu\n", (long unsigned) mmalloc(50) - heapStart);
  printFreeList();

  exit(0);
}
