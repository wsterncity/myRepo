#include "iGameTensorWidgetBase.h"
#include "iGameScene.h"
IGAME_NAMESPACE_BEGIN
iGameTensorWidgetBase::iGameTensorWidgetBase()
{
	this->m_TensorManager = iGameTensorRepresentation::New();
	this->m_TensorManager->SetSliceNum(5);
	this->m_DrawGlyphPoints = Points::New();
	this->m_DrawGlyphPointOrders = UnsignedIntArray::New();
	this->m_DrawGlyphColors = FloatArray::New();
	this->m_DrawGlyphColors->SetElementSize(3);
}
iGameTensorWidgetBase::~iGameTensorWidgetBase()
{

}
void iGameTensorWidgetBase::SetPoints(Points::Pointer points)
{
	this->m_Points = points;
}
void iGameTensorWidgetBase::SetTensorAttributes(ArrayObject::Pointer attributes)
{
	this->m_TensorAttributes = attributes;
}
void iGameTensorWidgetBase::ShowTensorField()
{
	UpdateGlyphDrawData();
	UpdateGlyphDrawColor();
}

void iGameTensorWidgetBase::UpdateGlyphDrawData()
{
	clock_t time1 = clock();
	int PointNum = this->m_Points ? this->m_Points->GetNumberOfPoints() : 0;
	Point p;
	if (PointNum == 0 || !this->m_TensorAttributes ||
		this->m_TensorAttributes->GetNumberOfValues() != (9 * PointNum)) {
		return;
	}
	auto GlyphPointIndexOrders = m_TensorManager->GetEllipsoidDrawPointIndexOrders();
	int GlyphPointNum = m_TensorManager->GetNumberOfDrawPoints();
	m_DrawGlyphPoints->Resize(PointNum * GlyphPointNum);
	m_DrawGlyphPointOrders->Resize(PointNum * GlyphPointIndexOrders->GetNumberOfValues());
	auto GlyphPoints = m_DrawGlyphPoints->RawPointer();
	auto GlyphPointOrders = m_DrawGlyphPointOrders->RawPointer();
	double t[9];
	for (int i = 0; i < PointNum; i++) {
		p = this->m_Points->GetPoint(i);
		m_TensorManager->SetPosition(p);
		for (int j = 0; j < 9; j++) {
			t[j] = this->m_TensorAttributes->GetValue(9 * i + j);
		}
		m_TensorManager->SetTensor(t);
		auto EllipsoidDrawPoints = m_TensorManager->GetEllipsoidDrawPoints()->RawPointer();
		IGsize st = i * 3 * GlyphPointNum;
		std::copy(EllipsoidDrawPoints,
			EllipsoidDrawPoints + 3 * GlyphPointNum,
			GlyphPoints + st);

		st = i * GlyphPointIndexOrders->GetNumberOfValues();
		IGsize offset = i * GlyphPointNum;
		for (int j = 0; j < GlyphPointIndexOrders->GetNumberOfValues(); j++) {
			//这里不太好用copy，不能保证两个类型相同
			m_DrawGlyphPointOrders->SetValue(st + j, offset + GlyphPointIndexOrders->GetValue(j));
			//m_DrawGlyphPointOrders->AddValue(st + GlyphPointIndexOrders->GetValue(j));
		}
	}
	clock_t time2 = clock();
	std::cout << time2 - time1 << "ms\n";
}
void iGameTensorWidgetBase::UpdateGlyphDrawColor()
{
	int PointNum = this->m_Points ? this->m_Points->GetNumberOfPoints() : 0;
	if (PointNum == 0 || this->m_PositionColors == nullptr ||
		this->m_PositionColors->GetNumberOfElements() != PointNum) {
		return;
	}
	int GlyphPointNum = m_TensorManager->GetNumberOfDrawPoints();
	float rgb[16] = { .0 };
	m_DrawGlyphColors->Resize(PointNum * GlyphPointNum);
	auto GlyphColors = m_DrawGlyphColors->RawPointer();
	IGsize offset = 0;
	for (int i = 0; i < PointNum; i++) {
		m_PositionColors->GetElement(i, rgb);
		for (int j = 0; j < GlyphPointNum; j++) {
			offset += 3;
			std::copy(GlyphColors + offset, GlyphColors + offset + 3, rgb);
			//m_DrawGlyphColors->AddValue(rgb[0]);
			//m_DrawGlyphColors->AddValue(rgb[1]);
			//m_DrawGlyphColors->AddValue(rgb[2]);
		}
	}

}
void iGameTensorWidgetBase::UpdateGlyphScale(double s)
{

}

void iGameTensorWidgetBase::SetPositionColors(FloatArray::Pointer colors)
{
	this->m_PositionColors = colors;
}

