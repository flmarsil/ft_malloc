#ifndef LIBFT_MALLOC_H
#define LIBFT_MALLOC_H

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>

#include <stdio.h>

/* Active / Désactive l'affichage d'informations dans le terminal lors de l'execution du programme */
#define DEBUG 							1

/* 	
	Définition des tailles minimum et maximum d'une donnée allouée 
	pointée par *void data dans un block mémoire t_block
*/
#define MIN_DATA_SIZE_TINY_BLOCK 		1
#define MAX_DATA_SIZE_TINY_BLOCK 		4096
#define MIN_DATA_SIZE_SMALL_BLOCK 		MAX_DATA_SIZE_TINY_BLOCK + 1
#define MAX_DATA_SIZE_SMALL_BLOCK 		8192
#define MIN_DATA_SIZE_LARGE_BLOCK 		MAX_DATA_SIZE_SMALL_BLOCK + 1

/*  Blocs minimum pré-alloués lors de la création d'une nouvelle zone  */
#define MIN_BLOCKS_BY_ZONE 				100
#define PAGE_SIZE 						getpagesize()

/* Constants */
#define STRUCT_BLOCK_SIZE 				sizeof(t_block)
#define STRUCT_ZONE_SIZE 				sizeof(t_zone)
#define PTR_SIZE 						sizeof(void*)

#define TINY_ZONE 						0
#define SMALL_ZONE 						1
#define LARGE_ZONE 						2

#define SUCCESS 						0
#define FAILURE 						1

/* Fonctions macros */
/* Arrondi la taille pour etre un multiple de 2 */
#define ROUND_UP(size) ((size + 1) >> 1) << 1;

/* Renvoi l'écart entre la taille d'un block et la taille demandée pour l'allocation */
#define GET_GAP(block_size, request_size) ((block_size) - (request_size))

/* Arrondie la taille demandée pour s'aligner parfaitement avec la taille des pointeurs dans la zone */
#define ALIGN_PTR(request_size) (((((request_size) - 1) >> 2) << 2 ) + (PTR_SIZE))

/* Retourne la taille totale de la zone */
#define GET_TOTAL_ZONE_SIZE(max_data_size_block) ((((max_data_size_block) + (STRUCT_BLOCK_SIZE)) * (MIN_BLOCKS_BY_ZONE)) + (STRUCT_ZONE_SIZE))
#define GET_TOTAL_LARGE_ZONE_SIZE(request_size) ((request_size) + (STRUCT_BLOCK_SIZE) + (STRUCT_ZONE_SIZE))
/* Retourne l'emplacement dans la mémoire où la donnée sera écrite */
#define GET_DATA_ADDR_FROM_BLOCK(block_addr) ((void*)(block_addr) + (STRUCT_BLOCK_SIZE))

/* Récupère l'adresse d'un block depuis l'adresse du pointeur data */
#define GET_BLOCK_ADDR_FROM_DATA(data_addr) ((void*)(data_addr) - (STRUCT_BLOCK_SIZE))

/* Calculs d'initialisation du premier block d'une nouvelle zone */
#define INIT_FIRST_BLOCK_ADDR(zone_addr) ((void*) (zone_addr) + (STRUCT_ZONE_SIZE))
#define INIT_FIRST_BLOCK_SIZE(rounded_size) ((rounded_size) - (STRUCT_ZONE_SIZE + STRUCT_BLOCK_SIZE))

/* Calcul l'adresse du nouveau bloc suivant, apres l'ajustement d'une taille : best-fit */
#define INIT_NEW_NEXT_BLOCK_ADDR(block_addr, request_size) ((void*) (block_addr) + (STRUCT_BLOCK_SIZE) + (request_size))
#define INIT_NEW_NEXT_BLOCK_SIZE(block_size, request_size) ((block_size) - ((STRUCT_BLOCK_SIZE) + (request_size)))

/* Fusionne le block libre suivant avec le block courant */
#define COALESCE_NEXT_MEMORY_BLOCK_SIZE(current_block_size, next_block_size) ((current_block_size) += ((STRUCT_BLOCK_SIZE) + (next_block_size)))


/* Structure pointant sur un block de donnée alloué par l'utilisateur */
typedef struct 							s_block {
	size_t 								size;
	bool 								free;
	struct s_block*					 	next;
	struct s_block*			 			prev; 
}										t_block;

/* Structure pointant sur une pagination virtuelle retournée par la fonction mmap() */
typedef struct 							s_zone {	
	struct s_zone* 						next; 
	struct s_zone* 						prev;
	t_block*							blocks;
	size_t 								size;
}										t_zone;

/* Tableau de zones représentant le heap */
extern t_zone* 								heap[3];	// [0] = TINY_ZONE - [1] = SMALL_ZONE - [2] = LARGE_ZONE
extern pthread_mutex_t 						g_mutex;

/* Fonctions */

/* utils.c */
int 		ft_strlen(char* s);
void 		message(char* s);
size_t 		rounded_to_nearest_multiple_of(size_t number, size_t multiple);
t_zone* 	get_the_end_of_zone_list(t_zone* heap_start);
t_block* 	get_block_addr_from_data(void* data);
t_zone* 	get_zone_addr_from_block(void* block);
int 		define_zone_type(size_t request_size);

/* block.c */
bool 		check_block(void* ptr);
t_block* 	realloc_with_new_block_creation(t_block* block, size_t new_request_size, void* data);
t_block* 	realloc_with_prev_block(t_block* block);
t_block* 	realloc_with_next_block(t_block* block);
void 		copy_data_memory_block(void* dst, void* src, size_t len);
t_block*	coalesce_memory_block(t_block* block);
t_block* 	create_memory_block(t_block* best, size_t request_size);
t_block* 	search_free_block(int type_zone, size_t request_size);

/* zone.c */
void 		delete_pagination(t_zone* zone);
void 		zone_struct_init(t_zone* zone, size_t rounded_size);
void* 		request_memory_from_os(size_t rounded_zone_size);
void* 		create_pagination(int type_zone, size_t request_size);

/* malloc.c */
void* 		malloc_init(size_t request_size);
void*		malloc(size_t size);

/* free.c */
void 		free_init(void* data);
void		free(void* ptr);

/* realloc.c */
void* 		realloc_init(void* data, size_t new_request_size);
void* 		realloc(void *ptr, size_t size);

/* show.c */
void 		show_alloc_mem(void);

#endif
