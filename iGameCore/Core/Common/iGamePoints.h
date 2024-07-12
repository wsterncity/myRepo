#ifndef iGamePoints_h
#define iGamePoints_h

#include "iGameObject.h"
#include "iGameVectorD.h"
#include "iGameDataBuffer.h"
#include "iGameDataArray.h"

IGAME_NAMESPACE_BEGIN

typedef Vector3f Point;
class Points : public Object {
public:
	I_OBJECT(Points);
	static Pointer New() { return new Points; }

	void Initialize()
	{
		this->InitializeMemory();
		this->NumberOfPoints = 0;
		this->Size = 0;
	}

	bool Allocate(igIndex numPoints, int strategy = 0)
	{
		return this->AllocateInternal(numPoints * 3, 0);
	}

	igIndex GetNumberOfPoints() const noexcept { return this->NumberOfPoints; }

	void GetPoint(igIndex ptId, Vector3d& p);
	void GetPoint(igIndex ptId, Vector3f& p);
	Point& GetPoint(igIndex ptId);
	const Point& GetPoint(igIndex ptId) const;

	void SetPoint(igIndex ptId, const Vector3d& p);
	void SetPoint(igIndex ptId, const Vector3f& p);
	void SetPoint(igIndex ptId, float x, float y, float z);

	igIndex AddPoint(const Vector3d& p);
	igIndex AddPoint(const Vector3f& p);
	igIndex AddPoint(float x, float y, float z);
	igIndex AddPoint(float p[3]);
	igIndex AddPoint(double p[3]);

	void SetNumberOfPoints(const igIndex numPoints)
	{
		this->AllocateInternal(numPoints * 3, numPoints);
	}

	bool Resize(const igIndex numPoints);

	/**
	 * Reset to an empty state but retain previously allocated memory.
	 */
	void Reset() 
	{
		this->NumberOfPoints = 0;
	}
	
	/**
	 * Free any unused memory.
	 */
	void Squeeze() { this->Resize(this->NumberOfPoints); }
	
	FloatArray::Pointer ConvertToDataArray() 
	{
		FloatArray::Pointer ConvertedArray = FloatArray::New();
		ConvertedArray->SetArray(this->Buffer, 3, this->NumberOfPoints * 3, this->Size);
		return ConvertedArray;
	}

	float* GetRawPointer() { return this->Buffer->Data; }

protected:
	Points() 
	{
		this->Buffer = DataBuffer<float>::New();
	}
	~Points() override = default;

	bool AllocateInternal(igIndex sz, igIndex numberOfPoints)
	{
		if (sz > this->Size)
		{
			this->InitializeMemory();
			this->Size = (sz > 0 ? sz : 1);
			this->Buffer->Allocate(this->Size);
		}
		this->NumberOfPoints = numberOfPoints;
		return true;
	}

	/**
	 * Release memory.
	 */
	void InitializeMemory()
	{
		this->Buffer->Initialize();
	}

	DataBuffer<float>::Pointer Buffer{};
	igIndex NumberOfPoints{ 0 };
	igIndex Size{ 0 };
};

IGAME_NAMESPACE_END
#endif