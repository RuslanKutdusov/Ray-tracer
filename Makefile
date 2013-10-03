all:
	nvcc -ccbin gcc -c -O2 raytracer_cuda.cu -o raytracer_cuda.o
	nvcc -ccbin gcc -c -O2 Texture.cpp -o Texture.o
	nvcc raytracer_cuda.o Texture.o -o raytracer -lpng
	