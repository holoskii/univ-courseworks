#include <stdio.h>
#include "headers/piper.h"

#define BUFSIZE 5000

Piper::Piper(){}

int Piper::connect() {
    hPipe = CreateFile(
        L"\\\\.\\pipe\\ICS0025",
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) return 1;
    return 0;
}

void Piper::stop() {
    wchar_t command[] = L"00Stop";
    int len = sizeof(command);
    memcpy(command, &len, sizeof(int));

    WriteFile(hPipe, command, (DWORD)len, NULL, NULL);
    if (hPipe != NULL) CloseHandle(hPipe);
    hPipe = NULL;
}

int Piper::sendMessage(wchar_t* command, double xBegin, double xEnd, int nPoints, int order) {
    lastNPoints = nPoints;
    char message[100] = { 0 };
    int n = 4;
    memcpy(message + n, command, (wcslen(command) + 1) * sizeof(wchar_t));
    n += (wcslen(command) + 1) * sizeof(wchar_t);

    double d = xBegin;
    memcpy(message + n, &d, sizeof(d));
    n += sizeof(d);

    d = xEnd;
    memcpy(message + n, &d, sizeof(d));
    n += sizeof(d);

    int j = nPoints;
    memcpy(message + n, &j, sizeof(j));
    n += sizeof(j);

    if (wcscmp(command, L"Bessel function") == 0) {
        j = order;
        memcpy(message + n, &j, sizeof(j));
        n += sizeof(j);
    }

    memcpy(message, &n, sizeof(n));

    DWORD cbWritten, cbToWrite;
    cbToWrite = n;
    BOOL fSuccess = WriteFile(
        hPipe, message,     // message
        cbToWrite,              // message length
        &cbWritten, NULL);      // bytes written
    if (!fSuccess) {
        printf("WriteFile to pipe failed. GLE=%lu\n", GetLastError());
        return 1;
    }

    printf("\nMessage sent to server, receiving reply as follows:\n");
    return 0;
}

double* Piper::readData(int *pSize) {
    BOOL fSuccess;
    DWORD  cbRead;
    int packageSize = 0;

    fSuccess = ReadFile(
        hPipe, &packageSize, sizeof(int),
        &cbRead, NULL);
    char* chBuf = (char*) malloc(sizeof(char) * packageSize);

    memcpy(chBuf, &packageSize, sizeof(int));
    fSuccess = ReadFile(
        hPipe, chBuf + sizeof(int), packageSize * sizeof(char) - sizeof(int),
        &cbRead, NULL);


    int size = 0;
    double* temp = parse(&size, chBuf);
    *pSize = size;
    free(chBuf);

    return temp;

    if (!fSuccess) {
        printf("Reading wasn't successfull\n");
    }

    if (!fSuccess) {
        printf("ReadFile from pipe failed. GLE = %lu\n", GetLastError());
        return NULL;
    }
}

double* Piper::parse(int* pSize, char* str) {
    int n;
    memcpy(&n, str, sizeof(int));

    const int mesLen = 32;
    wchar_t message[mesLen];

    int i = 4, j = 0;
    while ((i + 1 < n) && (j + 1 < mesLen) && (str[i] + str[i + 1] != 0)) {
        j += 1;
        i += 2;
    }
    memcpy(message, str + 4, sizeof(wchar_t) * (j + 1));

    if (wcscmp(message, L"Curve") != 0) {
        wprintf(L"Incorrect message: %ls\n", message);
        return NULL;
    }


    if ((n - 16) % (2 * sizeof(double)) != 0) {
        printf("Sizes wrong\n");
        return NULL;
    }

    double* arr = (double*)malloc(n - 16);
    if (arr == NULL) {
        printf("Memory allocation error\n");
        return NULL;
    }

    memcpy(arr, str + 16, (n - 16));
    *pSize = (n - 16) / sizeof(double);

    for (int i = 0; i < *pSize / 2; i++) {
        double x = arr[i * 2];
        double y = arr[i * 2 + 1];
        if (x < -1000000 || x > 1000000) arr[i * 2] = 0;
        if (y < -1000000 || y > 1000000) arr[i * 2 + 1] = 0;
    }

    return arr;
}

double* Piper::getData(int* pSize, wchar_t* command, double xBegin, double xEnd, int nPoints, int order) {
    if (sendMessage(command,xBegin, xEnd, nPoints, order) != 0) return NULL;
    double* data = readData(pSize);
    return data;
}
