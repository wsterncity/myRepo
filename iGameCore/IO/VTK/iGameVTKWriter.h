#ifndef iGameVTKWriter_h
#define iGameVTKWriter_h

#include "iGameFileWriter.h"
#include "iGameVTKabstractReader.h"

IGAME_NAMESPACE_BEGIN

class VTKWriter : public FileWriter {
public:
	I_OBJECT(VTKWriter);
	static Pointer New() { return new VTKWriter; }
	bool GenerateBuffers() override;
	const void WriteHeaderToBuffer(CharArray::Pointer& buffer);
	const void WritePointsToBuffer(CharArray::Pointer&);
	const void WriteCellsToBuffer(CharArray::Pointer& buffer);
	const void WriteCellsTypeToBuffer(CharArray::Pointer& buffer);
	const void WritePointsAttributesToBuffer(CharArray::Pointer& buffer);
	const void WriteCellsAttributesToBuffer(CharArray::Pointer& buffer);
	const void WriteArrayToBuffer(CharArray::Pointer& buffer, ArrayObject::Pointer data);
protected:
	VTKWriter() = default;
	~VTKWriter() override = default;
	SurfaceMesh::Pointer m_SurfaceMesh{ nullptr };
	UnstructuredMesh::Pointer m_UnstructuredMesh{ nullptr };
	StructuredMesh::Pointer m_StructuredMesh{ nullptr };

};

IGAME_NAMESPACE_END
#endif