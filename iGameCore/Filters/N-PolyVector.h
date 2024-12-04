//
// Created by stang on 2024/10/23.
//
#ifndef N_VECTOR_H
#define N_VECTOR_H

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <random>

#include "iGameModel.h"
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include<Eigen/Sparse>

#include "iGameSceneManager.h"

IGAME_NAMESPACE_BEGIN
    class N_PolyMesh : public Filter
    {
    public:
        I_OBJECT(N_PolyMesh);
        static Pointer New() { return new N_PolyMesh; }


        void SetModel(Model* model)
        {
            m_Model = model;
        }

        bool Execute() override
        {
            model = m_Model;

            n_polymesh();
            // auto obj = this->GetInput(0);
            // this->SetOutput(obj);
            // auto model = iGame::SceneManager::Instance()->GetCurrentScene()->CreateModel(obj);
            // iGame::SceneManager::Instance()->GetCurrentScene()->AddModel(model);
            return true;
        }

    protected:
        N_PolyMesh()
        {
            SetNumberOfInputs(1);
            SetNumberOfOutputs(1);
        }

        ~N_PolyMesh() override = default;

        Model* source_Model{};
        Model* target_Model{};
        SurfaceMesh::Pointer m_Mesh{};
        Points::Pointer m_Points{};
        CellLinks::Pointer m_EdgeLinks{};

        SurfaceMesh::Pointer source_mesh{};
        SurfaceMesh::Pointer target_mesh{};
        Model::Pointer model{};
        int INF = 0x3f3f3f;

        // std::unordered_map<igIndex, std::vector<std::pair<int, double>>> CotValue{};
        // Eigen::SparseMatrix<double> M_cot;
        // Eigen::MatrixX2d uv; //目标网格坐标

        void n_polymesh();
        // void init();
        // std::unordered_map<igIndex, unsigned int> vh2idx;
        // void exec(double t);
        // void Exec(int _n);

        inline double calc_cot(const Point& v1, Point& v2, Point& v3)
        {
            auto t1 = v1 - v2, t2 = v3 - v2;
            return 1.0 / tan(acos(t1 * t2 / (t1.norm() * t2.norm())));
        }


        // bool isBoundaryPoint(igIndex pointIndex);

        // double get_theta(igIndex _fh, igIndex _vh1, igIndex _vh2);
        //
        // double get_area(igIndex _fh, igIndex _vh1, igIndex _vh2);
        void loadConstrains(const std::string& filename, std::vector<int>& cons_id, std::vector<Vector3d>& cons_vec);
        void writeCrossField(std::vector<float>& lines, std::vector<Vector3d>& crossfield);

        void calculateMeshFaceBase(std::vector<Vector3d>& f_base);

    void writeCrossField_file(const std::string& filename, std::vector<Vector3d>& crossfield)
    {


        std::fstream ofile(filename.c_str(), std::ios_base::out);
        if (!ofile.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
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


#endif //N_VECTOR_H
