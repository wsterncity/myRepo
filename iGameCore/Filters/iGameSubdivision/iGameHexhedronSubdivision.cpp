#include "iGameHexhedronSubdivision.h"
IGAME_NAMESPACE_BEGIN

bool HexhedronSubdivision::Execute()
{
	this->mesh = DynamicCast<VolumeMesh>(this->mesh);
	if (mesh == nullptr) { return false; }

	this->mesh->BuildFacesAndEdges();
	this->mesh->BuildEdges();
	this->mesh->RequestEditStatus();
	auto Cells = mesh->GetCells();
	igIndex CellNum = mesh->GetNumberOfVolumes();
	auto Faces = mesh->GetFaces();
	igIndex FaceNum = mesh->GetNumberOfFaces();
	auto Edges = mesh->GetEdges();
	igIndex EdgeNum = mesh->GetNumberOfEdges();
	auto Points = mesh->GetPoints();
	igIndex PointNum = mesh->GetNumberOfPoints();
	igIndex i, j, k;
	igIndex vcnt = 0;
	igIndex vhs[IGAME_CELL_MAX_SIZE];
	//对于一个六面体，有一个体，六个面，十二条边，八个点，共有1*8+6*4+12*2+8*1=64
	//内点
	std::vector<std::vector<Point>>inPts(CellNum);
	//面点
	std::vector<std::vector<Point>>FacePts(FaceNum);
	//边点
	std::vector<std::vector<Point>>EdgePts(EdgeNum);
	//角点
	std::vector<Point>CornerPts(PointNum);

	//计算内点
	for (i = 0; i < CellNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		assert(vcnt == 8);
		Point p[8];
		for (j = 0; j < vcnt; j++) {
			p[j] = mesh->GetPoint(vhs[j]);
		}
		//每个体有八个内点
		inPts[i].resize(8);
		for (j = 0; j < vcnt; j++) {
			if (j < 4) {
				inPts[i][j] = p[j] * 8 + (p[(j + 1) % 4] + p[(j + 3) % 4] + p[j + 4]) * 4 +
					(p[(j + 2) % 4] + p[(j + 1) % 4 + 4] + p[(j + 3) % 4 + 4]) * 2 + p[(j + 2) % 4 + 4];
			}
			else {
				inPts[i][j] = p[j] * 8 + (p[(j + 1) % 4 + 4] + p[(j - 1) % 4 + 4] + p[j - 4]) * 4 +
					(p[(j + 2) % 4 + 4] + p[(j + 1) % 4] + p[(j - 1) % 4]) * 2 + p[(j + 2) % 4];
			}
			inPts[i][j] /= 27;
		}

	}
	//计算面点
	igIndex cellIds[16];
	igIndex neighborNum = 0;
	for (i = 0; i < FaceNum; i++) {
		vcnt = Faces->GetCellIds(i, vhs);
		assert(vcnt == 4);
		//每个面有四个内点
		FacePts[i].resize(4);
		neighborNum = mesh->GetFaceToNeighborVolumes(i, cellIds);
		if (neighborNum == 2) {
			for (j = 0; j < 4; j++) {
				auto cell = mesh->GetVolume(cellIds[0]);
				for (k = 0; k < 8; k++) {
					if (cell->GetPointId(k) == vhs[j]) {
						FacePts[i][j] = inPts[cellIds[0]][k];
						break;
					}
				}
				cell = mesh->GetVolume(cellIds[1]);
				for (k = 0; k < 8; k++) {
					if (cell->GetPointId(k) == vhs[j]) {
						FacePts[i][j] += inPts[cellIds[1]][k];
						break;
					}
				}
				FacePts[i][j] /= 2;
			}
		}
		else if (neighborNum == 1) {
			Point p[4];
			for (j = 0; j < vcnt; j++) {
				p[j] = mesh->GetPoint(vhs[j]);
			}
			for (j = 0; j < 4; j++) {
				FacePts[i][j] = p[j] * 4 + (p[(j + 1) % 4] + p[(j + 3) % 4]) * 2 + p[(j + 2) % 4];
				FacePts[i][j] /= 9;
			}
		}
	}

	//for (i = 0; i < EdgeNum; i++) {
	//	vcnt = Edges->GetCellIds(i, vhs);
	//	for (j = 0; j < vcnt; j++) {
	//		std::cout << vhs[j] << ' ';
	//	}
	//	std::cout << '\n';
	//	neighborNum = mesh->GetEdgeToNeighborVolumes(i, cellIds);
	//	for (j = 0; j < neighborNum; j++) {
	//		vcnt = Cells->GetCellIds(cellIds[j], vhs);
	//		for (k = 0; k < vcnt; k++) {
	//			std::cout << vhs[k] << ' ';
	//		}
	//		std::cout << '\n';
	//	}
	//}
	//计算边点
	for (i = 0; i < EdgeNum; i++) {
		vcnt = Edges->GetCellIds(i, vhs);
		assert(vcnt == 2);
		//每个边都有两个内点
		EdgePts[i].resize(2);
		igIndex cellIds_1[64];
		std::set<igIndex>tmp;
		igIndex index = 0;
		neighborNum = mesh->GetPointToNeighborVolumes(vhs[0], cellIds_1);
		for (j = 0; j < neighborNum; j++) {
			if (tmp.count(cellIds_1[j]))cellIds[index++] = cellIds_1[j];
			else tmp.insert(cellIds_1[j]);
		}
		neighborNum = mesh->GetPointToNeighborVolumes(vhs[1], cellIds_1);
		for (j = 0; j < neighborNum; j++) {
			if (tmp.count(cellIds_1[j]))cellIds[index++] = cellIds_1[j];
			else tmp.insert(cellIds_1[j]);
		}
		neighborNum = index;
		//如果是内部边
		if (neighborNum == 4) {
			for (j = 0; j < 2; j++) {
				EdgePts[i][j] = Point{ 0,0,0 };
				int tmpindex = 0;
				for (k = 0; k < neighborNum; k++) {
					auto cell = mesh->GetVolume(cellIds[k]);

					for (igIndex id = 0; id < 8; id++) {
						if (cell->GetPointId(id) == vhs[j]) {
							EdgePts[i][j] += inPts[cellIds[k]][id];
							tmpindex++;
							break;
						}
					}
				}
				if (tmpindex != neighborNum) {
					std::cout << "dsad" << '\n';
				}
				EdgePts[i][j] /= neighborNum;
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
		//每个顶点一个角点
		//如果是内部点
		if (!mesh->isBoundryPoint(i)) {
			neighborNum = mesh->GetPointToNeighborVolumes(i, cellIds);
			CornerPts[i] = { 0,0,0 };
			for (j = 0; j < neighborNum; j++) {
				auto cell = mesh->GetVolume(cellIds[j]);
				for (k = 0; k < 8; k++) {
					if (cell->GetPointId(k) == i) {
						CornerPts[i] += inPts[cellIds[j]][k];
						break;
					}
				}
			}
			CornerPts[i] /= neighborNum;

		}
		else if (!mesh->isCornerPoint(i)) {
			igIndex fhs[64];
			int count = 0;
			CornerPts[i] = { 0,0,0 };
			neighborNum = mesh->GetPointToNeighborFaces(i, fhs);
			for (j = 0; j < neighborNum; j++) {
				if (mesh->isBoundryFace(fhs[j])) {
					count++;
					auto f = mesh->GetFace(fhs[j]);

					for (k = 0; k < 4; k++) {
						if (f->GetPointId(k) == i) {
							CornerPts[i] += FacePts[fhs[j]][k];
							break;
						}
					}
				}
			}
			CornerPts[i] /= count;
		}
		else {
			CornerPts[i] = p;
		}
	}

	VolumeMesh::Pointer res = VolumeMesh::New();
	Points::Pointer ControlPoints = Points::New();
	CellArray::Pointer SubdivisionCells = CellArray::New();
	//igIndex TestID = 0;
	//for (auto& tmpP : inPts[TestID]) {
	//	ControlPoints->AddPoint(tmpP);
	//}
	//igIndex fhs[64];
	//mesh->GetVolumeFaceIds(TestID, fhs);
	//for (i = 0; i < 6; i++) {
	//	for (auto& tmpP : FacePts[fhs[i]]) {
	//		ControlPoints->AddPoint(tmpP);
	//	}
	//}
	//igIndex ehs[64];
	//mesh->GetVolumeEdgeIds(TestID, ehs);
	//for (i = 0; i < 12; i++) {
	//	for (auto& tmpP : EdgePts[ehs[i]]) {
	//		ControlPoints->AddPoint(tmpP);
	//	}
	//}
	//vcnt = Cells->GetCellIds(TestID, vhs);
	//for (i = 0; i < vcnt; i++) {
	//	ControlPoints->AddPoint(CornerPts[vhs[i]]);
	//}
	//ControlPoints->Reserve(8 * CellNum + 4 * FaceNum + 2 * EdgeNum + PointNum);
	//for (i = 0; i < CellNum; i++) {
	//	for (auto& tmpP : inPts[i]) {
	//		ControlPoints->AddPoint(tmpP);
	//	}
	//}
	//for (i = 0; i < FaceNum; i++) {
	//	for (auto& tmpP : FacePts[i]) {
	//		ControlPoints->AddPoint(tmpP);
	//	}
	//}
	//for (i = 0; i < EdgeNum; i++) {
	//	for (auto& tmpP : EdgePts[i]) {
	//		ControlPoints->AddPoint(tmpP);
	//	}
	//}
	//for (i = 0; i < PointNum; i++) {
	//	ControlPoints->AddPoint(CornerPts[i]);
	//}



	neighborNum = mesh->GetFaceToNeighborVolumes(0, cellIds);
	std::cout << neighborNum << '\n';
	//生成细分体
	igIndex cellID = 0;
	for (cellID = 0; cellID < CellNum; cellID++) {
		//每个体有六十四个控制点
		Point ControlPts[4][4][4];
		vcnt = Cells->GetCellIds(cellID, vhs);
		for (k = 0; k < 4; k++) {
			for (j = 0; j < 4; j++) {
				for (i = 0; i < 4; i++) {
					ControlPts[k][j][i] = { 0,0,0 };
				}
			}
		}

		auto Cell = mesh->GetCell(cellID);
		ControlPts[0][0][0] = CornerPts[vhs[0]];
		ControlPts[0][0][3] = CornerPts[vhs[1]];
		ControlPts[0][3][3] = CornerPts[vhs[2]];
		ControlPts[0][3][0] = CornerPts[vhs[3]];
		ControlPts[3][0][0] = CornerPts[vhs[4]];
		ControlPts[3][0][3] = CornerPts[vhs[5]];
		ControlPts[3][3][3] = CornerPts[vhs[6]];
		ControlPts[3][3][0] = CornerPts[vhs[7]];

		//k边
		for (i = 0; i < 4; i++) {
			auto EdgeID = mesh->GetEdgeIdFormPointIds(Cell->GetPointId(i), Cell->GetPointId(i + 4));
			auto edge = mesh->GetEdge(EdgeID);
			switch (i)
			{
			case 0:
				j = k = 0;
				break;
			case 1:
				j = 0;
				k = 3;
				break;
			case 2:
				j = 3;
				k = 3;
				break;
			case 3:
				j = 3;
				k = 0;
				break;
			default:
				break;
			}
			ControlPts[1][j][k] = EdgePts[EdgeID][0];
			ControlPts[2][j][k] = EdgePts[EdgeID][1];
			if (edge->GetPointId(0) != Cell->GetPointId(i)) {
				std::swap(ControlPts[1][j][k], ControlPts[2][j][k]);
			}
		}
		//i边
		igIndex edgeIndexs[4][4] = { {0,1,0,0},{3,2,0,3},{4,5,3,0},{7,6,3,3} };
		for (i = 0; i < 4; i++) {
			auto EdgeID = mesh->GetEdgeIdFormPointIds(Cell->GetPointId(edgeIndexs[i][0]),
				Cell->GetPointId(edgeIndexs[i][1]));
			auto edge = mesh->GetEdge(EdgeID);
			j = edgeIndexs[i][2];
			k = edgeIndexs[i][3];
			ControlPts[j][k][1] = EdgePts[EdgeID][0];
			ControlPts[j][k][2] = EdgePts[EdgeID][1];
			if (edge->GetPointId(0) != Cell->GetPointId(i)) {
				std::swap(ControlPts[j][k][1], ControlPts[j][k][2]);
			}
		}
		//j边
		igIndex edgeIndexs_2[4][4] = { {0,3,0,0},{1,2,0,3}, {4,7,3,0},{5,6,3,3} };
		for (i = 0; i < 4; i++) {
			auto EdgeID = mesh->GetEdgeIdFormPointIds(Cell->GetPointId(edgeIndexs_2[i][0]),
				Cell->GetPointId(edgeIndexs_2[i][1]));
			auto edge = mesh->GetEdge(EdgeID);
			j = edgeIndexs_2[i][2];
			k = edgeIndexs_2[i][3];
			ControlPts[j][1][k] = EdgePts[EdgeID][0];
			ControlPts[j][2][k] = EdgePts[EdgeID][1];
			if (edge->GetPointId(0) != Cell->GetPointId(i)) {
				std::swap(ControlPts[j][1][k], ControlPts[j][2][k]);
			}
		}


		//面
		igIndex faceindexs[6][5] = { {0,1,2,3,0},{4,5,6,7,3},
			{0,1,5,4,0},{3,2,6,7,3},{0,3,7,4,0},{1,2,6,5,3} };
		for (i = 0; i < 6; i++) {
			igIndex tmpVhs[4] = { vhs[faceindexs[i][0]],vhs[faceindexs[i][1]],
				vhs[faceindexs[i][2]], vhs[faceindexs[i][3]] };
			auto FaceID = mesh->GetFaceIdFormPointIds(tmpVhs, 4);
			igIndex tmpvhs[64];
			igIndex tmpvcnt = 0;
			igIndex st = 0;
			tmpvcnt = mesh->GetFacePointIds(FaceID, tmpvhs);
			for (j = 0; j < tmpvcnt; j++) {
				if (tmpvhs[j] == tmpVhs[0]) {
					st = j;
					break;
				}
			}
			if (i < 2) {
				j = faceindexs[i][4];
				ControlPts[j][1][1] = FacePts[FaceID][st % 4];
				ControlPts[j][1][2] = FacePts[FaceID][(st + 1) % 4];
				ControlPts[j][2][2] = FacePts[FaceID][(st + 2) % 4];
				ControlPts[j][2][1] = FacePts[FaceID][(st + 3) % 4];
			}
			else if (i < 4) {
				j = faceindexs[i][4];
				ControlPts[1][j][1] = FacePts[FaceID][st % 4];
				ControlPts[1][j][2] = FacePts[FaceID][(st + 1) % 4];
				ControlPts[2][j][2] = FacePts[FaceID][(st + 2) % 4];
				ControlPts[2][j][1] = FacePts[FaceID][(st + 3) % 4];
			}
			else {
				j = faceindexs[i][4];
				ControlPts[1][1][j] = FacePts[FaceID][st % 4];
				ControlPts[1][2][j] = FacePts[FaceID][(st + 1) % 4];
				ControlPts[2][2][j] = FacePts[FaceID][(st + 2) % 4];
				ControlPts[2][1][j] = FacePts[FaceID][(st + 3) % 4];
			}
		}


		//内点
		std::array<std::array<int, 3>, 8> interior_map = { {
			{1, 1, 1}, {1, 1, 2},  {1, 2, 2},{1, 2, 1},
			{2, 1, 1}, {2, 1, 2},  {2, 2, 2}, {2, 2, 1}
		} };
		for (i = 0; i < 8; i++) {
			ControlPts[interior_map[i][0]][interior_map[i][1]][interior_map[i][2]]
				= inPts[cellID][i];
		}

		//for (i = 0; i < vcnt; i++) {
		//	ControlPoints->AddPoint(mesh->GetPoint(vhs[i]));
		//}
		//SubdivisionCells->AddCellId8(0, 1, 2, 3, 4, 5, 6, 7);

		//continue;
		const int n = 10;
		std::array<std::array<std::array<Point, n>, n>, n> SubdivisionPts;


		for (int i = 0; i < n; ++i) {
			double u = static_cast<double>(i) / (n - 1);
			for (int j = 0; j < n; ++j) {
				double v = static_cast<double>(j) / (n - 1);
				for (int k = 0; k < n; ++k) {
					double w = static_cast<double>(k) / (n - 1);
					SubdivisionPts[i][j][k] = BSplineSurface(ControlPts, u, v, w);
					ControlPoints->AddPoint(SubdivisionPts[i][j][k]);
				}
			}
		}
		int size[3] = { n,n,n };
		igIndex tmpvhs[8] = {
						0,1,1 + size[0] * size[1],size[0] * size[1],
						size[0],1 + size[0],1 + size[0] + size[0] * size[1],size[0] + size[0] * size[1]
		};
		igIndex st;
		for (k = 0; k < size[2] - 1; ++k) {
			for (j = 0; j < size[1] - 1; ++j) {
				st = j * size[0] + k * size[0] * size[1] + cellID * n * n * n;
				for (i = 0; i < size[0] - 1; ++i) {
					for (int it = 0; it < 8; it++) {
						vhs[it] = st + tmpvhs[it];
					}
					SubdivisionCells->AddCellIds(vhs, 8);
					st++;
				}
			}
		}
		for (k = 0; k < 4; k++) {
			for (j = 0; j < 4; j++) {
				for (i = 0; i < 4; i++) {
					ControlPoints->AddPoint(ControlPts[k][j][i]);
					//std::cout << k << ' ' << j << ' ' << i << ' ' << ControlPts[k][j][i][0] << " " << ControlPts[k][j][i][1] << " " << ControlPts[k][j][i][2] << "\n";
				}
			}
		}
		break;
	}
	res->SetVolumes(SubdivisionCells);
	res->SetPoints(ControlPoints);
	this->SetOutput(res);

}

Point HexhedronSubdivision::B_spline(Point p[4], float t)
{
	double B[4];
	B[0] = (1 - t) * (1 - t) * (1 - t) / 6.0;
	B[1] = (3 * t * t * t - 6 * t * t + 4) / 6.0;
	B[2] = (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0;
	B[3] = t * t * t / 6.0;

	Point result = { 0, 0, 0 };
	for (int i = 0; i < 4; ++i) {
		result[0] += B[i] * p[i][0];
		result[1] += B[i] * p[i][1];
		result[2] += B[i] * p[i][2];
	}
	return result;
}

double BezierBasis(int i, double t) {
	switch (i) {
	case 0: return (1 - t) * (1 - t) * (1 - t);
	case 1: return 3 * t * (1 - t) * (1 - t);
	case 2: return 3 * t * t * (1 - t);
	case 3: return t * t * t;
	default: throw std::out_of_range("Index out of bounds for Bezier basis");
	}
}
Point HexhedronSubdivision::BSplineSurface(Point ControlPts[4][4][4], double u, double v, double w)
{
	Point result;
	for (int i = 0; i < 3; ++i) {
		result[i] = 0.0;
		for (int i0 = 0; i0 < 4; ++i0) {
			for (int i1 = 0; i1 < 4; ++i1) {
				for (int i2 = 0; i2 < 4; ++i2) {
					result[i] += ControlPts[i0][i1][i2][i] *
						BezierBasis(i0, u) *
						BezierBasis(i1, v) *
						BezierBasis(i2, w);
				}
			}
		}
	}
	return result;
}

IGAME_NAMESPACE_END