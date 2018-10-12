/* Memory Allocator using Best Fit Algorithm.	*
 *	By Sougata Dafader			*
 *						*/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

typedef struct block block;
struct block
{
    size_t size;
    struct block *next;
    struct block *addr;
    int free;
};

struct block *Flist[3];
size_t sz;
#define BLOCK_SIZE sizeof(struct block)
#define MEM_LIMIT 64
#define MEMORY_POOL 256000

void init()
{	
	/*Initialising the Flist elements as Null*/ 
	
	Flist[0]=NULL;
	Flist[1]=NULL;
	Flist[2]=NULL;
}
struct block *get_free_block(size_t size)
{
	struct block *temp;
	size_t total_size = size + BLOCK_SIZE;
	if((total_size >0 ) && (total_size <=16)){
		if(Flist[0]!=NULL) /*Checking if any element is present for 16bit*/{
			Flist[0]= temp->next;
			temp->next =NULL;
			return temp;
		}	
		else
			return NULL;
	}
	else if((total_size >16 ) && (total_size <=32)){	
		if(Flist[0]!=NULL) /*Checking if any element is present for 32bit*/{
                        temp = Flist[1];
                        Flist[1]= temp->next;
			temp->next =NULL;
                        return temp;
                }
                else
                        return NULL;
        }
	else if((total_size >32 ) && (total_size <64)){
		if(Flist[2]!=NULL) /*Checking if any element is present for 64bit*/{
                        temp = Flist[2];
                        Flist[2]= temp->next;
			temp->next=NULL;
                        return temp;
                }
                else
                        return NULL;
        }
	else 
		return NULL;
}


void traverse_and_add(struct block *head, struct block *bl){
	struct block *tail= head;
	while(tail->next!=NULL)
	{
		tail =tail->next;
	}
	tail->next= bl;
	return;
}

void add_to_list(struct block *curr_head)
{
	size_t total_size;
	struct block *head_to_traverse;
	total_size = curr_head->size + BLOCK_SIZE;
	curr_head->next =NULL;
	if((total_size >0 ) && (total_size <=16))
        {
                if(Flist[0]!=NULL) /*Checking if any element is present for 16bit*/
                {
                        head_to_traverse = Flist[0];
			traverse_and_add(head_to_traverse,curr_head);
			return;
                }
                else
                        Flist[0]=curr_head;
			return;
        }
        else if((total_size >16 ) && (total_size <=32))
        {
                if(Flist[1]!=NULL) /*Checking if any element is present for 32bit*/
                {
                        head_to_traverse = Flist[1];
			traverse_and_add(head_to_traverse,curr_head);
			return;
                }
                else
                        Flist[1]=curr_head;
			return;
        }
        else if((total_size >32 ) && (total_size <64))
        {
		if(Flist[2]!=NULL) /*Checking if any element is present for 32bit*/
                {
                        head_to_traverse = Flist[1];
			traverse_and_add(head_to_traverse,curr_head);
                        return;
                }
                else
                        Flist[1]=curr_head;
                        return;
	}

}
//Thread Lock Implemented.
pthread_mutex_t thread_lock;
void myfree(void *memblock)
{
    struct block *header;
    struct block *temp;
    void *p;
    size_t total_size;
    int *plen= (int*)memblock;
    if (memblock==NULL)
	{
       		 return;
	}
    pthread_mutex_lock(&thread_lock);
    	int *ptr_blck= (int*)memblock;
       	plen--;
	if(sz > MEM_LIMIT)
        {
    		munmap( (void*)plen, sz);
		printf("Freed %zu bytes \n",sz);
		//sleep(1);
		pthread_mutex_unlock(&thread_lock);
		return;
	 }
	else
	{
		header->free = 1;
		add_to_list(header);
        	printf("Freed %zu bytes\n", total_size);
        	pthread_mutex_unlock(&thread_lock);
        	return;
    
	}
}

void *mymalloc(size_t size)
{
    init();
    size_t total_size;
    void *memblock;
    void* reference;
    struct block *header;
    if (!size)
        return NULL;
    pthread_mutex_lock(&thread_lock);
    if(BLOCK_SIZE + size > MEM_LIMIT)
        {

		int *plen;
    		int len = size + sizeof( size );

    		header = mmap( NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
		if(header == MAP_FAILED)
		{
			pthread_mutex_unlock(&thread_lock);
			return NULL;
		}
    		header->size =(size_t) len;
    		header->free = 0;
    		header->next = NULL;
		printf("Memory Allocated using Mmap:  %zu\n",header->size);
		sz= (size_t)len;
		pthread_mutex_unlock(&thread_lock);
		return (void*)(header + 1);
 }
else{
    header = get_free_block(size);
    if (header!=NULL) {
        header->free = 0;
	printf("Allocated %zu bytes\n from freelist",size);
        pthread_mutex_unlock(&thread_lock);
        return (void*)(header + 1);
    }
    total_size = BLOCK_SIZE + size;
	if((total_size >0 ) && (total_size <=16)){
		total_size = 16;
	}
else if((total_size >16 ) && (total_size <=32)){
		total_size =32;
	}
else if((total_size >32 ) && (total_size <=64)){
		total_size =64;
	}
    memblock = sbrk(total_size);
    if (memblock == (void*) -1) {
	printf("Error Allocating\n");
        pthread_mutex_unlock(&thread_lock);
        return NULL;
    }
    header = memblock;
    header->size = size;
    header->free = 0;
    header->next = NULL;
    printf("Allocated %zu bytes \n by sbrk", total_size);
    pthread_mutex_unlock(&thread_lock);
    reference = (void*)(header + 1);
    return reference;
}
}
