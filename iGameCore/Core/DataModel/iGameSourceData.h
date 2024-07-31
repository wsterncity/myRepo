#ifndef iGameSourceData_h
#define iGameSourceData_h

#include "iGameDataObject.h"

IGAME_NAMESPACE_BEGIN
class SourceData : public DataObject {
public:
	I_OBJECT(SourceData);
	static Pointer New() { return new SourceData; }

	struct Source {
		Vector3d position;
		Vector3d direction;
		double size;
	};

protected:
	SourceData() { }
	~SourceData() override = default;

	ElementArray<Source> m_Buffer{};

public:
	void Draw(Scene*) override {}
	void ConvertToDrawableData() override{}
	bool IsDrawable() override { return true; }

private:
	GLVertexArray m_PointVAO;
	GLBuffer m_PositionVBO, m_ColorVBO;

	FloatArray::Pointer m_Positions{};
	FloatArray::Pointer m_Colors{};
	bool m_UseColor{false};
};
IGAME_NAMESPACE_END
#endif