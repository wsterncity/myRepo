#ifndef iGamePointSet_h
#define iGamePointSet_h

#include "iGameDataObject.h"
#include "iGamePoints.h"
#include "iGamePropertySet.h"
#include "iGameDrawableObject.h"
#include "iGameMarker.h"

IGAME_NAMESPACE_BEGIN
class PointSet : public DataObject {
public:
	I_OBJECT(PointSet);
	static Pointer New() { return new PointSet; }

	void SetPoints(Points::Pointer points)
	{
		if (m_Points != points)
		{
			m_Points = points;
			this->Modified();
		}
	}
	Points::Pointer GetPoints() { return m_Points; }

	IGsize GetNumberOfPoints() { return m_Points ? m_Points->GetNumberOfPoints() : 0; }

	Point& GetPoint(const IGsize ptId) { return m_Points->GetPoint(ptId); }

	void SetPoint(const IGsize ptId, const Point& p) 
	{
		m_Points->SetPoint(ptId, p);
		Modified();
	}

	virtual IGsize AddPoint(const Point& p)
	{
		if(!InEditStatus())
		{
			RequestEditStatus();
		}
		IGsize id = m_Points->AddPoint(p);
		m_PointDeleteMarker->AddTag();
		Modified();
		return id;
	}

	virtual void RequestEditStatus() 
	{
		if (InEditStatus())
		{
			return;
		}
		RequestPointStatus();
		MakeEditStatusOn();
	}

	virtual void DeletePoint(const IGsize ptId)
	{
		if(!InEditStatus())
		{
			RequestEditStatus();
		}
		m_PointDeleteMarker->MarkDeleted(ptId);
	}

	bool IsPointDeleted(const IGsize ptId) {
		return m_PointDeleteMarker->IsDeleted(ptId);
	}

	virtual void GarbageCollection()
	{
		IGsize i, mapId = 0;
		for (i = 0; i < GetNumberOfPoints(); i++) {
			if (IsPointDeleted(i)) continue;
			if (i != mapId) {
				m_Points->SetPoint(mapId, m_Points->GetPoint(i));
			}
			mapId++;
		}
		m_Points->Resize(mapId);

		m_PointDeleteMarker = nullptr;
		Modified();
		MakeEditStatusOff();
	}

protected:
	PointSet() { 
		m_Points = Points::New();
		m_ViewStyle = IG_POINT;
	}
	~PointSet() override = default;

	bool InEditStatus() { return m_InEditStatus; }
	void MakeEditStatusOn() { m_InEditStatus = true; }
	void MakeEditStatusOff() { m_InEditStatus = false; }

	void RequestPointStatus() 
	{
		if (m_PointDeleteMarker == nullptr)
		{
			m_PointDeleteMarker = DeleteMarker::New();
		}
		m_PointDeleteMarker->Initialize(this->GetNumberOfPoints());
	}

	Points::Pointer m_Points{};
	DeleteMarker::Pointer m_PointDeleteMarker{};
	bool m_InEditStatus{ false };

public:
	void Draw(Scene*) override;
	void ConvertToDrawableData() override;
	bool IsDrawable() override { return true; }
	void ViewCloudPicture(int index, int demension = -1) override
	{
		auto& attr = this->GetPropertySet()->GetProperty(index);
		if (!attr.isDeleted && attr.attachmentType == IG_POINT)
		{
			this->SetAttributeWithPointData(attr.pointer, demension);
		}
	}

	virtual void SetAttributeWithPointData(ArrayObject::Pointer attr, igIndex i = -1)
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
				m_Colors->RawPointer());

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

	ArrayObject::Pointer m_ViewAttribute;
	int m_ViewDemension;
};


IGAME_NAMESPACE_END
#endif