#include "iGameVTKWriter.h"

IGAME_NAMESPACE_BEGIN



bool VTKWriter::GenerateBuffers()
{
	switch (m_DataObject->GetDataObjectType())
	{
	case IG_SURFACE_MESH:
		m_SurfaceMesh = DynamicCast<SurfaceMesh>(m_DataObject);
		break;
	case IG_UNSTRUCTURED_MESH:
		m_UnstructuredMesh = DynamicCast<UnstructuredMesh>(m_DataObject);
		break;
	case IG_STRUCTURED_MESH:
		m_StructuredMesh = DynamicCast<StructuredMesh>(m_DataObject);
		break;
	default:
		return false;
	}
	m_Buffers.resize(10, nullptr);
	WriteHeaderToBuffer(m_Buffers[0]);
	WritePointsToBuffer(m_Buffers[1]);
	WriteCellsToBuffer(m_Buffers[2]);
	WriteCellsTypeToBuffer(m_Buffers[3]);
	WritePointsAttributesToBuffer(m_Buffers[4]);
	WriteCellsAttributesToBuffer(m_Buffers[5]);
	return true;
}
const void VTKWriter::WriteHeaderToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	std::string data = "# vtk DataFile Version 3.0\nGenerated by iGameMeshLab\nASCII\nDATASET ";
	switch (m_DataObject->GetDataObjectType())
	{
	case IG_SURFACE_MESH:
		data += "POLYDATA\n";
		break;
	case IG_UNSTRUCTURED_MESH:
		data += "UNSTRUCTURED_GRID\n";
		break;
	case IG_STRUCTURED_MESH:
		data += "STRUCTURED_GRID\n";
		break;
	default:
		return;
	}
	for (int k = 0; k < data.size(); k++) {
		buffer->AddValue(data[k]);
	}
}
const void VTKWriter::WritePointsToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	auto Points = m_UnstructuredMesh->GetPoints();
	int VertexNum = Points ? Points->GetNumberOfPoints() : 0;
	std::string data = "POINTS " + std::to_string(VertexNum) + " float\n";
	for (int k = 0; k < data.size(); k++) {
		buffer->AddValue(data[k]);
	}
	Point p;
	std::string str;
	for (int i = 0; i < VertexNum; i++) {
		p = Points->GetPoint(i);
		for (int j = 0; j < 3; j++) {
			if (j)buffer->AddValue(' ');
			str = std::to_string(p[j]);
			for (int k = 0; k < str.size(); k++) {
				buffer->AddValue(str[k]);
			}
		}
		buffer->AddValue('\n');
	}

}
const void VTKWriter::WriteCellsToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int CellNum = m_UnstructuredMesh->GetNumberOfCells();
	UnsignedIntArray* CellsType = UnsignedIntArray::New();
	CellsType->Resize(CellNum);
	std::string data = "CELLS " + std::to_string(CellNum) + ' ';
	size_t idSize = m_UnstructuredMesh->GetCells()->GetNumberOfCellIds() + m_UnstructuredMesh->GetCells()->GetNumberOfCells();
	data += std::to_string(idSize) + '\n';
	std::cout << data << '\n';
	for (int k = 0; k < data.size(); k++) {
		buffer->AddValue(data[k]);
	}
	igIndex vcnt = 0;
	igIndex vhs[IGAME_CELL_MAX_SIZE] = { 0 };

	igIndex cellNum = m_UnstructuredMesh->GetNumberOfCells();
	for (igIndex i = 0; i < CellNum; i++) {
		vcnt = m_UnstructuredMesh->GetCellPointIds(i, vhs);
		data = std::to_string(vcnt);
		for (int k = 0; k < data.size(); k++) {
			buffer->AddValue(data[k]);
		}
		for (int j = 0; j < vcnt; j++) {
			buffer->AddValue(' ');
			data = std::to_string(vhs[j]);
			for (int k = 0; k < data.size(); k++) {
				buffer->AddValue(data[k]);
			}
		}
		buffer->AddValue('\n');
	}
}
const void VTKWriter::WriteCellsTypeToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	auto CellsType = m_UnstructuredMesh->GetCellTypes();
	int CellNum = CellsType->GetNumberOfValues();
	std::string data = "CELL_TYPES " + std::to_string(CellNum) + '\n';
	for (int k = 0; k < data.size(); k++) {
		buffer->AddValue(data[k]);
	}
	igIndex vtkType;
	for (int i = 0; i < CellNum; i++) {
		switch ((int)CellsType->GetValue(i))
		{
		case IG_TRIANGLE:
			vtkType = VTKAbstractReader::TRIANGLE;
			break;
		case IG_QUAD:
			vtkType = VTKAbstractReader::QUAD;
			break;
		case IG_POLYGON:
			vtkType = VTKAbstractReader::POLYGON;
			break;
		case IG_TETRA:
			vtkType = VTKAbstractReader::TETRA;
			break;
		case IG_HEXAHEDRON:
			vtkType = VTKAbstractReader::HEXAHEDRON;
			break;
		case IG_PRISM:
			vtkType = VTKAbstractReader::WEDGE;
			break;
		case IG_PYRAMID:
			vtkType = VTKAbstractReader::PYRAMID;
			break;
		case IG_POLYHEDRON:
			vtkType = VTKAbstractReader::POLYHEDRON;
			break;
		default:
			vtkType = VTKAbstractReader::T0;
			break;
		}
		data = std::to_string(vtkType);
		for (int k = 0; k < data.size(); k++) {
			buffer->AddValue(data[k]);
		}
		buffer->AddValue('\n');
	}
}
const void VTKWriter::WritePointsAttributesToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	auto AttributeSet = m_UnstructuredMesh->GetAttributeSet();
	if (AttributeSet == nullptr)return;
	auto PointData = AttributeSet->GetAllPointAttributes();
	if (!PointData || PointData->GetNumberOfElements() == 0)return;
	int VertexNum = m_UnstructuredMesh->GetNumberOfPoints();
	std::string data = "POINT_DATA " + std::to_string(VertexNum) + "\n";
	for (int k = 0; k < data.size(); k++) {
		buffer->AddValue(data[k]);
	}
	igDebug("Writing " + data);

	for (int i = 0; i < PointData->GetNumberOfElements(); i++)
	{
		auto attribute = PointData->GetElement(i);
		auto array = attribute.pointer;
		switch (attribute.type)
		{
		case IG_SCALAR:
			data = "SCALARS " + array->GetName() + " float " + std::to_string(array->GetElementSize()) + "\nLOOKUP_TABLE default\n";
			break;
		case IG_VECTOR:
			data = "VECTORS " + array->GetName() + " float\n";
			break;
		case IG_TENSOR:
			data = "TENSORS " + array->GetName() + " float\n";
			break;
		case IG_NORMAL:
			data = "NORMALS " + array->GetName() + " float\n";
			break;
		default:
			data = "SCALARS ";
			break;
		}
		for (int k = 0; k < data.size(); k++) {
			buffer->AddValue(data[k]);
		}
		WriteArrayToBuffer(buffer, array);
	}
	return;
}
const void VTKWriter::WriteCellsAttributesToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	auto AttributeSet = m_UnstructuredMesh->GetAttributeSet();
	if (AttributeSet == nullptr)return;
	auto CellData = AttributeSet->GetAllCellAttributes();
	if (!CellData || CellData->GetNumberOfElements() == 0)return;
	int CellNum = m_UnstructuredMesh->GetNumberOfCells();
	std::string data = "CELL_DATA " + std::to_string(CellNum) + '\n';
	for (int k = 0; k < data.size(); k++) {
		buffer->AddValue(data[k]);
	}
	igDebug("Writing " + data);
	for (int i = 0; i < CellData->GetNumberOfElements(); i++)
	{
		auto attribute = CellData->GetElement(i);
		auto array = attribute.pointer;
		switch (attribute.type)
		{
		case IG_SCALAR:
			data = "SCALARS " + array->GetName() + " float " + std::to_string(array->GetElementSize()) + "\nLOOKUP_TABLE default\n";
			break;
		case IG_VECTOR:
			data = "VECTORS " + array->GetName() + " float\n";
			break;
		case IG_TENSOR:
			data = "TENSORS " + array->GetName() + " float\n";
			break;
		case IG_NORMAL:
			data = "NORMALS " + array->GetName() + " float\n";
			break;
		default:
			data = "SCALARS ";
			break;
		}
		for (int k = 0; k < data.size(); k++) {
			buffer->AddValue(data[k]);
		}
		WriteArrayToBuffer(buffer, array);
	}
}
const void VTKWriter::WriteArrayToBuffer(CharArray::Pointer& buffer, ArrayObject::Pointer data)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int Num = data->GetNumberOfElements();
	int Component = data->GetElementSize();
	//这边先全都默认为float
	float values[16];
	std::string str;
	for (int i = 0; i < Num; i++) {
		data->GetElement(i, values);
		for (int j = 0; j < Component; j++) {
			if (j)buffer->AddValue(' ');
			str = std::to_string(values[j]);
			for (int k = 0; k < str.size(); k++) {
				buffer->AddValue(str[k]);
			}
		}
		buffer->AddValue('\n');
	}
}

IGAME_NAMESPACE_END