#ifndef iGameARAPTest_h
#define iGameARAPTest_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameModel.h"
#include <random>
#include <Eigen/sparseLU>
#include <Eigen/dense>

IGAME_NAMESPACE_BEGIN
class ARAPTest : public Filter {
public:
	I_OBJECT(ARAPTest);
	static Pointer New() { return new ARAPTest; }

	bool Initialize() {
		mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		mesh->RequestEditStatus();
		for(igIndex vid = 0;vid < mesh->GetNumberOfPoints(); vid ++)
			pos_ref.push_back(mesh->GetPoint(vid));
		IGsize e_num = mesh->GetNumberOfEdges();
		edgeCotWeight.resize(e_num, 0);
		/*CalcEdgeCotWeight();*/
		CalcEdgeUnifromWeight();
		Lts.resize(mesh->GetNumberOfPoints(), Eigen::Matrix3d::Zero());

		model = m_Model;
		if (mesh == nullptr) { return false; }

		// 这里请求进行选点
		Points* ps = mesh->GetPoints();
		fixed = Selection::New();
		fixed->SetFilterEvent(&ARAPTest::CallbackEvent, this, std::placeholders::_1);
		model->RequestPointSelection(ps, fixed);

		// 执行算法初始化
		//auto painter = model->GetPainter();
		//painter->SetPen(Color::Red);
		//painter->SetPen(3);
		//painter->DrawLine(mesh->GetPoint(0), mesh->GetPoint(1));

		//FloatArray::Pointer color = FloatArray::New();
		//color->SetName("test");
		//for (int i = 0; i < mesh->GetNumberOfPoints(); i++) {
		//	color->AddValue(mesh->GetPoint(i)[0]);
		//}
		//mesh->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_POINT, color);

		return true;
	}

	bool Begin() {
		// 这里请求拖动点
		Points* ps = mesh->GetPoints();
		moved = Selection::New();
		moved->SetFilterEvent(&ARAPTest::CallbackEvent, this, std::placeholders::_1);
		model->RequestDragPoint(ps, moved);

		return true;
	}

	bool Execute() override {
		// TODO: 执行算法
		
		//mesh->SetPoint(dragId, dragNew);
		//std::cout << "Number of selected ids: " << fixed->GetSelectedIdsSize() << std::endl;
		std::vector<Eigen::Triplet<double>> Coeff;
		Eigen::SparseMatrix<double> LaplaceOperator;

		IGsize ne = mesh->GetNumberOfEdges();
		IGsize nv = mesh->GetNumberOfPoints();

		for(igIndex ei = 0; ei < ne; ei++)
		{
			igIndex pids[2];
			mesh->GetEdgePointIds(ei, pids);

			if(!fix_hanles.count(pids[0]) && pids[0] != dragId)
			{
				Coeff.emplace_back(Eigen::Triplet<double>(pids[0], pids[0], edgeCotWeight[ei]));
				Coeff.emplace_back(Eigen::Triplet<double>(pids[0], pids[1], -edgeCotWeight[ei]));
			}

			if(!fix_hanles.count(pids[1]) && pids[0] != dragId)
			{
				Coeff.emplace_back(Eigen::Triplet<double>(pids[1], pids[1], edgeCotWeight[ei]));
				Coeff.emplace_back(Eigen::Triplet<double>(pids[1], pids[0], -edgeCotWeight[ei]));
			}
		}

		Coeff.emplace_back(Eigen::Triplet<double>(dragId, dragId, 1.0));

		for(auto vid : fix_hanles)
			Coeff.emplace_back(Eigen::Triplet<double>(vid, vid, 1.0));

		LaplaceOperator.resize(nv, nv);
		LaplaceOperator.setFromTriplets(Coeff.begin(), Coeff.end());
		LaplaceOperatorSolver.compute(LaplaceOperator);
		assert(LaplaceOperatorSolver.info() == Eigen::Success);

		for(int i = 0; i < 4; i ++)
		{
			Local();
			Global();
		}


		mesh->Modified();
		model->Update(); // 更新模型
		return true;
    }

	void CallbackEvent(Selection::Event _event) {
		switch (_event.type)
		{
		case Selection::Event::PickPoint:
			// 选几个固定点, 并保存下来
			std::cout << "Pick point id: " << _event.pickId << std::endl;
			fix_hanles.emplace(_event.pickId);
			break;
		case Selection::Event::PickFace:
			std::cout << "Pick face id: " << _event.pickId << std::endl;
			break;
		case Selection::Event::DragPoint:
			std::cout << "Drag point id: " << _event.pickId << " " << _event.pos << std::endl;
			dragId = _event.pickId;
			dragNew = _event.pos;
			oldPos = mesh->GetPoint(dragId);
			this->Execute();
			break;
		default:
			break;
		}
	}

protected:
	ARAPTest()
	{
		SetNumberOfInputs(1);
	}
	~ARAPTest() override = default;

