#ifndef BilateralNormalFiltering_h
#define BilateralNormalFiltering_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
class BilateralNormalFiltering : public Filter
{
public:
	I_OBJECT(BilateralNormalFiltering);
	static BilateralNormalFiltering::Pointer New(){return new BilateralNormalFiltering;}
	bool Execute() override;
	void SetSigema_r(float r){sigema_r = r;}
	void SetN_iter(int n){n_iter = n;}
	void SetC_iter(int c){c_iter = c;}
protected:
	BilateralNormalFiltering()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(1);
	}
	~BilateralNormalFiltering() noexcept override = default;
	SurfaceMesh::Pointer mesh;
	std::vector<Vector3f> newNormal_f;
	std::vector<Vector3f> center_f;
	std::vector<float> area_f;
	float sigema_c = 0;
	float sigema_r = 1.0f;
	int n_iter = 8;
	int c_iter = 8;
	void InitCache();
	void UpdateFaceNormal();
	void UpdateVerticePosition();
	Point GetFaceCenter(igIndex fid);
	float GetFaceArea(igIndex fid);
};


IGAME_NAMESPACE_END


#endif