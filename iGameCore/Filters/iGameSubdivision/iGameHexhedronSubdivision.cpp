#include "iGameHexhedronSubdivision.h"
IGAME_NAMESPACE_BEGIN
// Calculate the value of the Bernstein polynomial
inline double Bernstein(int i, int n, double t) {
	double binomial_coeff = 1.0;
	for (int j = 0; j < i; ++j) {
		binomial_coeff *= (n - j);
		binomial_coeff /= (j + 1);
	}
	return binomial_coeff * pow(t, i) * pow(1 - t, n - i);
}

// Generate points on the Bezier surface
inline std::vector<std::vector<std::vector<Point>>> GenerateBezierSurface(Point ControlPts[4][4][4], int resolution) {
	std::vector<std::vector<std::vector<Point>>> bezierSurface(resolution,
		std::vector<std::vector<Point>>(resolution, std::vector<Point>(resolution)));

	for (int u = 0; u < resolution; ++u) {
		for (int v = 0; v < resolution; ++v) {
			for (int w = 0; w < resolution; ++w) {

				double u_t = static_cast<double>(u) / (resolution - 1);
				double v_t = static_cast<double>(v) / (resolution - 1);
				double w_t = static_cast<double>(w) / (resolution - 1);
				Point p = { 0, 0, 0 }; // Assuming Point has a default constructor

				// Double loop to calculate the weight of each control point
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						for (int k = 0; k < 4; ++k) {
							double Bu = Bernstein(i, 3, u_t);
							double Bv = Bernstein(j, 3, v_t);
							double Bw = Bernstein(k, 3, w_t);
							Point weightedPoint = ControlPts[i][j][k] * (Bu * Bv * Bw);
							p += weightedPoint;
						}
					}
				}

				bezierSurface[u][v][w] = p;
			}
		}
	}

	return bezierSurface;
}

