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
    using size_type = typename VectorType::size_type;

    void Initialize() {
        this->VectorType::swap(std::vector<TElement>());
    }

    void Resize(const size_type _Newsize) {
        this->VectorType::resize(_Newsize);
    }

    void Resize(const size_type _Newsize, const TElement& _Element) {
        this->VectorType::resize(_Newsize, _Element);
    }

    void Reserve(const size_type _Newcapacity) {
        this->VectorType::reserve(_Newcapacity);
    }

    void Reset() {
        this->VectorType::clear();
    }

    void Squeeze() {
        this->Resize(Size());
    }

    void InsertToBack(TElement&& _Element) {
        this->VectorType::push_back(_Element);
    }
    void InsertToBack(const TElement& _Element) {
        this->VectorType::push_back(_Element);
    }

    Reference GetElement(const size_type _Pos) {
        return this->VectorType::operator[](_Pos);
    }
    ConstReference GetElement(const size_type _Pos) const {
        return this->VectorType::operator[](_Pos);
    }

    void SetElement(const size_type _Pos, TElement&& _Element) {
        this->VectorType::operator[](_Pos) = _Element;
    }
    void SetElement(const size_type _Pos, const TElement& _Element) {
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

    size_type GetNumberOfElements() const {
        return this->VectorType::size();
    }

    size_type Size() const {
        return this->VectorType::size();
    }

    TElement* RawPointer() {
        return this->VectorType::data();
    }

protected:
    ElementArray() {}
    ~ElementArray() override = default;
};


class IntArray2 : public ElementArray<int> {
public:
    I_OBJECT(IntArray2);
    static Pointer New() { return new IntArray2; }

protected:
    IntArray2() = default;
    ~IntArray2() override = default;
};

class FloatArray2 : public ElementArray<float> {
public:
    I_OBJECT(FloatArray2);
    static Pointer New() { return new FloatArray2; }

protected:
    FloatArray2() = default;
    ~FloatArray2() override = default;
};

class DoubleArray2 : public ElementArray<double> {
public:
    I_OBJECT(DoubleArray2);
    static Pointer New() { return new DoubleArray2; }

protected:
    DoubleArray2() = default;
    ~DoubleArray2() override = default;
};

class StringArray : public ElementArray<std::string> {
public:
    I_OBJECT(StringArray);
    static Pointer New() { return new StringArray; }

protected:
    StringArray() = default;
    ~StringArray() override = default;
};
IGAME_NAMESPACE_END
#endif