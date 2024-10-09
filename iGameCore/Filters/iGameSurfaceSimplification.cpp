#include "iGameSurfaceSimplification.h"
using namespace iGame;


bool SurfaceSimplification::Execute() {
    mesh = DynamicCast<SurfaceMesh>(GetInput(0));
    if (mesh == nullptr) { return false; }
    InitCache();

    //std::cout << mesh->GetNumberOfPoints() << " " << mesh->GetNumberOfEdges()
    //         << " " << mesh->GetNumberOfFaces() << std::endl;

    UpdateProgress(0);
    IGsize times = mesh->GetNumberOfPoints() / 4;
    for (IGsize i = 0; i < times; i++) { 
        step();
        UpdateProgress((float)(i + 1) / (float)times);
    }
    mesh->GarbageCollection();

    //std::cout << mesh->GetNumberOfPoints() << " " << mesh->GetNumberOfEdges()
    //          << " " << mesh->GetNumberOfFaces() << std::endl;
    return true;
}

void SurfaceSimplification::InitCache() 
{
    mesh->RequestEditStatus();
    int PointNum = mesh->GetNumberOfPoints();
    int EdgeNum = mesh->GetNumberOfEdges();
    for (int i = 0; i < PointNum; i++) 
    { 
        isPointDeleted[i] = false; 
        Qmap[i] = GetPointErrorQuadricsByIndex(i);
    }

    for (int i = 0; i < EdgeNum; i++) 
    { 
        igIndex pids[2];
        mesh->GetEdgePointIds(i, pids);
        GeneratePointPairIntoHeapByIndex(pids[0], pids[1]);
    }
}

ErrorQuadrics SurfaceSimplification::GetPointErrorQuadricsByIndex(igIndex pid) {
    igIndex fids[64];
    int fnum = mesh->GetPointToNeighborFaces(pid, fids);
    ErrorQuadrics error = ErrorQuadrics::Zero();
    Point p = mesh->GetPoint(pid);

    for (int i = 0; i < fnum; i++) { 
        Face* face = mesh->GetFace(fids[i]);
        Vector3f normal = face->GetNormal();
        float d = -p.dot(normal);
        Eigen::Vector4f FaceCoeff{normal[0], normal[1], normal[2], d};
        error += FaceCoeff * FaceCoeff.transpose();
    }
    return error;
}

void SurfaceSimplification::GeneratePointPairIntoHeapByIndex(
        igIndex pid1,
    igIndex pid2)
{
    Point pos1 = mesh->GetPoint(pid1);
    Point pos2 = mesh->GetPoint(pid2);
    Vector3f ContractPoint;

    ErrorQuadrics Q_bar = Qmap[pid1] + Qmap[pid2];
    ErrorQuadrics condition = Q_bar;
    ErrorQuadrics Inverse;
    Eigen::Vector4f b{0, 0, 0, 1.0f};
    Eigen::Vector4f v_bar;

    float cost;
    PointPair pair;

    condition(3, 0) = 0;
    condition(3, 1) = 0;
    condition(3, 2) = 0;
    condition(3, 3) = 1.0f;
    bool isInversible = false;
    condition.computeInverseWithCheck(Inverse, isInversible);
    if (isInversible) {
        v_bar = Inverse * b;
        ContractPoint[0] = v_bar[0];
        ContractPoint[1] = v_bar[1];
        ContractPoint[2] = v_bar[2];
    }
    else
    {
        ContractPoint = (pos1 + pos2) / 2;
        v_bar << ContractPoint[0], ContractPoint[1], ContractPoint[2],
                1.0f;
    }
    
    pair.pid1 = pid1;
    pair.pid2 = pid2;
    pair.cost = v_bar.transpose() * Q_bar * v_bar;
    pair.ContractPoint = ContractPoint;
    PairHeap.push(pair);
}

void SurfaceSimplification::ContractPointPair(const PointPair& pair) 
{
    igIndex AdjPids[64];
    igIndex AdjEids[64];
    int esize = 0;
    int psize = 0;
    igIndex ContractIndex = -1;

    ContractIndex = mesh->AddPoint(pair.ContractPoint);
    //std::cout << "New point: " << ContractIndex << std::endl;
    Qmap[ContractIndex] = Qmap[pair.pid1] + Qmap[pair.pid2];
    isPointDeleted[ContractIndex] = false;
    
    assert(ContractIndex != -1);
    ReplaceTopology(ContractIndex, pair.pid1);
    ReplaceTopology(ContractIndex, pair.pid2);

    esize = mesh->GetPointToNeighborEdges(ContractIndex, AdjEids);
    for (int i = 0; i < esize; i++)
    { 
        igIndex pids[2];
        mesh->GetEdgePointIds(AdjEids[i], pids);
        GeneratePointPairIntoHeapByIndex(pids[0], pids[1]);
    }
}

void SurfaceSimplification::step()
{
    PointPair pair;
    
    do 
    { 
        pair = PairHeap.top();
        PairHeap.pop();
    } while (isPointDeleted[pair.pid1] || isPointDeleted[pair.pid2]);
    ContractPointPair(pair);
}

void SurfaceSimplification::ReplaceTopology(igIndex newpid, igIndex oldpid)
{
    igIndex fids[64];
    int fnum = 0;

    fnum = mesh->GetPointToNeighborFaces(oldpid, fids);
    for (int i = 0; i < fnum; i++)
    {
        igIndex pids[3];
        igIndex newfpids[3];
        igIndex newfid = -1;
        int idx = 0;
        int psize = 0;
        psize = mesh->GetFacePointIds(fids[i], pids);
        assert(psize == 3);
        while (pids[idx] != oldpid) idx++;
        newfpids[0] = newpid;
        newfpids[1] = pids[(idx + 1) % 3];
        newfpids[2] = pids[(idx + 2) % 3];
        if (newfpids[1] != newpid && newfpids[2] != newpid)
        {
            newfid = mesh->AddFace(newfpids, 3);
            //std::cout << "New face: "
            //          << "(" << newfpids[0] << "," << newfpids[1] << ","
            //          << newfpids[2] << " )" << std::endl;
        }

    }
    mesh->DeletePoint(oldpid);
    isPointDeleted[oldpid] = true;
}