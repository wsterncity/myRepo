#ifndef SurfaceMeshFilterTest_h
#define SurfaceMeshFilterTest_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <random>

IGAME_NAMESPACE_BEGIN
class SurfaceMeshFilterTest : public Filter {
public:
	I_OBJECT(SurfaceMeshFilterTest);
	static Pointer New() { return new SurfaceMeshFilterTest; }


protected:
	SurfaceMeshFilterTest()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(1);
	}
	~SurfaceMeshFilterTest() override = default;

	SurfaceMesh::Pointer m_Mesh{};

	bool Execute() override
	{ 
		m_Mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		if (m_Mesh == nullptr)
		{
			return false;
		}
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		m_Mesh->RequestEditStatus();
		
		AddFaceTest();
		DeleteFaceTest();
		m_Mesh->GarbageCollection();
		SetOutput(0, m_Mesh);
		return true;
	}

	void AddFaceTest() {
		int id = m_Mesh->AddPoint(Point(1, 1, 0));
		igIndex face[3]{ 1,id,2 };
		m_Mesh->AddFace(face, 3);
	}

	void DeleteFaceTest() {
		//for (int i = 0; i < m_Mesh->GetNumberOfFaces()/2; i++) {
		//	m_Mesh->DeleteFace(i);
		//}
		m_Mesh->DeletePoint(0);
	}
};
IGAME_NAMESPACE_END
#endif