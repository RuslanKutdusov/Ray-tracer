#ifndef CUDA_HPP_
#define CUDA_HPP_

#ifdef __CUDACC__
#include <cuda_runtime.h>
#include <vector_types.h>
#include <vector_functions.h>
#define HOST_DEVICE __host__ __device__
#define HOST __host__
#else
#define HOST
#define HOST_DEVICE
#endif


#endif /* CUDA_HPP_ */
