#ifndef THREADABLE_H
#define THREADABLE_H

#include <sstream>
#include <thread>
#include <string>
#include <mutex>

class Threadable {
private:
    std::thread::id m_id;       //< ID of the thread.
    bool m_stop;                //< Flag indicating whether the thread should stop.
    std::thread m_thread;       //< Underlying thread object.
    bool m_finished;            //< Flag to indicate if the thread has finished execution.
    mutable std::mutex m_mutex; //< Mutex to protect access to m_finished.
        
protected:
    /**
     * @brief Sets the finished flag to true.
     */
    void setFinished();

    /**
     * @brief Sets the stop flag to true.
     */
    void setStop();

    /**
     * @brief Returns the value of the stop flag.
     *
     * @return The value of the stop flag.
     */
    bool shouldStop() const;

    /**
     * @brief Method to be implemented by derived classes.
     *        Contains the code that the thread will execute.
     */
    virtual void run() = 0;

public:
    /**
     * @brief Constructs a Threadable object with the given ID.
     */
    explicit Threadable();


    /**
     * @brief Destructs the Threadable object.
     */
    ~Threadable();

    /**
     * @brief Returns the ID of the thread.
     *
     * @return The ID of the thread.
     */
    std::string getId() const;
    
    /**
     * @brief Returns true if the thread has finished execution, false otherwise.
     *
     * @return true if the thread has finished execution, false otherwise.
     */
    bool isFinished() const;

    /**
     * @brief Returns true if the thread is currently running, false otherwise.
     *
     * @return true if the thread is currently running, false otherwise.
     */
    bool isRunning() const;

    /**
     * @brief Returns a reference to the underlying std::thread object.
     *
     * @return A reference to the underlying std::thread object.
     */
    std::thread& getThread();

    /**
     * @brief Starts execution of the thread.
     */
    void start();

    /**
     * @brief Blocks the calling thread until the thread has finished execution.
     */
    void join();

    /**
     * @brief Detaches the thread from the calling thread.
     */
    void detach();

    /**
     * @brief Sets the finished flag to true and signals the thread to stop execution.
     */
    void stop();
};

#endif