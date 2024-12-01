/*
Author: Rui Wang
Class : ECE6122
Last Date Modified : Dec 1st, 2024

Description:
The chess engine class, including initialEngine, sendMove, and getResponseMove methods.
*/

#include "ECE_ChessEngine.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <fcntl.h>
#endif

#include <iostream>
#include <thread>
#include <chrono>

// Deconstructor
ECE_ChessEngine::~ECE_ChessEngine()
{
#ifdef _WIN32
    if (hInputWrite) CloseHandle(hInputWrite);
    if (hInputRead) CloseHandle(hInputRead);
    if (hOutputWrite) CloseHandle(hOutputWrite);
    if (hOutputRead) CloseHandle(hOutputRead);
#else
    if (inputPipes[0] != -1) close(inputPipes[0]);
    if (inputPipes[1] != -1) close(inputPipes[1]);
    if (outputPipes[0] != -1) close(outputPipes[0]);
    if (outputPipes[1] != -1) close(outputPipes[1]);

    // Terminate the child process if it's still running
    if (enginePid > 0) {
        kill(enginePid, SIGTERM);
        waitpid(enginePid, NULL, 0);
    }
#endif
}

// Add movement into movement history
void ECE_ChessEngine::addMovement(std::string uMovement)
{
    movementHistory.push_back(uMovement);
}

// Get movement history in string
std::string ECE_ChessEngine::getMovements()
{
    std::string movements = "";
    size_t length = movementHistory.size();
    for (int i = 0; i != length; i++)
        movements += movementHistory[i] + (i == length - 1 ? "" : " ");
    return movements;
}

// Initailize chess engine
bool ECE_ChessEngine::InitializeEngine()
{
#ifdef _WIN32
    // Windows-specific initialization (as in your original code)
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0);
    CreatePipe(&hInputRead, &hInputWrite, &sa, 0);

    // Start the Komodo engine
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hInputRead;
    si.hStdOutput = hOutputWrite;
    si.hStdError = hOutputWrite;

    // Path to Komodo executable
    std::string enginePath = komodoAddres;
    if (!CreateProcess(NULL, const_cast<char*>(enginePath.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

#else
    // Linux-specific initialization
    if (pipe(inputPipes) == -1 || pipe(outputPipes) == -1) {
        std::cerr << "Failed to create pipes" << std::endl;
        return false;
    }

    enginePid = fork();
    if (enginePid == -1) {
        std::cerr << "Failed to fork" << std::endl;
        return false;
    }

    if (enginePid == 0) {  // Child process
        // Redirect stdin and stdout
        dup2(inputPipes[0], STDIN_FILENO);
        dup2(outputPipes[1], STDOUT_FILENO);

        // Close unused pipe ends
        close(inputPipes[1]);
        close(outputPipes[0]);

        // Execute the Komodo engine
        execl(komodoAddres.c_str(), komodoAddres.c_str(), NULL);
        exit(1);  // Only reached if execl fails
    }

    // Parent process
    close(inputPipes[0]);
    close(outputPipes[1]);
#endif

    // Common initialization code
    try {
        sendToEngine("uci");
        sendToEngine("isready");
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Send history movement to chess engine
bool ECE_ChessEngine::sendMove()
{
    try {
        std::string command = "position startpos moves " + getMovements();
        sendToEngine(command);
        command = "go depth " + std::to_string(SEARCH_DEPTH);
        sendToEngine(command);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Get best movement response from chess engine
bool ECE_ChessEngine::getResponseMove(std::string& strMove)
{
    try {
        std::string response;
        while ((response = readFromEngine()).find("bestmove") == std::string::npos) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        int index = (int)response.find("bestmove") + 9;
        strMove = response.substr(index, 4);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Helper function -- send command to chess engine
void ECE_ChessEngine::sendToEngine(const std::string& command)
{
#ifdef _WIN32
    DWORD written;
    if (!WriteFile(hInputWrite, command.c_str(), (DWORD)command.length(), &written, NULL) ||
        !WriteFile(hInputWrite, "\n", 1, &written, NULL)) {
        throw std::runtime_error("Failed to write command to engine.");
    }
#else
    ssize_t bytesWritten = write(inputPipes[1], (command + "\n").c_str(), command.length() + 1);
    if (bytesWritten == -1) {
        throw std::runtime_error("Failed to write command to engine.");
    }
#endif
}

// Helper function -- read response from chess engine
std::string ECE_ChessEngine::readFromEngine()
{
#ifdef _WIN32
    char buffer[READ_BUFFER_SIZE];
    DWORD read;
    if (!ReadFile(hOutputRead, buffer, sizeof(buffer) - 1, &read, NULL)) {
        throw std::runtime_error("Failed to read from engine.");
    }
    buffer[read] = '\0';
    return std::string(buffer);
#else
    char buffer[READ_BUFFER_SIZE];
    ssize_t bytesRead = read(outputPipes[0], buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        throw std::runtime_error("Failed to read from engine.");
    }
    buffer[bytesRead] = '\0';
    return std::string(buffer);
#endif
}
