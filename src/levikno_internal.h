#ifndef HG_LEVIKNO_INTERNAL_H
#define HG_LEVIKNO_INTERNAL_H

#include "levikno.h"



// -- logging

struct LvnLogger
{
    LvnString loggerName;
    LvnString logPatternFormat;
    LvnLogLevel logLevel;
    LvnVector<LvnLogPattern> logPatterns;
    LvnVector<LvnSink> sinks;
};

struct LvnContext
{
    // logging
    bool                                 logging;
    bool                                 enableCoreLogging;
    LvnLogger                            coreLogger;
    LvnLogger                            clientLogger;
    LvnString                            appName;
    LvnVector<LvnLogPattern>             userLogPatterns;
    LvnSink                              coreSink;

    // memory
    size_t                               sTypeMemoryAllocationCounts[Lvn_Stype_Max_Value];
    size_t                               memAllocCount;

    // api
    bool                                 graphicsInitCtx;
    void*                                sharedSuface;
};

template<typename T> struct LvnRemoveReference { using type = T; };
template<typename T> struct LvnRemoveReference<T&> { using type = T; };
template<typename T> struct LvnRemoveReference<T&&> { using type = T; };

namespace lvn
{
    template <typename T>
    T* createObject(LvnStructureType stype = Lvn_Stype_Undefined)
    {
        LVN_ASSERT(stype != Lvn_Stype_Max_Value, "sType cannot be max value"); 
        T* object = lvn::memNew<T>();
        lvn::getContext()->sTypeMemoryAllocationCounts[stype] += 1;
        return object;
    }

    template <typename T>
    void destroyObject(T* object, LvnStructureType stype = Lvn_Stype_Undefined)
    {
        LVN_ASSERT(stype != Lvn_Stype_Max_Value, "sType cannot be max value"); 
        lvn::memDelete<T>(object);
        lvn::getContext()->sTypeMemoryAllocationCounts[stype] -= 1;
    }

    template <typename T>
    constexpr typename LvnRemoveReference<T>::type&& move(T&& arg)
    {
        return static_cast<typename LvnRemoveReference<T>::type&&>(arg);
    }

    template <typename T>
    void swap(T& arg1, T& arg2)
    {
        T temp = lvn::move(arg1);
        arg1 = lvn::move(arg2);
        arg2 = lvn::move(temp);
    }

    template <typename T, size_t N>
    void swap(T (&arg1)[N], T (&arg2)[N])
    {
        for (size_t i = 0; i < N; i++)
            lvn::swap(arg1[i], arg2[i]);
    }
}

#endif /* !HG_LEVIKNO_INTERNAL_H */
