//
// Created by stang on 2024/10/23.
//
#include<iostream>

// #include <Dens>
#include "N-PolyVector.h"
namespace iGame
{
void N_PolyMesh::n_polymesh() {
    std::cout << "N_PolyMesh" << std::endl;

    auto obj = this->GetInput(0);
    SurfaceMesh::Pointer surface = DynamicCast<SurfaceMesh>(this->GetInput(0));

    m_Mesh = DynamicCast<SurfaceMesh>(GetInput(0));

    surface->RequestEditStatus(); // 开启编辑模式，并生成邻接关系等数据结构
    auto v_size = surface->GetNumberOfPoints();
    auto e_size = surface->GetNumberOfEdges();
    auto f_size = surface->GetNumberOfFaces();

    std::vector<int> cons_id(0);
    std::vector<Vector3d> crossfield(0);
    std::vector<int> status(f_size, 0);
    typedef std::complex<double> COMPLEX;
    std::vector<COMPLEX> f_dir(f_size, 0);
    std::vector<Vector3d> cons_vec(0);
    std::vector<Vector3d> f_base(f_size * 2);

    for (igIndex fid = 0; fid < 2; ++fid)
    {
        cons_id.push_back(fid);
        igIndex pids[3];
        m_Mesh->GetFacePointIds(fid, pids);
        Vector3d p1 = m_Mesh->GetPoint(pids[0]);
        Vector3d p2 = m_Mesh->GetPoint(pids[1]);
        cons_vec.push_back(p2 - p1);
    }

    //cons_id.push_back(2746);
    //cons_vec.push_back(Vector3d(0.004945, 0.0141348, -0.004092));
    //cons_id.push_back(8692);
    //cons_vec.push_back(Vector3d(0.011338, -0.0192, -0.00949099999999999));

    calculateMeshFaceBase(f_base);

    for (int i = 0; i < cons_id.size(); i++) {
        int fid = cons_id[i];
        status[fid] = 1;
        // std::cout << "fid:" << fid << std::endl;
        auto cf = cons_vec[i].normalized();
        f_dir[fid] = std::pow(
                COMPLEX(cf.dot(f_base[fid * 2]), cf.dot(f_base[fid * 2 + 1])),
                4);
    }
    std::vector<int> id2sln(f_size, -1);
    std::vector<int> sln2id(0);
    int count = 0;
    for (int i = 0; i < f_size; i++) {
        if (status[i] == 0) {
            sln2id.push_back(i);
            id2sln[i] = count;
            count++;
        }
    }
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<COMPLEX>> slu;
    Eigen::SparseMatrix<COMPLEX> A;
    Eigen::VectorXcd b_pre(e_size);
    Eigen::VectorXcd b;
    b_pre.setZero();
    std::vector<Eigen::Triplet<COMPLEX>> tris;
    count = 0;
    for (int f = 0; f < f_size; f++) {
        // std::cout << "f:" << f << std::endl;
        COMPLEX sum = 0;
        igIndex face_edges[64]{};
        auto faceEdgesNum = surface->GetFaceEdgeIds(f, face_edges);
        for (int f_e = 0; f_e < faceEdgesNum; f_e++) {
            auto e = face_edges[f_e];
            // std::cout << "e:" << e << std::endl;
            if (!surface->IsBoundaryEdge(e)) {
                igIndex edge_Nerfaces[64]{};
                auto edgeNerFacesNum = surface->GetEdgeToNeighborFaces(
                        e, edge_Nerfaces);
                auto f_g = f == edge_Nerfaces[0]
                               ? edge_Nerfaces[1]
                               : edge_Nerfaces[0];
                // std::cout << "f_g:" << f_g << std::endl;
                igIndex edge_points[64]{};
                auto edgePiontsNum = surface->GetEdgePointIds(e, edge_points);
                auto p1 = surface->GetPoint(edge_points[0]);
                auto p2 = surface->GetPoint(edge_points[1]);
                // std::cout << "f < f_g:" << f_g << std::endl;
                if (f < f_g) {
                    auto e = (p2 - p1).normalized();
                    auto e_f = COMPLEX(e.dot(f_base[f * 2]),
                                       e.dot(f_base[f * 2 + 1]));
                    auto e_g = COMPLEX(e.dot(f_base[f_g * 2]),
                                       e.dot(f_base[f_g * 2 + 1]));
                    COMPLEX e_f_c_4 = pow(conj(e_f), 4);
                    COMPLEX e_g_c_4 = pow(conj(e_g), 4);

                    if (status[f] == 1 && status[f_g] == 1) continue;
                    if (status[f] == 0) {
                        tris.push_back(
                                Eigen::Triplet<COMPLEX>(
                                        count, id2sln[f], e_f_c_4));
                    } else { b_pre[count] += -e_f_c_4 * f_dir[f]; }
                    if (status[f_g] == 0) {
                        tris.push_back(
                                Eigen::Triplet<COMPLEX>(
                                        count, id2sln[f_g], -e_g_c_4));
                    } else { b_pre[count] += e_g_c_4 * f_dir[f_g]; }
                    // std::cout << "count:" << count << std::endl;
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

    crossfield.resize(4 * f_size);
    for (int i = 0; i < f_size; i++) {
        if (status[i] == 0) { f_dir[i] = x(id2sln[i]); }
        double length = 0.005;
        double arg = std::arg(f_dir[i]) / 4;
        for (int j = 0; j < 4; j++) {
            crossfield[i * 4 + j] =
                    f_base[i * 2] * length * cos(arg + j * M_PI / 2) + f_base[
                        i * 2 + 1] * length *
                    sin(arg + j * M_PI / 2);
        }
    }
    std::vector<float> lines;
    Painter::Pointer painter = model->GetPainter();
    painter->SetPen(1);
    painter->SetPen(Black);
    for(igIndex fid = 0; fid < m_Mesh->GetNumberOfFaces(); fid ++)
    {
        igIndex pids[3];
        m_Mesh->GetFacePointIds(fid, pids);
        Point p1, p2, p3, cp;
        p1 = m_Mesh->GetPoint(pids[0]);
        p2 = m_Mesh->GetPoint(pids[1]);
        p3 = m_Mesh->GetPoint(pids[2]);
        cp = (p1 + p2 + p3) / 3;
        for (int j = 0; j < 4; j++)
            painter->Show(painter->DrawLine(cp, cp + crossfield[fid * 4 + j]));

    }
    //writeCrossField(lines, crossfield);

}

void N_PolyMesh::calculateMeshFaceBase(std::vector<Vector3d>& f_base) {
    auto obj = this->GetInput(0);
    SurfaceMesh::Pointer surface = DynamicCast<SurfaceMesh>(this->GetInput(0));

    m_Mesh = DynamicCast<SurfaceMesh>(GetInput(0));

    surface->RequestEditStatus(); // 开启编辑模式，并生成邻接关系等数据结构
    auto v_size = surface->GetNumberOfPoints();
    auto e_size = surface->GetNumberOfEdges();
    auto f_size = surface->GetNumberOfFaces();

    for (int f = 0; f < f_size; f++) {
        igIndex face_points[64]{};
        auto facePointNum = surface->GetFacePointIds(f, face_points);
        auto p1 = face_points[0];
        auto p2 = face_points[1];
        auto p3 = face_points[2];
        auto v1 = surface->GetPoint(face_points[0]);
        auto v2 = surface->GetPoint(face_points[1]);
        auto v3 = surface->GetPoint(face_points[2]);
        auto normal_ = surface->GetFace(f)->GetNormal();

        auto b1 = (v2 - v1).normalized();
        auto b2 = normal_.cross(b1);
        f_base[f * 2] = b1;
        f_base[f * 2 + 1] = b2;
    }
}

void N_PolyMesh::writeCrossField(std::vector<float>& lines,
                                 std::vector<Vector3d>& crossfield) {
    lines.clear();
    std::cout << "writing crossfield" << std::endl;
    auto obj = this->GetInput(0);
    SurfaceMesh::Pointer surface = DynamicCast<SurfaceMesh>(this->GetInput(0));

    m_Mesh = DynamicCast<SurfaceMesh>(GetInput(0));

    surface->RequestEditStatus(); // 开启编辑模式，并生成邻接关系等数据结构
    Painter::Pointer painter = model->GetPainter();
    // 创建画笔
    Pen::Pointer pen = Pen::New();
    // 设置画笔粗细
    pen->SetWidth(1);
    // 设置画笔颜色
    pen->SetColor(Color::Red);
    // Painter设置
    painter->SetPen(pen);

    auto v_size = surface->GetNumberOfPoints();
    auto e_size = surface->GetNumberOfEdges();
    auto f_size = surface->GetNumberOfFaces();
    int num = crossfield.size();


    for (int i = 0; i < num / 4; i++) {
        // auto center = mesh_.calc_face_centroid(mesh_.face_handle(i));
        igIndex face_points[64]{};
        auto facePointNum = surface->GetFacePointIds(i, face_points);
        auto p1 = surface->GetPoint(face_points[0]);
        auto p2 = surface->GetPoint(face_points[1]);
        auto p3 = surface->GetPoint(face_points[2]);
        Point center = (p1 + p2 + p3) / 3;

        lines.emplace_back(center[0] + crossfield[i * 4][0]);
        lines.emplace_back(center[1] + crossfield[i * 4][1]);
        lines.emplace_back(center[2] + crossfield[i * 4][2]);
        lines.emplace_back(0.0);
        lines.emplace_back(0.0);
        lines.emplace_back(1.0);

        float p1_1 = center[0] + crossfield[i * 4][0];
        float p1_2 = center[1] + crossfield[i * 4][1];
        float p1_3 = center[2] + crossfield[i * 4][2];
        Point p_1{p1_1, p1_2, p1_3};

        // IGuint handle_1 = painter->DrawPoint(Point{p1_1, p1_2, p1_3});
        // 显示该图元
        // painter->Show(handle_1);

        //  auto handle_1_1 =  painter->DrawLine(p_1, center);
        // // 显示该图元
        //  painter->Show(handle_1_1);

        lines.emplace_back(center[0] + crossfield[i * 4 + 2][0]);
        lines.emplace_back(center[1] + crossfield[i * 4 + 2][1]);
        lines.emplace_back(center[2] + crossfield[i * 4 + 2][2]);
        lines.emplace_back(0.0);
        lines.emplace_back(0.0);
        lines.emplace_back(1.0);

        float p2_1 = center[0] + crossfield[i * 4 + 2][0];
        float p2_2 = center[1] + crossfield[i * 4 + 2][1];
        float p2_3 = center[2] + crossfield[i * 4 + 2][2];
        // IGuint handle_2 = painter->DrawPoint(Point{p2_1, p2_2, p2_3});
        Point p_2{p2_1, p2_2, p2_3};
        // auto handle_1_2 =  painter->DrawLine(p_1, p_2);
        //  // 显示该图元
        //  painter->Show(handle_1_2);
        // auto handle_1_2 =  painter->DrawLine(p_2, center);
        auto handle_1_2 = painter->DrawLine(p_2, p_1);
        // 显示该图元
        painter->Show(handle_1_2);

        lines.emplace_back(center[0] + crossfield[i * 4 + 1][0]);
        lines.emplace_back(center[1] + crossfield[i * 4 + 1][1]);
        lines.emplace_back(center[2] + crossfield[i * 4 + 1][2]);
        lines.emplace_back(0.0);
        lines.emplace_back(0.0);
        lines.emplace_back(1.0);
        float p3_1 = center[0] + crossfield[i * 4 + 1][0];
        float p3_2 = center[1] + crossfield[i * 4 + 1][1];
        float p3_3 = center[2] + crossfield[i * 4 + 1][2];
        Point p_3{p3_1, p3_2, p3_3};

        // IGuint handle_3 = painter->DrawPoint(Point{p3_1, p3_2, p3_3});
        // // 显示该图元
        // painter->Show(handle_3);
        //  auto handle_1_3 =  painter->DrawLine(p_3, center);
        // // 显示该图元
        //  painter->Show(handle_1_3);

        lines.emplace_back(center[0] + crossfield[i * 4 + 3][0]);
        lines.emplace_back(center[1] + crossfield[i * 4 + 3][1]);
        lines.emplace_back(center[2] + crossfield[i * 4 + 3][2]);
        lines.emplace_back(0.0);
        lines.emplace_back(0.0);
        lines.emplace_back(1.0);

        float p4_1 = center[0] + crossfield[i * 4 + 3][0];
        float p4_2 = center[1] + crossfield[i * 4 + 3][1];
        float p4_3 = center[2] + crossfield[i * 4 + 3][2];
        Point p_4{p4_1, p4_2, p4_3};

        // IGuint handle_4 = painter->DrawPoint(Point{p4_1, p4_2, p4_3});
        // // 显示该图元
        //
        // auto handle__3_4 = painter->DrawLine(p_3, p_4);
        // painter->Show(handle__3_4);

        auto handle_1_4 = painter->DrawLine(p_4, p_3);
        // auto handle_1_4 =  painter->DrawLine(p_4, center);
        // 显示该图元
        painter->Show(handle_1_4);
        // std::cout << i << std::endl;
        // std::cout << crossfield[i * 4][0] << '\t' << crossfield[i * 4][1] << '\t' << crossfield[i * 4][2] << std::endl;
        // std::cout << crossfield[i * 4 + 1][0] << '\t' << crossfield[i * 4 + 1][1] << '\t' << crossfield[i * 4 + 1][2] << std::endl;
        // std::cout << crossfield[i * 4 + 2][0] << '\t' << crossfield[i * 4 + 2][1] << '\t' << crossfield[i * 4 + 2][2] << std::endl;
        // std::cout << crossfield[i * 4 + 3][0] << '\t' << crossfield[i * 4 + 3][1] << '\t' << crossfield[i * 4 + 3][2] << std::endl << std::endl;
    }
}
}
