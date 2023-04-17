#include "../includes/libft_malloc.h"

void* realloc_init(void* data, size_t new_request_size) {
	/* Récuperation de l'adresse du block en fonction de l'adresse de la donnée */
	t_block* block = get_block_addr_from_data(data);
	new_request_size = ALIGN_PTR(new_request_size);

	/* Si le block actuel est suffisant */
	if (block->size >= new_request_size) 
		goto _SPLIT_NEW_MEMORY_BLOCK_WITH_REST;
	/* Coalesce avec le block suivant si disponible et que son espace est suffisant pour realloc */
	else if (block->next && block->next->free == true
		&& (block->size + STRUCT_BLOCK_SIZE + block->next->size) >= new_request_size)
			block = realloc_with_next_block(block);	
	/* Coalesce avec le block précédent si disponible et que son espace est suffisant pour realloc */
	else if (block->prev && block->prev->free == true
		&& (block->size + STRUCT_BLOCK_SIZE + block->prev->size) >= new_request_size)
			block = realloc_with_prev_block(block);
	/* Coalesce avec le block précédent et suivant si disponibles et que l'addition de leur espace est suffisant pour realloc */
	else if (block->next && block->next->free == true
			&& block->prev && block->prev->free == true
			&& block->size + ((STRUCT_BLOCK_SIZE * 2) + (block->next->size + block->prev->size)) >= new_request_size) {
		block = realloc_with_prev_block(block);
		block = realloc_with_next_block(block);
	}
	/* Creation d'un nouveau block sur-mesure */
	else {

		if ((block = realloc_with_new_block_creation(block, new_request_size, data)) && !block)
			return (NULL);

		goto _RETURN_;
	}

	_SPLIT_NEW_MEMORY_BLOCK_WITH_REST:
	if (block->size - new_request_size >= (STRUCT_BLOCK_SIZE + PTR_SIZE))
		block = create_memory_block(block, new_request_size);

	_RETURN_: 
	return (GET_DATA_ADDR_FROM_BLOCK(block));
}

void* realloc(void *ptr, size_t size) {
	if (!ptr)
		return (malloc(size));
	else if (check_block(ptr) == false)
		return (NULL);
	
	if (!size && ptr) {
		free_init(ptr);
		return (malloc(MIN_DATA_SIZE_TINY_BLOCK));
	}

	void* addr;

	pthread_mutex_lock(&g_mutex);
	addr = realloc_init(ptr, size);
	pthread_mutex_unlock(&g_mutex);
	
	return (addr);
}
