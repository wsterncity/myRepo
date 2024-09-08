/**
 * @class   iGameVTUReader
 * @brief   iGameVTUReader's brief
 */

#pragma once

#include "iGameXMLFileReader.h"
#include "iGameUnstructuredMesh.h"

IGAME_NAMESPACE_BEGIN
class iGameVTUReader : public iGameXMLFileReader {

public:
	I_OBJECT(iGameVTUReader);

	static Pointer New() { return new iGameVTUReader; }

	bool Parsing() override;
	bool CreateDataObject() override;

protected:

	iGameVTUReader() = default;
	~iGameVTUReader() = default;
	UnstructuredMesh::Pointer m_UnstructuredMesh{ nullptr };
	igIndex m_DataObjectType = IG_NONE;
};

IGAME_NAMESPACE_END
