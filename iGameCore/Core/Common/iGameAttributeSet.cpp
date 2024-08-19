#include "iGameAttributeSet.h"

IGAME_NAMESPACE_BEGIN

IGsize AttributeSet::AddScalar(IGenum attachmentType,
                              ArrayObject::Pointer attr) {
    return this->AddAttribute(IG_SCALAR, attachmentType, attr);
}


IGsize AttributeSet::AddVector(IGenum attachmentType,
                              ArrayObject::Pointer attr) {
    return this->AddAttribute(IG_VECTOR, attachmentType, attr);
}

AttributeSet::Attribute& AttributeSet::GetScalar()
{
    return GetScalar(0);
}

const AttributeSet::Attribute& AttributeSet::GetScalar() const
{
    return GetScalar(0);
}

AttributeSet::Attribute& AttributeSet::GetScalar(const IGsize index)
{
    return GetAttribute(index, IG_SCALAR);
}

const AttributeSet::Attribute& AttributeSet::GetScalar(const IGsize index) const
{
    return GetAttribute(index, IG_SCALAR);
}

AttributeSet::Attribute& AttributeSet::GetVector()
{
    return GetVector(0);
}

const AttributeSet::Attribute& AttributeSet::GetVector() const
{
    return GetVector(0);
}

AttributeSet::Attribute& AttributeSet::GetVector(const IGsize index)
{
    return GetAttribute(index, IG_VECTOR);
}

const AttributeSet::Attribute& AttributeSet::GetVector(const IGsize index) const
{
    return GetAttribute(index, IG_VECTOR);
}

IGsize AttributeSet::AddAttribute(IGenum type, IGenum attachmentType,
                   ArrayObject::Pointer attr) {
    if (!attr) { return -1; }

    m_Buffer->AddElement(Attribute{attr, type, attachmentType, false});
    return m_Buffer->GetNumberOfElements() - 1;
}


AttributeSet::Attribute& AttributeSet::GetAttribute(const IGsize index) {
    return m_Buffer->ElementAt(index);
}

const AttributeSet::Attribute& 
AttributeSet::GetAttribute(const IGsize index) const {
    return m_Buffer->ElementAt(index);
}

AttributeSet::Attribute& AttributeSet::GetAttribute(const IGsize index, IGenum type)
{
    int count = 0;
    for (int i = 0; i < m_Buffer->GetNumberOfElements(); i++) {
        auto& attrb = m_Buffer->GetElement(i);
        if (!attrb.isDeleted && attrb.type == type) {
            if (count == index) {
                return attrb;
            }
            count++;
        }
    }
    return AttributeSet::Attribute{};
}

const AttributeSet::Attribute& AttributeSet::GetAttribute(const IGsize index, IGenum type) const
{
    return GetAttribute(index, type);
}

ArrayObject* AttributeSet::GetArrayPointer(IGenum type, IGenum attachmentType,
                             const std::string& name) {
    for (int i = 0; i < m_Buffer->GetNumberOfElements(); i++) {
        auto& p = GetAttribute(i);
        if (p.isDeleted) continue;
        if (p.attachmentType == attachmentType &&
            p.pointer->GetName() == name) {
            return p.pointer.get();
        }
    }
    return nullptr;
}

void AttributeSet::DeleteAttribute(const IGsize index) {
    if (index < 0 || index >= m_Buffer->GetNumberOfElements()) { return; }
    auto& p = GetAttribute(index);
    p.isDeleted = true;
    p.pointer = nullptr;
}

ElementArray<AttributeSet::Attribute>::Pointer AttributeSet::GetAllAttributes() {
    return m_Buffer;
}


AttributeSet::AttributeSet() { m_Buffer = ElementArray<Attribute>::New(); }

IGAME_NAMESPACE_END