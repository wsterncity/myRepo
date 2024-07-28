#include "iGamePointSet.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN

void PointSet::Draw(Scene* scene)
{
	if (!m_Visibility)
	{
		return;
	}

	if (m_UseColor) {
		scene->UBO().useColor = true;
	}
	else {
		scene->UBO().useColor = false;
	}
	scene->UpdateUniformBuffer();

	if (m_ViewStyle == IG_POINTS)
	{
		scene->GetShader(Scene::NOLIGHT)->use();
		m_PointVAO.bind();
		glPointSize(m_PointSize);
		glad_glDrawArrays(GL_POINTS, 0, m_Positions->GetNumberOfValues() / 3);
		m_PointVAO.release();
	}
}
void PointSet::ConvertToDrawableData()
{
	if (m_Positions && m_Positions->GetMTime() > this->GetMTime())
	{
		return;
	}
	//if (m_Positions)
	//{
	//	std::cout << m_Positions->GetMTime() << " " << this->GetMTime() << std::endl;
	//}

	m_Positions = m_Points->ConvertToArray();

	m_PointVAO.destroy();
	m_PositionVBO.destroy();
	m_ColorVBO.destroy();
	m_PointEBO.destroy();

	m_PointVAO.create();
	m_PositionVBO.create();
	m_ColorVBO.create();
	m_PointEBO.create();

	GLAllocateGLBuffer(m_PositionVBO,
		m_Positions->GetNumberOfValues() * sizeof(float),
		m_Positions->RawPointer());

	m_PointVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
		GL_FALSE, 0);

	m_Positions->Modified();
}

IGAME_NAMESPACE_END