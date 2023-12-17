#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)     ((b) + 1)
#define BLOCK_HEADER(ptr) ((struct _block *)(ptr) - 1)

static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;

struct _block *heapList = NULL; /* Free list to track the _blocks available */
struct _block *nextFitPrevious = NULL;

struct _block 
{
   size_t  size;         /* Size of the allocated _block of memory in bytes */
   struct _block *next;  /* Pointer to the next _block of allcated memory   */
   bool   free;          /* Is this _block free?                            */
   char   padding[3];    /* Padding: IENTRTMzMjAgU3ByaW5nIDIwMjM            */
};

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics( void )
{
   struct _block *temp = heapList;
   while(temp)
   {
      num_blocks++;
      temp = temp->next;
   }

  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes 
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */
struct _block *findFreeBlock(struct _block **last, size_t size) 
{
   struct _block *curr = heapList;

#if defined FIT && FIT == 0
   /* First fit */
   //
   // While we haven't run off the end of the linked list and
   // while the current node we point to isn't free or isn't big enough
   // then continue to iterate over the list.  This loop ends either
   // with curr pointing to NULL, meaning we've run to the end of the list
   // without finding a node or it ends pointing to a free node that has enough
   // space for the request.
   // 
   while (curr && !(curr->free && curr->size >= size)) 
   {
      *last = curr;
      curr  = curr->next;
   }
#endif

// \TODO Put your Best Fit code in this #ifdef block
#if defined BEST && BEST == 0
   /** \TODO Implement best fit here */
   size_t currBestSize = (size_t)-1;
   struct _block *currBestFit = NULL;
   while (curr) 
   {
      if(curr->free && curr->size >= size && curr->size < currBestSize)
      {
         currBestSize = curr->size;
         currBestFit = curr;
      }

      *last = curr;
      curr  = curr->next;
   }
   if(currBestFit != NULL)
   {
      curr = currBestFit;
   }
#endif

// \TODO Put your Worst Fit code in this #ifdef block
#if defined WORST && WORST == 0
   /** \TODO Implement worst fit here */
   size_t currWorstSize = 0;
   struct _block *currWorstFit = NULL;
   while (curr) 
   {
      if(curr->free && curr->size >= size && curr->size > currWorstSize)
      {
         currWorstSize = curr->size;
         currWorstFit = curr;
      }

      *last = curr;
      curr  = curr->next;
   }
   if(currWorstFit != NULL)
   {
      curr = currWorstFit;
   }
#endif

// \TODO Put your Next Fit code in this #ifdef block
#if defined NEXT && NEXT == 0
   /** \TODO Implement next fit here */
   struct _block *nextFitTemp = nextFitPrevious;
   while (nextFitTemp && !(nextFitTemp->free && nextFitTemp->size >= size))
   {
      *last = nextFitTemp;
      nextFitTemp = nextFitTemp->next;
   }
   if(nextFitTemp == NULL)
   {
      while(curr && !(curr->free && curr->size >= size))
      {
         if(curr && curr == nextFitPrevious)
         {
            return NULL;
         }
         *last = curr;
         curr  = curr->next;
      }
   }
   else
   {
      curr = nextFitTemp;
   }
#endif

   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically 
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size) 
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1) 
   {
      return NULL;
   }

   /* Update heapList if not set */
   if (heapList == NULL) 
   {
      heapList = curr;
   }

   /* Attach new _block to previous _block */
   if (last) 
   {
      last->next = curr;
   }

   /* Update _block metadata:
      Set the size of the new block and initialize the new block to "free".
      Set its next pointer to NULL since it's now the tail of the linked list.
   */
   max_heap += size;

   curr->size = size;
   curr->next = NULL;
   curr->free = false;
   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the 
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process 
 * or NULL if failed
 */
void *malloc(size_t size) 
{

   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0) 
   {
      return NULL;
   }

   /* Look for free _block.  If a free block isn't found then we need to grow our heap. */

   struct _block *last = heapList;
   struct _block *next = findFreeBlock(&last, size);

   if(next && next->size - size > sizeof(struct _block) + 4)
   {
      struct _block *tempNext = NULL;
      size_t oldSize = next->size;
      size_t remainder = oldSize - size - sizeof(struct _block);

      next->free = false;
      next->size = oldSize - remainder;
      
      if(next && next->next != NULL)
      {
         tempNext = next->next;
      }
      
      struct _block* splitRemainder = (struct _block*)((char*)next + sizeof(struct _block) + size);
      next->next = splitRemainder;
      
      splitRemainder->free = true;
      splitRemainder->size = remainder;
      splitRemainder->next = tempNext;

      num_splits++;
   }

   /* Could not find free _block, so grow heap */
   if (next == NULL) 
   {
      next = growHeap(last, size);
      num_grows++;
   }
   else
   {
      num_reuses++;
      nextFitPrevious = next;
   }

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL) 
   {
      return NULL;
   }
   
   /* Mark _block as in use */
   next->free = false;

   num_mallocs++;
   num_requested += size;
   
   /* Return data address associated with _block to the user */
   return BLOCK_DATA(next);
}

/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr) 
{
   if (ptr == NULL) 
   {
      return;
   }

   /* Make _block as free */
   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;

   struct _block* tempCurr = heapList;
   struct _block *tempNext = NULL;
   while(tempCurr)
   {
      if(tempCurr && tempCurr->free && tempCurr->next && tempCurr->next->free)
      {
         tempNext = tempCurr->next->next;
         size_t oldSize = tempCurr->size + sizeof(struct _block);
         size_t tempSize = tempCurr->next->size + sizeof(struct _block);
         
         tempCurr->size = oldSize + tempSize;
         tempCurr->next = tempNext;
         tempCurr->free = true;

         num_coalesces++;
      }
      else
      {
         tempCurr = tempCurr->next;
      }
   }
   
   num_frees++;
}

void *calloc( size_t nmemb, size_t size )
{
   // \TODO Implement calloc
   struct _block *ptr = malloc(nmemb * size);
   ptr = memset( ptr, 0, nmemb * size);
   
   return ptr;
}

void *realloc( void *ptr, size_t size )
{
   struct _block *newPtr = NULL;

   // \TODO Implement realloc
   if(size > 0)
   {
      if(ptr == NULL)
      {
         malloc(size);
      }
      else
      {
         newPtr = malloc(size);
         memcpy((void*)newPtr,(void*)ptr,size);

         free(ptr);
      }
   }
   else
   {
      free(ptr);
   }

   return newPtr;
}



/* vim: IENTRTMzMjAgU3ByaW5nIDIwMjM= -----------------------------------------*/
/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
