#ifndef NPolyVctorFields_H
#define NPolyVctorFields_H


#include<complex>
#include<Eigen/Eigen>
#include<math.h>
#include<string>
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#define PI 3.14159265358979323846
IGAME_NAMESPACE_BEGIN

class  NPolyVectorFields : public Filter {
public:
	I_OBJECT(NPolyVectorFields);
	static Pointer New() { return new NPolyVectorFields; }
	bool Execute() override
	{
		SurfaceMesh::Pointer mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		mesh->RequestEditStatus();
		std::vector<int> cons_id(0);
		std::vector<Vector3d> cons_vec(0), crossfield(0);
		loadConstrains(cons_id, cons_vec, mesh);
		crossfieldCreator(mesh, cons_id, cons_vec, crossfield);
		//writeCrossField(crossfield);
		for(igIndex fid = 0; fid < mesh->GetNumberOfFaces(); fid ++)
		{
			igIndex pids[3];
			mesh->GetFacePointIds(fid, pids);
			Point p1, p2, p3, cp;
			p1 = mesh->GetPoint(pids[0]);
			p2 = mesh->GetPoint(pids[1]);
			p3 = mesh->GetPoint(pids[2]);
			cp = (p1 + p2 + p3) / 3;
			for (int j = 0; j < 4; j++)
				painter->Show(painter->DrawLine(cp, cp + 0.003 * crossfield[fid * 4 + j]));
			
		}

		//igIndex pids[3];
		//mesh->GetFacePointIds(0, pids);
		//Point p1, p2, p3, op;
		//p1 = mesh->GetPoint(pids[0]);
		//p2 = mesh->GetPoint(pids[1]);
		//p3 = mesh->GetPoint(pids[2]);
		//op = (p1 + p2 + p3) / 3;
		//for (int j = 0; j < 4; j++)
		//	painter->Show(painter->DrawLine(op, op + 0.001* crossfield[j].normalized()));
		return true;
	}

	void setPainter(Painter::Pointer pen)
	{
		painter = pen;
		painter->SetPen(Red);
		painter->SetPen(1);
	}
protected:
	Painter::Pointer painter;

