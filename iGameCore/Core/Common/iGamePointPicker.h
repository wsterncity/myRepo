#ifndef iGamePointPicker_h
#define iGamePointPicker_h

#include "iGamePicker.h"
#include "iGamePointSet.h"
#include "iGameLine.h"

IGAME_NAMESPACE_BEGIN
class PointPicker : public Filter {
public:
	I_OBJECT(PointPicker);
	static Pointer New() { return new PointPicker; }

	void SetPointSet(PointSet::Pointer points)
	{
		if (m_PointSet != points) {
			m_PointSet = points;
		}
	}

	void SetPickRadius(double radius) {
		m_PickRadius = radius;
	}

	igIndex PickClosetPointWithLine(const Vector3d& linePoint, const Vector3d& lineDir) {
		if (m_PointSet == nullptr)
		{
			return (-1);
		}

		igIndex closeId = -1;
		double minDist = std::numeric_limits<double>::max();

		for (int i = 0; i < m_PointSet->GetNumberOfPoints(); i++) {
			auto& point = m_PointSet->GetPoint(i);
			double dist = Line::ComputePointToLineDis(linePoint, lineDir, point);

			if (dist < minDist && dist < m_PickRadius) {
				closeId = i;
				minDist = dist;
			}
		}
		return closeId;
	}

protected:
	PointPicker() {}
	~PointPicker() override {}

	PointSet::Pointer m_PointSet{};
	double m_PickRadius{ 1.0 };
};
IGAME_NAMESPACE_END
#endif