	igIndex dragId{ -1 };
	Vector3f dragNew{};
	Point oldPos;

	Selection::Pointer fixed{};
	Selection::Pointer moved{};
	SurfaceMesh::Pointer mesh{};
	Model::Pointer model{};

	std::vector<Eigen::Matrix3d> Lts;
	std::set<igIndex> fix_hanles;
	std::vector<double> edgeCotWeight;
	std::vector<Point> pos_ref;
	Eigen::SparseLU<Eigen::SparseMatrix<double>> LaplaceOperatorSolver;

	void CalcEdgeCotWeight()
	{
		IGsize fnum = mesh->GetNumberOfFaces();

		for(igIndex fid = 0; fid < fnum; fid++)
		{
			igIndex pids[3];
			mesh->GetFacePointIds(fid, pids);
			Point p[3];
			for(int i=0;i<3;i++)
				p[i] = mesh->GetPoint(pids[i]);
			for(int i=0;i<3;i++)
			{
				auto eid = mesh->GetEdgeIdFormPointIds(pids[i], pids[(i+1)%3]);
				auto e1 = p[i] - p[(i+2)%3];
				auto e2 = p[(i+1)%3] - p[(i+2)%3];
				double weight = e1.dot(e2) / e1.cross(e2).norm();
				edgeCotWeight[eid] += weight;
			}
		}
	}

	void CalcEdgeUnifromWeight()
	{
		IGsize e_num = mesh->GetNumberOfEdges();
		for(igIndex eid=0;eid<e_num;eid++)
			edgeCotWeight[eid] = 1.0;
	}

	void Local()
	{
		IGsize nv = mesh->GetNumberOfPoints();
		IGsize ne = mesh->GetNumberOfEdges();
		std::vector<Eigen::Matrix3d> S;
		S.resize(nv, Eigen::Matrix3d::Zero());

		
		for(igIndex ei = 0; ei < ne; ei ++)
		{
			igIndex pids[2];
			mesh->GetEdgePointIds(ei, pids);
			auto e = mesh->GetPoint(pids[0]) - mesh->GetPoint(pids[1]);
			auto e_f = pos_ref[pids[0]] - pos_ref[pids[1]];

			Eigen::Vector3d e_(e[0], e[1], e[2]);
			Eigen::Vector3d e_f_(e_f[0], e_f[1], e_f[2]);

			Eigen::Matrix3d s = edgeCotWeight[ei] * (e_f_ * e_.transpose());
			S[pids[0]] += s;
			S[pids[1]] += s;
		}

#pragma omp parallel for
		for(int i=0;i<nv;i++)
		{
			Eigen::JacobiSVD<Eigen::Matrix3d> svd(S[i], Eigen::ComputeFullU | Eigen::ComputeFullV);

			Eigen::Matrix3d U = svd.matrixU();
			Eigen::Matrix3d V = svd.matrixV();

			Eigen::Matrix3d R = V * U.transpose();

			if (R.determinant() < 0)
			{
				U.col(2) = -U.col(2);
				R = V * U.transpose();
			}
			Lts[i] = R;
		}
	}

	void Global()
	{

		IGsize ne = mesh->GetNumberOfEdges();
		IGsize nv = mesh->GetNumberOfPoints();
		Eigen::MatrixX3d b, uv;
		b.resize(nv, 3);
		b.setZero();
		uv.resize(nv, 3);
		for(igIndex ei=0;ei<ne;ei++)
		{
			igIndex pids[2];
			mesh->GetEdgePointIds(ei, pids);

			auto esub = pos_ref[pids[0]] - pos_ref[pids[1]];
			Eigen::Vector3d esub_(esub[0], esub[1], esub[2]);
			Eigen::Vector3d right = 0.5 * edgeCotWeight[ei] * ((Lts[pids[0]] + Lts[pids[1]]) * esub_);

			b(pids[0], 0) += right[0];
			b(pids[0], 1) += right[1];
			b(pids[0], 2) += right[2];

			b(pids[1], 0) -= right[0];
			b(pids[1], 1) -= right[1];
			b(pids[1], 2) -= right[2];
		}

		for(auto vid : fix_hanles)
		{
			b(vid, 0) = pos_ref[vid][0];
			b(vid, 1) = pos_ref[vid][1];
			b(vid, 2) = pos_ref[vid][2];
		}

		b(dragId, 0) = dragNew[0];
		b(dragId, 1) = dragNew[1];
		b(dragId, 2) = dragNew[2];

		uv.col(0) = LaplaceOperatorSolver.solve(b.col(0));
		uv.col(1) = LaplaceOperatorSolver.solve(b.col(1));
		uv.col(2) = LaplaceOperatorSolver.solve(b.col(2));

#pragma omp parallel for
		for(int vid=0;vid<nv;vid++)
			mesh->SetPoint(vid, Point(uv(vid, 0), uv(vid, 1), uv(vid, 2)));
		
	}
};
IGAME_NAMESPACE_END
#endif