#ifndef iGamePointSet_h
#define iGamePointSet_h

#include "iGameDataObject.h"
#include "iGamePoints.h"
#include "iGameAttributeData.h"
#include "iGameDrawableObject.h"

IGAME_NAMESPACE_BEGIN
class PointSet : public DataObject {
public:
	I_OBJECT(PointSet);
	static Pointer New() { return new PointSet; }

	igIndex GetNumberOfPoints() { return m_Points ? m_Points->GetNumberOfPoints() : 0;}

	Points::Pointer GetPoints() { return m_Points; }

	Point& GetPoint(igIndex ptId) { return m_Points->GetPoint(ptId); }

	virtual void SetPoints(Points::Pointer points) 
	{
		if (m_Points != points)
		{
			m_Points = points;
			this->Modified();
		}
	}

	virtual void SetPoint(igIndex ptId, const Point& p) 
	{
		m_Points->SetPoint(ptId, p);
	}

	virtual igIndex AddPoint(const Point& p) 
	{
		return m_Points->AddPoint(p);
	}

protected:
	PointSet() { 
		m_Points = Points::New();
		m_ViewStyle = IG_POINT;
	}
	~PointSet() override {}

	Points::Pointer m_Points{};

public:
	void Draw(Scene*) override;
	void ConvertToDrawableData() override;
	bool IsDrawable() override { return true; }
	void ViewCloudPicture(int index, int demension = -1) override
	{
		auto& attr = this->GetAttributes()->GetAttribute(index);
		if (attr.active && attr.attachmentType == IG_POINT)
		{
			this->SetAttributeWithPointData(attr.array, demension);
		}
	}

	virtual void SetAttributeWithPointData(DataArray::Pointer attr, igIndex i = -1)
	{
		if (m_ViewAttribute != attr || m_ViewDemension != i)
		{
			if (attr == nullptr)
			{
				m_UseColor = false;
				m_ViewAttribute = nullptr;
				m_ViewDemension = -1;
				return;
			}
			m_ViewAttribute = attr;
			m_ViewDemension = i;

			m_UseColor = true;
			ScalarsToColors::Pointer mapper = ScalarsToColors::New();

			if (i == -1)
			{
				mapper->InitRange(attr);
			}
			else
			{
				mapper->InitRange(attr, i);
			}

			m_Colors = mapper->MapScalars(attr, i);
			if (m_Colors == nullptr)
			{
				return;
			}

			GLAllocateGLBuffer(m_ColorVBO,
				m_Colors->GetNumberOfValues() * sizeof(float),
				m_Colors->GetRawPointer());

			m_PointVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);
		}
	}

private:
	GLVertexArray m_PointVAO;
	GLBuffer m_PositionVBO, m_ColorVBO;
	GLBuffer m_PointEBO;

	FloatArray::Pointer m_Positions{};
	FloatArray::Pointer m_Colors{};
	bool m_UseColor{false};
	int m_PointSize{4};

	DataArray::Pointer m_ViewAttribute;
	int m_ViewDemension;
};


IGAME_NAMESPACE_END
#endif