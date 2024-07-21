#ifndef iGameArrayObject_h
#define iGameArrayObject_h

#include "iGameObject.h"
#include "iGameVector.h"

IGAME_NAMESPACE_BEGIN
class ArrayObject : public Object {
public:
    I_OBJECT(ArrayObject);
    //static Pointer New() { return new ArrayObject; }

    virtual double GetValue(const igIndex _Pos) = 0;
    virtual void SetValue(igIndex valueId, double value) = 0;

    virtual void GetElement(const igIndex _Pos, double* _Element) = 0;
    virtual void GetElement(const igIndex _Pos, std::vector<double>& _Element) = 0;
    virtual int GetElementSize(const igIndex _Pos) = 0;

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