#include <windows.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Data.h"

#define TIMEOUT_MS 10000

using std::string;
using std::cout;
using std::endl;

inline void printRed(string s) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    cout << s;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

class Solver {
    Data data;

    std::mutex mx;
    std::condition_variable cv;
    std::thread threads[3];
    std::atomic<bool> isConnected = false;

    OVERLAPPED overLap;
    HANDLE hEvents[2];
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    HANDLE hExit = INVALID_HANDLE_VALUE;
    void pipeWriter();
    void pipeReader();
    void controller();

public:
    Solver() {
        controller();
    }
};