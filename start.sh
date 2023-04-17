#!/bin/sh 

GREEN='\033[0;33m'
NC='\033[0m'

LIBRARY_NAME=libft_malloc
LIBRARY_EXTENSION=.so
LIBRARY=${LIBRARY_NAME}${LIBRARY_EXTENSION}

if [ -z "$1" ]
then
    echo You have to specify a main file...
    exit 1
fi

make re > /dev/null 2>&1

gcc ./tests/$1 $LIBRARY > /dev/null 2>&1
echo "${GREEN}\n------------------------------------------------ exec a.out :${NC}"
LD_LIBRARY_PATH=. LD_PRELOAD=libft_malloc.so /usr/bin/time -v ./a.out
echo "${GREEN}------------------------------------------------ \n${NC}"