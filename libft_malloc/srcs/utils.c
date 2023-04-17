#include "../includes/libft_malloc.h"

int ft_strlen(char* s) {
	if (!s)
		return (0);

	int i = 0;

	for (; s[i++];);
	
	return (i);
}

void message(char* s) {
	if (!s)
		return ;

	write(STDERR_FILENO, s, ft_strlen(s));
	write(STDERR_FILENO, "\n", 1);

}

/* Arrondi le nombre passé en paramètre au nombre multiple de 4096 le plus proche */
// https://researchmaniacs.com/Math/Multiples/What-Are-The-Multiples-Of-4096.html
size_t rounded_to_nearest_multiple_of(size_t number, size_t multiple) {
	if (!number || !multiple)
		return (0);
	// for (; (number % multiple) != 0 ; number++);
	number += (multiple - (number % multiple));

	return (number);
}

/* Parcours une liste chainée et renvoit un pointeur sur le dernier noeud */
t_zone* get_the_end_of_zone_list(t_zone* heap_start) {
	t_zone* tmp = heap_start;
	
	for (; tmp && tmp->next; tmp = tmp->next);

	return (tmp);
}

/* Retrouve l'adresse du block mémoire depuis le pointeur sur la donnée */
t_block* get_block_addr_from_data(void* data) {
	if (!data)
		return (NULL);

	t_block* block;

	block = data -= STRUCT_BLOCK_SIZE;
	return (block);
}

/* Retrouve l'adresse de la zone mémoire depuis le pointeur sur le block */
t_zone* get_zone_addr_from_block(void* block) {
	if (!block)
		return (NULL);
		
	t_zone* zone = block -= STRUCT_ZONE_SIZE;

	return (zone);
}

/* Renvoi le type de zone en fonction de la taille demandée */
int define_zone_type(size_t request_size) {
	int zone_type = 0;

	(request_size >= MIN_DATA_SIZE_TINY_BLOCK  && request_size <= MAX_DATA_SIZE_TINY_BLOCK)  ? zone_type = TINY_ZONE  : 0;
	(request_size >= MIN_DATA_SIZE_SMALL_BLOCK && request_size <= MAX_DATA_SIZE_SMALL_BLOCK) ? zone_type = SMALL_ZONE : 0;
	(request_size >= MIN_DATA_SIZE_LARGE_BLOCK) ?  zone_type = LARGE_ZONE : 0;

	return (zone_type);
}
