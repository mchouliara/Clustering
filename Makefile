cluster:
OBJS 	= main.o lsh.o filedata.o math_functions.o cluster.o
SOURCE	= main.cpp lsh.cpp filedata.cpp math_functions.cpp cluster.cpp
HEADER  = lsh.h filedata.h cluster.h math_functions.h
OUT  	= cluster
CC	= g++
FLAGS   = -g -c -pedantic  -Wall

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@ -lm


clean:
	rm -f $(OBJS) $(OUT)