	NPolyVectorFields()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}

	~NPolyVectorFields() override = default;
	void calculateMeshFaceBase(SurfaceMesh::Pointer mesh, std::vector<Vector3d>& f_base)
	{
		IGsize fnum = mesh->GetNumberOfFaces();
		f_base.resize( fnum * 2);
		for (igIndex fid = 0; fid < fnum; ++fid)
		{
			igIndex pids[3];
			Point p1, p2, p3;
			mesh->GetFacePointIds(fid, pids);
			p1 = mesh->GetPoint(pids[0]);
			p2 = mesh->GetPoint(pids[1]);
			p3 = mesh->GetPoint(pids[2]);

			Point normal = mesh->GetFace(fid)->GetNormal();
			f_base[fid * 2] = (p2 - p1).normalized();
			f_base[fid * 2 + 1] = normal.cross(f_base[fid * 2]);
		}
	}

	void crossfieldCreator(SurfaceMesh::Pointer mesh, std::vector<int>& cons_id, std::vector<Vector3d>& cons_vec, std::vector<Vector3d>& crossfield)
	{
		using namespace std;
		typedef complex<double> COMPLEX;

		int fnum = mesh->GetNumberOfFaces();
		vector<int> status(fnum, 0);
		vector<COMPLEX> f_dir(fnum);

		crossfield.clear();
		crossfield.resize(fnum);
		vector<Vector3d> f_base(fnum * 2);
		calculateMeshFaceBase(mesh, f_base);//在每个面上算一组基

		for (int i = 0; i < cons_id.size(); i++)
		{
			int fid = cons_id[i];
			status[fid] = 1;
			Vector3d cf = cons_vec[i].normalized();
			f_dir[fid] = std::pow(COMPLEX(cf.dot(f_base[fid * 2]), cf.dot(f_base[fid * 2 + 1])), 4);
		}

		vector<int> id2sln(fnum, -1);
		vector<int> sln2id(0);
		int count = 0;
		for (igIndex fid = 0; fid < fnum; fid++)
		{
			if (status[fid] == 0)
			{
				sln2id.push_back(fid);
				id2sln[fid] = count;
				count++;
			}
		}

		Eigen::SimplicialLDLT<Eigen::SparseMatrix<COMPLEX>> slu;
		Eigen::SparseMatrix<COMPLEX> A;
		Eigen::VectorXcd b_pre(mesh->GetNumberOfEdges());
		Eigen::VectorXcd b;
		b_pre.setZero();
		vector<Eigen::Triplet<COMPLEX>> tris;

		count = 0;
		for (igIndex fid = 0; fid < fnum; ++fid)
		{
			COMPLEX sum = 0;
			igIndex eids[3];
			mesh->GetFaceEdgeIds(fid, eids);
			for (auto fe_id : eids)
			{
				if (!mesh->IsBoundaryEdge(fe_id))
				{
					igIndex pids[2],fids[2], fid_g;
					Vector3d p1, p2;
					int efnum = mesh->GetEdgeToNeighborFaces(fe_id, fids);
					int evnum = mesh->GetEdgePointIds(fe_id, pids);
					assert(efnum == 2 && evnum == 2);
					fid_g = (fids[0] + fids[1]) - fid;
					p1 = mesh->GetPoint(pids[0]);
					p2 = mesh->GetPoint(pids[1]);
					if (fid < fid_g)
					{
						Point e = (p2 - p1).normalized();

						COMPLEX e_f = COMPLEX(e.dot(f_base[fid * 2]), e.dot(f_base[fid * 2 + 1]));
						COMPLEX e_g = COMPLEX(e.dot(f_base[fid_g * 2]), e.dot(f_base[fid * 2 + 1]));

						COMPLEX e_f_c_4 = pow(conj(e_f), 4);
						COMPLEX e_g_c_4 = pow(conj(e_g), 4);

						if (status[fid] == 1 && status[fid_g] == 1) continue;
						if (status[fid] == 0)
						{
							tris.push_back(Eigen::Triplet<COMPLEX>(count, id2sln[fid], e_f_c_4));
						}
						else
						{
							b_pre[count] += -e_f_c_4 * f_dir[fid];
						}
						if (status[fid_g] == 0)
						{
							tris.push_back(Eigen::Triplet<COMPLEX>(count, id2sln[fid_g], -e_g_c_4));
						}
						else
						{
							b_pre[count] += e_g_c_4 * f_dir[fid_g];
						}
						count++;
					}
				}
			}

		}
		A.resize(count, sln2id.size());
		b.resize(count);
		b = b_pre.head(count);
		A.setFromTriplets(tris.begin(), tris.end());
		Eigen::SparseMatrix<COMPLEX> AT = A.adjoint();
		slu.compute(AT * A);
		Eigen::VectorXcd x = slu.solve(AT * b);

		crossfield.resize(4 * fnum);
		for (int i = 0; i < fnum; i++)
		{
			if (status[i] == 0)
			{
				f_dir[i] = x(id2sln[i]);
			}
			double length = 1;
			double arg = std::arg(f_dir[i]) / 4;
			for (int j = 0; j < 4; j++)
			{
				crossfield[i * 4 + j] = f_base[i * 2] * length * cos(arg + j * PI / 2) + f_base[i * 2 + 1] * length * sin(arg + j * PI / 2);
			}
		}
	}

	void loadConstrains(std::vector<int>& cons_id, std::vector<Vector3d>& cons_vec, SurfaceMesh::Pointer mesh)
	{
		cons_id.reserve(2);
		cons_vec.reserve(2);		
		for (igIndex fid = 0; fid < 2; ++fid)
		{
			cons_id.push_back(fid);
			igIndex pids[3];
			mesh->GetFacePointIds(fid, pids);
			Vector3d p1 = mesh->GetPoint(pids[0]);
			Vector3d p2 = mesh->GetPoint(pids[1]);
			cons_vec.push_back(p2 - p1);
		}
	}

	void writeCrossField(std::vector<Vector3d>& crossfield)
	{
		std::fstream ofile("crossfield.txt", std::ios_base::out);
		int num = crossfield.size();
		ofile << num << std::endl;
		for (int i = 0; i < num; i++)
		{
			ofile << crossfield[i][0] << " " << crossfield[i][1] << " " << crossfield[i][2] << std::endl;
		}
		ofile.close();
	}
};

IGAME_NAMESPACE_END


#endif // NPolyVctorFields_H