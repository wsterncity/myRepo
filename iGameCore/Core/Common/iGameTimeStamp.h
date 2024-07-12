#ifndef iGameTimeStamp_h
#define iGameTimeStamp_h

#include "iGameMacro.h"
#include "iGameType.h"

IGAME_NAMESPACE_BEGIN

class TimeStamp
{
public:
    using Self = TimeStamp;
    using ModifiedTimeType = unsigned long;
    TimeStamp() {}
    ~TimeStamp() {}
    

    void Delete()
    {
        delete this;
    }

    static const char* GetNameOfClass()
    {
        return "TimeStamp";
    }

    void Modified()
    {
        static std::atomic<ModifiedTimeType> GlobalTimeStamp(0U);
        this->m_ModifiedTime = ++GlobalTimeStamp;
    }

    ModifiedTimeType GetMTime() const
    {
        return m_ModifiedTime;
    }

    bool operator>(TimeStamp& ts) const
    {
        return (m_ModifiedTime > ts.m_ModifiedTime);
    }
    bool operator<(TimeStamp& ts) const
    {
        return (m_ModifiedTime < ts.m_ModifiedTime);
    }

    operator ModifiedTimeType() const { return m_ModifiedTime; }

private:
    ModifiedTimeType m_ModifiedTime{ 0 };
};
IGAME_NAMESPACE_END
#endif