void iGameTensorWidgetBase::GenerateVectorField()
{
	int PointNum = this->m_Points ? this->m_Points->GetNumberOfPoints() : 0;
	if (PointNum == 0 || !this->m_TensorAttributes ||
		this->m_TensorAttributes->GetNumberOfValues() != (9 * PointNum)) {
		return;
	}
	double vector[3];
	DoubleArray::Pointer vectorData = DoubleArray::New();
	vectorData->SetElementSize(3);
	vectorData->SetName(this->m_TensorAttributes->GetName() + "_PrimaryFeature");
	double t[9];
	for (int i = 0; i < PointNum; i++) {
		for (int j = 0; j < 9; j++) {
			t[j] = this->m_TensorAttributes->GetValue(9 * i + j);
		}
		m_TensorManager->InitTensorEigenData(t);
		m_TensorManager->GetEigenVector(0, vector);
		vectorData->SetElement(i, vector);
	}
}
void iGameTensorWidgetBase::Draw(Scene* scene) 
{
	if (!m_Visibility) { return; }
	// Update uniform buffer
	if (m_UseColor) {
		scene->UBO().useColor = true;
	}
	else {
		scene->UBO().useColor = false;
	}
	scene->UpdateUniformBuffer();

	if (m_ViewStyle & IG_POINTS) {
		scene->GetShader(Scene::NOLIGHT)->use();
		m_PointVAO.bind();
		//        glPointSize(m_PointSize);
		glPointSize(9);
		glad_glDrawArrays(GL_POINTS, 0, m_Positions->GetNumberOfValues() / 3);
		m_PointVAO.release();

	}
	//if (m_ViewStyle & IG_WIREFRAME) {
	//	if (m_UseColor) {
	//		scene->GetShader(Scene::NOLIGHT)->use();
	//	}
	//	else {
	//		auto shader = scene->GetShader(Scene::PURECOLOR);
	//		shader->use();
	//		shader->setUniform(shader->getUniformLocation("inputColor"),
	//			igm::vec3{ 0.0f, 0.0f, 0.0f });
	//	}

	//	m_LineVAO.bind();
	//	glLineWidth(m_LineWidth);
	//	glad_glDrawElements(GL_LINES, M_LineIndices->GetNumberOfValues(),
	//		GL_UNSIGNED_INT, 0);
	//	m_LineVAO.release();
	//}
	if (m_ViewStyle & IG_SURFACE) {
		scene->GetShader(Scene::PATCH)->use();
		m_TriangleVAO.bind();
		glad_glDrawElements(GL_TRIANGLES,
			M_TriangleIndices->GetNumberOfValues(),
			GL_UNSIGNED_INT, 0);
		m_TriangleVAO.release();

		//m_VertexVAO.bind();
		//glPointSize(m_PointSize);
		//glad_glDrawElements(GL_POINTS, M_VertexIndices->GetNumberOfValues(),
		//	GL_UNSIGNED_INT, 0);
		//m_PointVAO.release();

	}
}

void iGameTensorWidgetBase::ConvertToDrawableData()
{
	if (!m_Flag) {
		m_PointVAO.create();
		m_VertexVAO.create();
		m_LineVAO.create();
		m_TriangleVAO.create();

		m_PositionVBO.create();
		m_PositionVBO.target(GL_ARRAY_BUFFER);
		m_ColorVBO.create();
		m_ColorVBO.target(GL_ARRAY_BUFFER);
		m_NormalVBO.create();
		m_NormalVBO.target(GL_ARRAY_BUFFER);
		m_TextureVBO.create();
		m_TextureVBO.target(GL_ARRAY_BUFFER);

		m_VertexEBO.create();
		m_VertexEBO.target(GL_ELEMENT_ARRAY_BUFFER);
		m_LineEBO.create();
		m_LineEBO.target(GL_ELEMENT_ARRAY_BUFFER);
		m_TriangleEBO.create();
		m_TriangleEBO.target(GL_ELEMENT_ARRAY_BUFFER);
		m_Flag = true;
	}
	m_Positions = m_DrawGlyphPoints->ConvertToArray();
	m_Positions->Modified();
	M_TriangleIndices = m_DrawGlyphPointOrders;



	GLAllocateGLBuffer(m_PositionVBO,
		m_Positions->GetNumberOfValues() * sizeof(float),
		m_Positions->RawPointer());

	//GLAllocateGLBuffer(m_VertexEBO,
	//	M_VertexIndices->GetNumberOfValues() *
	//	sizeof(unsigned int),
	//	M_VertexIndices->RawPointer());

	//GLAllocateGLBuffer(m_LineEBO,
	//	M_LineIndices->GetNumberOfValues() *
	//	sizeof(unsigned int),
	//	M_LineIndices->RawPointer());

	GLAllocateGLBuffer(m_TriangleEBO,
		M_TriangleIndices->GetNumberOfValues() *
		sizeof(unsigned int),
		M_TriangleIndices->RawPointer());

	m_PointVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
		GL_FALSE, 0);

	//m_VertexVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	//GLSetVertexAttrib(m_VertexVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
	//	GL_FALSE, 0);
	//m_VertexVAO.elementBuffer(m_VertexEBO);

	//m_LineVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	//GLSetVertexAttrib(m_LineVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
	//	GL_FALSE, 0);
	//m_LineVAO.elementBuffer(m_LineEBO);

	m_TriangleVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0,
		3 * sizeof(float));
	GLSetVertexAttrib(m_TriangleVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3,
		GL_FLOAT, GL_FALSE, 0);
	m_TriangleVAO.elementBuffer(m_TriangleEBO);

}
IGAME_NAMESPACE_END