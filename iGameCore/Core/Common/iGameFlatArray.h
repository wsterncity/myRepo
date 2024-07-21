#ifndef iGameFlatArray_h
#define iGameFlatArray_h

#include "iGameArrayObject.h"

IGAME_NAMESPACE_BEGIN
template<typename TValue>
class FlatArray : public ArrayObject, private std::vector<TValue> {
public:
    I_OBJECT(FlatArray);
    static Pointer New() { return new FlatArray; }
    using VectorType = std::vector<TValue>;
    using Iterator = typename VectorType::iterator;
    using ConstIterator = typename VectorType::const_iterator;
    using Reference = typename VectorType::reference;
    using ConstReference = typename VectorType::const_reference;
    using size_type = typename VectorType::size_type;

    void Initialize() {
        this->VectorType::swap(std::vector<TValue>());
    }

    void Reserve(const size_type _NewElementNum) {
        this->VectorType::reserve(_NewElementNum * ElementSize);
    }

    void Resize(const size_type _NewElementNum) {
        this->VectorType::resize(_NewElementNum * ElementSize);
    }

    void Reset() {
        this->VectorType::clear();
    }

    void Squeeze() {
        this->Resize(GetNumberOfElements());
    }

    void SetElementSize(const int _Newsize) {
        assert(_Newsize > 0);
        m_ElementSize = _Newsize;
    }

    size_type GetNumberOfValues() const {
        return this->VectorType::size();
    }

    size_type GetNumberOfElements() const {
        return this->GetNumberOfValues() / m_ElementSize;
    }

    template<int dimension_t>
    size_type InsertToBack(Vector<TValue, dimension_t>&& _Element) 
    {
        assert(dimension_t >= ElementSize);
        size_type index = this->GetNumberOfElements();
        if (index * m_ElementSize >= this->GetNumberOfValues())
        {
            this->Resize(2 * index + 1)
        }

        TValue* data = this->RawPointer(index);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
        return index;
    }
    template<int dimension_t>
    size_type InsertToBack(const Vector<TValue, dimension_t>& _Element)
    {
        assert(dimension_t >= ElementSize);
        size_type index = this->GetNumberOfElements();
        if (index * m_ElementSize >= this->GetNumberOfValues())
        {
            this->Resize(2 * index + 1)
        }

        TValue* data = this->RawPointer(index);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
        return index;
    }
    size_type InsertToBack(const std::vector<TValue>& _Element)
    {
        assert(_Element.size() >= ElementSize);
        size_type index = this->GetNumberOfElements();
        if (index * m_ElementSize >= this->GetNumberOfValues())
        {
            this->Resize(2 * index + 1)
        }

        TValue* data = this->RawPointer(index);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
        return index;
    }
    size_type InsertToBack(TValue* _Element)
    {
        size_type index = this->GetNumberOfElements();
        if (index * m_ElementSize >= this->GetNumberOfValues())
        {
            this->Resize(2 * index + 1)
        }

        TValue* data = this->RawPointer(index);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
        return index;
    }

    void ElementAt(const size_type _Pos, TValue*& _Element) {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        _Element = this->RawPointer(_Pos);
    }
    void ElementAt(const size_type _Pos, const TValue*& _Element) const {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        _Element = this->RawPointer(_Pos);
    }
    void GetElement(const size_type _Pos, TValue* _Element) const {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            _Element[i] = data[i];
        }
    }
    void GetElement(const size_type _Pos, std::vector<TValue>& _Element) const {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        _Element.clear();
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            _Element.push_back(data[i]);
        }
    }
    const std::vector<TValue>& GetElement(const size_type _Pos) {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        m_Element.clear();
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            _Element.push_back(data[i]);
        }
        return m_Element;
    }

    template<int dimension_t>
    void SetElement(const size_type _Pos, Vector<TValue, dimension_t>&& _Element) {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        assert(dimension_t >= ElementSize);
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
    }
    template<int dimension_t>
    void SetElement(const size_type _Pos, const Vector<TValue, dimension_t>& _Element) {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        assert(dimension_t >= ElementSize);
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
    }
    void SetElement(const size_type _Pos, const std::vector<TValue>& _Element) {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        assert(_Element.size() >= ElementSize);
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
    }
    void SetElement(const size_type _Pos, TValue* _Element) {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            data[i] = _Element[i];
        }
    }

    void InsertToBack(TValue&& _Value) {
        this->VectorType::push_back(_Value);
    }
    void InsertToBack(const TValue& _Value) {
        this->VectorType::push_back(_Value);
    }
    TValue GetValue(const size_type _Pos) {
        return this->VectorType::operator[](_Pos);
    }
    Reference ValueAt(const size_type _Pos) {
        return this->VectorType::operator[](_Pos);
    }
    ConstReference ValueAt(const size_type _Pos) const {
        return this->SuperClass::operator[](_Pos);
    }
    void SetValue(const size_type _Pos, TValue&& _Value) {
        this->VectorType::operator[](_Pos) = _Value;
    }
    void SetValue(const size_type _Pos, const TValue& _Value) {
        this->VectorType::operator[](_Pos) = _Value;
    }




    double GetValue(const igIndex _Pos) override {
        assert(0 <= _Pos && _Pos < this->GetNumberOfValues());
        return static_cast<double>(this->VectorType::operator[](_Pos));
    }
    void SetValue(igIndex valueId, double value) override {
        this->VectorType::operator[](_Pos) = static_cast<TValue>(_Value);
    }
    
    void GetElement(const igIndex _Pos, double* _Element) override {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            _Element[i] = static_cast<double>(data[i]);
        }
    }
    void GetElement(const igIndex _Pos, std::vector<double>& _Element) override {
        assert(0 <= _Pos && _Pos < this->GetNumberOfElements());
        _Element.clear();
        TValue* data = this->RawPointer(_Pos);
        for (int i = 0; i < m_ElementSize; ++i) {
            _Element.push_back(static_cast<double>(data[i]));
        }
    }
    
    int GetElementSize(const igIndex _Pos) override {
        return m_ElementSize;
    }

    TValue* RawPointer(const size_type _Pos = 0) {
        return this->VectorType::data() + _Pos * m_ElementSize;
    }

protected:
    FlatArray() = default;
    ~FlatArray() override = default;

    int m_ElementSize{ 1 };

    std::vector<TValue> m_Element{};
};
IGAME_NAMESPACE_END
#endif