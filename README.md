# ECE6122 Lab 4 CUDA-based John Conway’s Game of Life
## Nov 2 2024
* Finished the **Normal Memory** Transfer, but haven't test it at the Cuda GPU.  
* The next stuffs need to finish are:  
    * **Pinned memory** AND **Managed Memory** transfer at the CUDA
    * Test the running time and correctify the code (it should be time for *pinned memory < time for Managed Memory < time for Normal Memory*)

## Nov 3 2024
* Instead of using the shared memory at yesterday, used two **Streaming Multiprocessors (SM)** to achieve **asychronous memory transfer** and transfer faster.
* The possible improvements could be:
    * In the kernel computation, use **Shared Memory** to expediate the single computing
    * For the task assignment function, pass the **Partitioned Memory** within SM to expediate the parallel computing 
    * To combine the above twp suggestions, try to use **Halo Algorithm** to ped the boundary and save into the shared/local memory to expediate
* By the way, the current computing time is: *pinned memory < normal memory < managed memory*