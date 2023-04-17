#include "../includes/libft_malloc.h"

/* Verifie l'existence d'un block en parcourant tous les blocks de chaque zone */
bool check_block(void* ptr) {
	t_block* to_find = get_block_addr_from_data(ptr);
	t_block* block;
	t_zone* zone;
	
   for (int zone_type = TINY_ZONE; zone_type <= LARGE_ZONE; zone_type++) {
        if (heap[zone_type]) {
			zone = heap[zone_type];
			for (; zone; zone = zone->next) {
				block = zone->blocks;
				for (; block; block = block->next) {
					if (block == to_find)
						return (true);
				}
			}
		}
    }
	return (false);
}

/* Création d'un nouveau block dans le cas ou aucune coalescion possible avec un block précédent et/ou suivant */
t_block* realloc_with_new_block_creation(t_block* block, size_t new_request_size, void* data) {
	void* new_alloc;
	t_block* old = block;
	
	if (!(new_alloc = malloc_init(new_request_size)))
		return (NULL);

	copy_data_memory_block(new_alloc, GET_DATA_ADDR_FROM_BLOCK(old), old->size);
	block = get_block_addr_from_data(new_alloc);
	free_init(data);

	return (block);
}

/* Realloue de la mémoire grace à celle du block précédent */
t_block* realloc_with_prev_block(t_block* block) {
	block->free = true;
	copy_data_memory_block(GET_DATA_ADDR_FROM_BLOCK(block->prev), GET_DATA_ADDR_FROM_BLOCK(block), block->size);
	return (coalesce_memory_block(block->prev));
}

/* Realloue de la mémoire grace à celle du block suivant */
t_block* realloc_with_next_block(t_block* block) {
	return (coalesce_memory_block(block));
}

/* Copie la donnée d'un block mémoire dans un autre */
void copy_data_memory_block(void* dst, void* src, size_t len) {
	for (size_t i = 0; i * sizeof(int) < len; i++)
        ((int*)dst)[i] = ((int*)src)[i];
}

/* Fusionne le block libre suivant avec le block courant */
t_block* coalesce_memory_block(t_block* block) {
	if (block->next && block->next->free == true) {
		size_t tmp_size = block->size;
		block->size = COALESCE_NEXT_MEMORY_BLOCK_SIZE(tmp_size, block->next->size);
		block->next = block->next->next;
		if (block->next)
			block->next->prev = block;
	}
	return (block);
}

/* Créait un nouveau block à l'adresse du block prit en paramètre */
t_block* create_memory_block(t_block* block, size_t request_size) {
	t_block* new_next = NULL;
	
	new_next = INIT_NEW_NEXT_BLOCK_ADDR(block, request_size);
	new_next->size = INIT_NEW_NEXT_BLOCK_SIZE(block->size, request_size);
	new_next->next = block->next;
	new_next->prev = block;
	new_next->free = true;
	
	if (new_next->next)
		new_next->next->prev = new_next;
	
	block->size = request_size;
	block->next = new_next;
	block->free = false;

	return (block);
}

/* Recherche un block libre dont la taille est la plus proche de celle demandée */
t_block* search_free_block(int zone_type, size_t request_size) {
	if (zone_type == LARGE_ZONE)
		return (NULL);

	t_zone* zone;
	t_block* block = NULL;
	t_block* best = NULL;
	size_t tmp, best_size = 0;

	/* Itère à travers toutes les zones correspondantes et tous les blocks de chaque zone */
	zone = heap[zone_type];
	for (; zone; zone = zone->next) {
		block = zone->blocks;
		for (; block; block = block->next) {
			/* Algorithme Best-fit */
			if ((block->size >= request_size && block->free == true)) {
				tmp = GET_GAP(block->size, request_size); // calcul la différence entre la taile demandée et celle du block trouvé
				if (tmp == 0)
					return (block);
				else if (tmp < best_size || best_size == 0) {
					best = block;
					best_size = block->size;
				}
			}
		}
	}
	if (best && best->size > request_size)
		best = create_memory_block(best, request_size);
	return ((best) ? best : NULL);
}
