#include "../includes/libft_malloc.h"

/* Supprime une zone dans une liste zone */
void delete_pagination(t_zone* zone) {
	if (zone->prev) {
		/* Fait pointer le pointeur next de la zone précédente sur la zone suivante de celle qui va être supprimée */
		zone->prev->next = zone->next;
		/* Fait pointer le pointeur précédent de la zone suivante sur la zone précédente de celle qui va être supprimée */
		if (zone->next)
			zone->next->prev = zone->prev;
	}

	/* Si zone précédente == NULL nous somme sur la premiere zone de la liste */
	else if (!zone->prev && zone->next) {
		int zone_type = 0;

		for (; heap[zone_type] != zone; zone_type++);
		/* Mise de la zone suivante en tant que premiere zone de la liste */
		heap[zone_type] = zone->next;
		/* Mise du pointeur prev de la nouvelle premiere zone à NULL */
		zone->next->prev = NULL;
	}
	/* Conserve la premiere zone d'une liste zone TINY ou SMALL meme si totalement libre */
	else if (!zone->prev && !zone->next && zone->size <= rounded_to_nearest_multiple_of(GET_TOTAL_ZONE_SIZE(MAX_DATA_SIZE_SMALL_BLOCK), PAGE_SIZE))
			return ;

	if (munmap(zone, zone->size) == -1)
		(DEBUG) ? message("Munmaping zone has been failed\n") : 0;
}

/* Demande de pagination dans la mémoire virtuelle du système d'exploitation */
void* request_memory_from_os(size_t rounded_zone_size) {
	void* addr = NULL;
	
	if ((addr = mmap(NULL, rounded_zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
		(DEBUG) ? message("Map failed : mmap()") : 0;
		return (NULL);
	}

	return (addr);
}

/* Initie une structure zone */
void zone_struct_init(t_zone* zone, size_t rounded_size) {
	zone->next = NULL;
	zone->prev = NULL;
	zone->size = rounded_size;

	/* initialisation du premier block de la zone nouvellement crée */
	zone->blocks = INIT_FIRST_BLOCK_ADDR(zone);
	zone->blocks->size = INIT_FIRST_BLOCK_SIZE(rounded_size);
	zone->blocks->next = NULL;
	zone->blocks->prev = NULL;
	zone->blocks->free = true;
}

/* Pré-allocation de la zone mémoire et initialisation du premier block libre de la structure zone crée */
void* create_pagination(int zone_type, size_t request_size) {
	t_zone* addr = NULL;
	size_t zone_size, rounded_zone_size = 0;

	(zone_type == TINY_ZONE) ? zone_size = GET_TOTAL_ZONE_SIZE(MAX_DATA_SIZE_TINY_BLOCK) : 0;
	(zone_type == SMALL_ZONE) ? zone_size = GET_TOTAL_ZONE_SIZE(MAX_DATA_SIZE_SMALL_BLOCK) : 0;
	(zone_type == LARGE_ZONE) ? zone_size = GET_TOTAL_LARGE_ZONE_SIZE(request_size) : 0;

	/* Arrondissement de la taille au multiple de getpagesize() le plus proche (multiple: 4096) */
	if (!(rounded_zone_size = rounded_to_nearest_multiple_of(zone_size, PAGE_SIZE))) {
		(DEBUG) ? message("Rounded to the nearest multiple failed : rounded_to_nearest_multiple_of()") : 0;
		return (NULL);
	}

	/* Pré-allocation de la zone mémoire > demande de pagination */
	if (!(addr = (t_zone*) request_memory_from_os(rounded_zone_size))){
		(DEBUG) ? message("Memory allocation failed : request_memory_from_os()") : 0;
		return (NULL);
	}

	zone_struct_init(addr, rounded_zone_size);

	(zone_type == LARGE_ZONE) ? addr->blocks->free = false : 0;

	return (addr);
}
