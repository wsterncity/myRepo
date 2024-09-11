#include "iGameQuadSubdivision.h"
IGAME_NAMESPACE_BEGIN

inline double Bernstein(int i, int n, double t) {
	double binomial_coeff = 1.0;
	for (int j = 0; j < i; ++j) {
		binomial_coeff *= (n - j);
		binomial_coeff /= (j + 1);
	}
	return binomial_coeff * pow(t, i) * pow(1 - t, n - i);
}

inline double BSplineBasisFunction(int i, int p, double t, const std::vector<double>& knots) {
	if (p == 0) {
		return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
	}
	else {
		double denom1 = knots[i + p] - knots[i];
		double denom2 = knots[i + p + 1] - knots[i + 1];

		double term1 = (denom1 != 0) ? (t - knots[i]) / denom1 * BSplineBasisFunction(i, p - 1, t, knots) : 0;
		double term2 = (denom2 != 0) ? (knots[i + p + 1] - t) / denom2 * BSplineBasisFunction(i + 1, p - 1, t, knots) : 0;

		return term1 + term2;
	}
}

// Generate points on a Bezier surface
inline std::vector<std::vector<Point>>GenerateBezierSurface(Point ControlPts[4][4], int resolution) {
	std::vector<std::vector<Point>> bezierSurface(resolution, std::vector<Point>(resolution));

	// Define node vectors (uniform node vector example)
	std::vector<double> knots = { 0, 0, 0, 0, 1+1e-10, 1 + 1e-10, 1 + 1e-10, 1 + 1e-10 }; // Node vectors of bicubic B-splines
	for (int u = 0; u < resolution; ++u) {
		for (int v = 0; v < resolution; ++v) {
			double u_t = static_cast<double>(u) / (resolution - 1);
			double v_t = static_cast<double>(v) / (resolution - 1);
			Point p = { 0, 0, 0 }; // Assuming Point has a default constructor

			// The double loop computes the weight of each control point
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					double Bu = Bernstein(i, 3, u_t);
					double Bv = Bernstein(j, 3, v_t);
					//double Bu = BSplineBasisFunction(i, 3, u_t, knots);
					//double Bv = BSplineBasisFunction(j, 3, v_t, knots);
					Point weightedPoint = ControlPts[i][j] * (Bu * Bv);
					p = p + weightedPoint;
				}
			}

			bezierSurface[u][v] = p;
		}
	}

	return bezierSurface;
}

