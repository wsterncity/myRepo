#include "iGamePropertySet.h"

IGAME_NAMESPACE_BEGIN

    // Add a scalar property to array back.
IGsize PropertySet::AddScalar(IGenum attachmentType,
                              ArrayObject::Pointer attr) {
    return this->AddProperty(IG_SCALAR, attachmentType, attr);
}

// Add a scalar property to array back.
IGsize PropertySet::AddVector(IGenum attachmentType,
                              ArrayObject::Pointer attr) {
    return this->AddProperty(IG_VECTOR, attachmentType, attr);
}

// Add a property to array back.
// @param type: The type of property
// @param attachmentType: Which element is attached to, such as point,cell
// @param attr: The pointer of property array
IGsize PropertySet::AddProperty(IGenum type, IGenum attachmentType,
                   ArrayObject::Pointer attr) {
    if (!attr) { return -1; }

    m_Buffer->AddElement(Property{attr, type, attachmentType, false});
    return m_Buffer->GetNumberOfElements() - 1;
}

// Get a property by index
PropertySet::Property& PropertySet::GetProperty(const IGsize index) {
    return m_Buffer->ElementAt(index);
}
const PropertySet::Property& 
PropertySet::GetProperty(const IGsize index) const {
    return m_Buffer->ElementAt(index);
}

// Get pointer of a property by type,attachmentType and name.
ArrayObject* PropertySet::GetArrayPointer(IGenum type, IGenum attachmentType,
                             const std::string& name) {
    for (int i = 0; i < m_Buffer->GetNumberOfElements(); i++) {
        auto& p = GetProperty(i);
        if (p.isDeleted) continue;
        if (p.attachmentType == attachmentType &&
            p.pointer->GetName() == name) {
            return p.pointer.get();
        }
    }
    return nullptr;
}

// Delete a property by index
void PropertySet::DeleteProperty(const IGsize index) {
    if (index < 0 || index >= m_Buffer->GetNumberOfElements()) { return; }
    auto& p = GetProperty(index);
    p.isDeleted = true;
    p.pointer = nullptr;
}

// Get all propertys
ElementArray<PropertySet::Property>::Pointer PropertySet::GetAllPropertys() {
    return m_Buffer;
}


PropertySet::PropertySet() { m_Buffer = ElementArray<Property>::New(); }

IGAME_NAMESPACE_END