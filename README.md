# ECE6122 Lab6: A Definite Integral using the Monte Carlo method using OpenMPI
## Use OpenMPI to estimate the value of a definite integral using the Monte Carlo method
## Problem 1: $\int_0^1 x^2 \, dx$
## Problem 2: $\int_0^1 e^{-x^2} \, dx$
### Nov 17 2024
* Use the MPI reduction (SUM) to finish this Lab.
* How to run:
On PACE clusters, compile: `mpic++ ./Lab6.cpp -o Lab6`  
execute: `srun ./Lab6 -P 1 -N 100000`  
    `-P`: the problem that you want to solve, please choose 1 or 2  
    `-N`: the total generated number of random samples to distribute acrocess processors  
