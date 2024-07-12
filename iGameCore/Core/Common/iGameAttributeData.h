#ifndef iGameAttributeData_h
#define iGameAttributeData_h

#include "iGameDataArray.h"

IGAME_NAMESPACE_BEGIN
class AttributeData : public Object {
public:
    I_OBJECT(AttributeData);
    static Pointer New() { return new AttributeData; }

    struct Attribute {
        DataArray::Pointer array{};
        IGenum type{ IG_NONE };
        IGenum attachmentType{ IG_NONE };
        bool active{ true };
    };

    int AddScalars(IGenum attachmentType, DataArray::Pointer attr)
    {
        return this->AddAttribute(IG_SCALAR, attachmentType, attr);
    }

    int AddAttribute(IGenum type, IGenum attachmentType, DataArray::Pointer attr)
    {
        if (!attr)
        {
            return -1;
        }

        Attribute att;
        att.array = attr;
        att.type = type;
        att.attachmentType = attachmentType;
        att.active = true;
        m_Data.push_back(att);
        return m_Data.size() - 1;
    }


    Attribute& GetAttribute(int index) { return m_Data[index]; }
    const Attribute& GetAttribute(int index) const { return m_Data[index]; }

    DataArray* GetAttribute(IGenum type, IGenum attachmentType, const std::string& name)
    {
        for (int i = 0; i < this->m_Data.size(); i++)
        {
            if (m_Data[i].active && m_Data[i].attachmentType == attachmentType && m_Data[i].array->GetName() == name)
            {
                return m_Data[i].array.get();
            }
        }
        return nullptr;
    }

    void DeleteAttribute(int i)
    {
        if (i < 0 || i >= m_Data.size())
        {
            return;
        }
        m_Data[i].active = false;
        m_Data[i].array = nullptr;
    }

    std::vector<Attribute>& GetArrays() {
        return m_Data;
    }

protected:
    AttributeData() {}
    ~AttributeData() override {}

    std::vector<Attribute> m_Data;
};
IGAME_NAMESPACE_END
#endif