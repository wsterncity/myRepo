#ifndef iGameArrayObject_h
#define iGameArrayObject_h

#include "iGameObject.h"
#include "iGameVector.h"

IGAME_NAMESPACE_BEGIN
class ArrayObject : public Object {
public:
    I_OBJECT(ArrayObject);
    //static Pointer New() { return new ArrayObject; }

    virtual IGsize GetNumberOfElements() const = 0;
    virtual IGsize GetNumberOfValues() const = 0;
    virtual void SetElementSize(const int _Newsize) = 0;
    virtual int GetElementSize() = 0;

    virtual double GetValue(const IGsize _Pos) = 0;
    virtual void SetValue(IGsize _Pos, double value) = 0;

    virtual void GetElement(const IGsize _Pos, float* _Element) = 0;
    virtual void GetElement(const IGsize _Pos, double* _Element) = 0;
    virtual void GetElement(const IGsize _Pos, std::vector<float>& _Element) = 0;
    virtual void GetElement(const IGsize _Pos, std::vector<double>& _Element) = 0;

    virtual IGenum GetArrayType() {
        return IG_ARRAY_OBJECT;
    }

    template<typename _Ty>
    SmartPointer<_Ty> DownCast() {
        return DynamicCast<_Ty>(this);
    }

protected:
    ArrayObject() {}
    ~ArrayObject() override = default;
};
IGAME_NAMESPACE_END
#endif