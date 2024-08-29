#include "iGameQuadSubdivision.h"
IGAME_NAMESPACE_BEGIN
// 计算 Bernstein 多项式的值
double Bernstein(int i, int n, double t) {
	double binomial_coeff = 1.0;
	for (int j = 0; j < i; ++j) {
		binomial_coeff *= (n - j);
		binomial_coeff /= (j + 1);
	}
	return binomial_coeff * pow(t, i) * pow(1 - t, n - i);
}

// 生成 Bezier 曲面上的点
std::vector<std::vector<Point>> GenerateBezierSurface(Point ControlPts[4][4], int resolution) {
	std::vector<std::vector<Point>> bezierSurface(resolution, std::vector<Point>(resolution));

	for (int u = 0; u < resolution; ++u) {
		for (int v = 0; v < resolution; ++v) {
			double u_t = static_cast<double>(u) / (resolution - 1);
			double v_t = static_cast<double>(v) / (resolution - 1);
			Point p = { 0, 0, 0 }; // Assuming Point has a default constructor

			// 双重循环计算每个控制点的权重
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					double Bu = Bernstein(i, 3, u_t);
					double Bv = Bernstein(j, 3, v_t);
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
	//面点
	std::vector<std::vector<Point>>FacePts(FaceNum);
	//边点
	std::vector<std::vector<Point>>EdgePts(EdgeNum);
	//角点
	std::vector<Point>CornerPts(PointNum);

	//计算面点
	for (i = 0; i < FaceNum; i++) {
		vcnt = Faces->GetCellIds(i, vhs);
		assert(vcnt == 4);
		//每个面有四个内点
		FacePts[i].resize(4);
		Point p[4];
		for (j = 0; j < vcnt; j++) {
			p[j] = mesh->GetPoint(vhs[j]);
		}
		for (j = 0; j < 4; j++) {
			FacePts[i][j] = p[j] * 4 + p[(j + 1) % 4] * 2 + p[(j + 3) % 4] * 2 + p[(j + 2) % 4];
			FacePts[i][j] /= 9;
		}
	}

	//计算边点
	for (i = 0; i < EdgeNum; i++) {
		vcnt = Edges->GetCellIds(i, vhs);
		assert(vcnt == 2);
		//每个边都有两个内点
		EdgePts[i].resize(2);
		fcnt = mesh->GetEdgeToNeighborFaces(i, fhs);
		//如果是内部边
		if (fcnt == 2) {
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
		//如果是Boundry
		else {
			Point p[2];
			for (j = 0; j < vcnt; j++) {
				p[j] = mesh->GetPoint(vhs[j]);
			}
			EdgePts[i][0] = (p[0] * 2 + p[1]) / 3;
			EdgePts[i][1] = (p[1] * 2 + p[0]) / 3;
		}
	}
	//计算角点
	for (i = 0; i < PointNum; i++) {
		auto p = mesh->GetPoint(i);
		fcnt = mesh->GetPointToNeighborFaces(i, fhs);
		if (fcnt>= 3) {
			CornerPts[i] = { 0,0,0 };
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
		else {
			CornerPts[i] = p;
		}
	}

	SurfaceMesh::Pointer res = SurfaceMesh::New();
	Points::Pointer ControlPoints = Points::New();
	CellArray::Pointer SubdivisionCells = CellArray::New();


	for (auto FaceId = 0; FaceId < FaceNum; FaceId++) {
		//每个面有十六个控制点
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

		//for (j = 0; j < 4; j++) {
		//	for (i = 0; i < 4; i++) {
		//		ControlPoints->AddPoint(ControlPts[j][i]);
		//	}
		//}
		int n = 9;
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

}


IGAME_NAMESPACE_END