bool QuadSubdivision::Execute()
{
	this->mesh = DynamicCast<SurfaceMesh>(this->mesh);
	if (mesh == nullptr) { return false; }
	this->mesh->BuildEdges();
	this->mesh->RequestEditStatus();
	auto Faces = mesh->GetFaces();
	igIndex FaceNum = mesh->GetNumberOfFaces();
	auto Edges = mesh->GetEdges();
	igIndex EdgeNum = mesh->GetNumberOfEdges();
	auto Points = mesh->GetPoints();
	igIndex PointNum = mesh->GetNumberOfPoints();
	igIndex i, j, k;
	igIndex vcnt = 0;
	igIndex vhs[IGAME_CELL_MAX_SIZE];
	igIndex fcnt = 0;
	igIndex fhs[IGAME_CELL_MAX_SIZE];
	igIndex ecnt = 0;
	igIndex ehs[IGAME_CELL_MAX_SIZE];

	std::vector<std::vector<Point>>FacePts(FaceNum); // pastry
	std::vector<std::vector<Point>>EdgePts(EdgeNum); // Edge point
	std::vector<Point>CornerPts(PointNum);
	std::vector<Point>CornerNormals(PointNum);
	std::vector<Point>PointNormals(PointNum);
	std::vector<Point>FaceNormals(FaceNum);

	//for (i = 0; i < FaceNum; i++) {
	//	vcnt = Faces->GetCellIds(i, vhs);
	//	Point p[4];
	//	for (j = 0; j < vcnt; j++) {
	//		p[j] = mesh->GetPoint(vhs[j]);
	//	}
	//	FaceNormals[i] = { 0,0,0 };
	//	for (j = 2; j < vcnt; j++) {
	//		FaceNormals[i] += (p[j - 1] - p[0]).cross(p[j] - p[0]);
	//	}
	//	FaceNormals[i].normalize();
	//}

	//for (i = 0; i < PointNum; i++) {
	//	fcnt = mesh->GetPointToNeighborFaces(i, fhs);
	//	PointNormals[i] = { 0,0,0 };
	//	for (j = 0; j < fcnt; j++) {
	//		PointNormals[i] += FaceNormals[fhs[j]];
	//	}
	//	PointNormals[i].normalize();
	//}

	for (i = 0; i < FaceNum; i++) {
		vcnt = Faces->GetCellIds(i, vhs);
		assert(vcnt == 4);
		// Each face has four interior points
		FacePts[i].resize(4);
		Point p[4];
		for (j = 0; j < vcnt; j++) {
			p[j] = mesh->GetPoint(vhs[j]);
		}
		for (j = 0; j < 4; j++) {
			int n = mesh->GetPointToNeighborFaces(vhs[j], fhs);
			FacePts[i][j] = p[j] * n + p[(j + 1) % 4] * 2 + p[(j + 3) % 4] * 2 + p[(j + 2) % 4];
			FacePts[i][j] /= 5 + n;
		}
	}

	// Computing edge points
	for (i = 0; i < EdgeNum; i++) {
		vcnt = Edges->GetCellIds(i, vhs);
		assert(vcnt == 2);
		// Each edge has two interior points
		EdgePts[i].resize(2);
		fcnt = mesh->GetEdgeToNeighborFaces(i, fhs);
		// If it's an internal edge
		if (!mesh->IsBoundaryEdge(i)) {
			for (j = 0; j < 2; j++) {
				EdgePts[i][j] = Point{ 0,0,0 };
				for (k = 0; k < fcnt; k++) {
					auto f = mesh->GetFace(fhs[k]);
					for (igIndex id = 0; id < 4; id++) {
						if (f->GetPointId(id) == vhs[j]) {
							EdgePts[i][j] += FacePts[fhs[k]][id];
							break;
						}
					}
				}
				EdgePts[i][j] /= fcnt;
			}
		}

		else {
			Point p[2];
			for (j = 0; j < vcnt; j++) {
				p[j] = mesh->GetPoint(vhs[j]);
			}
			EdgePts[i][0] = (p[0] * 2 + p[1]) / 3;
			EdgePts[i][1] = (p[1] * 2 + p[0]) / 3;
		}
	}

	for (i = 0; i < PointNum; i++) {
		auto p = mesh->GetPoint(i);
		fcnt = mesh->GetPointToNeighborFaces(i, fhs);
		CornerPts[i] = { 0,0,0 };
		if (!mesh->IsBoundaryPoint(i)) {	
			for (k = 0; k < fcnt; k++) {
				auto f = mesh->GetFace(fhs[k]);
				for (igIndex id = 0; id < 4; id++) {
					if (f->GetPointId(id) == i) {
						CornerPts[i] += FacePts[fhs[k]][id];
						break;
					}
				}
			}
			CornerPts[i] /= fcnt;
		}
		else if (!mesh->IsCornerPoint(i)) {
			vcnt = mesh->GetPointToOneRingPoints(i, vhs);
			for (j = 0; j < vcnt; j++) {
				auto eh = mesh->GetEdgeIdFormPointIds(i, vhs[j]);
				if (mesh->IsBoundaryEdge(eh)) {
					CornerPts[i] += p * 2 + mesh->GetPoint(vhs[j]);
				}
			}
			CornerPts[i] /= 6;
		}
		else {
			CornerPts[i] = p;
		}
	}

	//for (i = 0; i < PointNum; i++) {
	//	int n = mesh->GetPointToOneRingPoints(i, vhs);
	//	if (mesh->IsBoundryPoint(i))continue;
	//	double tmp = M_PI / (n * 1.0);
	//	double An = 1 + std::cos(2.0 * tmp) + std::cos(tmp) * std::sqrt(2 * (9 + std::cos(2 * tmp)));
	//	Point c2 = { 0,0,0 };
	//	Point c3 = { 0,0,0 };
	//	ecnt = mesh->GetPointToNeighborEdges(i, ehs);
	//	fcnt = mesh->GetPointToNeighborFaces(i, fhs);
	//	std::vector<Point>ep(n);
	//	std::vector<Point>fp(n);

	//	for (j = 0; j < n; j++) {
	//		auto edge = mesh->GetEdge(ehs[j]);
	//		if (edge->GetPointId(0) == i) {
	//			ep[j] = EdgePts[ehs[j]][1];
	//		}
	//		else {
	//			ep[j] = EdgePts[ehs[j]][0];
	//		}
	//		auto face = mesh->GetFace(fhs[j]);
	//		for (k = 0; k < 4; k++) {
	//			if (face->GetPointId(k) == i) {
	//				fp[j] = FacePts[fhs[j]][(k + 2) % 4];
	//			}
	//		}
	//	}
	//	for (j = 0; j < n; j++) {
	//		c2 += ep[j] * An * std::cos(2 * j * tmp) + fp[j] * (std::cos(2 * j * tmp) + std::cos((2 * j + 2) * tmp));
	//		c3 += ep[(j + 1) % n] * An * std::cos(2 * j * tmp) + fp[(j + 1) % n] * (std::cos(2 * j * tmp) + std::cos((2 * j + 2) * tmp));
	//	}
	//	CornerNormals[i] = (c2.cross(c3)).normalized();
	//	//std::cout << CornerNormals[i].dot(PointNormals[i]) << '\n';
	//}


	SurfaceMesh::Pointer res = SurfaceMesh::New();
	Points::Pointer ControlPoints = Points::New();
	CellArray::Pointer SubdivisionCells = CellArray::New();


	for (auto FaceId = 0; FaceId < FaceNum; FaceId++) {
		// Each face has sixteen control points
		Point ControlPts[4][4];
		vcnt = Faces->GetCellIds(FaceId, vhs);
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				ControlPts[i][j] = { 0,0,0 };
			}
		}
		ControlPts[0][0] = CornerPts[vhs[0]];
		ControlPts[0][3] = CornerPts[vhs[1]];
		ControlPts[3][3] = CornerPts[vhs[2]];
		ControlPts[3][0] = CornerPts[vhs[3]];

		auto EdgeID = mesh->GetEdgeIdFormPointIds(vhs[0], vhs[1]);
		auto edge = mesh->GetEdge(EdgeID);
		ControlPts[0][1] = EdgePts[EdgeID][0];
		ControlPts[0][2] = EdgePts[EdgeID][1];
		if (edge->GetPointId(0) != vhs[0])std::swap(ControlPts[0][1], ControlPts[0][2]);
		EdgeID = mesh->GetEdgeIdFormPointIds(vhs[1], vhs[2]);
		edge = mesh->GetEdge(EdgeID);
		ControlPts[1][3] = EdgePts[EdgeID][0];
		ControlPts[2][3] = EdgePts[EdgeID][1];
		if (edge->GetPointId(0) != vhs[1])std::swap(ControlPts[1][3], ControlPts[2][3]);
		EdgeID = mesh->GetEdgeIdFormPointIds(vhs[3], vhs[2]);
		edge = mesh->GetEdge(EdgeID);
		ControlPts[3][1] = EdgePts[EdgeID][0];
		ControlPts[3][2] = EdgePts[EdgeID][1];
		if (edge->GetPointId(0) != vhs[3])std::swap(ControlPts[3][1], ControlPts[3][2]);
		EdgeID = mesh->GetEdgeIdFormPointIds(vhs[0], vhs[3]);
		edge = mesh->GetEdge(EdgeID);
		ControlPts[1][0] = EdgePts[EdgeID][0];
		ControlPts[2][0] = EdgePts[EdgeID][1];
		if (edge->GetPointId(0) != vhs[0])std::swap(ControlPts[1][0], ControlPts[2][0]);

		ControlPts[1][1] = FacePts[FaceId][0];
		ControlPts[1][2] = FacePts[FaceId][1];
		ControlPts[2][2] = FacePts[FaceId][2];
		ControlPts[2][1] = FacePts[FaceId][3];


		int n = 17;
		auto tmp = GenerateBezierSurface(ControlPts, n);
		for (j = 0; j < n; j++) {
			for (i = 0; i < n; i++) {
				ControlPoints->AddPoint(tmp[j][i]);
			}
		}
		for (j = 0; j < n - 1; ++j) {
			int st = j * n + FaceId * n * n;
			for (i = 0; i < n - 1; ++i) {
				SubdivisionCells->AddCellId4(st, st + 1, st + 1 + n, st + n);
				st++;
			}
		}
	}

	res->SetFaces(SubdivisionCells);
	res->SetPoints(ControlPoints);
	this->SetOutput(res);
	return true;
}


IGAME_NAMESPACE_END