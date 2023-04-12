#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN4(s) (((((s)-1) >> 2) << 2) + 4)
#define BLOCK_DATA(b) ((b) + 1)
#define BLOCK_HEADER(ptr) ((struct _block *)(ptr)-1)

static int atexit_registered = 0;
static int num_mallocs = 0;
static int num_frees = 0;
static int num_reuses = 0;
static int num_grows = 0;
static int num_splits = 0;
static int num_coalesces = 0;
static int num_blocks = 0;
static int num_requested = 0;
static int max_heap = 0;

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
void printStatistics(void)
{
   printf("\nheap management statistics\n");
   printf("mallocs:\t%d\n", num_mallocs);
   printf("frees:\t\t%d\n", num_frees);
   printf("reuses:\t\t%d\n", num_reuses);
   printf("grows:\t\t%d\n", num_grows);
   printf("splits:\t\t%d\n", num_splits);
   printf("coalesces:\t%d\n", num_coalesces);
   printf("blocks:\t\t%d\n", num_blocks);
   printf("requested:\t%d\n", num_requested);
   printf("max heap:\t%d\n", max_heap);
}

#if defined NEXT && NEXT == 0
static struct _block *lastAlloc = NULL;
#endif

struct _block
{
   size_t size;         /* Size of the allocated _block of memory in bytes */
   struct _block *next; /* Pointer to the next _block of allcated memory   */
   bool free;           /* Is this _block free?                            */
   char padding[3];     /* Padding: IENTRTMzMjAgU3ByaW5nIDIwMjM            */
};

struct _block *heapList = NULL; /* Free list to track the _blocks available */

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
#if defined NEXT && NEXT == 0
   struct _block *curr = lastAlloc;
#else
   struct _block *curr = heapList;
#endif

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
      curr = curr->next;
   }
#endif

#if defined BEST && BEST == 0
   //
   size_t bestDiff = SIZE_MAX;
   struct _block *best = NULL; // In case there is no available block
   while (curr)
   {
      if (curr->free && (curr->size >= size) &&
          ((curr->size - size) < bestDiff))
      {
         best = curr;
         bestDiff = curr->size - size;
      }
      *last = curr;
      curr = curr->next;
   }
   // Return the best
   curr = best;
#endif

#if defined WORST && WORST == 0
   size_t worstDiff = 0;
   struct _block *worst = NULL; // In case there is no available block
   while (curr)
   {
      if (curr->free && (curr->size >= size) &&
          ((curr->size - size) > worstDiff))
      {
         worst = curr;
         worstDiff = curr->size - size;
      }
      *last = curr;
      curr = curr->next;
   }

   // Return the worst (low-key worst-fit is best algo)
   curr = worst;
#endif

#if defined NEXT && NEXT == 0
   while (curr)
   {
      if (curr->free && curr->size >= size)
         return curr;

      *last = curr;
      curr = curr->next;
   }

   curr = heapList;
   while (curr != lastAlloc)
   {
      if (curr->free && curr->size >= size)
         return curr;

      curr = curr->next;
   }

   curr = NULL;
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

   ++num_grows;
   ++num_blocks;

   /* Update _block metadata:
      Set the size of the new block and initialize the new block to "free".
      Set its next pointer to NULL since it's now the tail of the linked list.
   */
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
   if (atexit_registered == 0)
   {
      atexit_registered = 1;
      atexit(printStatistics);
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0)
   {
      return NULL;
   }

   // After alignment or not??
   num_requested += size;

   /* Look for free _block.  If a free block isn't found then we need to grow our heap. */

   struct _block *last = heapList;
   struct _block *next = findFreeBlock(&last, size);

   /* If the block found by findFreeBlock is larger than we need then:
         If the leftover space in the new block is greater than the sizeof(_block)+4 then
         split the block.
         If the leftover space in the new block is less than the sizeof(_block)+4 then
         don't split the block.
   */

   /* Could not find free _block, so grow heap */
   if (next == NULL)
   {
      next = growHeap(last, size);
   }
   else
   {
      // Some other allocation can use this 4 bytes or more
      // so split this current block into 2
      if ((next->size - size) >= sizeof(struct _block) + 4)
      {
         /* We have to cast it to a uintptr_t first so we don't
         use the regular pointer arithmetic since that will offset the
         address by sizeof(struct _block) instead of by one
         */
         struct _block *newNode = (struct _block *)((uintptr_t)next + sizeof(struct _block) + size);

         newNode->next = next->next; // Add a node in the middle of next and next->next
         newNode->free = true;       // we just split this from a free block so it should be free
         newNode->size = next->size - size - sizeof(struct _block);

         /* Set the new size of next, as well as the pointer to the "node" in between*/
         next->next = newNode;
         next->size = size;

         ++num_splits;
         ++num_blocks;
      }
      ++num_reuses;
   }

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL)
   {
      return NULL;
   }

// This will always be the last point we allocated memory
#if defined NEXT && NEXT == 0
   lastAlloc = next;
#endif

   /* Mark _block as in use */
   next->free = false;

   /* The user has sucessfully gotten a valid address at this point*/
   ++num_mallocs;

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
   struct _block *prev;
   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;

   /* Coalesce free _blocks.  If the next block or previous block 
      are free then combine them with this block being freed.
   */

   // If we free the beginning of list, there is no
   // previous node that can be free for us to coalesce
   if (curr == heapList)
      goto second_check;

   prev = heapList;
   while (prev && prev->next != curr)
      prev = prev->next;

   // The only way if prev is NULL is if curr is heapList
   // which we check before-hand
   assert(prev != NULL);
   // If the previous is not free, we proceed to the second coalescing
   if (!prev->free)
      goto second_check;

   // Check that we are actually coalescing consecutive blocks.
   // This is usually the case unless the user messes with the node metadata
   assert((char *)BLOCK_DATA(prev) + prev->size == (char *)curr);

   // TODO: maybe we memset the old block's meta data to zero or something?
   prev->next = curr->next;
   prev->size += curr->size + sizeof(struct _block);
   curr = prev;

   /* The first coalesce just happened */
   ++num_coalesces;
   --num_blocks;

second_check:
   // If there is a node after this and it is free then we coalesce
   if (curr->next && curr->next->free)
   {
      // Before we coalesce, perform quick sanity check
      assert((char *)BLOCK_DATA(curr) + curr->size == (char *)curr->next);

      curr->size += curr->next->size + sizeof(struct _block);
      curr->next = curr->next->next;

      /* The second coalesce just happened */
      ++num_coalesces;
      --num_blocks;
   }
   // There should be no more than 3 free blocks in a row
   if (curr->next)
      assert(!curr->next->free);

   /* Free is sucessful*/
   ++num_frees;
}

void *calloc(size_t nmemb, size_t size)
{
   void *ret = malloc(nmemb * size);
   memset(ret, 0, nmemb * size);

   return ret;
}

void *realloc(void *ptr, size_t size)
{
   void *dst = malloc(size);

   /* If we could allocate the memory, only then do we copy */
   if (dst != NULL)
   {
      struct _block *this = BLOCK_HEADER(ptr);
      size = this->size > size ? size : this->size;

      memcpy(dst, ptr, size);
   }

   free(ptr);
   return dst;
}

/* vim: IENTRTMzMjAgU3ByaW5nIDIwMjM= -----------------------------------------*/
/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
