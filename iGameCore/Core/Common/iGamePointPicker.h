#ifndef iGamePointPicker_h
#define iGamePointPicker_h

#include "iGamePicker.h"
#include "iGamePoints.h"
#include "iGameLine.h"

IGAME_NAMESPACE_BEGIN
class PointPicker : public Filter {
public:
	I_OBJECT(PointPicker);
	static Pointer New() { return new PointPicker; }

	void SetPoints(Points::Pointer points)
	{
		if (m_Points != points) {
			m_Points = points;
		}
		BoundingBox bbox;
		for (int i = 0; i < m_Points->GetNumberOfPoints(); i++) {
			auto& point = m_Points->GetPoint(i);
			bbox.add(point);
		}
		float len = (bbox.max - bbox.min).length();
		m_PickRadius = len * 0.005;
	}

	void SetPickRadius(double radius) {
		m_PickRadius = radius;
	}

	igIndex PickClosetPointOnLine(const Vector3d& startPoint, const Vector3d& lineDir) {
		if (m_Points == nullptr)
		{
			return (-1);
		}

		igIndex closeId = -1;
		double minDist = std::numeric_limits<double>::max();
		double zDist = std::numeric_limits<double>::max();

		for (int i = 0; i < m_Points->GetNumberOfPoints(); i++) {
			auto& point = m_Points->GetPoint(i);
			double dist = Line::ComputePointToLineDis(startPoint, lineDir, point);
			double dist2 = (point - startPoint).length();

			if (dist < minDist && dist < m_PickRadius && dist2 < zDist) {
				closeId = i;
				minDist = dist;
				zDist = dist2;
			}
		}
		return closeId;
	}

protected:
	PointPicker() {}
	~PointPicker() override {}

	Points::Pointer m_Points{};
	double m_PickRadius{ 0.01 };
};
IGAME_NAMESPACE_END
#endif