# Défini la variable d'environnement HOSTTYPE si non définie.
# (-s = kernel name / -m = machine hardware name)
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

CC					= gcc
CFLAGS				= -Wall -Wextra -Werror -fPIC 
# fPIC = Position Independent Code : permet l'excecution du code nimporte ou dans la memoire
DLFLAGS				= -shared 
RM 					= rm -rf

DIR_LIBFT_MALLOC 	= ./libft_malloc
SRCS_LIBFT_MALLOC 	= $(DIR_LIBFT_MALLOC)/srcs/
INCS_LIBFT_MALLOC 	= $(DIR_LIBFT_MALLOC)/includes/
OBJS_LIBFT_MALLOC	= $(DIR_LIBFT_MALLOC)/objs/

SOURCES				= malloc.c realloc.c free.c block.c utils.c zone.c show.c

OBJS 				= $(SOURCES:%.c=$(OBJS_LIBFT_MALLOC)%.o)
TARGET 				= libft_malloc.so
DYN_LIB 			= libft_malloc_$(HOSTTYPE).so

all: $(TARGET)

$(TARGET): $(DYN_LIB)
	ln -fs $(DYN_LIB) $(TARGET)
# -f override if allready exist
# -s symbolic link

# Shared library
$(DYN_LIB): $(OBJS)
	$(CC) $(CFLAGS) $(DLFLAGS) $(OBJS) -o $(DYN_LIB)

# compiled sources :
$(OBJS): | $(OBJS_LIBFT_MALLOC)

$(OBJS_LIBFT_MALLOC)%.o: $(SRCS_LIBFT_MALLOC)%.c
	$(CC) $(CFLAGS) -I $(INCS_LIBFT_MALLOC) -c $< -o $@

$(OBJS_LIBFT_MALLOC):
	@mkdir -p $(OBJS_LIBFT_MALLOC)

clean: 
	@$(RM) $(OBJS_LIBFT_MALLOC)
	@$(RM) a.out

fclean: clean
	@$(RM) $(TARGET)
	@$(RM) $(DYN_LIB)

re: fclean all

.PHONY: all clean fclean re



# compile with libft_malloc
# gcc -o someoutput somefiles.c -L. -lft_malloc
# -L. regarde dans le repertoire courant pour la librairie
# -l link le programme avec la librairie libft_malloc.so / -lft_malloc = -libft_malloc
