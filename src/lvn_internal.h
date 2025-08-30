#ifndef HG_LVN_INTERNAL_H
#define HG_LVN_INTERNAL_H

#include "levikno.h"


struct LvnHash;
template <typename K, typename T>
struct LvnHashEntry;
template <typename K, typename T, typename Hash>
class LvnHashMap;


// -- LvnHash, LvnHashEntry, LvnHashMap
// ------------------------------------------------------------
// simple and light weight replacement to std::hash, std::unordered_map
// designed to be more cache effecient, all hash entries stored in a single allocated array
// note that LvnHashMap only takes in integral types for the key value

struct LvnHash
{
    /* splitmix64 */
    size_t operator()(size_t k) const
    {
        k += 0x9E3779B97F4A7C15;
        k = (k ^ (k >> 30)) * 0xBF58476D1CE4E5B9;
        k = (k ^ (k >> 27)) * 0x94D049BB133111EB;
        k = k ^ (k >> 31);
        return k;
    }
};

template <typename K, typename T>
struct LvnHashEntry
{
    T data;
    K key;
    size_t nextIndex;
    bool taken, hasNext;
};

template <typename K, typename T, typename Hash = LvnHash>
class LvnHashMap
{
    static_assert(std::is_integral_v<K>, "cannot have non integral type as key");
    using MoveRef = std::remove_reference_t<T>&&;
private:
    LvnHashEntry<K, T>* m_HashEntries;
    size_t m_Size;
    size_t m_Capacity;
    Hash m_Hasher;

    void destruct()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < m_Capacity; i++)
            {
                if (m_HashEntries[i].taken)
                    m_HashEntries[i].data.~T();
            }
        }
    }
    bool erase_recursive(size_t index)
    {
        if (m_HashEntries[index].hasNext)
        {
            size_t nextIndex = m_HashEntries[index].nextIndex;
            m_HashEntries[index].key = m_HashEntries[nextIndex].key;
            m_HashEntries[index].nextIndex = m_HashEntries[nextIndex].nextIndex;
            m_HashEntries[index].taken = m_HashEntries[nextIndex].taken;
            m_HashEntries[index].hasNext = m_HashEntries[nextIndex].hasNext;
            m_HashEntries[index].data = static_cast<MoveRef>(m_HashEntries[nextIndex].data);
            if (erase_recursive(nextIndex))
            {
                m_HashEntries[index].nextIndex = 0;
                m_HashEntries[index].hasNext = false;
            }
        }
        else /* last entry in chain */
        {
            if (m_HashEntries[index].taken && !std::is_trivially_destructible_v<T>)
                m_HashEntries[index].data.~T();
            m_HashEntries[index].key = 0;
            m_HashEntries[index].nextIndex = 0;
            m_HashEntries[index].taken = false;
            m_HashEntries[index].hasNext = false;
            return true;
        }

        return false;
    }

