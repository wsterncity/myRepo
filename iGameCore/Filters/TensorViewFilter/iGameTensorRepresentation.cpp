#include "iGameTensorRepresentation.h" 
#include <Eigen/Dense>
using namespace Eigen;
#define DEFAULT_SLICE_NUM 5
const float pi = IGM_PI;
IGAME_NAMESPACE_BEGIN
iGameTensorRepresentation::iGameTensorRepresentation()
{
	this->EllipsoidDrawPoints = FloatArray::New();
	this->EllipsoidDrawPoints->SetElementSize(3);
	this->SphereDrawPointsBaseData = Points::New();
	this->EllipsoidDrawPointIndexOrders = IntArray::New();
	this->EllipsoidDrawPointIndexOrders->SetElementSize(3);
	this->SetSliceNum(DEFAULT_SLICE_NUM);
	this->scale = 0.01;
	this->DrawType = ELLIPSOID;
}
iGameTensorRepresentation::~iGameTensorRepresentation()
{
}



/**
* @brief Set the tensor data (a 3x3 symmetric tensor). The eigenvalues ​​and eigenvectors of
* the tensor matrix are automatically calculated, and the drawing data of the glyph is updated.
*/
void iGameTensorRepresentation::SetTensor(double tensor[9])
{
	std::copy(tensor, tensor + 9, this->Tensor);

	this->InitTensorEigenData();
	// Now update the widget/representation from the tensor
	this->UpdateDrawDataFromTensor();
}
/**
* @brief Initialize the eigen date of tensor,
* including eigenvalues ​​and eigenvectors.
* @param t[9] The tensor data, is an 3*3 matrix.
*/
void iGameTensorRepresentation::InitTensorEigenData(double tensor[9]) {
	// Evaluate eigenfunctions: set up working matrices
	double m[3][3];
	double v[3][3];
	//Prevent the input matrix from being asymmetric
	for (auto j = 0; j < 3; j++) {
		for (auto i = 0; i < 3; i++) {
			m[i][j] = 0.5 * (tensor[i + 3 * j] + tensor[j + 3 * i]);
		}
	}
	this->InitTensorCharacteristicData(m, this->Eigenvalues, v);
	this->SortTensorCharacteristicData(v);
}
/**
* @brief Compute the eigenvalues ​​and eigenvectors of a matrix.
* @param m[3][3] An 3*3 matrix to compute.
* @param values[3] The array of eigenvalues.
* @param v[3][3] The matrix of eigenvectors.
* Note values[i] is the eigenvalue corresponding to the eigenvector v[i]
*/
void iGameTensorRepresentation::InitTensorCharacteristicData(double m[3][3], double values[3], double v[3][3]) {
	Matrix3d matrix;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			matrix(i, j) = m[i][j];
		}
	}
	SelfAdjointEigenSolver<Matrix3d> eigensolver(matrix);
	if (eigensolver.info() != Success) {
		std::cerr << "Eigenvalue computation failed!" << std::endl;
		return;
	}
	for (int i = 0; i < 3; ++i) {
		values[i] = eigensolver.eigenvalues()[i];
	}
	// Store the feature vectors into the v array
	// Note the difference between row vectors and column vectors
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			v[i][j] = eigensolver.eigenvectors()(j, i);
		}
	}
}
/*
* @brief Update the tensor information. The tensor data is sorted
* from largest to smallest eigenvalues.
*/
void iGameTensorRepresentation::SortTensorCharacteristicData(double vec[3][3])
{
	double n[3]; // eigenvector norms
	int order[3] = { -1, -1, -1 };
	n[0] = std::abs(this->Eigenvalues[0]);
	n[1] = std::abs(this->Eigenvalues[1]);
	n[2] = std::abs(this->Eigenvalues[2]);
	order[0] = (n[0] >= n[1] ? (n[0] >= n[2] ? 0 : 2) : (n[1] >= n[2] ? 1 : 2)); // max
	order[2] = (n[0] < n[1] ? (n[0] < n[2] ? 0 : 2) : (n[1] < n[2] ? 1 : 2));    // min
	order[1] = 3 - order[0] - order[2];                                          // second 

	this->Eigenvalues[0] = n[order[0]];
	this->Eigenvalues[1] = n[order[1]];
	this->Eigenvalues[2] = n[order[2]];

	std::copy(vec[order[0]], vec[order[0]] + 3, this->Eigenvectors[0]);
	std::copy(vec[order[1]], vec[order[1]] + 3, this->Eigenvectors[1]);
	std::copy(vec[order[2]], vec[order[2]] + 3, this->Eigenvectors[2]);
}
/*
* @brief Update the drawing information of the glyph.
* Currently only ellipsoid glyph are supported.
*/
void iGameTensorRepresentation::UpdateDrawDataFromTensor()
{
	//Different graphics will be added later
	switch (DrawType)
	{
	case ELLIPSOID:
		InitEllipsoidDrawPoints();
		break;
	case CUBOID:
		break;
	}
}
/**
* @brief Get the ellipsoid coordinate point at the current longitude and latitude.
* @param rx,ry,yz The lengths of the three axes of the ellipsoid.
* @param a The current longitude.
* @param b The current latitude.
* @param p The coordinate point.
*/
void  iGameTensorRepresentation::GetPointOval(double& rx, double& ry, double& rz, double& a, double& b, iGame::Point& p) {
	p[0] = rx * sin(a * pi / 180.0) * cos(b * pi / 180.0);
	p[1] = ry * sin(a * pi / 180.0) * sin(b * pi / 180.0);
	p[2] = rz * cos(a * pi / 180.0);
	p = p * this->scale;
	return;
}
/**
* @brief Initialize the draw points.
* Enter the three axis lengths of the ellipsoid and the number of m_Slices.
*/
void iGameTensorRepresentation::InitEllipsoidDrawPoints(double& rx, double& ry, double& rz, int m_Slices) {
	int i, j, w = 2 * m_Slices, h = m_Slices;
	double a = 0.0, b = 0.0;
	double hStep = 180.0 / (h - 1);
	double wStep = 360.0 / w;
	int length = w * h;
	this->EllipsoidDrawPoints->Reset();
	this->EllipsoidDrawPoints->Reserve(length);
	Point tmpP;
	Point radius(rx, ry, rz);
	radius = radius * this->scale;
	Point translation(TensorPosition[0], TensorPosition[1], TensorPosition[2]);
	Point rotate[3];
	rotate[0] = Point(Eigenvectors[0][0], Eigenvectors[1][0], Eigenvectors[2][0]);
	rotate[1] = Point(Eigenvectors[0][1], Eigenvectors[1][1], Eigenvectors[2][1]);
	rotate[2] = Point(Eigenvectors[0][2], Eigenvectors[1][2], Eigenvectors[2][2]);
	Point p;

	for (igIndex i = 0; i < this->SphereDrawPointsBaseData->GetNumberOfPoints(); i++) {
		tmpP = this->SphereDrawPointsBaseData->GetPoint(i).scale(radius);
		p[0] = tmpP.dot(rotate[0]);
		p[1] = tmpP.dot(rotate[1]);
		p[2] = tmpP.dot(rotate[2]);
		p += translation;
		this->EllipsoidDrawPoints->AddElement3(p[0], p[1], p[2]);
	}
}
/**
* @brief Set the number of m_Slices. It will automatically initialize basic drawing information,
* such as the order of drawing points of the slice and basic spherical coordinates.
*/
void iGameTensorRepresentation::SetSliceNum(int s) {
	this->m_Slices = s;
	UpdateEllipsoidDrawPointBasedData();
	UpdateEllipsoidDrawPointIndexOrders();
}
/**
* @brief Update the basic spherical coordinates.
*/
void iGameTensorRepresentation::UpdateEllipsoidDrawPointBasedData()
{
	int i, j, w = 2 * m_Slices, h = m_Slices;
	double a = 0.0, b = 0.0;
	double hStep = 180.0 / (h - 1);
	double wStep = 360.0 / w;
	int length = w * h;
	this->SphereDrawPointsBaseData->Reserve(length);
	double degreeScale = pi / 180.0;
	Point p;
	for (a = 0.0, i = 0; i < h; i++, a += hStep) {
		for (b = 0.0, j = 0; j < w; j++, b += wStep) {
			p[0] = sin(a * degreeScale) * cos(b * degreeScale);
			p[1] = sin(a * degreeScale) * sin(b * degreeScale);
			p[2] = cos(a * pi / 180.0);
			this->SphereDrawPointsBaseData->AddPoint(p);
		}
	}
}
/**
* @brief Update the order of drawing points of the slice.
*/
void iGameTensorRepresentation::UpdateEllipsoidDrawPointIndexOrders() {
	int i = 0, j = 0, w = 2 * this->m_Slices, h = this->m_Slices;
	EllipsoidDrawPointIndexOrders->Reserve(w * h);
	for (; i < h - 1; i++) {
		for (j = 0; j < w - 1; j++) {
			EllipsoidDrawPointIndexOrders->AddElement3(i * w + j, i * w + j + 1, (i + 1) * w + j + 1);
			EllipsoidDrawPointIndexOrders->AddElement3(i * w + j, (i + 1) * w + j + 1, (i + 1) * w + j);
		}
		EllipsoidDrawPointIndexOrders->AddElement3(i * w + j, i * w, (i + 1) * w);
		EllipsoidDrawPointIndexOrders->AddElement3(i * w + j, (i + 1) * w, (i + 1) * w + j);
	}
}


IGAME_NAMESPACE_END
