#include "levikno.h"

// [FILE]: lvn_cds.cpp (Custom Data Structures)
// ------------------------------------------------------------
//
// [SECTION]: Timing Structures (chrono)
// -- [SUBSECT]: LvnTimer
// [SECTION]: Multithreading Structures
// -- [SUBSECT]: LvnThread
// -- [SUBSECT]: LvnMutex
// [SECTION]: Internal Data Structures
// -- [SUBSECT]: LvnString
// -- [SUBSECT]: LvnDrawList

#include <chrono>
#include <thread>
#include <mutex>

// ------------------------------------------------------------
// [SECTION]: Timing Structures (chrono)
// ------------------------------------------------------------


// -- [SUBSECT]: LvnTimer
// ------------------------------------------------------------

void LvnTimer::begin()
{
    m_Start = m_Current = std::chrono::high_resolution_clock::now().time_since_epoch().count();
}
void LvnTimer::reset()
{
    m_Start = m_Current = std::chrono::high_resolution_clock::now().time_since_epoch().count();
}
double LvnTimer::elapsed()
{
    m_Current = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<int64_t, std::ratio<1, 1000000000>>(m_Current - m_Start)).count() * 0.001 * 0.001 * 0.001;
}
double LvnTimer::elapsedms()
{
    return LvnTimer::elapsed() * 1000.0;
}


// ------------------------------------------------------------
// [SECTION]: Multithreading Structures
// ------------------------------------------------------------


// -- [SUBSECT]: LvnThread
// ------------------------------------------------------------

LvnThread::LvnThread(void* (*funcptr)(void*), void* arg)
{
    static_assert(sizeof(std::thread) <= m_ThreadBuffSize, "thread storage too small");
    static_assert(alignof(std::thread) <= alignof(max_align_t), "thread alignment too strict");
    new (m_ThreadBuff) std::thread(funcptr, arg);
}
LvnThread::~LvnThread()
{
    LvnThread::join();
    std::thread* t = reinterpret_cast<std::thread*>(m_ThreadBuff);
    t->~thread();
}
LvnThread::LvnThread(LvnThread&& other)
{
    static_assert(sizeof(std::thread) <= m_ThreadBuffSize, "thread storage too small");
    static_assert(alignof(std::thread) <= alignof(max_align_t), "thread alignment too strict");
    std::swap(m_ThreadBuff, other.m_ThreadBuff);
}
LvnThread& LvnThread::operator=(LvnThread&& other)
{
    LVN_CORE_ASSERT(!LvnThread::joinable(), "cannot set thread to other while current thread is running/joinable");
    static_assert(sizeof(std::thread) <= m_ThreadBuffSize, "thread storage too small");
    static_assert(alignof(std::thread) <= alignof(max_align_t), "thread alignment too strict");
    std::swap(m_ThreadBuff, other.m_ThreadBuff);
    return *this;
}
void LvnThread::join()
{
    std::thread* t = reinterpret_cast<std::thread*>(m_ThreadBuff);
    if (LvnThread::joinable())
        t->join();
}
bool LvnThread::joinable()
{
    std::thread* t = reinterpret_cast<std::thread*>(m_ThreadBuff);
    return t->joinable();
}
uint64_t LvnThread::id()
{
    std::thread* t = reinterpret_cast<std::thread*>(m_ThreadBuff);
    return std::hash<std::thread::id>{}(t->get_id());
}

// -- [SUBSECT]: LvnMutex
// ------------------------------------------------------------

