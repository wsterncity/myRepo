#include "iGamePoints.h"

IGAME_NAMESPACE_BEGIN

void Points::GetPoint(igIndex ptId, Vector3d& p)
{
	float* ptr = this->Buffer->Data + ptId * 3;
	for (int i = 0; i < 3; i++) {
		p[i] = ptr[i];
	}
}
void Points::GetPoint(igIndex ptId, Vector3f& p)
{
	float* ptr = this->Buffer->Data + ptId * 3;
	for (int i = 0; i < 3; i++) {
		p[i] = ptr[i];
	}
}
Point& Points::GetPoint(igIndex ptId) {
	float* ptr = this->Buffer->Data + ptId * 3;
	Point* p = reinterpret_cast<Point*>(ptr);
	return *p;
}
const Point& Points::GetPoint(igIndex ptId) const {
	float* ptr = this->Buffer->Data + ptId * 3;
	const Point* p = reinterpret_cast<const Point*>(ptr);
	return *p;
}

void Points::SetPoint(igIndex ptId, const Vector3d& p)
{
	float* ptr = this->Buffer->Data + ptId * 3;
	for (int i = 0; i < 3; i++)
	{
		ptr[i] = p[i];
	}
}
void Points::SetPoint(igIndex ptId, const Vector3f& p)
{
	float* ptr = this->Buffer->Data + ptId * 3;
	for (int i = 0; i < 3; i++)
	{
		ptr[i] = p[i];
	}
}
void Points::SetPoint(igIndex ptId, float x, float y, float z) {
	float* ptr = this->Buffer->Data + ptId * 3;
	ptr[0] = x;
	ptr[1] = y;
	ptr[2] = z;
}

igIndex Points::AddPoint(const Vector3d& p)
{
	if (this->NumberOfPoints * 3 >= this->Size)
	{
		if (!this->Resize(2 * this->NumberOfPoints + 1)) // grow by factor of 2
		{
			return this->NumberOfPoints - 1;
		}
	}

	float* ptr = this->Buffer->Data + this->NumberOfPoints * 3;
	for (int i = 0; i < 3; i++) {
		ptr[i] = p[i];
	}
	return this->NumberOfPoints++;
}

igIndex Points::AddPoint(const Vector3f& p)
{
	if (this->NumberOfPoints * 3 >= this->Size)
	{
		if (!this->Resize(2 * this->NumberOfPoints + 1)) // grow by factor of 2
		{
			return this->NumberOfPoints - 1;
		}
	}

	float* ptr = this->Buffer->Data + this->NumberOfPoints * 3;
	for (int i = 0; i < 3; i++) {
		ptr[i] = p[i];
	}
	return this->NumberOfPoints++;
}

igIndex Points::AddPoint(float x, float y, float z)
{
	if (this->NumberOfPoints * 3 >= this->Size)
	{
		if (!this->Resize(2 * this->NumberOfPoints + 1)) // grow by factor of 2
		{
			return this->NumberOfPoints - 1;
		}
	}

	float* ptr = this->Buffer->Data + this->NumberOfPoints * 3;
	ptr[0] = x;
	ptr[1] = y;
	ptr[2] = z;
	return this->NumberOfPoints++;
}

igIndex Points::AddPoint(float p[3])
{
	if (this->NumberOfPoints * 3 >= this->Size)
	{
		if (!this->Resize(2 * this->NumberOfPoints + 1)) // grow by factor of 2
		{
			return this->NumberOfPoints - 1;
		}
	}

	float* ptr = this->Buffer->Data + this->NumberOfPoints * 3;
	for (int i = 0; i < 3; i++) {
		ptr[i] = p[i];
	}
	return this->NumberOfPoints++;
}

igIndex Points::AddPoint(double p[3]) {
	if (this->NumberOfPoints * 3 >= this->Size)
	{
		if (!this->Resize(2 * this->NumberOfPoints + 1)) // grow by factor of 2
		{
			return this->NumberOfPoints - 1;
		}
	}
	float* ptr = this->Buffer->Data + this->NumberOfPoints * 3;
	for (int i = 0; i < 3; i++) {
		ptr[i] = p[i];
	}
	return this->NumberOfPoints++;
}
bool Points::Resize(const igIndex numPoints)
{
	igIndex newSize = numPoints * 3;

	if (newSize == this->Size)
	{
		return true;
	}

	if (newSize <= 0)
	{
		this->Initialize();
		return true;
	}

	this->Buffer->Resize(newSize);

	if (this->NumberOfPoints > numPoints)
	{
		this->NumberOfPoints = numPoints;
	}

	this->Size = newSize;
	return true;
}
IGAME_NAMESPACE_END
