#include "iGameVTKAbstractReader.h"
IGAME_NAMESPACE_BEGIN

VTKAbstractReader::VTKAbstractReader()
{
	this->Header = nullptr;
}
VTKAbstractReader::~VTKAbstractReader()
{
	if (this->Header) delete[] this->Header;
}

int VTKAbstractReader::ReadHeader()
{
	char line[256];
	igDebug("Reading vtk file header");
	//
	// read header
	//
	if (!this->ReadLine(line)) {
		igError("Premature EOF reading first line! ");
		return 0;
	}
	const int VERSION_PREFIX_LENGTH = 22;
	if (strncmp("# vtk DataFile Version", line, VERSION_PREFIX_LENGTH) != 0) {
		igError("Unrecognized file type! ");
		return 0;
	}
	if (sscanf(line + VERSION_PREFIX_LENGTH, "%d.%d", &this->FileMajorVersion,
		&this->FileMinorVersion) != 2) {
		igDebug("Cannot read file version:!");
		this->FileMajorVersion = 0;
		this->FileMinorVersion = 0;
	}
	if (this->FileMajorVersion > vtkLegacyReaderMajorVersion ||
		(this->FileMajorVersion == vtkLegacyReaderMajorVersion &&
			this->FileMinorVersion > vtkLegacyReaderMinorVersion)) {
		// newer file than the reader version
		igDebug("Reading file version: " << this->FileMajorVersion << "."
			<< this->FileMinorVersion << " with older reader version "
			<< vtkLegacyReaderMajorVersion << "." << vtkLegacyReaderMinorVersion);
	}
	// Compose FileVersion
	this->FileVersion = 10 * this->FileMajorVersion + this->FileMinorVersion;

	//
	// read title
	//
	if (!this->ReadLine(line)) {
		igError("Premature EOF reading title! ");
		return 0;
	}
	//delete[] this->Header;
	this->Header = new char[strlen(line) + 1];
	strcpy(this->Header, line);

	igDebug("Reading vtk file entitled!" << line);
	//
	// read type
	//
	if (!this->ReadString(line)) {
		//igError("Premature EOF reading file type!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}

	if (!strncmp(this->LowerCase(line), "ascii", 5)) {
		m_FileType = IGAME_ASCII;
	}
	else if (!strncmp(line, "binary", 6)) {
		m_FileType = IGAME_BINARY;
	}
	else {
		//igError("Unrecognized file type: " << line
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		m_FileType = 0;
		return 0;
	}
	return 1;
}


//------------------------------------------------------------------------------
// Read point coordinates. Return 0 if error.
int VTKAbstractReader::ReadPointCoordinates(Points::Pointer points, int PointsNum)
{
	char line[256];
	FloatArray::Pointer data;
	if (!this->ReadString(line)) {
		//igError("Cannot read points type!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}

	data = DynamicCast<FloatArray>(this->ReadArray(line, PointsNum, 3));
	if (data != nullptr) {
		//points->SetArray(data->RawPointer(), PointsNum);
		float p[3]{};
		for (int i = 0; i < PointsNum; i++) {
			data->GetElement(i, p);
			points->AddPoint(p);
		}
	}
	else {
		return 0;
	}
	igDebug("Read " << points->GetNumberOfPoints() << " points");
	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read the cell data of a vtk data file. The number of cells (from the
// dataset) must match the number of cells defined in cell attributes (unless
// no geometry was defined).
int VTKAbstractReader::ReadCellData(int CellsNum)
{
	char line[256];
	this->DataType = CELL_TYPE;
	igDebug("Reading vtk cell data");
	//
	// Read keywords until end-of-file
	//
	while (this->ReadString(line)) {
		if (line[0] == '\0') {
			continue;
		}
		//
		// read scalar data
		//
		if (!strncmp(this->LowerCase(line), "scalars", 7)) {
			if (!this->ReadScalarData(CellsNum)) {
				return 0;
			}
		}
		//
		// read vector data
		//
		else if (!strncmp(line, "vectors", 7)) {
			if (!this->ReadVectorData(CellsNum)) {
				return 0;
			}
		}
		//
		// read 3x2 symmetric tensor data
		//
		else if (!strncmp(line, "tensors6", 8)) {
			if (!this->ReadTensorData(CellsNum, 6)) {
				return 0;
			}
		}
		//
		// read 3x3 tensor data
		//
		else if (!strncmp(line, "tensors", 7)) {
			if (!this->ReadTensorData(CellsNum)) {
				return 0;
			}
		}
		//
		// read normals data
		//
		else if (!strncmp(line, "normals", 7)) {
			if (!this->ReadNormalData(CellsNum)) {
				return 0;
			}
		}
		//
		// read texture coordinates data
		//
		else if (!strncmp(line, "texture_coordinates", 19)) {
			if (!this->ReadTCoordsData(CellsNum)) {
				return 0;
			}
		}
		//
		// read the global id data
		//
		else if (!strncmp(line, "global_ids", 10)) {
			if (!this->ReadGlobalIds(CellsNum)) {
				return 0;
			}
		}
		//
		// read the pedigree id data
		//
		else if (!strncmp(line, "pedigree_ids", 12)) {
			if (!this->ReadPedigreeIds(CellsNum)) {
				return 0;
			}
		}
		//
		// read color scalars data
		//
		else if (!strncmp(line, "color_scalars", 13)) {
			if (!this->ReadCoScalarData(CellsNum)) {
				return 0;
			}
		}
		//
		// read lookup table. Associate with scalar data.
		//
		else if (!strncmp(line, "lookup_table", 12)) {
			if (!this->ReadLutData()) {
				return 0;
			}
		}
		//
		// read field of data
		//
		else if (!strncmp(line, "field", 5)) {
			this->ReadFieldData();
		}
		//
		// maybe bumped into point data
		//
		else if (!strncmp(line, "point_data", 10)) {
			int npts;
			if (!this->Read(&npts)) {
				//igError("Cannot read point data!");
				return 0;
			}
			this->ReadPointData(npts);
			break;
		}
		else {
			//igError("Unsupported cell attribute type: " << line
			//	<< " for file: " << (fname ? fname : "(Null FileName)"));
			return 0;
		}
	}
	this->UpdateReadProgress();
	return 1;
}


//------------------------------------------------------------------------------
// Read the point data of a vtk data file. The number of points (from the
// dataset) must match the number of points defined in point attributes (unless
// no geometry was defined).
int VTKAbstractReader::ReadPointData(int PointsNum)
{
	char line[256];
	PropertySet::Pointer PointData = m_Data.GetData();
	
	this->DataType = POINT_TYPE;
	igDebug("Reading vtk point data");
	//
	// Read keywords until end-of-file
	//
	while (this->ReadString(line)) {
		if (line[0] == '\0') {
			continue;
		}
		//
		// read scalar data
		//
		if (!strncmp(this->LowerCase(line), "scalars", 7)) {
			if (!this->ReadScalarData(PointsNum)) {
				return 0;
			}
		}
		//
		// read vector data
		//
		else if (!strncmp(line, "vectors", 7)) {
			if (!this->ReadVectorData(PointsNum)) {
				return 0;
			}
		}
		//
		// read 3x2 symmetric tensor data
		//
		else if (!strncmp(line, "tensors6", 8)) {
			if (!this->ReadTensorData(PointsNum, 6)) {
				return 0;
			}
		}
		//
		// read 3x3 tensor data
		//
		else if (!strncmp(line, "tensors", 7)) {
			if (!this->ReadTensorData(PointsNum)) {
				return 0;
			}
		}
		//
		// read normals data
		//
		else if (!strncmp(line, "normals", 7)) {
			if (!this->ReadNormalData(PointsNum)) {
				return 0;
			}
		}
		//
		// read texture coordinates data
		//
		else if (!strncmp(line, "texture_coordinates", 19)) {
			if (!this->ReadTCoordsData(PointsNum)) {
				return 0;
			}
		}
		//
		// read the global id data
		//
		else if (!strncmp(line, "global_ids", 10)) {
			if (!this->ReadGlobalIds(PointsNum)) {
				return 0;
			}
		}
		//
		// read the pedigree id data
		//
		else if (!strncmp(line, "pedigree_ids", 12)) {
			if (!this->ReadPedigreeIds(PointsNum)) {
				return 0;
			}
		}
		//
		// read the edge flags data
		//
		else if (!strncmp(line, "edge_flags", 10)) {
			if (!this->ReadEdgeFlags(PointsNum)) {
				return 0;
			}
		}
		//
		// read color scalars data
		//
		else if (!strncmp(line, "color_scalars", 13)) {
			if (!this->ReadCoScalarData(PointsNum)) {
				return 0;
			}
		}
		//
		// read lookup table. Associate with scalar data.
		//
		else if (!strncmp(line, "lookup_table", 12)) {
			if (!this->ReadLutData()) {
				return 0;
			}
		}
		//
		// read field of data
		//
		else if (!strncmp(line, "field", 5))
		{
			this->ReadFieldData();
		}
		//
		// maybe bumped into cell data
		//
		else if (!strncmp(line, "cell_data", 9)) {
			int ncells;
			if (!this->Read(&ncells)) {
				igError("Cannot read cell data!");
				return 0;
			}
			this->ReadCellData(ncells);
			break;
		}
		else {
			//igError("Unsupported point attribute type: " << line
			//	<< " for file: " << (fname ? fname : "(Null FileName)"));
			return 0;
		}
	}
	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read scalar point attributes. Return 0 if error.
int VTKAbstractReader::ReadScalarData(int PointsNum)
{
	char line[256], name[256], key[256], tableName[256];
	int numComp = 1;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		/*igError("Cannot read scalar header!" << " for file: " << (fname ? fname : "(Null FileName)"));*/
		return 0;
	}
	this->DecodeString(name, buffer);

	if (!this->ReadString(key)) {
		//igError("Cannot read scalar header!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}

	// the next string could be an integer number of components or a lookup table
	if (strcmp(this->LowerCase(key), "lookup_table") != 0) {
		numComp = atoi(key);
		if (numComp < 1 || !this->ReadString(key))
		{
			//igError("Cannot read scalar header!"
			//	<< " for file: " << (fname ? fname : "(Null FileName)"));
			return 0;
		}
	}

	if (strcmp(this->LowerCase(key), "lookup_table") != 0) {
		//igError("Lookup table must be specified with scalar.\n"
		//	<< "Use \"LOOKUP_TABLE default\" to use default table.");
		return 0;
	}

	if (!this->ReadString(tableName)) {
		//igError("Cannot read scalar header!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}

	// Read the data
	auto data = this->ReadArray(line, PointsNum, numComp);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			m_Data.GetData()->AddScalars(IG_POINT, data);
		}
		else if (this->DataType == CELL_TYPE) {
			m_Data.GetData()->AddScalars(IG_CELL, data);
		}
	}
	else {
		return 0;
	}
	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read vector point attributes. Return 0 if error.
int VTKAbstractReader::ReadVectorData(int PointsNum)
{
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		//igError("Cannot read vector data!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);


	data = this->ReadArray(line, PointsNum, 3);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			m_Data.GetData()->AddProperty(IG_VECTOR, IG_POINT, data);
		}
		else if (this->DataType == CELL_TYPE) {
			m_Data.GetData()->AddProperty(IG_VECTOR, IG_CELL, data);
		}
	}
	else {
		return 0;
	}
	this->UpdateReadProgress();

	return 1;
}

//------------------------------------------------------------------------------
// Read normal point attributes. Return 0 if error.
int VTKAbstractReader::ReadNormalData(int PointsNum)
{
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		//igError("Cannot read normal data!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);

	data = this->ReadArray(line, PointsNum, 3);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			m_Data.GetData()->AddProperty(IG_NORMAL, IG_POINT, data);
		}
		else if (this->DataType == CELL_TYPE) {
			m_Data.GetData()->AddProperty(IG_NORMAL, IG_CELL, data);
		}
	}
	else {
		return 0;
	}

	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read tensor point attributes. Return 0 if error.
int VTKAbstractReader::ReadTensorData(int PointsNum, int numComp)
{
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		//igError("Cannot read tensor data!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);

	data = this->ReadArray(line, PointsNum, numComp);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			m_Data.GetData()->AddProperty(IG_TENSOR, IG_POINT, data);
		}
		else if (this->DataType == CELL_TYPE) {
			m_Data.GetData()->AddProperty(IG_TENSOR, IG_CELL, data);
		}
	}
	else {
		return 0;
	}
	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read color scalar point attributes. Return 0 if error.
int VTKAbstractReader::ReadCoScalarData(int PointsNum)
{
	int i, j, idx, numComp = 0;
	char name[256];
	char buffer[256];

	if (!(this->ReadString(buffer) && this->Read(&numComp))) {
		//igError("Cannot read color scalar data!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);

	// handle binary different from ASCII since they are stored
	// in a different format float versus uchar
	if (m_FileType == IGAME_BINARY) {
		ArrayObject::Pointer data;
		char type[14] = "unsigned_char";
		data = this->ReadArray(type, PointsNum, numComp);
		if (data != nullptr) {
			data->SetName(name);
			if (this->DataType == POINT_TYPE) {
				m_Data.GetData()->AddProperty(IG_SCALAR, IG_POINT, data);
			}
			else if (this->DataType == CELL_TYPE) {
				m_Data.GetData()->AddProperty(IG_SCALAR, IG_CELL, data);
			}
		}
		else {
			return 0;
		}
	}
	else {
		ArrayObject::Pointer data;
		char type[6] = "float";
		data = this->ReadArray(type, PointsNum, numComp);
		if (data != nullptr) {
			UnsignedCharArray::Pointer scalars = UnsignedCharArray::New();
			scalars->SetElementSize(numComp);
			scalars->Resize(PointsNum);
			scalars->SetName(name);
			for (i = 0; i < PointsNum; i++) {
				for (j = 0; j < numComp; j++) {
					idx = i * numComp + j;
					scalars->SetValue(idx, (unsigned char)(255.0 * data->GetValue(idx) + 0.5));
				}
			}
			if (this->DataType == POINT_TYPE) {
				m_Data.GetData()->AddProperty(IG_SCALAR, IG_POINT, scalars);
			}
			else if (this->DataType == CELL_TYPE) {

			}
		}
		else {
			return 0;
		}
	}

	this->UpdateReadProgress();

	return 1;
}

//------------------------------------------------------------------------------
// Read texture coordinates point attributes. Return 0 if error.
int VTKAbstractReader::ReadTCoordsData(int PointsNum)
{
	int dim = 0;
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->Read(&dim) && this->ReadString(line))) {
		//igError("Cannot read texture data!"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);

	if (dim < 1 || dim > 3) {
		//igError("Unsupported texture coordinates dimension: " << dim
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}


	data = this->ReadArray(line, PointsNum, dim);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			m_Data.GetData()->AddProperty(IG_TCOORD, IG_POINT, data);
		}
		else if (this->DataType == CELL_TYPE) {
			m_Data.GetData()->AddProperty(IG_TCOORD, IG_CELL, data);
		}
	}
	else {
		return 0;
	}
	this->UpdateReadProgress();

	return 1;
}

