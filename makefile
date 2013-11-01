CC = clang++
CFLAGS = -O3 -msse3 -c -std=c++11

.cpp.o: 
	$(CC) $(CFLAGS) $< -o $@

all: Vector.o Texture.o main.o raytracer.o
	$(CC) -pthread -lpng  Vector.o Texture.o main.o raytracer.o -o raytracer

clean:
	rm *.o	

