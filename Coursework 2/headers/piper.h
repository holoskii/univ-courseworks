#pragma once
#include <windows.h>

class Piper {

private:
    HANDLE hPipe = NULL;
    int lastNPoints = 0;
public:

    Piper();
    int connect();
    void stop();
    int sendMessage(wchar_t* command, double xBegin, double xEnd, int nPoints, int order);
    double* readData(int* pSize);
    double* parse(int* pSize, char* str);
    double* getData(int* pSize, wchar_t* command, double xBegin, double xEnd, int nPoints, int order);
};