//------------------------------------------------------------------------------
// Read texture coordinates point attributes. Return 0 if error.
int VTKAbstractReader::ReadGlobalIds(int PointsNum)
{
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		//igError("Cannot read global id data"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);
	data = this->ReadArray(line, PointsNum, 1);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			//this->DataSet->GetPointData()->SetGlobalIds(data);
		}
		else if (this->DataType == CELL_TYPE) {

		}
	}
	else {
		return 0;
	}

	this->UpdateReadProgress();

	return 1;
}

//------------------------------------------------------------------------------
// Read pedigree ids. Return 0 if error.
int VTKAbstractReader::ReadPedigreeIds(int PointsNum)
{
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		//igError("Cannot read global id data"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);

	data = this->ReadArray(line, PointsNum, 1);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			//this->DataSet->GetPointData()->SetPedigreeIds(data);
		}
		else if (this->DataType == CELL_TYPE) {

		}
	}
	else {
		return 0;
	}

	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read edge flags. Return 0 if error.
int VTKAbstractReader::ReadEdgeFlags(int PointsNum)
{
	int skipEdgeFlags = 0;
	char line[256], name[256];
	ArrayObject::Pointer data;
	char buffer[256];

	if (!(this->ReadString(buffer) && this->ReadString(line))) {
		//igError("Cannot read edge flags data"
		//	<< " for file: " << (fname ? fname : "(Null FileName)"));
		return 0;
	}
	this->DecodeString(name, buffer);

	data = this->ReadArray(line, PointsNum, 1);
	if (data != nullptr) {
		data->SetName(name);
		if (this->DataType == POINT_TYPE) {
			//this->DataSet->GetPointData()->AddProperty(EDGEFLAG, data);
		}
		else if (this->DataType == CELL_TYPE) {

		}

	}
	else {
		return 0;
	}

	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
// Read lookup table. Return 0 if error.
int VTKAbstractReader::ReadLutData()
{
	this->UpdateReadProgress();
	return 1;
}

//------------------------------------------------------------------------------
int VTKAbstractReader::ReadFieldData()
{
	int i, numArrays = 0, skipField = 0;

	char name[256], type[256];
	int numComp, numTuples;
	ArrayObject::Pointer data;

	if (!(this->ReadString(name) && this->Read(&numArrays))) {
		/*igError("Cannot read field header!" << " for file: " << (fname ? fname : "(Null FileName)"));*/
		return 0;
	}
	// Read the number of arrays specified
	for (i = 0; i < numArrays; i++) {
		char buffer[256];
		this->ReadString(buffer);
		if (strcmp(buffer, "NULL_ARRAY") == 0) {
			continue;
		}
		this->DecodeString(name, buffer);
		this->Read(&numComp);
		this->Read(&numTuples);
		this->ReadString(type);
		data = this->ReadArray(type, numTuples, numComp);
		if (data != nullptr) {
			data->SetName(name);
			if (this->DataType == POINT_TYPE) {
				m_Data.GetData()->AddProperty(IG_SCALAR, IG_POINT, data);
			}
			else if (this->DataType == CELL_TYPE) {
				m_Data.GetData()->AddProperty(IG_SCALAR, IG_CELL, data);
			}
		}
	}
	this->UpdateReadProgress();
	return 1;
}


const void VTKAbstractReader::TransferVtkCellToiGameCell(IntArray::Pointer VtkCells, IntArray::Pointer VtkCellsType)
{
	CellArray::Pointer Lines = m_Data.GetLines();
	CellArray::Pointer Faces = m_Data.GetFaces();
	CellArray::Pointer Volumes = m_Data.GetVolumes();

	int CellNum = VtkCellsType->GetNumberOfElements();
	int IndexNum = VtkCells->GetNumberOfValues();
	int index = 0;
	int size = 0;
	int* vhs;
	for (int i = 0; i < CellNum; i++) {
		size = VtkCells->GetValue(index);
		vhs = VtkCells->RawPointer() + index + 1;
		index += size + 1;
		VTKTYPE type = (VTKTYPE)VtkCellsType->GetValue(i);
		//if(type != 7)
		switch (type)
		{
		case iGame::VTKAbstractReader::T0:
			break;
		case iGame::VTKAbstractReader::VERTEX:
			break;
		case iGame::VTKAbstractReader::POLYVERTEX:
			break;
		case iGame::VTKAbstractReader::LINE:
			assert(size == 2);
			Lines->AddCellId2(vhs[0], vhs[1]);
			break;
		case iGame::VTKAbstractReader::POLYLINE:
			for (int k = 0; k < size; k++) {
				Lines->AddCellId2(vhs[k], vhs[(k + 1) % size]);
			}
			break;
		case iGame::VTKAbstractReader::TRIANGLE:
			assert(size == 3);
			Faces->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::TRIANGLESTRIP:
			break;
		case iGame::VTKAbstractReader::POLYGON:
			Faces->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::PIXEL:
			break;
		case iGame::VTKAbstractReader::QUAD:
			assert(size == 4);
			Faces->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::TETRA:
			assert(size == 4);
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::VOXEL:
			break;
		case iGame::VTKAbstractReader::HEXAHEDRON:
			assert(size == 8);
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::WEDGE:
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::PYRAMID:
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::PENTAGONAL_PRISM:
			break;
		case iGame::VTKAbstractReader::HEXAGONAL_PRISM:
			break;
		case iGame::VTKAbstractReader::T17:
			break;
		case iGame::VTKAbstractReader::T18:
			break;
		case iGame::VTKAbstractReader::T19:
			break;
		case iGame::VTKAbstractReader::T20:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_EDGE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_TRIANGLE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_QUAD:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_TETRA:
		{
			igIndex tmp[16][3] = { {0,6,4},{6,2,5},{5,1,4},{6,5,4},
				{0,4,7},{4,1,8},{7,8,3},{7,4,8},
				{1,5,8},{5,2,9},{8,9,3},{8,5,9},
				{0,7,6},{3,9,7},{9,6,7},{2,6,9}
			};
			for (int i = 0; i < 16; i++) {
				igIndex tmpvhs[3] = { vhs[tmp[i][0]] ,vhs[tmp[i][1]] ,vhs[tmp[i][2]] };
				Faces->AddCellIds(tmpvhs, 3);
			}
		}
		break;
		case iGame::VTKAbstractReader::QUADRATIC_HEXAHEDRON:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_WEDGE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_PYRAMID:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_QUAD:
			break;
		case iGame::VTKAbstractReader::TRIQUADRATIC_HEXAHEDRON:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_LINEAR_QUAD:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_LINEAR_WEDGE:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_QUADRATIC_WEDGE:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_QUADRATIC_HEXAHEDRON:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_TRIANGLE:
			break;
		case iGame::VTKAbstractReader::CUBIC_LINE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_POLYGON:
			break;
		case iGame::VTKAbstractReader::TRIQUADRATIC_PYRAMID:
			break;
		default:
			break;
		}
	}

	this->UpdateReadProgress();
}
const void VTKAbstractReader::TransferVtkCellToiGameCell(ArrayObject::Pointer CellsID, ArrayObject::Pointer CellsConnect, IntArray::Pointer VtkCellsType)
{
	CellArray::Pointer Lines = m_Data.GetLines();
	CellArray::Pointer Faces = m_Data.GetFaces();
	CellArray::Pointer Volumes = m_Data.GetVolumes();

	int CellNum = VtkCellsType->GetNumberOfElements();
	int index = 0;
	int size = 0;
	int vhs[64];
	int st, ed;
	for (int i = 0; i < CellNum; i++) {
		st = static_cast<int>(CellsID->GetValue(i));
		ed = static_cast<int>(CellsID->GetValue(i + 1));
		size = ed - st;
		for (int j = 0; j < size; j++) {
			vhs[j] = static_cast<int>(CellsConnect->GetValue(st + j));
		}
		VTKTYPE type = (VTKTYPE)VtkCellsType->GetValue(i);
		switch (type)
		{
		case iGame::VTKAbstractReader::T0:
			break;
		case iGame::VTKAbstractReader::VERTEX:
			break;
		case iGame::VTKAbstractReader::POLYVERTEX:
			break;
		case iGame::VTKAbstractReader::LINE:
			assert(size == 2);
			Lines->AddCellId2(vhs[0], vhs[1]);
			break;
		case iGame::VTKAbstractReader::POLYLINE:
			for (int k = 0; k < size; k++) {
				Lines->AddCellId2(vhs[k], vhs[(k + 1) % size]);
			}
			break;
		case iGame::VTKAbstractReader::TRIANGLE:
			assert(size == 3);
			Faces->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::TRIANGLESTRIP:
			break;
		case iGame::VTKAbstractReader::POLYGON:
			Faces->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::PIXEL:
			break;
		case iGame::VTKAbstractReader::QUAD:
			assert(size == 4);
			Faces->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::TETRA:
			assert(size == 4);
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::VOXEL:
			break;
		case iGame::VTKAbstractReader::HEXAHEDRON:
			assert(size == 8);
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::WEDGE:
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::PYRAMID:
			Volumes->AddCellIds(vhs, size);
			break;
		case iGame::VTKAbstractReader::PENTAGONAL_PRISM:
			break;
		case iGame::VTKAbstractReader::HEXAGONAL_PRISM:
			break;
		case iGame::VTKAbstractReader::T17:
			break;
		case iGame::VTKAbstractReader::T18:
			break;
		case iGame::VTKAbstractReader::T19:
			break;
		case iGame::VTKAbstractReader::T20:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_EDGE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_TRIANGLE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_QUAD:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_TETRA:
		{
			igIndex tmp[16][3] = { {0,6,4},{6,2,5},{5,1,4},{6,5,4},
				{0,4,7},{4,1,8},{7,8,3},{7,4,8},
				{1,5,8},{5,2,9},{8,9,3},{8,5,9},
				{0,7,6},{3,9,7},{9,6,7},{2,6,9}
			};
			for (int i = 0; i < 16; i++) {
				igIndex tmpvhs[3] = { vhs[tmp[i][0]] ,vhs[tmp[i][1]] ,vhs[tmp[i][2]] };
				Faces->AddCellIds(tmpvhs, 3);
			}
		}
		break;
		case iGame::VTKAbstractReader::QUADRATIC_HEXAHEDRON:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_WEDGE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_PYRAMID:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_QUAD:
			break;
		case iGame::VTKAbstractReader::TRIQUADRATIC_HEXAHEDRON:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_LINEAR_QUAD:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_LINEAR_WEDGE:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_QUADRATIC_WEDGE:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_QUADRATIC_HEXAHEDRON:
			break;
		case iGame::VTKAbstractReader::BIQUADRATIC_TRIANGLE:
			break;
		case iGame::VTKAbstractReader::CUBIC_LINE:
			break;
		case iGame::VTKAbstractReader::QUADRATIC_POLYGON:
			break;
		case iGame::VTKAbstractReader::TRIQUADRATIC_PYRAMID:
			break;
		default:
			break;
		}
	}

	this->UpdateReadProgress();
}

void VTKAbstractReader::TransferVtkCellToiGameCell(DataCollection &m_Data, ArrayObject::Pointer CellsID,
												   ArrayObject::Pointer CellsConnect, ArrayObject::Pointer VtkCellsType) {


	CellArray::Pointer Lines = m_Data.GetLines();
	CellArray::Pointer Faces = m_Data.GetFaces();
	CellArray::Pointer Volumes = m_Data.GetVolumes();

	int CellNum = VtkCellsType->GetNumberOfElements();
	int index = 0;
	int size = 0;
	int vhs[64];
	int st, ed;
	for (int i = 0; i < CellNum; i++) {
		st = static_cast<int>(CellsID->GetValue(i));
		ed = static_cast<int>(CellsID->GetValue(i + 1));
		size = ed - st;
		for (int j = 0; j < size; j++) {
			vhs[j] = static_cast<int>(CellsConnect->GetValue(st + j));
		}
		VTKTYPE type = (VTKTYPE)VtkCellsType->GetValue(i);
		switch (type)
		{
			case iGame::VTKAbstractReader::T0:
				break;
			case iGame::VTKAbstractReader::VERTEX:
				break;
			case iGame::VTKAbstractReader::POLYVERTEX:
				break;
			case iGame::VTKAbstractReader::LINE:
				assert(size == 2);
				Lines->AddCellId2(vhs[0], vhs[1]);
				break;
			case iGame::VTKAbstractReader::POLYLINE:
				for (int k = 0; k < size; k++) {
					Lines->AddCellId2(vhs[k], vhs[(k + 1) % size]);
				}
				break;
			case iGame::VTKAbstractReader::TRIANGLE:
				assert(size == 3);
				Faces->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::TRIANGLESTRIP:
				break;
			case iGame::VTKAbstractReader::POLYGON:
				Faces->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::PIXEL:
				break;
			case iGame::VTKAbstractReader::QUAD:
				assert(size == 4);
				Faces->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::TETRA:
				assert(size == 4);
				Volumes->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::VOXEL:
				break;
			case iGame::VTKAbstractReader::HEXAHEDRON:
				assert(size == 8);
				Volumes->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::WEDGE:
				Volumes->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::PYRAMID:
				Volumes->AddCellIds(vhs, size);
				break;
			case iGame::VTKAbstractReader::PENTAGONAL_PRISM:
				break;
			case iGame::VTKAbstractReader::HEXAGONAL_PRISM:
				break;
			case iGame::VTKAbstractReader::T17:
				break;
			case iGame::VTKAbstractReader::T18:
				break;
			case iGame::VTKAbstractReader::T19:
				break;
			case iGame::VTKAbstractReader::T20:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_EDGE:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_TRIANGLE:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_QUAD:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_TETRA:
			{
				igIndex tmp[16][3] = { {0,6,4},{6,2,5},{5,1,4},{6,5,4},
									   {0,4,7},{4,1,8},{7,8,3},{7,4,8},
									   {1,5,8},{5,2,9},{8,9,3},{8,5,9},
									   {0,7,6},{3,9,7},{9,6,7},{2,6,9}
				};
				for (int i = 0; i < 16; i++) {
					igIndex tmpvhs[3] = { vhs[tmp[i][0]] ,vhs[tmp[i][1]] ,vhs[tmp[i][2]] };
					Faces->AddCellIds(tmpvhs, 3);
				}
			}
				break;
			case iGame::VTKAbstractReader::QUADRATIC_HEXAHEDRON:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_WEDGE:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_PYRAMID:
				break;
			case iGame::VTKAbstractReader::BIQUADRATIC_QUAD:
				break;
			case iGame::VTKAbstractReader::TRIQUADRATIC_HEXAHEDRON:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_LINEAR_QUAD:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_LINEAR_WEDGE:
				break;
			case iGame::VTKAbstractReader::BIQUADRATIC_QUADRATIC_WEDGE:
				break;
			case iGame::VTKAbstractReader::BIQUADRATIC_QUADRATIC_HEXAHEDRON:
				break;
			case iGame::VTKAbstractReader::BIQUADRATIC_TRIANGLE:
				break;
			case iGame::VTKAbstractReader::CUBIC_LINE:
				break;
			case iGame::VTKAbstractReader::QUADRATIC_POLYGON:
				break;
			case iGame::VTKAbstractReader::TRIQUADRATIC_PYRAMID:
				break;
			default:
				break;
		}
	}
}

IGAME_NAMESPACE_END
