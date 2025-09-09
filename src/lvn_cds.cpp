#include "levikno.h"

#include <cstring>

#include <thread>
#include <mutex>

// [LAYOUT]: lvn_cds.cpp (Custom Data Structures)
// ------------------------------------------------------------
//
// [SECTION]: Internal Data Structures
// -- [SUBSECT]: LvnThread
// -- [SUBSECT]: LvnMutex
// -- [SUBSECT]: LvnString


// -- [SUBSECT]: LvnThread
// ------------------------------------------------------------

LvnThread::LvnThread(void (*funcptr)(void*), void* arg)
{
    m_Thread = lvn::memNew<std::thread>();
    new (m_Thread) std::thread(funcptr, arg);
}
LvnThread::~LvnThread()
{
    LvnThread::join();
    lvn::memDelete<std::thread>(static_cast<std::thread*>(m_Thread));
}
LvnThread::LvnThread(LvnThread&& other)
{
    std::swap(m_Thread, other.m_Thread);
}
LvnThread& LvnThread::operator=(LvnThread&& other)
{
    LVN_ASSERT(!LvnThread::joinable(), "cannot set thread to other while current thread is running/joinable");
    std::swap(m_Thread, other.m_Thread);
    return *this;
}
void LvnThread::join()
{
    std::thread* t = reinterpret_cast<std::thread*>(m_Thread);
    if (LvnThread::joinable())
        t->join();
}
bool LvnThread::joinable()
{
    std::thread* t = reinterpret_cast<std::thread*>(m_Thread);
    return t->joinable();
}
uint64_t LvnThread::id()
{
    std::thread* t = reinterpret_cast<std::thread*>(m_Thread);
    return std::hash<std::thread::id>{}(t->get_id());
}

// -- [SUBSECT]: LvnMutex
// ------------------------------------------------------------

LvnMutex::LvnMutex()
{
    m_Mutex = lvn::memNew<std::mutex>();
    new (m_Mutex) std::mutex();
}
LvnMutex::~LvnMutex()
{
    lvn::memDelete<std::mutex>(static_cast<std::mutex*>(m_Mutex));
}
LvnMutex::LvnMutex(LvnMutex&& other)
{
    std::swap(m_Mutex, other.m_Mutex);
}
LvnMutex& LvnMutex::operator=(LvnMutex&& other)
{
    LvnMutex::~LvnMutex();
    std::swap(m_Mutex, other.m_Mutex);
    return *this;
}
void LvnMutex::lock()
{
    std::mutex* mutex = reinterpret_cast<std::mutex*>(m_Mutex);
    mutex->lock();
}
void LvnMutex::unlock()
{
    std::mutex* mutex = reinterpret_cast<std::mutex*>(m_Mutex);
    mutex->unlock();
}
