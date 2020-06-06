#include "Data.h"
#include "main.h"

void Solver::pipeWriter() {

    hPipe = CreateFile(L"\\\\.\\pipe\\ICS0025", GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        printRed("cant open pipe\n");
        cout << "Writer thread is over" << endl;
        isConnected = false;
        cv.notify_one();
        return;
    }
    cout << "pipe opened" << endl;
    isConnected = true;

    while (isConnected) {
        std::unique_lock<std::mutex> lock(mx);
        // send message ready to server
        string message = string("ready");
        unsigned long cbWritten, cbToWrite = (message.size() + 1) * sizeof(char);
        bool fSuccess = WriteFile(hPipe,
            message.c_str(), cbToWrite, &cbWritten, &overLap);

        if (!fSuccess) {
            if (GetLastError() == ERROR_IO_PENDING) {
                switch (WaitForMultipleObjects(2, hEvents, FALSE, TIMEOUT_MS)) {
                case WAIT_OBJECT_0: break;
                case WAIT_OBJECT_0 + 1: break;
                case WAIT_TIMEOUT:
                    printRed("timeout while writing\n");
                    isConnected = false;
                    break;
                default:
                    printRed(string("writing failed #1, error: ") + std::to_string(GetLastError()));
                    cout << endl;
                    isConnected = false;
                    break;
                }
            }
            else {
                printRed(string("writing failed #2, error: ") + std::to_string(GetLastError()));
                cout << endl;
                isConnected = false;
            }
        }

        if (hPipe != INVALID_HANDLE_VALUE && isConnected) {
            GetOverlappedResult(hPipe, &overLap, &cbWritten, FALSE);
            cout << "message " << message << " send" << endl;
            if (cbWritten < cbToWrite) {
                cout << "incomplete message written: " << cbWritten << " from " << cbToWrite << endl;
            }
        }
        else {
            cout << "writer thread: pipe is not connected" << endl;
        }

        cv.notify_one();
        if (isConnected)
            cv.wait_for(lock, std::chrono::milliseconds(TIMEOUT_MS));
    }

    if (hPipe != INVALID_HANDLE_VALUE) {
        // send stop message to server
        string message = string("stop");
        bool fSuccess = WriteFile(hPipe, message.c_str(),
            (message.size() + 1) * sizeof(char), NULL, NULL);
        CloseHandle(hPipe);
    }

    isConnected = false;
    cout << "writer thread is over" << endl;
}

void Solver::pipeReader() {

    do {
        std::unique_lock<std::mutex> lock(mx);
        // wait until the connection is established and "ready" is sent
        cv.wait_for(lock, std::chrono::milliseconds(TIMEOUT_MS));

        if (hPipe == INVALID_HANDLE_VALUE) {
            cv.notify_one();
            break;
        }

        // read message from server
        const int BUFSIZE = 128;
        char chBuf[BUFSIZE] = { 0 };
        unsigned long cbRead;
        bool fSuccess = ReadFile(hPipe, chBuf,
            BUFSIZE * sizeof(char), &cbRead, &overLap);

        if (!fSuccess) {
            if (GetLastError() == ERROR_IO_PENDING) {
                unsigned long res = WaitForMultipleObjects(2, hEvents, FALSE, TIMEOUT_MS);
                if (hPipe == INVALID_HANDLE_VALUE)
                    continue;
                switch (res) {
                case WAIT_OBJECT_0: break;
                case WAIT_OBJECT_0 + 1: break;
                case WAIT_TIMEOUT:
                    printRed("timeout while reading\n");
                    isConnected = false;
                    break;
                default:
                    printRed(string("reading failed #1, error: ") + std::to_string(GetLastError()));
                    cout << endl;
                    isConnected = false;
                    break;
                }
            }
            else {
                printRed(string("reading failed #2, error: ") + std::to_string(GetLastError()));
                cout << endl;
                isConnected = false;
                break;
            }
        }

        if (hPipe != INVALID_HANDLE_VALUE && isConnected) {
            GetOverlappedResult(hPipe, &overLap, &cbRead, FALSE);
            if (cbRead > 10) data.Add(new Item(chBuf));
            cout << " >>> message: " << chBuf << endl;
        }
        else {
            cout << "from reader: incorrect pipe" << endl;
        }
        cv.notify_one();
    } while (isConnected && hPipe != INVALID_HANDLE_VALUE);
    cout << "reader thread is over" << endl;
}

void Solver::controller() {
    string msg;
    while (1) {
        std::cin >> msg;
        if (msg == string("connect")) {
            if (isConnected == true) {
                printRed("already connected\n");
                continue;
            }
            isConnected = true;

            memset(&overLap, 0, sizeof(overLap));
            overLap.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
            hExit = CreateEventA(NULL, FALSE, FALSE, NULL);

            hEvents[0] = overLap.hEvent; hEvents[1] = hExit;

            if (threads[1].joinable()) threads[1].join();
            if (threads[2].joinable()) threads[2].join();

            threads[1] = std::thread(&Solver::pipeReader, this);
            threads[2] = std::thread(&Solver::pipeWriter, this);
        }
        else if (msg == string("stop") || msg == string("exit")) {
            if (msg == string("stop") && isConnected == false) {
                printRed("already stopped\n");
                continue;
            }
            isConnected = false;

            SetEvent(hExit);
            if (threads[1].joinable()) threads[1].join();
            cv.notify_one();
            if (threads[2].joinable()) threads[2].join();
        }
        else {
            printRed("Unknown command\n");
        }

        if (msg == string("exit")) {
            break;
        }
    }
    data.PrintAll();
}

int main() {
    Solver();
    return 0;
}