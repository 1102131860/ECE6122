/*
Author: Rui Wang
Class : ECE6122
Last Date Modified : Dec 1st, 2024

Description:
The chess engine class, including initialEngine, sendMove, and getResponseMove methods. 
*/

#ifndef ECE_CHESSENGINE_H
#define ECE_CHESSENGINE_H

#include <string>
#include <vector>

// Platform-specific handle type
#ifdef _WIN32
    #define KOMODO_ADDRESS "./Windows/komodo-14.1-64bit.exe"
    typedef void* HANDLE;
#else
    #include <unistd.h>
    #define KOMODO_ADDRESS "./Linux/komodo-14.1-linux"
    typedef pid_t ProcessHandle;
#endif

#define SEARCH_DEPTH 10
#define READ_BUFFER_SIZE 4096

class ECE_ChessEngine
{
public:
    // Constructor
    ECE_ChessEngine(std::string address) : komodoAddres(address) {};
    // Deconstructor
    ~ECE_ChessEngine();

    // Add movement into movement history
    void addMovement(std::string uMovement);
    // Get movement history in string
    std::string getMovements();
    // Initailize chess engine
    bool InitializeEngine();
    // Send history movement to chess engine
    bool sendMove();
    // Get best movement response from chess engine
    bool getResponseMove(std::string& strMove);

private:
    // Helper function -- send command to chess engine
    void sendToEngine(const std::string& command);
    // Helper function -- read response from chess engine
    std::string readFromEngine();

    std::vector<std::string> movementHistory;
    const std::string komodoAddres;

    // Platform-specific handles
#ifdef _WIN32
    HANDLE hInputWrite = NULL, hInputRead = NULL;
    HANDLE hOutputWrite = NULL, hOutputRead = NULL;
#else
    int inputPipes[2];   // Read and write pipes for input
    int outputPipes[2];  // Read and write pipes for output
    pid_t enginePid = -1;
#endif
};

#endif // !ECE_CHESSENGINE_H
