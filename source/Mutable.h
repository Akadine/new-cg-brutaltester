

#ifndef MUTABLE_H
#define MUTABLE_H

#include <mutex>

/*
 * @brief Class describing a Mutable object shared across threads 
 */
template<typename T>
class Mutable {
public:
    /**
     *
     * @brief Constructor
     *
     * @param value The initial value of the mutable object.
     */
    Mutable(T value) : m_value(value) {}

    /**
     *
     * @brief Get the current value of the mutable object.
     *
     * @return The current value of the mutable object.
     */
    T& get() {
        std::unique_lock<std::mutex> lock(m_mutex);
        //m_cv.wait(lock, [this] { return m_readers == 0; });
        //++m_readers;
        return m_value;
    }

    /**
     *
     * @brief Set the value of the atomic variable.
     *
     * @param value The new value to set for the mutable object.
     */
    void set(T value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_value = value;
        m_readers = 0;
        m_cv.notify_all();
    }

    private:

        T m_value;                     //< The value of the mutable object.

        std::mutex m_mutex;             //< Mutex to protect access to m_value and m_readers.
        std::condition_variable m_cv;   //< Condition variable to synchronize access to m_value.
        int m_readers = 0;              //< Number of threads currently reading the value of the object.
};

#endif