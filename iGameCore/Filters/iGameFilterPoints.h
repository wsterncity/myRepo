#ifndef FilterPoints_h
#define FilterPoints_h

#include "iGameFilter.h"
#include "iGamePointSet.h"
#include <random>

IGAME_NAMESPACE_BEGIN
class FilterPoints : public Filter {
public:
	I_OBJECT(FilterPoints);
	static Pointer New() { return new FilterPoints; }

	void SetFilterRate(double value) {
		m_FilterRate = value;
	}

protected:
	FilterPoints()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(1);
	}
	~FilterPoints() override = default;

	PointSet::Pointer m_PointSet{};
	double m_FilterRate{ 0.5 };

	bool Execute() override 
	{ 
		m_PointSet = DynamicCast<PointSet>(GetInput(0));
		if (m_PointSet == nullptr)
		{
			return false;
		}
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		m_PointSet->RequestEditStatus();
		for (int i = 0; i < m_PointSet->GetNumberOfPoints(); i++) {
			double random_number = dis(gen);
			if (random_number <= m_FilterRate) {
				m_PointSet->DeletePoint(i);
			}
		}
		m_PointSet->GarbageCollection();
		SetOutput(0, m_PointSet);
		return true;
	}
};
IGAME_NAMESPACE_END
#endif