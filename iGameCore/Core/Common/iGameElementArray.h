#ifndef iGameElementArray_h
#define iGameElementArray_h

#include "iGameObject.h"
#include "iGameVector.h"

IGAME_NAMESPACE_BEGIN
template<typename TElement>
class ElementArray : public Object, private std::vector<TElement>{
public:
    I_OBJECT(ElementArray);
    static Pointer New() { return new ElementArray; }
    using VectorType = std::vector<TElement>;
    using Iterator = typename VectorType::iterator;
    using ConstIterator = typename VectorType::const_iterator;
    using Reference = typename VectorType::reference;
    using ConstReference = typename VectorType::const_reference;

    void Initialize() {
        this->VectorType::swap(std::vector<TElement>());
    }

    void Resize(const IGsize _Newsize) {
        this->VectorType::resize(_Newsize);
    }

    void Resize(const IGsize _Newsize, const TElement& _Element) {
        this->VectorType::resize(_Newsize, _Element);
    }

    void Reserve(const IGsize _Newcapacity) {
        this->VectorType::reserve(_Newcapacity);
    }

    void Reset() {
        this->VectorType::clear();
    }

    void Squeeze() {
        this->Resize(Size());
    }

    void AddElement(TElement&& _Element) {
        this->VectorType::push_back(_Element);
    }
    void AddElement(const TElement& _Element) {
        this->VectorType::push_back(_Element);
    }

    Reference ElementAt(const IGsize _Pos) {
        return this->VectorType::operator[](_Pos);
    }
    ConstReference ElementAt(const IGsize _Pos) const {
        return this->VectorType::operator[](_Pos);
    }

    Reference GetElement(const IGsize _Pos) {
        return this->VectorType::operator[](_Pos);
    }
    ConstReference GetElement(const IGsize _Pos) const {
        return this->VectorType::operator[](_Pos);
    }

    void SetElement(const IGsize _Pos, TElement&& _Element) {
        this->VectorType::operator[](_Pos) = _Element;
    }
    void SetElement(const IGsize _Pos, const TElement& _Element) {
        this->VectorType::operator[](_Pos) = _Element;
    }

    Iterator Begin() {
        return this->VectorType::begin();
    }
    ConstIterator Begin() const {
        return this->VectorType::begin();
    }
    Iterator End() {
        return this->VectorType::end();
    }
    ConstIterator End() const {
        return this->VectorType::end();
    }

    IGsize GetNumberOfElements() const {
        return this->VectorType::size();
    }

    IGsize Size() const {
        return this->VectorType::size();
    }

    TElement* RawPointer() {
        return this->VectorType::data();
    }

protected:
    ElementArray() {}
    ~ElementArray() override = default;
};

class StringArray : public ElementArray<std::string> {
public:
    I_OBJECT(StringArray);
    static Pointer New() { return new StringArray; }

protected:
    StringArray() = default;
    ~StringArray() override = default;
};

class IdArray : public ElementArray<igIndex> {
public:
    I_OBJECT(IdArray);
    static Pointer New() { return new IdArray; }
    using SuperClass = ElementArray<igIndex>;
    using Reference = typename SuperClass::Reference;
    using ConstReference = typename SuperClass::ConstReference;

    Reference GetId(const IGsize _Pos) {
        return this->SuperClass::GetElement(_Pos);
    }
    ConstReference GetId(const IGsize _Pos) const {
        return this->SuperClass::GetElement(_Pos);
    }

    void SetId(const IGsize _Pos, igIndex&& _Id) {
        this->SuperClass::SetElement(_Pos, _Id);
    }
    void SetId(const IGsize _Pos, const igIndex& _Id) {
        this->SuperClass::SetElement(_Pos, _Id);
    }

    void AddId(igIndex&& _Id) {
        this->SuperClass::AddElement(_Id);
    }
    void AddId(const igIndex& _Id) {
        this->SuperClass::AddElement(_Id);
    }

    IGsize SearchId(igIndex id) const
    {
        IGsize i;
        for (i = 0; i < this->GetNumberOfIds(); i++)
        {
            if (this->GetId(i) == id)
            {
                return i;
            }
        }
        return (-1);
    }

    void SetNumberOfIds(const IGsize _Newsize) {
        this->SuperClass::Resize(_Newsize);
    }

    IGsize GetNumberOfIds() const {
        return this->SuperClass::GetNumberOfElements();
    }

    Reference GetElement(const IGsize _Pos) = delete;
    ConstReference GetElement(const IGsize _Pos) const = delete;
    void SetElement(const IGsize _Pos, igIndex&& _Element) = delete;
    void SetElement(const IGsize _Pos, const igIndex& _Element) = delete;
    void AddElement(igIndex&& _Element) = delete;
    void AddElement(const igIndex& _Element) = delete;
    IGsize GetNumberOfElements() const = delete;
protected:
    IdArray() = default;
    ~IdArray() override = default;
};
IGAME_NAMESPACE_END
#endif