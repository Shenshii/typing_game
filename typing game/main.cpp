#include <windows.h>
#include <shlobj.h>  // For SHGetFolderPath
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#pragma comment(lib, "Shell32.lib") // Link with Shell32.lib

// Thread function to simulate game logic or UI update
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    std::cout << "Thread is running..." << std::endl;

    // Simulate game state updates (e.g., handling input, updating UI)
    for (int i = 0; i < 10; i++) {
        std::cout << "Updating game state... " << i + 1 << std::endl;
        Sleep(1000); // Simulate work for 1 second
    }
    return 0;
}

// Function to calculate word and character count from file content
void calculateWordAndCharCount(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line, word;
    int wordCount = 0, charCount = 0;

    while (getline(file, line)) {
        std::istringstream lineStream(line);
        while (lineStream >> word) {
            wordCount++;
        }
        charCount += line.length(); // Counts characters in each line
    }

    file.close();
    std::cout << "File content successfully loaded." << std::endl;
    std::cout << "Word Count: " << wordCount << std::endl;
    std::cout << "Character Count (excluding spaces): " << charCount << std::endl;
}

int main() {
    // Use SHGetFolderPath to get the Documents folder path dynamically
    char docPath[MAX_PATH];
    if (SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, docPath) != S_OK) {
        std::cout << "Failed to get Documents folder path." << std::endl;
        return 1;
    }

    // Append the file name to the Documents folder path
    std::string filePath = std::string(docPath) + "\\typing_game_output.txt";

    // 1. Create a new process (example: notepad.exe)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Command line to launch Notepad with the file
    std::string notepadCommand = "write.exe " + filePath;

    // Convert command string to LPCSTR for CreateProcess
    LPSTR command = const_cast<char*>(notepadCommand.c_str());

    // Create the new process
    if (!CreateProcess(
        NULL,              // Application name
        command,           // Command line arguments
        NULL,              // Process handle not inheritable
        NULL,              // Thread handle not inheritable
        FALSE,             // No handle inheritance
        0,                 // Creation flags (e.g., CREATE_NEW_CONSOLE)
        NULL,              // Use parent's environment block
        NULL,              // Use parent's starting directory
        &si,               // Pointer to STARTUPINFO structure
        &pi                // Pointer to PROCESS_INFORMATION structure
    )) {
        std::cout << "CreateProcess failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Process created (Notepad launched). Waiting for it to exit..." << std::endl;

    // 2. Create a new thread for game logic/UI updates
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
        std::cout << "CreateThread failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Thread created with ID: " << threadId << std::endl;

    // 3. Wait for the process to exit
    WaitForSingleObject(pi.hProcess, INFINITE);
    std::cout << "Process (Notepad) terminated." << std::endl;

    // 4. Wait for the thread to finish
    WaitForSingleObject(hThread, INFINITE);
    std::cout << "Thread execution finished." << std::endl;

    // 5. Read the saved file and calculate word and character count
    calculateWordAndCharCount(filePath);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hThread);

    return 0;
}
