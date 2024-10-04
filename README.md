# ECE6122 Lab2

## Sep 30th 2024
- Finish the code in single thread and multiple thread. 

- The left task is to implement OpenMP.

## Oct 1st 2024
- Finished implementing OpenMP and checked the invalid argument input.

- The next improvement is to tune the number of thread for both Thread and OpenMP, finally reflecting the advantages of using multi-threads.

## Oct 3rd 2024
- Update the time count method for 100 generations, now it excludes the time for drawing.

- However, there seems to be a very potential improvement for copying new grid to original grid. It seems that the directly assign (deep copy) 
method for vector<vector<Sprit>> will cost a lot of time, which maybe three times of the update grid method.

- The proposed solution is to build a new class that contains the origin spirt and also include a new state LIVE aganist DEATH. This may save a
lot of executing time when updating grid. 

## Oct 4th 2024
- Add a new structure named Grid to update the live state of grid instead of directly using the sprite class from SFML.

- Debug the activation of OpenMP by adding the OpenMP packet checking in the CMakeLists.txt

- The potential improvement could be using one-dimensional vector instead of two-dimensional vector to improve the memory access (continous memory
is much faster than discrete memory). 