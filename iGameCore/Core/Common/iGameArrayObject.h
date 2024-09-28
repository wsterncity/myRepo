#ifndef iGameArrayObject_h
#define iGameArrayObject_h

#include "iGameObject.h"
#include "iGameVector.h"

IGAME_NAMESPACE_BEGIN
class ArrayObject : public Object {
public:
	I_OBJECT(ArrayObject);
	//static Pointer New() { return new ArrayObject; }

	// Get the number of elements
	virtual IGsize GetNumberOfElements() const = 0;

	// Get the number of values
	virtual IGsize GetNumberOfValues() const = 0;

	// Set the dimension of the element, such as VECTOR = 3
	virtual void SetDimension(const int _Newsize) = 0;

	// Get the dimension of the element
	virtual int GetDimension() = 0;

	// Get value by index _Pos. Return double type.
	virtual double GetValue(const IGsize _Pos) = 0;

	// Set value by index _Pos.
	virtual void SetValue(IGsize _Pos, double value) = 0;

	// Get element by index _Pos.
	virtual void GetElement(const IGsize _Pos, float* _Element) = 0;
	virtual void GetElement(const IGsize _Pos, double* _Element) = 0;
	virtual void GetElement(const IGsize _Pos, std::vector<float>& _Element) = 0;
	virtual void GetElement(const IGsize _Pos, std::vector<double>& _Element) = 0;

	// Get the type of array.
	virtual IGenum GetArrayType() { return IG_ARRAY_OBJECT; }

	virtual IGsize GetArrayTypedSize() { return 1; };
	// Type down-cast, which is a subclass of ArrayObject
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