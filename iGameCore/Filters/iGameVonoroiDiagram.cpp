#include "iGameVonoroiDiagram.h"

using namespace iGame;

void VonoroiDiagram::AddTriangle(igIndex pid1, igIndex pid2, igIndex pid3) {
    float _radius;
    Vector2f _circumcenter;
    Point p1, p2, p3;

    //Calculate circumcenter
    p1 = pointCluster->GetPoint(pid1);
    p2 = pointCluster->GetPoint(pid2);
    p3 = pointCluster->GetPoint(pid3);
    float d1 = powf(p2[0], 2) + powf(p2[1], 2) - powf(p1[0], 2) +
               powf(p1[1], 2);
    float d2 = powf(p3[0], 2) + powf(p3[1], 2) - powf(p2[0], 2) +
               powf(p2[1], 2);
    float fm = 2 * ((p3[1] - p2[1]) * (p2[0] - p1[0]) -
                    (p2[1] - p1[1]) * (p3[0] - p2[0]));
    _circumcenter[0] =
            ((p3[1] - p2[1]) * d1 - (p2[1] - p1[1]) * d2) / fm;
    _circumcenter[1] =
            ((p2[0] - p1[0]) * d2 - (p3[0] - p2[0]) * d1) / fm;

    _radius = (_circumcenter - p1).norm();

    DelaunayTriangle *t = new DelaunayTriangle;
    t->pid1 = pid1;
    t->pid2 = pid2;
    t->pid3 = pid3;
    t->circumCenter = _circumcenter;
    t->e1 = DelaunayEdge{pid1, pid2};
    t->e2 = DelaunayEdge{pid1, pid3};
    t->e3 = DelaunayEdge{pid2, pid3};
    t->radius = _radius;
    t->next = DelaunayTriangleList->next;
    DelaunayTriangleList->next = t;
}


bool VonoroiDiagram::isInCircumCircle(igIndex pid, DelaunayTriangle *t) {
    Point p = pointCluster->GetPoint(pid);
    Vector2f p2d{p[0], p[1]};
    if ((p2d - t->circumCenter).norm() < t->radius)
        return true;
    else
        return false;
}

bool VonoroiDiagram::isBoundaryTriangle(DelaunayTriangle* t) {
    auto it1 = std::find(bbox.begin(), bbox.end(), t->pid1);
    auto it2 = std::find(bbox.begin(), bbox.end(), t->pid2);
    auto it3 = std::find(bbox.begin(), bbox.end(), t->pid3);
    if (it1 == bbox.end() || it2 == bbox.end() ||
        it3 == bbox.end())
        return true;
    else
        return false;
}


VonoroiDiagram::VonoroiDiagram() {
    pointCluster = Points::New();
    DelaunayTriangleList = new DelaunayTriangle;
    DelaunayTriangleList->next = nullptr;
    numOfInsertPoints = 0;

    SetNumberOfInputs(0);
    SetNumberOfOutputs(1);
    bbox.push_back(Point_2(0, 0));
    bbox.push_back(Point_2(1, 0));
    bbox.push_back(Point_2(1, 1));
    bbox.push_back(Point_2(0, 1));
    /*
    bbox.push_back(upLeft);
    bbox.push_back(upRight);
    bbox.push_back(downLeft);
    bbox.push_back(downRight);
    */

}

bool VonoroiDiagram::Execute()
{

    DT dt;
    std::vector<Point_2> points;
    GenerateRandomPointsInSquare(points, numOfInsertPoints, 0, 1.0f);
    dt.insert(points.begin(), points.end());
    VD voronoi(dt);
    // 裁剪Voronoi边到正方形区域内
    std::vector<Segment_2> clipped_edges = clip_voronoi_edges(voronoi);
    /*
    pointCluster->Initialize();
    pointCluster->AddPoint(1.5f, 3.0f, 0);
    pointCluster->AddPoint(-1.5f, -1.0f, 0);
    pointCluster->AddPoint(4.5f, -1.0f, 0);
    GenerateRandomPointsInSquare(numOfInsertPoints, 0, 1.0f);
    InitializeTriangleList();
    SetDelaunayTriangleList();
    */
    //InitVonoroiMap();
    SetSurfceMesh();
    SetOutput(mesh);
    return true;
}

