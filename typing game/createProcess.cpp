#include <windows.h>
#include <shlobj.h>  // For SHGetFolderPath
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <sstream>
#include <ctime>

#pragma comment(lib, "Shell32.lib") // Link with Shell32.lib

// Function to show a popup window with statistics
void showPopup(const std::wstring& message) {
    MessageBoxW(NULL, message.c_str(), L"Typing Game Stats", MB_OK | MB_ICONINFORMATION);
}

// Function to calculate word and character count, and find unique words
void calculateWordAndCharCount(const std::wstring& filePath, int& wordCount, int& charCount, int& uniqueWordCount) {
    std::wifstream file(filePath.c_str());
    if (!file.is_open()) {
        std::wcout << L"Failed to open file: " << filePath << std::endl;
        return;
    }

    std::wstring line, word;
    std::set<std::wstring> uniqueWords;
    wordCount = 0;
    charCount = 0;

    while (getline(file, line)) {
        std::wstringstream lineStream(line);
        while (lineStream >> word) {
            wordCount++;
            uniqueWords.insert(word);
        }
        charCount += line.length(); // Counts characters in each line
    }

    uniqueWordCount = uniqueWords.size();
    file.close();
}

// Main function
int main() {
    // Use SHGetFolderPathW to get the Documents folder path dynamically (wide char)
    WCHAR docPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, docPath) != S_OK) {
        std::wcout << L"Failed to get Documents folder path." << std::endl;
        return 1;
    }

    // Append the file name to the Documents folder path
    std::wstring filePath = std::wstring(docPath) + L"\\typing_game_output.txt";

    // 1. Check if the file exists, if not, create it
    std::wifstream testFile(filePath.c_str());
    if (!testFile) {
        std::wofstream createFile(filePath.c_str());
        createFile << L""; // Create an empty file
        createFile.close();
        std::wcout << L"File created: " << filePath << std::endl;
    }
    testFile.close();

    // 2. Record the start time
    std::time_t startTime = std::time(nullptr);

    // 3. Create a new process to open wordpad.exe with full path
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring writeCommand = L"C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe " + filePath;
    LPWSTR command = const_cast<LPWSTR>(writeCommand.c_str());

    if (!CreateProcessW(
        NULL,              // Application name
        command,           // Command line arguments
        NULL,              // Process handle not inheritable
        NULL,              // Thread handle not inheritable
        FALSE,             // No handle inheritance
        0,                 // Creation flags (e.g., CREATE_NEW_CONSOLE)
        NULL,              // Use parent's environment block
        NULL,              // Use parent's starting directory
        &si,               // Pointer to STARTUPINFOW structure
        &pi                // Pointer to PROCESS_INFORMATION structure
    )) {
        DWORD errorCode = GetLastError();
        std::wcout << L"CreateProcess failed. Error: " << errorCode << std::endl;
        return 1;
    }

    std::wcout << L"Process created (Wordpad launched). Waiting for it to exit..." << std::endl;

    // Ensure that WaitForSingleObject properly waits for Wordpad to close
    DWORD waitResult = WaitForSingleObject(pi.hProcess, INFINITE);
    if (waitResult == WAIT_OBJECT_0) {
        std::wcout << L"Process (Wordpad) terminated." << std::endl;
    } else {
        std::wcout << L"Error waiting for Wordpad process to finish." << std::endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // 4. Calculate the time the app was opened
    std::time_t endTime = std::time(nullptr);
    double elapsedTime = std::difftime(endTime, startTime);

    // 5. Read the saved file and calculate statistics
    int wordCount = 0, charCount = 0, uniqueWordCount = 0;
    calculateWordAndCharCount(filePath, wordCount, charCount, uniqueWordCount);

    // 6. Display statistics in a popup, including word count, character count, and unique word count
    std::wstringstream stats;
    stats << L"Words typed: " << wordCount << L"\n";
    stats << L"Unique words: " << uniqueWordCount << L"\n";
    stats << L"Character count: " << charCount << L"\n";
    stats << L"Time spent in app: " << elapsedTime << L" seconds\n";
    showPopup(stats.str());

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