public:
    LvnHashMap()
        : m_HashEntries(nullptr), m_Size(0), m_Capacity(0) {}
    ~LvnHashMap()
    {
        destruct();
        lvn::memDelete(m_HashEntries, 0);
        m_Size = m_Capacity = 0;
        m_HashEntries = nullptr;
    }

    LvnHashMap(size_t size)
        : m_Size(0)
    {
        reserve(size);
    }

    LvnHashMap(const LvnHashMap& other)
    {
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = lvn::memNew<LvnHashEntry<K, T>>(m_Capacity, false);
        for (size_t i = 0; i < other.m_Capacity; i++)
        {
            if (other.m_HashEntries[i].taken)
                new (&m_HashEntries[i]) LvnHashEntry<K, T>(other.m_HashEntries[i]);
        }
    }
    LvnHashMap(LvnHashMap&& other)
    {
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = other.m_HashEntries;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_HashEntries = nullptr;
    }
    LvnHashMap& operator=(const LvnHashMap& other)
    {
        if (this == &other) return *this;
        destruct();
        lvn::memDelete<LvnHashEntry<K, T>>(m_HashEntries, 0);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = lvn::memNew<LvnHashEntry<K, T>>(other.m_Capacity, false);
        for (size_t i = 0; i < other.m_Capacity; i++)
        {
            if (other.m_HashEntries[i].taken)
                new (&m_HashEntries[i]) LvnHashEntry<K, T>(other.m_HashEntries[i]);
        }
        return *this;
    }
    LvnHashMap& operator=(LvnHashMap&& other)
    {
        destruct();
        lvn::memDelete<LvnHashEntry<K, T>>(m_HashEntries, 0);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = other.m_HashEntries;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_HashEntries = nullptr;
        return *this;
    }

    T& operator[](const K& key)
    {
        return at(key);
    }
    const T& operator[](K key) const
    {
        return at(key);
    }

    /* reserves new memory space and rehashes entries */
    void reserve(size_t size)
    {
        /* step 1: reserve/allocate memory */
        if (size <= m_Size) return;
        LvnHashEntry<K, T>* temp = m_HashEntries;
        size_t tempSize = m_Capacity;
        m_HashEntries = lvn::memNew<LvnHashEntry<K, T>>(size);
        m_Capacity = size;

        /* step 2: rehash and insert entries into new table */
        m_Size = 0;
        for (size_t i = 0; i < tempSize; i++)
        {
            if (temp[i].taken)
                insert(temp[i].key, static_cast<MoveRef>(temp[i].data));
        }
        destruct();
        lvn::memDelete<LvnHashEntry<K, T>>(temp, 0);
    }
    void insert(const K& key, const T& value)
    {
        /* resize/rehash when size exceeds 70% capacity (0.7 load factor) */
        if (m_Size * 10 >= m_Capacity * 7)
            reserve(m_Capacity ? m_Capacity * 2 : 8);

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
        {
            m_HashEntries[index].data = value;
            return;
        }

        /* iterate through entries if key not found */
        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
            {
                entry->data = value;
                return;
            }
        }

        /* add a new entry if key still not found */
        LvnHashEntry<K, T>* findEntry = &m_HashEntries[index];
        while (findEntry->taken)
        {
            index = (index + 1) % m_Capacity;
            findEntry = &m_HashEntries[index];
        }

        findEntry->key = key;
        findEntry->data = value;
        findEntry->taken = true;
        m_Size++;

        if (entry->key != findEntry->key)
        {
            entry->nextIndex = index;
            entry->hasNext = true;
        }
    }
    void insert(const K& key, T&& value)
    {
        /* resize/rehash when size exceeds 70% capacity (0.7 load factor) */
        if (m_Size * 10 >= m_Capacity * 7)
            reserve(m_Capacity ? m_Capacity * 2 : 8);

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
        {
            m_HashEntries[index].data = static_cast<MoveRef>(value);
            return;
        }

        /* iterate through entries if key not found */
        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
            {
                entry->data = static_cast<MoveRef>(value);
                return;
            }
        }

        /* add a new entry if key still not found */
        LvnHashEntry<K, T>* findEntry = &m_HashEntries[index];
        while (findEntry->taken)
        {
            index = (index + 1) % m_Capacity;
            findEntry = &m_HashEntries[index];
        }

        findEntry->key = key;
        findEntry->data = static_cast<MoveRef>(value);
        findEntry->taken = true;
        m_Size++;

        if (entry->key != findEntry->key)
        {
            entry->nextIndex = index;
            entry->hasNext = true;
        }
    }
    void erase(const K& key)
    {
        if (m_Size == 0) return;

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].key == key)
        {
            erase_recursive(index);
            return;
        }

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
            {
                erase_recursive(index);
                return;
            }
        }
    }
    T& at(const K& key)
    {
        if (m_Size == 0)
            insert(key, T{});

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
            return m_HashEntries[index].data;

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
                return entry->data;
        }

        /* if key not found, create new entry */
        insert(key, T{});
        return at(key);
    }
    const T& at(const K& key) const
    {
        if (m_Size == 0)
            insert(key, T{});

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
            return m_HashEntries[index].data;

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
                return entry->data;
        }

        /* if key not found, create new entry */
        insert(key, T{});
        return at(key);
    }

    bool contains(const K& key)
    {
        if (m_Size == 0) return false;

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (key == m_HashEntries[index].key)
            return true;

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
                return true;
        }

        return false;
    }

    bool                   empty() { return m_Size == 0; }
    void                   clear() { if (m_Size) { destruct(); } m_Size = 0; }
    void                   clear_free() { lvn::memDelete<LvnHashEntry<K, T>>(m_HashEntries, m_Capacity); m_Size = m_Capacity = 0; m_HashEntries = nullptr; }
    size_t                 size() { return m_Size; }
    size_t                 capacity() { return m_Capacity; }
    size_t                 memcap() { return m_Capacity * sizeof(LvnHashEntry<K, T>); }
    LvnHashEntry<K, T>*    data() { return m_HashEntries; }
};

// -- logging

struct LvnLogger
{
    LvnString loggerName;
    LvnString logPatternFormat;
    LvnLogLevel logLevel;
    LvnVector<LvnLogPattern> logPatterns;

    LvnLogFile logfile;
};

struct LvnLoggingContext
{
    bool                                 logging;
    bool                                 enableCoreLogging;
    LvnLogger                            coreLogger;
    LvnLogger                            clientLogger;
    LvnVector<LvnLogPattern>             userLogPatterns;
    LvnString                            appName;
    size_t                               sTypeMemoryAllocationCounts[Lvn_Stype_Max_Value];
};


namespace lvn
{
    template <typename T>
    T* createObject(LvnStructureType stype = Lvn_Stype_Undefined)
    {
        LVN_ASSERT(stype != Lvn_Stype_Max_Value, "sType cannot be max value"); 
        T* object = lvn::memNew<T>();
        lvn::getLoggingContex()->sTypeMemoryAllocationCounts[stype] += 1;
        return object;
    }

    template <typename T>
    void destroyObject(T* object, LvnStructureType stype = Lvn_Stype_Undefined)
    {
        LVN_ASSERT(stype != Lvn_Stype_Max_Value, "sType cannot be max value"); 
        lvn::memDelete<T>(object);
        lvn::getLoggingContex()->sTypeMemoryAllocationCounts[stype] -= 1;
    }
}

#endif /* !HG_LVN_INTERNAL_H */
