#ifndef Curvature_h
#define Curvature_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <Eigen/dense>
#define MeanCurvatureMode 0
#define AbsoluteMeanCurvatureMode 1
#define GuassianCurvatureMode 2
#define PI 3.14159265359


IGAME_NAMESPACE_BEGIN
class Curvature : public Filter
{
public:
	I_OBJECT(Curvature);
	static Curvature::Pointer New(){return new Curvature;}
	bool Execute() override;
	void SetViewMode(int _mode){mode = _mode;}
protected:
	Curvature() 
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(1);
	}

	~Curvature() noexcept override = default;

	int mode = MeanCurvatureMode;
	SurfaceMesh::Pointer mesh;

	std::vector<float> LAR_v;

	//MeanCurvature
	std::vector<Vector3f> MeanCurvature_v;
	std::vector<Vector3f> Normal_v;

	//GuassianCurvature
	std::vector<float> GuassianCurvature_v;


	Vector3f CalcCircumCenter(const Vector3f& a, const Vector3f& b, const Vector3f& c);
	float CalcAngle(const Vector3f& a, const Vector3f& b, const Vector3f& c);
	void CalcNormal();
	void CalcLAR();
	void CalcMeanCurvature();
	void CalcGuassianCurvature();
	void InitCache();

};
IGAME_NAMESPACE_END

#endif // !Curture_h