bool HexhedronSubdivision::Execute()
{
	std::cout << "dsa\n";
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
	std::cout << "Cell Num: " << CellNum << "\nPoint Num: " << PointNum << '\n';
	// For a hexahedron, there is one volume, six faces, twelve edges, and eight points, with a total of 1*8+6*4+12*2+8*1=64
		// Internal points
	std::vector<std::vector<Point>>inPts(CellNum);
	// Face points
	std::vector<std::vector<Point>>FacePts(FaceNum);
	// Edge points
	std::vector<std::vector<Point>>EdgePts(EdgeNum);
	// Corner points
	std::vector<Point>CornerPts(PointNum);

	// Calculate internal points
	for (i = 0; i < CellNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		assert(vcnt == 8);
		Point p[8];
		for (j = 0; j < vcnt; j++) {
			p[j] = mesh->GetPoint(vhs[j]);
		}
		// Each volume has eight internal points
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
	// Calculate face points
	igIndex cellIds[16];
	igIndex neighborNum = 0;
	for (i = 0; i < FaceNum; i++) {
		vcnt = Faces->GetCellIds(i, vhs);
		assert(vcnt == 4);
		// Each face has four internal points
		FacePts[i].resize(4);
		neighborNum = mesh->GetFaceToNeighborVolumes(i, cellIds);
		if (1) {
			for (j = 0; j < 4; j++) {
				auto cell = mesh->GetVolume(cellIds[0]);
				for (k = 0; k < 8; k++) {
					if (cell->GetPointId(k) == vhs[j]) {
						FacePts[i][j] = inPts[cellIds[0]][k];
						break;
					}
				}
				continue;
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
	// Calculate edge points
	for (i = 0; i < EdgeNum; i++) {
		vcnt = Edges->GetCellIds(i, vhs);
		assert(vcnt == 2);
		// Each edge has two internal points
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
		// If it's an internal edge
		if (1) {
			for (j = 0; j < 2; j++) {
				EdgePts[i][j] = Point{ 0,0,0 };
				for (k = 0; k < neighborNum; k++) {
					auto cell = mesh->GetVolume(cellIds[k]);

					for (igIndex id = 0; id < 8; id++) {
						if (cell->GetPointId(id) == vhs[j]) {
							EdgePts[i][j] += inPts[cellIds[k]][id];
							break;
						}
					}
				}
				EdgePts[i][j] /= neighborNum;
			}

		}
		// If it's a boundary edge
		else {
			Point p[2];
			for (j = 0; j < vcnt; j++) {
				p[j] = mesh->GetPoint(vhs[j]);
			}
			EdgePts[i][0] = (p[0] * 2 + p[1]) / 3;
			EdgePts[i][1] = (p[1] * 2 + p[0]) / 3;
		}
	}
	// Calculate corner points
	for (i = 0; i < PointNum; i++) {
		auto p = mesh->GetPoint(i);
		// Each vertex has one corner point
		// If it's an internal point
		if (1) {
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
		else if (!mesh->IsCornerPoint(i)) {
			igIndex fhs[64];
			int count = 0;
			CornerPts[i] = { 0,0,0 };
			neighborNum = mesh->GetPointToNeighborFaces(i, fhs);
			for (j = 0; j < neighborNum; j++) {
				if (mesh->IsBoundaryFace(fhs[j])) {
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



	// Generate subdivided volumes
	igIndex cellID = 0;
	for (cellID = 0; cellID < CellNum; cellID++) {
		// Each volume has sixty-four control points
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


		for (i = 0; i < 4; i++) {
			auto EdgeID = mesh->GetEdgeIdFormPointIds(vhs[i], vhs[i + 4]);
			auto edge = mesh->GetEdge(EdgeID);
			switch (i)
			{
			case 0:
				j = 0;
				k = 0;
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
			if (edge->GetPointId(0) != vhs[i]) {
				std::swap(ControlPts[1][j][k], ControlPts[2][j][k]);
			}
		}

		igIndex edgeIndexs[4][4] = { {0,1,0,0},{3,2,0,3},{4,5,3,0},{7,6,3,3} };
		for (i = 0; i < 4; i++) {
			auto EdgeID = mesh->GetEdgeIdFormPointIds(vhs[edgeIndexs[i][0]],
				vhs[edgeIndexs[i][1]]);
			auto edge = mesh->GetEdge(EdgeID);
			j = edgeIndexs[i][2];
			k = edgeIndexs[i][3];
			ControlPts[j][k][1] = EdgePts[EdgeID][0];
			ControlPts[j][k][2] = EdgePts[EdgeID][1];
			if (edge->GetPointId(0) != vhs[edgeIndexs[i][0]]) {
				std::swap(ControlPts[j][k][1], ControlPts[j][k][2]);
			}
		}

		igIndex edgeIndexs_2[4][4] = { {0,3,0,0},{1,2,0,3}, {4,7,3,0},{5,6,3,3} };
		for (i = 0; i < 4; i++) {
			auto EdgeID = mesh->GetEdgeIdFormPointIds(vhs[edgeIndexs_2[i][0]],
				vhs[edgeIndexs_2[i][1]]);
			auto edge = mesh->GetEdge(EdgeID);
			j = edgeIndexs_2[i][2];
			k = edgeIndexs_2[i][3];
			ControlPts[j][1][k] = EdgePts[EdgeID][0];
			ControlPts[j][2][k] = EdgePts[EdgeID][1];
			if (edge->GetPointId(0) != vhs[edgeIndexs_2[i][0]]) {
				std::swap(ControlPts[j][1][k], ControlPts[j][2][k]);
			}
		}


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


		// interior point
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

		//int n = 65;
		//auto tmp = GenerateBezierSurface(ControlPts, n);
		//for (k = 0; k < n; k++) {
		//	for (j = 0; j < n; j++) {
		//		for (i = 0; i < n; i++) {
		//			ControlPoints->AddPoint(tmp[k][j][i]);
		//		}
		//	}
		//}
		//int size[3] = { n,n,n };
		//igIndex tmpvhs[8] = {
		//				0,1,1 + size[0] * size[1],size[0] * size[1],
		//				size[0],1 + size[0],1 + size[0] + size[0] * size[1],size[0] + size[0] * size[1]
		//};
		//igIndex st;
		//for (k = 0; k < size[2] - 1; ++k) {
		//	for (j = 0; j < size[1] - 1; ++j) {
		//		st = j * size[0] + k * size[0] * size[1] + cellID * n * n * n;
		//		for (i = 0; i < size[0] - 1; ++i) {
		//			for (int it = 0; it < 8; it++) {
		//				vhs[it] = st + tmpvhs[it];
		//			}
		//			SubdivisionCells->AddCellIds(vhs, 8);
		//			st++;
		//		}
		//	}
		//}
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
	return true;
}




IGAME_NAMESPACE_END