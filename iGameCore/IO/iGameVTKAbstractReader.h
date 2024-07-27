#ifndef iGameVTKAbstractReader_h
#define iGameVTKAbstractReader_h

#include "iGameFileReader.h"

#define POINT_TYPE 1
#define CELL_TYPE 2
IGAME_NAMESPACE_BEGIN

class VTKAbstractReader : public FileReader {
public:
	I_OBJECT(VTKAbstractReader);

	//virtual void ParseBufferWithFileType() = 0;
	//virtual void GenerateBufferWithFileType() = 0;

	enum VTKTYPE {
		T0,
		VERTEX = 1,
		POLYVERTEX = 2,
		LINE = 3,
		POLYLINE = 4,
		TRIANGLE = 5,
		TRIANGLESTRIP = 6,
		POLYGON = 7,
		PIXEL = 8,
		QUAD = 9,
		TETRA = 10,
		VOXEL = 11,
		HEXAHEDRON = 12,
		WEDGE = 13,
		PYRAMID = 14,
		PENTAGONAL_PRISM = 15,
		HEXAGONAL_PRISM = 16,
		T17,
		T18,
		T19,
		T20,
		QUADRATIC_EDGE = 21,
		QUADRATIC_TRIANGLE = 22,
		QUADRATIC_QUAD = 23,
		QUADRATIC_TETRA = 24,
		QUADRATIC_HEXAHEDRON = 25,
		QUADRATIC_WEDGE = 26,
		QUADRATIC_PYRAMID = 27,
		BIQUADRATIC_QUAD = 28,
		TRIQUADRATIC_HEXAHEDRON = 29,
		QUADRATIC_LINEAR_QUAD = 30,
		QUADRATIC_LINEAR_WEDGE = 31,
		BIQUADRATIC_QUADRATIC_WEDGE = 32,
		BIQUADRATIC_QUADRATIC_HEXAHEDRON = 33,
		BIQUADRATIC_TRIANGLE = 34,
		CUBIC_LINE = 35,
		QUADRATIC_POLYGON = 36,
		TRIQUADRATIC_PYRAMID = 37,
	};

	const int vtkLegacyReaderMajorVersion = 5;
	const int vtkLegacyReaderMinorVersion = 1;
	int ReadHeader();
	int ReadPointCoordinates(Points::Pointer ps, int numPts);

	const void TransferVtkCellToiGameCell(IntArray::Pointer, IntArray::Pointer);
	const void TransferVtkCellToiGameCell(ArrayObject::Pointer, ArrayObject::Pointer, IntArray::Pointer VtkCellsType);

	static void TransferVtkCellToiGameCell(DataCollection& m_Data, ArrayObject::Pointer, ArrayObject::Pointer, ArrayObject::Pointer VtkCellsType);
	int ReadPointData(int numPts);
	int ReadCellData(int numCells);

	int ReadFieldData();
	int ReadScalarData(int num);
	int ReadVectorData(int num);
	int ReadNormalData(int num);
	int ReadTensorData(int num, int numComp = 9);
	int ReadCoScalarData(int num);
	int ReadLutData();
	int ReadTCoordsData(int num);
	int ReadGlobalIds(int num);
	int ReadPedigreeIds(int num);
	int ReadEdgeFlags(int num);

protected:
	VTKAbstractReader();
	~VTKAbstractReader() override;

	int FileVersion;
	int FileMajorVersion;
	int FileMinorVersion;
	char* Header;
	int DataType = POINT_TYPE;
};

IGAME_NAMESPACE_END
#endif