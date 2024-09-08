#include "iGameStructuredMesh.h"
#include "iGameScene.h"
#include "iGameFaceTable.h"
#include "iGameModelSurfaceFilters/iGameModelGeometryFilter.h"
IGAME_NAMESPACE_BEGIN

void StructuredMesh::Draw(Scene* scene)
{
	if (!m_Visibility)
	{
		return;
	}
	if (m_DrawMesh) {
		m_DrawMesh->SetViewStyle(m_ViewStyle);
		return m_DrawMesh->Draw(scene);
	}
	//update uniform buffer
	if (m_UseColor) {
		scene->UBO().useColor = true;
	}
	else {
		scene->UBO().useColor = false;
	}
	scene->UpdateUniformBuffer();

	if (m_UseColor && m_ColorWithCell) {
		scene->GetShader(Scene::PATCH)->use();
		m_CellVAO.bind();
		glad_glDrawArrays(GL_TRIANGLES, 0, m_CellPositionSize);
		m_CellVAO.release();
		return;
	}

	if (m_ViewStyle & IG_POINTS) {
		scene->GetShader(Scene::NOLIGHT)->use();
		m_PointVAO.bind();
		glad_glPointSize(m_PointSize);
		glad_glDepthRange(0, 0.99999);
		glad_glDrawArrays(GL_POINTS, 0, m_Positions->GetNumberOfValues() / 3);
		glad_glDepthRange(0, 1);
		m_PointVAO.release();
	}
	if (m_ViewStyle & IG_WIREFRAME) {
		if (m_UseColor) {
			scene->GetShader(Scene::NOLIGHT)->use();
		}
		else {
			auto shader = scene->GetShader(Scene::PURECOLOR);
			shader->use();
			shader->setUniform(shader->getUniformLocation("inputColor"),
				igm::vec3{ 0.0f, 0.0f, 0.0f });
		}

		m_LineVAO.bind();
		glLineWidth(m_LineWidth);

		// TODO: A better way to render wireframes
		auto boundingBoxDiag = this->GetBoundingBox().diag();
		auto scaleFactor =
			1e-6 / std::pow(10, std::floor(std::log10(boundingBoxDiag)));
		glad_glDepthRange(scaleFactor, 1);
		glad_glDepthFunc(GL_GEQUAL);

		glad_glDrawElements(GL_LINES, m_LineIndices->GetNumberOfIds(),
			GL_UNSIGNED_INT, 0);

		glad_glDepthFunc(GL_GREATER);
		glad_glDepthRange(0, 1);

		m_LineVAO.release();
	}
	if (m_ViewStyle & IG_SURFACE) {
		scene->GetShader(Scene::PATCH)->use();
		m_TriangleVAO.bind();
		glad_glDrawElements(GL_TRIANGLES, m_TriangleIndices->GetNumberOfIds(),
			GL_UNSIGNED_INT, 0);
		m_TriangleVAO.release();
	}
}
void StructuredMesh::ConvertToDrawableData()
{
	if (m_Positions && m_Positions->GetMTime() > this->GetMTime()) {
		return;
	}
	if (m_DrawMesh == nullptr) {
		iGameModelGeometryFilter::Pointer extract = iGameModelGeometryFilter::New();
		m_DrawMesh = SurfaceMesh::New();
		if (!extract->Execute(this, m_DrawMesh)) {
			m_DrawMesh = nullptr;
		}
	}
	if (m_DrawMesh) {
		return m_DrawMesh->ConvertToDrawableData();
	}
	if (!m_Flag)
	{
		m_PointVAO.create();
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
		m_PointEBO.create();
		m_PointEBO.target(GL_ELEMENT_ARRAY_BUFFER);
		m_LineEBO.create();
		m_LineEBO.target(GL_ELEMENT_ARRAY_BUFFER);
		m_TriangleEBO.create();
		m_TriangleEBO.target(GL_ELEMENT_ARRAY_BUFFER);

		m_CellVAO.create();
		m_CellPositionVBO.create();
		m_CellPositionVBO.target(GL_ARRAY_BUFFER);
		m_CellColorVBO.create();
		m_CellColorVBO.target(GL_ARRAY_BUFFER);
		m_Flag = true;
	}

	m_Positions = m_Points->ConvertToArray();
	m_Positions->Modified();

	IdArray::Pointer triangleIndices = IdArray::New();
	IdArray::Pointer edgeIndices = IdArray::New();

	if (this->Hexahedrons == nullptr && this->Quads == nullptr) {
		this->GenStructuredCellConnectivities();
	}
	int fcnt = this->GetNumberOfQuads();
	igIndex vhs[IGAME_CELL_MAX_SIZE];
	igIndex vcnt = 0;
	for (int i = 0; i < fcnt; i++) {
		vcnt = this->Quads->GetCellIds(i, vhs);
		for (int k = 2; k < vcnt; k++) {
			triangleIndices->AddId(vhs[0]);
			triangleIndices->AddId(vhs[k - 1]);
			triangleIndices->AddId(vhs[k]);
		}
	}
	for (int k = 0; k < size[2]; k++) {
		for (int j = 0; j < size[1]; j++) {
			for (int i = 0; i < size[0]; i++) {
				if (i != size[0] - 1) {
					edgeIndices->AddId(GetStructuredIndex(i, j, k));
					edgeIndices->AddId(GetStructuredIndex(i + 1, j, k));
				}
				if (j != size[1] - 1) {
					edgeIndices->AddId(GetStructuredIndex(i, j, k));
					edgeIndices->AddId(GetStructuredIndex(i, j + 1, k));
				}
				if (k != size[2] - 1) {
					edgeIndices->AddId(GetStructuredIndex(i, j, k));
					edgeIndices->AddId(GetStructuredIndex(i, j, k + 1));
				}
			}
		}
	}

	m_TriangleIndices = triangleIndices;
	m_LineIndices = edgeIndices;

	// allocate buffer
	{
		GLAllocateGLBuffer(m_PositionVBO,
			m_Positions->GetNumberOfValues() * sizeof(float),
			m_Positions->RawPointer());

		GLAllocateGLBuffer(m_LineEBO,
			m_LineIndices->GetNumberOfIds() * sizeof(igIndex),
			m_LineIndices->RawPointer());

		GLAllocateGLBuffer(m_TriangleEBO,
			m_TriangleIndices->GetNumberOfIds() * sizeof(igIndex),
			m_TriangleIndices->RawPointer());
	}

	// set vertex attribute pointer
	{
		// point
		m_PointVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
		GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
			GL_FALSE, 0);

		// line
		m_LineVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
		GLSetVertexAttrib(m_LineVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
			GL_FALSE, 0);
		m_LineVAO.elementBuffer(m_LineEBO);

		// triangle
		m_TriangleVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0,
			3 * sizeof(float));
		GLSetVertexAttrib(m_TriangleVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3,
			GL_FLOAT, GL_FALSE, 0);
		m_TriangleVAO.elementBuffer(m_TriangleEBO);
	}
}
void StructuredMesh::ViewCloudPicture(Scene* scene, int index, int demension)
{
	//if (m_DrawMesh) {
	//	return m_DrawMesh->ViewCloudPicture(scene, index, demension);
	//}
	//return;
}
IGAME_NAMESPACE_END
