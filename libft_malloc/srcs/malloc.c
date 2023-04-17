#include "../includes/libft_malloc.h"

t_zone* heap[3] = {NULL, NULL, NULL};
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void* malloc_init(size_t request_size) {
	t_block* block = NULL;
	t_zone* new_zone = NULL;
	int zone_type = 0;

	request_size = (request_size) ? ALIGN_PTR(request_size) : ALIGN_PTR(MIN_DATA_SIZE_TINY_BLOCK);

	/* Défini le type de zone en fonction de la quantité de mémoire demandée */
	zone_type = define_zone_type(request_size);

	/* Recherche d'un block libre existant et pouvant accueillir la taille demandée */
	if ((block = search_free_block(zone_type, request_size)) && block)
		goto _FREE_BLOCK_FOUND_;	
	/* Si aucun bloque trouvé/crée dans aucune zone, création d'une nouvelle pagination */
	else if (!new_zone && (!(new_zone = create_pagination(zone_type, request_size)))) {
		(DEBUG) ? message("Create new pagination has been failed: create_new_pagination()") : 0;
		return (NULL);
	}
	else {
		
		/* Placement de la nouvelle pagination virtuelle a la fin de la liste zone correspondante */
		if (!heap[zone_type]) {
			heap[zone_type] = new_zone;
		}
		else {
			t_zone* last_node = get_the_end_of_zone_list(heap[zone_type]);
			last_node->next = new_zone;
			new_zone->prev = last_node;
		}
	}
	

	/* Création du block dans la nouvelle zone */
	if (zone_type != LARGE_ZONE)
		block = create_memory_block(new_zone->blocks, request_size);
	else 
		block = new_zone->blocks;

	_FREE_BLOCK_FOUND_:
	return (GET_DATA_ADDR_FROM_BLOCK(block));
}

void* malloc(size_t size) {
	void* addr;

	pthread_mutex_lock(&g_mutex);
	addr = malloc_init(size);
	pthread_mutex_unlock(&g_mutex);

	return (addr);
}

