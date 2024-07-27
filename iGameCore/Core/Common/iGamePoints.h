#ifndef iGamePoints_h
#define iGamePoints_h

#include "iGameObject.h"
#include "iGameVector.h"
#include "iGameFlatArray.h"

IGAME_NAMESPACE_BEGIN

typedef Vector3f Point;
class Points : public Object {
public:
	I_OBJECT(Points);
	static Pointer New() { return new Points; }

	void Initialize();

	void Reserve(const IGsize _NewPointsNum);

	void Resize(const IGsize _NewPointsNum);

	void Reset();

	void Squeeze();

	void SetNumberOfPoints(const IGsize _NewPointsNum);

	IGsize GetNumberOfPoints() const noexcept;

	void GetPoint(const IGsize ptId, Vector3d& p);
	void GetPoint(const IGsize ptId, Vector3f& p);
	Point& GetPoint(const IGsize ptId);
	const Point& GetPoint(const IGsize ptId) const;

	void SetPoint(const IGsize ptId, const Vector3d& p);
	void SetPoint(const IGsize ptId, const Vector3f& p);
	void SetPoint(const IGsize ptId, float x, float y, float z);

	IGsize AddPoint(const Vector3d& p);
	IGsize AddPoint(const Vector3f& p);
	IGsize AddPoint(float x, float y, float z);
	IGsize AddPoint(float p[3]);
	IGsize AddPoint(double p[3]);

	FloatArray::Pointer ConvertToArray();

	float* RawPointer();

protected:
	Points();
	~Points() override = default;

	FloatArray::Pointer m_Buffer{};
};

IGAME_NAMESPACE_END
#endif