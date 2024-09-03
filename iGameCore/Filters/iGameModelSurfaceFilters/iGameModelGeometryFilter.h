#ifndef iGameModelGeometryFilter_h
#define iGameModelGeometryFilter_h

#include "iGameDataObject.h"
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameVolumeMesh.h"
#include "iGameUnstructuredMesh.h"
IGAME_NAMESPACE_BEGIN

class StructuredGrid : public DataObject {
public:
    I_OBJECT(StructuredGrid);
    int GetDimension() { return 3; };
};
class PolyData : public SurfaceMesh {
public:
    I_OBJECT(PolyData);
    static Pointer New() { return new PolyData; };
    IGsize GetNumberOfCells() { return this->GetNumberOfFaces(); }
    PolyData(){};
};
class iGameModelGeometryFilter : public Filter {
public:
    I_OBJECT(iGameModelGeometryFilter);
    ///@{
    /**
     * Standard methods for instantiation, type information, and printing.
     */
    static iGameModelGeometryFilter* New() {
        return new iGameModelGeometryFilter;
    };
    ~iGameModelGeometryFilter();
    bool Execute() override;
    bool Execute(DataObject*);
    bool Execute(DataObject*, PolyData*);
    /**
     * Specify a (xmin,xmax, ymin,ymax, zmin,zmax) bounding box to clip data.
     */
    void SetExtent(double xMin, double xMax, double yMin, double yMax,
                   double zMin, double zMax);
    void SetExtent(double ex[6]);
    ///@{
    /**
     * Direct access methods so that this class can be used as an
     * algorithm without using it as a filter (i.e., no pipeline updates).
     * Also some internal methods with additional options.
     */
    int ExecuteWithPolyData(DataObject::Pointer input, PolyData* output,
                            PolyData* exc);
    virtual int ExecuteWithPolyData(DataObject::Pointer, PolyData*);

    int ExecuteWithUnstructuredGrid(DataObject::Pointer input, PolyData* output,
                                    PolyData* exc);
    virtual int ExecuteWithUnstructuredGrid(DataObject::Pointer input,
                                            PolyData* output);

    int ExecuteWithStructuredGrid(DataObject::Pointer input, PolyData* output,
                                  PolyData* exc, bool* extractFace = nullptr);
    virtual int ExecuteWithStructuredGrid(DataObject::Pointer input,
                                          PolyData* output,
                                          bool* extractFace = nullptr);

    int ExecuteWithDataSet(DataObject::Pointer input, PolyData* output,
                           PolyData* exc);
    virtual int ExecuteWithDataSet(DataObject::Pointer input, PolyData* output);
    ///@}
    void SetInput(DataObject::Pointer ip) { this->input = ip; }
    PolyData::Pointer GetOutPut() { return this->output; }

    void CompositeAttribute(std::vector<igIndex>& f2c,AttributeSet* inAllDataArray,AttributeSet* outAllDataArray);


protected:
    iGameModelGeometryFilter();
    //通常在文件里会有标注表面信息，如果有则不需要这边运算，
    //只需要把attribute的信息copy一份给表面就可以
    PolyData::Pointer excFaces;
    DataObject::Pointer input;
    PolyData::Pointer output;
    igIndex PointMaximum;
    igIndex PointMinimum;
    igIndex CellMinimum;
    igIndex CellMaximum;
    double Extent[6];
    bool PointClipping;
    bool CellClipping;
    bool ExtentClipping;
    int OutputPointsPrecision;
    bool RemoveGhostInterfaces;

    bool Merging;

    bool FastMode;

    // These methods support compatibility with iGameDataObject::PointerSurfaceFilter
    int PieceInvariant;
    bool PassThroughCellIds;
    char* OriginalCellIdsName;

    bool PassThroughPointIds;
    char* OriginalPointIdsName;

    int NonlinearSubdivisionLevel;

    bool Delegation;
  

private:
};
IGAME_NAMESPACE_END
#endif
