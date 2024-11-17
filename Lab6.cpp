#include "mpi.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <math.h>

#define LOWER_LIMIT 0
#define UPPER_LIMIT 1
#define MASTER 0        // task ID of master task

#define square(x) ((x)*(x))
double integral(int num, int problem)
{
    std::srand(static_cast<unsigned>(std::time(0)));
    double sum = 0;
    for (int i = 0; i < num; i++) 
    {
        double randomValue = static_cast<double>(std::rand()) / RAND_MAX;
        double x = problem == 2 ? exp(-1 * square(randomValue)) : square(randomValue);
        sum += x;
    }

    double avg = sum / num;
    double result = (double) (UPPER_LIMIT - LOWER_LIMIT) * avg;
    return result;
}

int main(int argc, char *argv[])
{   
    // Problem to solve and total samples
    int problem, totalNum;

    // Obtain the arguments from command line
    for (int i = 1; i < argc; i += 2)
    {
        std::string argKey = argv[i];
        std::string argValue = (i + 1 < argc) ? argv[i + 1] : "";

        if (argKey == "-P" && !argValue.empty())
        {
            try 
            {
                problem = std::stoi(argValue);
            }
            catch (const std::invalid_argument& e)
            {
                std::cout << "Invalid argument (-P): " << e.what() << std::endl;
            }
            catch (const std::out_of_range& e) 
            {
                std::cout << "Out of range (-P): " << e.what() << std::endl;
            }
            if (problem != 1 && problem != 2)
            {
                std::cout << "Please type 1 or 2 for argument (-P)" << std::endl;
                problem = 1;
            }
        }
        else if (argKey == "-N" && !argValue.empty())
        {
            try 
            {
                totalNum = std::stoi(argValue);
            }
            catch (const std::invalid_argument& e)
            {
                std::cout << "Invalid argument (-N): " << e.what() << std::endl;
            }
            catch (const std::out_of_range& e) 
            {
                std::cout << "Out of range (-N): " << e.what() << std::endl;
            }
            if (totalNum == 0)
            {
                std::cout << "Please type a non-negative integer for argument (-N)" << std::endl;
                totalNum = 1000000;
            }
        }
    }

    int rc = MPI_Init(&argc, &argv); 
    if (rc != MPI_SUCCESS) 
    {
        printf("Error starting MPI program. Terminating!.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    // Task id and number of tasks
    int	taskid, numtasks;

    // Obtain number of tasks and task ID
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    // Obtain the chunck size for each machine
    int chunkSize = totalNum / numtasks;
    std::cout << "MPI task " << taskid << " with a chunk size of " << chunkSize << " has started ... " << std::endl;
    
    // Calculate the intgeral at each node
    double homeIntegral = integral(chunkSize, problem);
    
    double sumIntegral = 0;
    // User MPI reduction (SUM) to simplifize the communication between mahcines
    // int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
    rc = MPI_Reduce(&homeIntegral, &sumIntegral, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
    if (rc != MPI_SUCCESS) 
    {
        printf("Error starting MPI program. Terminating!.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    
    // Output the 
    if (taskid == MASTER)
    {
        double avgIntegral = sumIntegral / numtasks;
        std::cout << "The estimate for integral Problem " << problem << " is: " << avgIntegral << std::endl;
        std::cout << "The real value of Problem " << problem << " is: " << (problem == 2 ? 0.746824 : 0.333333) << std::endl;
    }

    MPI_Finalize();
    return 0;
}
