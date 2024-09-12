#ifndef iGameAttributeSet_h
#define iGameAttributeSet_h

#include "iGameFlatArray.h"
#include "iGameElementArray.h"
#include <variant>

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

        std::pair<float, float> dataRange {0.f, 0.f};

        static Attribute None() {
            Attribute att;
            att.pointer = nullptr;
            att.type = IG_NONE;
            att.attachmentType = IG_NONE;
            att.isDeleted = false;
            return att;
        }

        bool isNone() const {
            return pointer == nullptr || type == IG_NONE || attachmentType == IG_NONE || isDeleted == true;
        }
    };

    // Add a scalar attribute to array back.
    IGsize AddScalar(IGenum attachmentType, ArrayObject::Pointer attr);
    // Add a scalar attribute to array back with range.
    IGsize AddScalar(IGenum attachmentType, ArrayObject::Pointer attr, const std::pair<float, float>& range);

    // Add a vector attribute to array back.
    IGsize AddVector(IGenum attachmentType, ArrayObject::Pointer attr);

    Attribute& GetScalar();
    const Attribute& GetScalar() const;
    Attribute& GetScalar(const IGsize i);
    const Attribute& GetScalar(const IGsize i) const;
    Attribute& GetScalar(const std::string& name);
    const Attribute& GetScalar(const std::string& name) const;

    Attribute& GetVector();
    const Attribute& GetVector() const;
    Attribute& GetVector(const IGsize i);
    const Attribute& GetVector(const IGsize i) const;
    Attribute& GetVector(const std::string& name);
    const Attribute& GetVector(const std::string& name) const;

    // Add a attribute to array back.
    // @param type: The type of attribute
    // @param attachmentType: Which element is attached to, such as point,cell
    // @param attr: The pointer of attribute array
    IGsize AddAttribute(IGenum type, IGenum attachmentType, ArrayObject::Pointer attr, std::pair<float, float> dataRange = {0.f, 0.f});

    // Get a attribute by index
    Attribute& GetAttribute(const IGsize index);
    const Attribute& GetAttribute(const IGsize index) const;

    // Get a attribute by index and type
    Attribute& GetAttribute(const IGsize index, IGenum type);
    const Attribute& GetAttribute(const IGsize index, IGenum type) const;

    // Get a attribute by name and type
    Attribute& GetAttribute(const std::string& name, IGenum type);
    const Attribute& GetAttribute(const std::string& name, IGenum type) const;

    // Get pointer of a attribute by type,attachmentType and name.
    ArrayObject* GetArrayPointer(IGenum type, IGenum attachmentType,
                                 const std::string& name);

    // Delete a attribute by index
    void DeleteAttribute(const IGsize index);
    // Get all attributes
    ElementArray<Attribute>::Pointer GetAllAttributes();
    // Get all point attributes, not thread safe
    ElementArray<Attribute>::Pointer GetAllPointAttributes();

    // Get all cell attributes, not thread safe
    ElementArray<Attribute>::Pointer GetAllCellAttributes();
    void AllocateSizeWithCopy(AttributeSet* ps,IGsize numCells) {
        auto allarray = ps->GetAllAttributes();
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
    ElementArray<Attribute>::Pointer tmpBuffer{};

    Attribute NONE{ AttributeSet::Attribute::None() };
};
IGAME_NAMESPACE_END
#endif