void VonoroiDiagram::GenerateRandomPointsInSquare(std::vector<Point_2>& container, int num, float rangeLow, float rangeHigh)
{
    std::default_random_engine engine;
    std::uniform_real_distribution<float> distrib{rangeLow, rangeHigh};
    for (int i = 0; i < num; i++) 
        container.push_back(Point_2{distrib(engine), distrib(engine)});
}

void VonoroiDiagram::InitializeTriangleList()
{
    DelaunayTriangle* p = DelaunayTriangleList->next;
    while (p) 
    { 
        DelaunayTriangleList->next = p->next;
        delete p;
        p = DelaunayTriangleList->next;
    }
    AddTriangle(0, 1, 2);
}

void VonoroiDiagram::SetDelaunayTriangleList() 
{
    std::vector<DelaunayEdge> EdgeArr;
    int psize = pointCluster->GetNumberOfPoints();
    DelaunayTriangle* iter = DelaunayTriangleList->next;
    DelaunayTriangle* preIter = DelaunayTriangleList;
    //Insert every point into delaunay diagram with Bowyer-Watson algorithm.

    for (igIndex i = 3; i < psize; i++)
    {
        iter = DelaunayTriangleList->next;
        preIter = DelaunayTriangleList;
        EdgeArr.clear();
        while (iter)
        {
            if (isInCircumCircle(i, iter)) 
            {
                EdgeArr.push_back(iter->e1);
                EdgeArr.push_back(iter->e2);
                EdgeArr.push_back(iter->e3);
                preIter->next = iter->next;
                delete iter;
                iter = preIter->next;
            } 
            else 
            {
                preIter = iter;
                iter = iter->next;
            }
            
        }

        int esize = EdgeArr.size();

        for (int j = 0; j < esize; j++)
        {
            auto edge1 = EdgeArr[j];
            int k = 0;
            for (; k < esize; k++)
            { 
                if (j == k) continue;
                auto edge2 = EdgeArr[k];
                if (edge1 == edge2)
                    break;
            }
            if (k == esize)
                AddTriangle(i, edge1.pid1, edge1.pid2);
        }



    }
}

void VonoroiDiagram::InitVonoroiMap()
{

}

void VonoroiDiagram::SetSurfceMesh()
{
    mesh = SurfaceMesh::New();
    CellArray::Pointer faces = CellArray::New();
    DelaunayTriangle* iter = DelaunayTriangleList->next;
    while (iter) { 
        faces->AddCellId3(iter->pid1, iter->pid2, iter->pid3);
        iter = iter->next;
    }
    
    mesh->SetPoints(pointCluster);
    mesh->SetFaces(faces);
    mesh->SetName("voronoi_mesh");
}

// 检查并裁剪Voronoi边
std::vector<Polygon_2> clip_voronoi_cells(const VD& voronoi) {
    std::vector<Polygon_2> clipped_cells;

    // 遍历Voronoi的每个单元
    for (auto cell = voronoi.cells_begin(); cell != voronoi.cells_end();
         ++cell) {
        if (cell->is_unbounded()) {
            continue; // 忽略无限单元
        }

        Polygon_2 cell_polygon;
        for (auto edge = cell->edges_begin(); edge != cell->edges_end();
             ++edge) {
            if (edge->has_source() && edge->has_target()) {
                Segment_2 voronoi_edge(edge->source()->point(),
                                       edge->target()->point());
                cell_polygon.push_back(voronoi_edge.source());
            }
        }

        // 裁剪Voronoi单元
        Polygon_2 clipped_cell;
        CGAL::intersection(cell_polygon, bbox,
                           std::back_inserter(clipped_cell));

        if (!clipped_cell.is_empty()) { clipped_cells.push_back(clipped_cell); }
    }

    return clipped_cells;
}