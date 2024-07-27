#ifndef iGamePropertySet_h
#define iGamePropertySet_h

#include "iGameFlatArray.h"
#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
class PropertySet : public Object {
public:
    I_OBJECT(PropertySet);
    static Pointer New() { return new PropertySet; }

    struct Property {
        ArrayObject::Pointer pointer{};
        IGenum type{ IG_NONE };
        IGenum attachmentType{ IG_NONE };
        bool isDeleted{ false };
    };

    IGsize AddScalars(IGenum attachmentType, ArrayObject::Pointer attr)
    {
        return this->AddProperty(IG_SCALAR, attachmentType, attr);
    }

    IGsize AddProperty(IGenum type, IGenum attachmentType, ArrayObject::Pointer attr)
    {
        if (!attr)
        {
            return -1;
        }

        m_Buffer->AddElement(Property{ attr, type, attachmentType, false });
        return m_Buffer->GetNumberOfElements() - 1;
    }

    Property& GetProperty(const IGsize index) { return m_Buffer->ElementAt(index); }
    const Property& GetProperty(const IGsize index) const { return m_Buffer->ElementAt(index); }

    ArrayObject* GetArrayPointer(IGenum type, IGenum attachmentType, const std::string& name)
    {
        for (int i = 0; i < m_Buffer->GetNumberOfElements(); i++)
        {
            auto& p = GetProperty(i);
            if (p.isDeleted) continue;
            if (p.attachmentType == attachmentType && p.pointer->GetName() == name)
            {
                return p.pointer.get();
            }
        }
        return nullptr;
    }

    void DeleteProperty(const IGsize index)
    {
        if (index < 0 || index >= m_Buffer->GetNumberOfElements())
        {
            return;
        }
        auto& p = GetProperty(index);
        p.isDeleted = true;
        p.pointer = nullptr;
    }

    ElementArray<Property>::Pointer GetAllPropertys() {
        return m_Buffer;
    }

protected:
    PropertySet() 
    {
        m_Buffer = ElementArray<Property>::New();
    }
    ~PropertySet() override {}

    ElementArray<Property>::Pointer m_Buffer{};
};
IGAME_NAMESPACE_END
#endif