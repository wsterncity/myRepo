#ifndef iGameVonoroiDiagram_h
#define iGameVonoroiDiagram_h

#include <map>
#include <random>
#include <ctime>
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameElementArray.h"
// includes for defining the Voronoi diagram adaptor
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_set_2.h>

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT> AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT, AT, AP> VD;

typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Segment_2<K> Segment_2;
typedef K::Point_2 Point_2;

IGAME_NAMESPACE_BEGIN

class VonoroiDiagram : public Filter
{
public:
    I_OBJECT(VonoroiDiagram);
    static Pointer New() { return new VonoroiDiagram; }
    bool Execute() override;
    void SetNumOfPoints(int num = 1) { numOfInsertPoints = num; }
    
private:
    struct DelaunayEdge
    {
        igIndex pid1, pid2;
        bool operator ==(const DelaunayEdge& e) const
        {
            if (pid1 == e.pid1 && pid2 == e.pid2) return true;
            else if (pid2 == e.pid1 && pid1 == e.pid2)
                return true;
            else
                return false;
        }
    };

    struct DelaunayTriangle
    {
        igIndex pid1, pid2, pid3;
        DelaunayEdge e1, e2, e3;
        Vector2f circumCenter;
        float radius;
        DelaunayTriangle* next;
    };


    VonoroiDiagram();
    ~VonoroiDiagram() override = default;
    DelaunayTriangle* DelaunayTriangleList;
    Polygon_2 bbox;
    std::vector<Vector2f> circumCenterList;
    std::vector<float> radiusList;
    //std::unordered_map<igIndex, IntArray> Pid2VonoroiFacePidsMap;
    SurfaceMesh::Pointer mesh;
    Points::Pointer pointCluster;
    int numOfInsertPoints;

    bool isInCircumCircle(igIndex pid, DelaunayTriangle* t);
    bool isBoundaryTriangle(DelaunayTriangle* t);
    void AddTriangle(igIndex pid1, igIndex pid2, igIndex pid3);
    void SetDelaunayTriangleList();
    void InitVonoroiMap();
    void SetSurfceMesh();
    void GenerateRandomPointsInSquare(std::vector<Point_2>& container, int num,
                                      float rangeLow, float rangeHigh);
    void InitializeTriangleList();
    std::vector<Segment_2> clip_voronoi_edges(const VD& voronoi);
};


IGAME_NAMESPACE_END

#endif // !iGameVonoroiGeneration_h
