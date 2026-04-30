all: p1_memory p2_memory p1_async p2_async

p1_memory: p1_memory.cu
	nvcc -o p1_memory p1_memory.cu

p2_memory: p2_memory.cu
	nvcc -o p2_memory p2_memory.cu

p1_async: p1_async.cu
	nvcc -o p1_async p1_async.cu

p2_async: p2_async.cu
	nvcc -o p2_async p2_async.cu

clean:
	rm -f p1_memory p2_memory p1_async p2_async