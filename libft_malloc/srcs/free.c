#include "../includes/libft_malloc.h"

void free_init(void* data) {
	/* Récuperation de l'adresse du block en fonction de l'adresse de la donnée */
	t_block* block = get_block_addr_from_data(data);

	if (block->free)
		block->free = true;
	/* Coalescion du block courant avec le block précédent s'il est libre */
	if (block->prev && block->prev->free == true)
		block = coalesce_memory_block(block->prev);

	/* Coalescion du block courant avec le block suivant s'il est libre */
	if (block->next && block->next->free == true)
		block = coalesce_memory_block(block);

	/* Si tous les blocks sont libres, donc aucun block précédent et suivant : supression de la zone mémoire */
	if (!block->prev && !block->next && block->free == true)
		delete_pagination(get_zone_addr_from_block(block));
		
}

void free(void* ptr) {
	if (!ptr)
		return ;
	else if (check_block(ptr) == false)
		return ;

	pthread_mutex_lock(&g_mutex);
	free_init(ptr);
	pthread_mutex_unlock(&g_mutex);
}