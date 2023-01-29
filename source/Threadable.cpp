#include "threadable.h"

Threadable::Threadable() : m_finished{ false }, m_stop{ false } {}

Threadable::~Threadable() { detach(); }


std::string Threadable::getId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::stringstream ss;
    ss << m_id;
    return ss.str();
}

void Threadable::setFinished() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_finished = true;
    setStop();
}

void Threadable::setStop() {
    m_stop = true;
}

bool Threadable::shouldStop() const {
    return m_stop;
}

bool Threadable::isFinished() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_finished;
}

bool Threadable::isRunning() const {
    return m_thread.joinable();
}

std::thread& Threadable::getThread() {
    return m_thread;
}

void Threadable::start() {
    if (m_thread.joinable()) {
        // Thread is already running, do nothing.
        return;
    }
    m_stop = false;
    m_id = m_thread.get_id();
    m_thread = std::thread(&Threadable::run, this);
}

void Threadable::join() {
    m_thread.join();
}

void Threadable::detach() {
    m_thread.detach();
}

void Threadable::stop() {
    setStop();
}