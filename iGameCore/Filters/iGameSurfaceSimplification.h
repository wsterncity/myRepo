#ifndef iGameSurfaceSimplification_h
#define iGameSurfaceSimplification_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <queue>
#include "Eigen/dense"
IGAME_NAMESPACE_BEGIN
using ErrorQuadrics = Eigen::Matrix4f;

struct PointPair {
igIndex pid1;
igIndex pid2;
Point ContractPoint;
float cost;
};

struct PairGreater
{
    bool operator()(const PointPair& l, const PointPair& r) { return l.cost > r.cost; }
};

class SurfaceSimplification : public Filter {
public:
    I_OBJECT(SurfaceSimplification);
    static Pointer New() { return new SurfaceSimplification; }
    bool Execute() override;


protected:
    SurfaceSimplification() {
        SetNumberOfInputs(1);
        SetNumberOfOutputs(1);
    }

    ~SurfaceSimplification() override = default;

    std::priority_queue<PointPair, std::vector<PointPair>, PairGreater>
            PairHeap;
    std::map<int, ErrorQuadrics> Qmap;
    std::map<int, bool> isPointDeleted;
    SurfaceMesh::Pointer mesh;

    void InitCache();
    ErrorQuadrics GetPointErrorQuadricsByIndex(igIndex pid);
    void GeneratePointPairIntoHeapByIndex(igIndex pid1, igIndex pid2);
    void ContractPointPair(const PointPair& pair);
    void step();
    void ReplaceTopology(igIndex newpid, igIndex oldpid);
};
IGAME_NAMESPACE_END

#endif // !SurfaceSimplification_H
