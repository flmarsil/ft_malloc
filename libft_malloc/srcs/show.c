#include "../includes/libft_malloc.h"

void			ft_putnbr(size_t n) {
	int				i;
	char			str[12];
	size_t			nnbr;
	int				size;

	size = 0;
	nnbr = n;
	while (nnbr)
		nnbr /= 10 + 0 * size++;
	nnbr = n;
	if (!size)
		str[size++] = '0';
	str[size] = '\0';
	i = size;
	while (i--)
	{
		str[i] = (nnbr % 10) + 48;
		nnbr /= 10;
	}
	write(STDOUT_FILENO, str, ft_strlen(str));
}

void			ft_puthexa(unsigned long n) {
	int				i;
	char			str[20];
	unsigned long	nnbr;
	int				size;

	size = 0;
	nnbr = n;
	while (n)
		n /= 16 + 0 * size++;
	if (!size)
		str[size++] = '0';
	str[size] = '\0';
	i = size;
	n = nnbr;
	while (i--)
	{
		if (nnbr % 16 < 10)
			str[i] = (nnbr % 16) + 48;
		else
			str[i] = (nnbr % 16) + (65 - 10);
		nnbr /= 16;
	}
	write(STDOUT_FILENO, str, ft_strlen(str));
}

void		print_block(t_block *block) {
	write(STDOUT_FILENO, "0x", 2);
	ft_puthexa((unsigned long)GET_DATA_ADDR_FROM_BLOCK(block));
	write(STDOUT_FILENO, " - ", 3);
	write(STDOUT_FILENO, "0x", 2);
	ft_puthexa((unsigned long)block + block->size);
	write(STDOUT_FILENO, " : ", 3);
	ft_putnbr(block->size);
	write(STDOUT_FILENO, " bytes\n", 7);
}

size_t get_blocks(t_block* block) {
    size_t sum_size_blocks = 0;
	
    for (; block; block = block->next) {
		if (block->free == false) {
        	print_block(block);
        	sum_size_blocks += block->size;
		}
    }

    return (sum_size_blocks);
}

void print_zone(t_zone* zone, int zone_type) {
    char* result;

    (zone_type == TINY_ZONE) ? result = "TINY : 0x" : 0;
    (zone_type == SMALL_ZONE) ? result = "SMALL : 0x" : 0;
    (zone_type == LARGE_ZONE) ? result = "LARGE : 0x" : 0;
    
    write(STDOUT_FILENO, result, ft_strlen(result));
    ft_puthexa((unsigned long)zone + STRUCT_ZONE_SIZE);
    write(STDOUT_FILENO, "\n", 1);
}

size_t get_zones(t_zone* zone, int zone_type) {
    size_t sum_size_blocks = 0;

    for (; zone; zone = zone->next) {
        print_zone(zone, zone_type);
        sum_size_blocks += get_blocks(zone->blocks);
    }

    return (sum_size_blocks);
}

void show_alloc_mem(void) {
    size_t total_bytes = 0;

    pthread_mutex_lock(&g_mutex);

    for (int zone_type = TINY_ZONE; zone_type <= LARGE_ZONE; zone_type++)
        (heap[zone_type]) ? total_bytes += get_zones(heap[zone_type], zone_type) : 0;

    write(STDOUT_FILENO, "Total : ", 8);
	ft_putnbr(total_bytes);
	write(STDOUT_FILENO, " bytes\n", 7);

    pthread_mutex_unlock(&g_mutex);
}