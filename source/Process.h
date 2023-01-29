//Process.h
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <io.h>
#include <windows.h>


/*
 * @brief Class describing a child process and it's pipes for reading and writing data to and from that child process.
 */
class Process
{
protected:
    std::string_view executable;        //< the executable filename
    std::string_view args;              //< the command line arguments for the executable

    int id;                             //< the id of the process

    bool use_window;                    //< does the child process use a window?

    bool running;                       //< is the process running?

    //The Pipe ends
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    HANDLE hChildStd_ERR_Rd = NULL;
    HANDLE hChildStd_ERR_Wr = NULL;

    PROCESS_INFORMATION process_info;
    STARTUPINFOW startup_info;
public:
    enum TYPE { INPUT, OUTPUT, ERR };
    const static int BUFSIZE = 4096;    //< buffersize

    /*
     * @brief Constructs a null Process object.
     */
    Process();


    /*
     * @brief Copy Constructs a Process object.
     */
    Process(const Process& other);

    /*
     * @brief Constructs a Process object.
     *
     * @param executable The file to run. Required.
     * @param args Any commandline arguments for that executable. Required.
     * @param width the width of the child window. Default 0.
     * @param height the height of the child window. Default 0.
     * @param useWindow bool true or false. Default false.
     */
    Process(int id, const std::string_view& executable, const std::string_view& args = "", bool useWindow = false);

    /*
     * @brief Destructs the Process object.
     */
    ~Process();

    /*
     * @brief Gets the filename of the executable this child process is running.
     *
     * @return The filename.
     */
    std::string_view& getExe() { return this->executable; }

    /*
     * @brief Gets the command line arguments used for the executable this child process is running.
     *
     * @return The arguments.
     */
    std::string_view& getArgs() { return this->args; }

    /*
     * @brief Gets a Handle.
     *
     * @param type INPUT, OUTPUT, Or ERRor?
     *
     * @return The end of the pipe handle.
     */
    HANDLE& getHandle(TYPE type);


    /*
     * @brief Checks if pipe is empty.
     *
     * @param the handle to the pipe.
     *
     * @return true or false.
     */
    bool isPipeEmpty(HANDLE handle);

    /*
     * @brief Reads from a pipe.
     *
     * @param the pipe.
     *
     * @return the string read from the pipe.
     */
    std::string readPipe(TYPE type);

    /*
     * @brief Writes to a pipe.
     *
     * @param the string to write to the pipe.\
     * @return sucess
     */
    bool writePipe(const std::string& data);

    /*
     * @brief Starts the child process.
     */
    bool start();

    /*
     * @brief Overload an operator.
     */
    Process& operator=(const Process& other)
    {
        ZeroMemory(&startup_info, sizeof(STARTUPINFOW));
        executable = other.executable;
        args = other.args;
        use_window = other.use_window;
        id = other.id + (other.running ? 10000 : 0); //we will never open 10000 processes. if it is running, we need unique pipe names 
        running = false; //remember to start() the new process, which will setup the pipes

        return *this;
    }

    /*
     * @brief checks if process is running.
     *
     * @return is running true or false.
     */
    bool isRunning();

private:
    /*
     * @brief Checks closes a Handle.
     *
     * @param the handle to the pipe.
     */
    void close(HANDLE handle);

    /*
     * @brief Creates the pipes.
     *
     * @return success true or false.
     */
    bool createPipes();
};
#endif
