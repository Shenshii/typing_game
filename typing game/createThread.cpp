#include <windows.h>
#include <iostream>
#include <ctime>

// Thread function to simulate game logic or UI update
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    std::wcout << L"Thread is running..." << std::endl;

    // Simulate game state updates (e.g., handling input, updating UI)
    for (int i = 0; i < 5; i++) {
        std::wcout << L"Updating game state... " << i + 1 << std::endl;
        Sleep(1000); // Simulate work for 1 second
    }

    return 0;
}

int main() {
    // Record the start time
    std::time_t startTime = std::time(nullptr);

    // Create a new thread
    HANDLE hThread;
    DWORD threadId;

    hThread = CreateThread(
        NULL,               // Default security attributes
        0,                  // Default stack size
        ThreadFunction,     // Thread function
        NULL,               // Thread function parameter
        0,                  // Default creation flags
        &threadId           // Receive thread identifier
    );

    if (hThread == NULL) {
        std::wcout << L"CreateThread failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::wcout << L"Thread created with ID: " << threadId << std::endl;

    // Wait for the thread to finish
    WaitForSingleObject(hThread, INFINITE);
    std::wcout << L"Thread execution finished." << std::endl;

    // Record the end time and calculate the elapsed time
    std::time_t endTime = std::time(nullptr);
    double elapsedTime = std::difftime(endTime, startTime);

    std::wcout << L"Time spent executing thread: " << elapsedTime << L" seconds" << std::endl;

    // Close the thread handle
    CloseHandle(hThread);

    return 0;
}
