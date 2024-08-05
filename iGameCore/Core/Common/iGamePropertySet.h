#ifndef iGamePropertySet_h
#define iGamePropertySet_h

#include "iGameFlatArray.h"
#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
// Property set stores all property data, like scalar, vector etc
class PropertySet : public Object {
public:
    I_OBJECT(PropertySet);
    static Pointer New() { return new PropertySet; }

    struct Property 
    {
        ArrayObject::Pointer pointer{};
        IGenum type{ IG_NONE };
        IGenum attachmentType{ IG_NONE };
        bool isDeleted{ false };
    };

    // Add a scalar property to array back.
    IGsize AddScalar(IGenum attachmentType, ArrayObject::Pointer attr);

    // Add a scalar property to array back.
    IGsize AddVector(IGenum attachmentType, ArrayObject::Pointer attr);

    // Add a property to array back.
    // @param type: The type of property
    // @param attachmentType: Which element is attached to, such as point,cell
    // @param attr: The pointer of property array
    IGsize AddProperty(IGenum type, IGenum attachmentType,
                       ArrayObject::Pointer attr);

    // Get a property by index
    Property& GetProperty(const IGsize index);
    const Property& GetProperty(const IGsize index) const;

    // Get pointer of a property by type,attachmentType and name.
    ArrayObject* GetArrayPointer(IGenum type, IGenum attachmentType,
                                 const std::string& name);

    // Delete a property by index
    void DeleteProperty(const IGsize index);
    // Get all propertys
    ElementArray<Property>::Pointer GetAllPropertys();

    void AllocateSizeWithCopy(PropertySet* ps,IGsize numCells) {
        auto& allarray = ps->GetAllPropertys();
        this->m_Buffer->Resize(allarray->Size());
        for (int i = 0; i < allarray->Size(); i++) {
            auto& array = ps->GetProperty(i);
            if (array.attachmentType == IG_POINT) {
                this->m_Buffer->AddElement(array);
            }
            else {
                Property EmptyArray;
                EmptyArray.type = IG_ARRAY_OBJECT;
                EmptyArray.attachmentType = IG_CELL;
                EmptyArray.isDeleted = false;
                this->m_Buffer->AddElement(EmptyArray);
            }
        }
    }

protected:
    PropertySet();
    ~PropertySet() override = default;

    ElementArray<Property>::Pointer m_Buffer{};
};
IGAME_NAMESPACE_END
#endif