LvnMutex::LvnMutex()
{
    static_assert(sizeof(std::mutex) <= m_MutexBuffSize, "mutex storage too small");
    static_assert(alignof(std::mutex) <= alignof(max_align_t), "mutex alignment too strict");
    new (m_MutexBuff) std::mutex();
}
LvnMutex::~LvnMutex()
{
    std::mutex* mutex = reinterpret_cast<std::mutex*>(m_MutexBuff);
    mutex->~mutex();
}
LvnMutex::LvnMutex(LvnMutex&& other)
{
    static_assert(sizeof(std::mutex) <= m_MutexBuffSize, "mutex storage too small");
    static_assert(alignof(std::mutex) <= alignof(max_align_t), "mutex alignment too strict");
    std::swap(m_MutexBuff, other.m_MutexBuff);
}
LvnMutex& LvnMutex::operator=(LvnMutex&& other)
{
    LvnMutex::~LvnMutex();
    static_assert(sizeof(std::mutex) <= m_MutexBuffSize, "mutex storage too small");
    static_assert(alignof(std::mutex) <= alignof(max_align_t), "mutex alignment too strict");
    std::swap(m_MutexBuff, other.m_MutexBuff);
    return *this;
}
void LvnMutex::lock()
{
    std::mutex* mutex = reinterpret_cast<std::mutex*>(m_MutexBuff);
    mutex->lock();
}
void LvnMutex::unlock()
{
    std::mutex* mutex = reinterpret_cast<std::mutex*>(m_MutexBuff);
    mutex->unlock();
}


// ------------------------------------------------------------
// [SECTION]: Internal Data Structures
// ------------------------------------------------------------


// -- [SUBSECT]: LvnString
// ------------------------------------------------------------

LvnString::LvnString()
{
    m_Data = lvn::memNew<char>();
    m_Data[0] = '\0';
    m_Size = 0;
    m_Capacity = 1;
}
LvnString::~LvnString()
{
    lvn::memDelete<char>(m_Data);
    m_Size = m_Capacity = 0;
    m_Data = nullptr;
}
LvnString::LvnString(const char* str)
{
    m_Size = strlen(str);
    m_Capacity = m_Size + 1;
    m_Data = lvn::memNew<char>(m_Capacity);
    memcpy(m_Data, str, m_Capacity);
}
LvnString::LvnString(const char* data, size_t size)
{
    m_Size = size;
    m_Capacity = m_Size + 1;
    m_Data = lvn::memNew<char>(m_Capacity);
    memcpy(m_Data, data, m_Capacity);
    m_Data[m_Size] = '\0';
}
LvnString::LvnString(const LvnString& other)
{
    m_Size = other.m_Size;
    m_Capacity = other.m_Capacity;
    m_Data = lvn::memNew<char>(other.m_Capacity);
    memcpy(m_Data, other.m_Data, other.m_Capacity);
}
LvnString& LvnString::operator=(const LvnString& other)
{
    if (this == &other) return *this;
    lvn::memDelete<char>(m_Data);
    m_Size = other.m_Size;
    m_Capacity = other.m_Capacity;
    m_Data = lvn::memNew<char>(other.m_Capacity);
    memcpy(m_Data, other.m_Data, other.m_Capacity);
    return *this;
}

char& LvnString::operator [](size_t index)
{
    LVN_CORE_ASSERT(index < length(), "string index out of range");
    return m_Data[index];
}
const char& LvnString::operator [](size_t index) const
{
    LVN_CORE_ASSERT(index < length(), "string index out of range");
    return m_Data[index];
}

