#ifndef iGameOBJWriter_h
#define iGameOBJWriter_h

#include "iGameFileWriter.h"

IGAME_NAMESPACE_BEGIN

class OBJWriter : public FileWriter {
public:
	I_OBJECT(OBJWriter);
	static Pointer New() { return new OBJWriter; }

	bool GenerateBuffers() override;

	bool WritePointsToBuffer(CharArray::Pointer&);
	bool WriteFacesToBuffer(CharArray::Pointer&);
protected:
	OBJWriter() = default;
	~OBJWriter() override = default;
	SurfaceMesh::Pointer m_SurfaceMesh{ nullptr };
};

IGAME_NAMESPACE_END
#endif