#ifndef iGameAttributeSet_h
#define iGameAttributeSet_h

#include "iGameFlatArray.h"
#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
// Attributes set stores all Attribute array data, like scalar, vector etc
class AttributeSet : public Object {
public:
    I_OBJECT(AttributeSet);
    static Pointer New() { return new AttributeSet; }

    struct Attribute 
    {
        ArrayObject::Pointer pointer{};
        IGenum type{ IG_NONE };
        IGenum attachmentType{ IG_NONE };
        bool isDeleted{ false };
    };

    // Add a scalar attribute to array back.
    IGsize AddScalar(IGenum attachmentType, ArrayObject::Pointer attr);

    // Add a scalar attribute to array back.
    IGsize AddVector(IGenum attachmentType, ArrayObject::Pointer attr);

    Attribute& GetScalar();
    const Attribute& GetScalar() const;
    Attribute& GetScalar(const IGsize i);
    const Attribute& GetScalar(const IGsize i) const;

    Attribute& GetVector();
    const Attribute& GetVector() const;
    Attribute& GetVector(const IGsize i);
    const Attribute& GetVector(const IGsize i) const;

    // Add a attribute to array back.
    // @param type: The type of attribute
    // @param attachmentType: Which element is attached to, such as point,cell
    // @param attr: The pointer of attribute array
    IGsize AddAttribute(IGenum type, IGenum attachmentType, ArrayObject::Pointer attr);

    // Get a attribute by index
    Attribute& GetAttribute(const IGsize index);
    const Attribute& GetAttribute(const IGsize index) const;

    // Get a attribute by index and type
    Attribute& GetAttribute(const IGsize index, IGenum type);
    const Attribute& GetAttribute(const IGsize index, IGenum type) const;

    // Get pointer of a attribute by type,attachmentType and name.
    ArrayObject* GetArrayPointer(IGenum type, IGenum attachmentType,
                                 const std::string& name);

    // Delete a attribute by index
    void DeleteAttribute(const IGsize index);
    // Get all attributes
    ElementArray<Attribute>::Pointer GetAllAttributes();

    void AllocateSizeWithCopy(AttributeSet* ps,IGsize numCells) {
        auto& allarray = ps->GetAllAttributes();
        this->m_Buffer->Resize(allarray->Size());
        for (int i = 0; i < allarray->Size(); i++) {
            auto& array = ps->GetAttribute(i);
            if (array.attachmentType == IG_POINT) {
                this->m_Buffer->AddElement(array);
            }
            else {
                Attribute EmptyArray;
                EmptyArray.type = IG_ARRAY_OBJECT;
                EmptyArray.attachmentType = IG_CELL;
                EmptyArray.isDeleted = false;
                this->m_Buffer->AddElement(EmptyArray);
            }
        }
    }

protected:
    AttributeSet();
    ~AttributeSet() override = default;

    ElementArray<Attribute>::Pointer m_Buffer{};
};
IGAME_NAMESPACE_END
#endif