bool LvnString::operator ==(const LvnString& other)
{
    if (this->length() != other.length())
        return false;
    return memcmp(m_Data, other.m_Data, this->length()) == 0;
}
bool LvnString::operator !=(const LvnString& other)
{
    if (this->length() != other.length())
        return true;
    return memcmp(m_Data, other.m_Data, this->length()) != 0;
}
bool LvnString::operator ==(const char* str)
{
    if (!str) { return false; }
    if (this->length() != strlen(str)) { return false; }
    return memcmp(m_Data, str, length()) == 0;
}
bool LvnString::operator !=(const char* str)
{
    if (!str) { return true; }
    if (this->length() != strlen(str)) { return true; }
    return memcmp(m_Data, str, length()) != 0;
}
LvnString LvnString::operator+(const LvnString& other)
{
    return operator+(other.m_Data);
}
LvnString LvnString::operator+(const char* str)
{
    LvnString s;
    size_t strsize = length();
    s.resize(strsize + strlen(str));
    memcpy(s.data(), m_Data, strsize);
    memcpy(s.data() + strsize, str, strlen(str));
    return s;
}
void LvnString::operator+=(const LvnString& other)
{
    append(other.c_str());
}
void LvnString::operator+=(const char* str)
{
    append(str);
}
void LvnString::operator+=(const char& ch)
{
    append(ch);
}
void LvnString::append(const char* str)
{
    size_t strsize = strlen(str);
    resize(m_Size + strsize);
    memcpy(&m_Data[m_Size - strsize], str, strsize * sizeof(char));
}
void LvnString::append(const char& ch)
{
    resize(m_Size + 1);
    memcpy(&m_Data[m_Size - 1], &ch, sizeof(char));
}
LvnString LvnString::substr(size_t index)
{
    LVN_CORE_ASSERT(index < m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(m_Size - index);
    memcpy(s.m_Data, &this->m_Data[index], m_Size - index);
    return s;
}
const LvnString LvnString::substr(size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(m_Size - index);
    memcpy(s.m_Data, &this->m_Data[index], m_Size - index);
    return s;
}
LvnString LvnString::substr(size_t index, size_t len)
{
    LVN_CORE_ASSERT(index + len <= m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(len);
    memcpy(s.m_Data, &this->m_Data[index], len);
    return s;
}
const LvnString LvnString::substr(size_t index, size_t len) const
{
    LVN_CORE_ASSERT(index + len <= m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(len);
    memcpy(s.m_Data, &this->m_Data[index], len);
    return s;
}
void LvnString::insert(const char* it, const char& ch)
{
    LVN_CORE_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within string bounds");
    size_t index = it - m_Data;
    insert_index(index, ch);
}
void LvnString::insert(const char* it, const char* begin, const char* end)
{
    LVN_CORE_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within string bounds");
    LVN_CORE_ASSERT(end >= begin, "end insert element cannot be less than begin insert element");
    size_t index = it - m_Data;
    insert_index(index, begin, end);
}
void LvnString::insert(const char* it, const char* data, size_t size)
{
    if (size == 0) return;
    LVN_CORE_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within string bounds");
    size_t index = it - m_Data;
    insert_index(index, data, size);
}
void LvnString::insert_index(size_t index, const char& ch)
{
    LVN_CORE_ASSERT(index <= m_Size, "insert index not within string bounds");
    resize(m_Size + 1);
    memmove(&m_Data[index + 1], &m_Data[index], (m_Size - index - 1) * sizeof(char));
    memcpy(&m_Data[index], &ch, sizeof(char));
}
void LvnString::insert_index(size_t index, const char* begin, const char* end)
{
    LVN_CORE_ASSERT(index <= m_Size, "insert index not within string bounds");
    LVN_CORE_ASSERT(end >= begin, "end insert element cannot be less than begin insert element");
    size_t count = end - begin;
    if (count == 0) return;
    resize(m_Size + count);
    memmove(&m_Data[index + count], &m_Data[index], (m_Size - index - count) * sizeof(char));
    memcpy(&m_Data[index], begin, count * sizeof(char));
}
void LvnString::insert_index(size_t index, const char* data, size_t size)
{
    if (size == 0) return;
    LVN_CORE_ASSERT(index <= m_Size, "insert index not within string bounds");
    resize(m_Size + size);
    memmove(&m_Data[index + size], &m_Data[index], (m_Size - index - size) * sizeof(char));
    memcpy(&m_Data[index], data, size * sizeof(char));
}
void LvnString::reserve(size_t size)
{
    if (size <= m_Capacity) { return; }
    char* temp = lvn::memNew<char>(size);
    memcpy(temp, m_Data, m_Size * sizeof(char));
    lvn::memDelete<char>(m_Data);
    m_Data = temp;
    m_Capacity = size;
}
void LvnString::resize(size_t size)
{
    reserve(size + 1);
    m_Size = size;
    m_Data[m_Size] = '\0';
}
void LvnString::clear()
{
    m_Size = 0;
    m_Data[m_Size] = '\0';
}
void LvnString::clear_free()
{
    lvn::memDelete<char>(m_Data);
    m_Data = nullptr;
    m_Size = m_Capacity = 0;
}
void LvnString::erase(const char* it)
{
    LVN_CORE_ASSERT(it >= m_Data && it < m_Data + m_Size, "erase element not within string bounds");
    size_t index = it - m_Data;
    erase_index(index);
}
void LvnString::erase_index(size_t index)
{
    LVN_CORE_ASSERT(index < m_Size, "index out of vector size range");
    size_t aftIndex = m_Size - index - 1;
    if (aftIndex != 0)
        memcpy(&m_Data[index], &m_Data[index + 1], aftIndex * sizeof(char));
    --m_Size;
}
void LvnString::push_back(const char& ch)
{
    resize(m_Size + 1);
    memcpy(&m_Data[m_Size - 1], &ch, sizeof(char));
}
void LvnString::push_range(const char* ch, size_t size)
{
    resize(m_Size + size);
    memcpy(&m_Data[m_Size - size], ch, size * sizeof(char));
}
void LvnString::pop_back()
{
    if (m_Size == 0) { return; }
    resize(m_Size - 1);
}
size_t LvnString::find(const LvnString& other) const
{
    return find(other.c_str());
}
size_t LvnString::rfind(const LvnString& other) const
{
    return rfind(other.c_str());
}
size_t LvnString::find(const char& ch) const
{
    if (m_Size == 0) { return LvnString::npos; }
    for (size_t i = 0; i < m_Size; i++)
    {
        if (m_Data[i] == ch)
            return i;
    }

    return LvnString::npos;
}
size_t LvnString::rfind(const char& ch) const
{
    if (m_Size == 0) { return LvnString::npos; }
    for (size_t i = m_Size - 1; i >= 0; i--)
    {
        if (m_Data[i] == ch)
            return i;

        if (i == 0) break;
    }

    return LvnString::npos;
}
size_t LvnString::find(const char* str) const
{
    if (!str || str == "\0" || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str);
    if (strsize > m_Size) { return LvnString::npos; }

    for (size_t i = 0; i <= m_Size - strsize; i++)
    {
        bool match = true;
        for (size_t j = 0; j < strsize; j++)
        {
            if (m_Data[i + j] != str[j])
            {
                match = false;
                break;
            }
        }

        if (match) { return i; }
    }

    return LvnString::npos;
}
size_t LvnString::rfind(const char* str) const
{
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str);
    if (strsize > m_Size) { return LvnString::npos; }

    for (size_t i = m_Size - strsize; i >= 0; i--)
    {
        bool match = true;
        for (size_t j = 0; j < strsize; j++)
        {
            if (m_Data[i + j] != str[j])
            {
                match = false;
                break;
            }
        }

        if (match) { return i; }
        if (i == 0) break;
    }

    return LvnString::npos;
}
size_t LvnString::find_first_of(const LvnString& other, size_t index) const
{
    return find_first_of(other.c_str(), index);
}
size_t LvnString::find_first_of(const LvnString& other, size_t index, size_t length) const
{
    return find_first_of(other.c_str(), index, length);
}
size_t LvnString::find_first_of(const char& ch, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size, "index not within string bounds");
    for (size_t i = index; i < m_Size; i++)
        if (m_Data[i] == ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_first_of(const char* str, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_first_of(str, index, strsize);
}
size_t LvnString::find_first_of(const char* str, size_t index, size_t length) const
{
    LVN_CORE_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_CORE_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = index; i < m_Size; i++)
    {
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
                return i;
        }
    }

    return LvnString::npos;
}
size_t LvnString::find_first_not_of(const LvnString& other, size_t index) const
{
    return find_first_not_of(other.c_str(), index);
}
size_t LvnString::find_first_not_of(const LvnString& other, size_t index, size_t length) const
{
    return find_first_not_of(other.c_str(), index, length);
}
size_t LvnString::find_first_not_of(const char& ch, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size, "index not within string bounds");
    for (size_t i = index; i < m_Size; i++)
        if (m_Data[i] != ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_first_not_of(const char* str, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_first_not_of(str, index, strsize);
}
size_t LvnString::find_first_not_of(const char* str, size_t index, size_t length) const
{
    LVN_CORE_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_CORE_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = index; i < m_Size; i++)
    {
        bool found = false;
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
            {
                found = true;
                break;
            }
        }

        if (!found) { return i; }
    }

    return LvnString::npos;
}
size_t LvnString::find_last_of(const LvnString& other, size_t index) const
{
    return find_last_of(other.c_str(), index);
}
size_t LvnString::find_last_of(const LvnString& other, size_t index, size_t length) const
{
    return find_last_of(other.c_str(), index, length);
}
size_t LvnString::find_last_of(const char& ch, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
        if (m_Data[i] == ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_last_of(const char* str, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_last_of(str, index, strsize);
}
size_t LvnString::find_last_of(const char* str, size_t index, size_t length) const
{
    LVN_CORE_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_CORE_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
    {
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
                return i;
        }
    }

    return LvnString::npos;
}
size_t LvnString::find_last_not_of(const LvnString& other, size_t index) const
{
    return find_last_not_of(other.c_str(), index);
}
size_t LvnString::find_last_not_of(const LvnString& other, size_t index, size_t length) const
{
    return find_last_not_of(other.c_str(), index, length);
}
size_t LvnString::find_last_not_of(const char& ch, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
        if (m_Data[i] != ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_last_not_of(const char* str, size_t index) const
{
    LVN_CORE_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_last_not_of(str, index, strsize);
}
size_t LvnString::find_last_not_of(const char* str, size_t index, size_t length) const
{
    LVN_CORE_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_CORE_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
    {
        bool found = false;
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
            {
                found = true;
                break;
            }
        }

        if (!found) { return i; }
    }

    return LvnString::npos;
}
bool LvnString::starts_with(const char& ch) const
{
    if (!m_Size) { return false; }
    return m_Data[0] == ch;
}
bool LvnString::ends_with(const char& ch) const
{
    if (!m_Size) { return false; }
    return m_Data[m_Size - 1] == ch;
}
bool LvnString::contains(const char& ch) const
{
    char* begin = m_Data;
    const char* end = m_Data + m_Size;
    while (begin < end)
    {
        if (*begin == ch) { return true; }
        begin++;
    }
    return false;
}
LvnString operator+(const char* str, const LvnString& other)
{
    LvnString s;
    size_t strsize = strlen(str);
    s.resize(strsize + other.length());
    memcpy(s.data(), str, strsize);
    memcpy(s.data() + strsize, other.data(), other.length());
    return s;
}

// -- [SUBSECT]: LvnDrawList
// ------------------------------------------------------------

void LvnDrawList::push_back(const LvnDrawCommand& drawCmd)
{
    m_Indices.insert(m_Indices.end(), drawCmd.pIndices, drawCmd.pIndices + drawCmd.indexCount);
    for (uint32_t i = m_Indices.size() - drawCmd.indexCount; i < m_Indices.size(); i++)
        m_Indices[i] += m_VertexCount;

    m_VerticesRaw.insert(m_VerticesRaw.end(), static_cast<uint8_t*>(drawCmd.pVertices), static_cast<uint8_t*>(drawCmd.pVertices) + drawCmd.vertexCount * drawCmd.vertexStride);
    m_VertexCount += drawCmd.vertexCount;
}
