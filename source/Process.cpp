//Process.cpp
#include "Process.h"

Process::Process() : executable{ "" }, args{ "" }, id{ 0 }, use_window{ false }, running{ false } {
    ZeroMemory(&startup_info, sizeof(STARTUPINFOW));
}

Process::Process(int id, const std::string_view& executable, const std::string_view& args, bool window) : id{ id }, executable{ executable }, args{ args }, use_window{ window }, running{ false } {
    ZeroMemory(&startup_info, sizeof(STARTUPINFOW));
}

Process::Process(const Process& other) {
    ZeroMemory(&startup_info, sizeof(STARTUPINFOW));
    executable = other.executable;
    args = other.args;
    use_window = other.use_window;
    id = other.id + (other.running ? 10000 : 0); //we will never open 10000 processes. if it is running, we need unique pipe names 
    running = false; //remember to start() the new process, which will setup the pipes
}


Process::~Process()
{
    if (this->running) {
        // Wait for the child process to terminate
        WaitForSingleObject(process_info.hProcess, 1000);

        // Get the exit code of the child process
        DWORD exitCode;
        GetExitCodeProcess(process_info.hProcess, &exitCode);

        //if still active, terminate the process
        if (exitCode == STILL_ACTIVE) {
            TerminateProcess(process_info.hProcess, 0);
        }
        try {
            if (hChildStd_IN_Rd != INVALID_HANDLE_VALUE) close(hChildStd_IN_Rd);
            if (hChildStd_IN_Wr != INVALID_HANDLE_VALUE) close(hChildStd_IN_Wr);
            if (hChildStd_OUT_Rd != INVALID_HANDLE_VALUE) close(hChildStd_OUT_Rd);
            if (hChildStd_OUT_Wr != INVALID_HANDLE_VALUE) close(hChildStd_OUT_Wr);
            if (hChildStd_ERR_Rd != INVALID_HANDLE_VALUE) close(hChildStd_ERR_Rd);
            if (hChildStd_ERR_Wr != INVALID_HANDLE_VALUE) close(hChildStd_ERR_Wr);
            if (process_info.hProcess != INVALID_HANDLE_VALUE) close(process_info.hProcess);
            if (process_info.hThread != INVALID_HANDLE_VALUE) close(process_info.hThread);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void Process::close(HANDLE handle) {
    __try {
        if (CloseHandle(handle) == FALSE) {
            throw GetLastError();
        }
    }

    __except (EXCEPTION_EXECUTE_HANDLER) { throw std::exception("Warning: Handles not closed. Memory may leak."); }
}

bool Process::createPipes() {
    SECURITY_ATTRIBUTES sa_attr;
    sa_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa_attr.bInheritHandle = TRUE;
    sa_attr.lpSecurityDescriptor = NULL;

    // Create the pipes
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &sa_attr, 0)
        || !CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &sa_attr, 0)
        || !CreatePipe(&hChildStd_ERR_Rd, &hChildStd_ERR_Wr, &sa_attr, 0)) {
        std::cout << "Error Creating Pipes, Code: " << GetLastError() << std::endl;
        return false;
    }

    if (hChildStd_OUT_Rd == INVALID_HANDLE_VALUE || hChildStd_OUT_Wr == INVALID_HANDLE_VALUE
        || hChildStd_IN_Rd == INVALID_HANDLE_VALUE || hChildStd_IN_Wr == INVALID_HANDLE_VALUE
        || hChildStd_ERR_Rd == INVALID_HANDLE_VALUE || hChildStd_ERR_Wr == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error Creating Handles, Code: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

HANDLE& Process::getHandle(TYPE type) {
    switch (type) {
    case INPUT:
        return hChildStd_OUT_Wr;
    case OUTPUT:
        return hChildStd_OUT_Rd;
    case ERR:
        return hChildStd_ERR_Rd;
    default:
        std::cout << "Fatal: improper values for getPipeEnd parameters." << std::endl;
        exit(1);
    }
}

bool Process::isPipeEmpty(HANDLE handle)
{
    DWORD bytesAvail = 0;

    // Check if there is data available to read from the pipe
    if (!PeekNamedPipe(handle, NULL, 0, NULL, &bytesAvail, NULL)) {
        std::cerr << "PeekNamedPipe failed (" << GetLastError() << ").\n";
        return true;
    }

    return (bytesAvail == 0);
}

std::string Process::readPipe(TYPE type) {
    //we cannot read from the child's IN, you silly user!
    if (type == INPUT) {
        return "";
    }

    HANDLE pipe = getHandle(type);
    if (isPipeEmpty(pipe))
    {
        return "";
    }

    char buffer[BUFSIZE + 1]{};
    DWORD bytesRead;
    if (!ReadFile(pipe, buffer, BUFSIZE, &bytesRead, NULL))
    {
        std::cerr << "Failed to read from pipe. Error code: " << GetLastError() << std::endl;
        return "";
    }

    //0 terminate and convert to a string
    buffer[bytesRead] = '\0';
    std::vector<char> v_data(buffer, buffer + bytesRead);
    return std::string(v_data.data(), v_data.size());
}

bool Process::writePipe(const std::string& data) {
    DWORD dwWritten;
    DWORD length = (DWORD)data.length() + 1; //no +1 creates problems
    return WriteFile(hChildStd_IN_Wr, data.c_str(), length, &dwWritten, NULL);
}

bool Process::start() {
    //convert the string_view executable to a string
    std::string name = "";
    name += this->executable;

    //convert the string executable name to a Wide Char String  
    std::wstring name_l(name.c_str(), name.c_str() + name.size());

    wchar_t current_dir[FILENAME_MAX];
    if (_wgetcwd(current_dir, FILENAME_MAX) == nullptr) {
        std::cerr << "Error getting current working directory. Code:" << GetLastError() << std::endl;
        exit(0);
    }

    wchar_t path_exe[MAX_PATH];
    GetModuleFileName(NULL, path_exe, MAX_PATH);
    std::wstring path = path_exe;
    path = std::filesystem::path(path).parent_path();
    path += L"\\";
    path += name_l;

    int found = _waccess_s(path.c_str(), 0);
    if (found != 0)
    {
        std::cerr << "Error: Executable file not found: " << name << std::endl;
        exit(0);
    }

    std::wstring commandLine = path;
    if (args != "") {
        std::string args_s = "";
        args_s += args;
        std::wstring args_l(args_s.c_str(), args_s.c_str() + args_s.size());
        commandLine += L" ";
        commandLine += args_l;
    }

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!createPipes())
    {
        std::cerr << "Error: Failed to create pipes for the child process" << std::endl;
        return false;
    }

    // Make sure the handles' inheritance is set correctly
    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)
        && !SetHandleInformation(hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)
        && !SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
        std::cerr << "Error: Failed to set handle information for the child process" << std::endl;
        return false;
    }

    // Set up the startup info struct    
    startup_info.cb = sizeof(STARTUPINFOW);
    startup_info.hStdOutput = hChildStd_OUT_Wr;
    startup_info.hStdError = hChildStd_ERR_Wr;
    startup_info.hStdInput = hChildStd_IN_Rd;
    startup_info.dwFlags |= STARTF_USESTDHANDLES;
    startup_info.dwFlags |= STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = use_window ? SW_SHOWDEFAULT: SW_HIDE;     
    

    // Create the child process
    if (!CreateProcess(commandLine.data(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startup_info, &process_info))
    {
        std::cerr << "Error: Failed to create the child process" << std::endl;
        return false;
    }

    Sleep(25); //let it start

    return isRunning();
}

bool Process::isRunning() {
    // Get the exit code of the child process
    DWORD exitCode;
    GetExitCodeProcess(process_info.hProcess, &exitCode);

    //set running
    running = (exitCode == STILL_ACTIVE);

    //return
    return running;
}