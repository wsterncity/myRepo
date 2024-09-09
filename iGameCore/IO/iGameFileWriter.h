#ifndef iGameFileWriter_h
#define iGameFileWriter_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameVolumeMesh.h"
#include "iGameUnstructuredMesh.h"
#include "iGameStructuredMesh.h"
IGAME_NAMESPACE_BEGIN
class FileWriter : public Filter {
public:
	I_OBJECT(FileWriter);
	//static Pointer New() { return new FileWriter; }

	virtual bool GenerateBuffers() = 0;

	bool WriteToFile();
	bool WriteToFile(DataObject::Pointer dataobject,std::string filePath);

	bool SaveBufferDataToFile();
	bool SaveBufferDataToFileWithWindows();
	bool SaveBufferDataToFileWithLinux();

	void SetFilePath(const std::string& filePath);

	void SetDataObject(DataObject::Pointer dataobject);


protected:
	FileWriter();
	~FileWriter() override;

	DataObject::Pointer m_DataObject{ nullptr };
	std::string m_FilePath = "default.txt";
	size_t m_FileSize;
	std::vector<CharArray::Pointer>m_Buffers;
	IGenum m_FileType{ IGAME_ASCII };
};
IGAME_NAMESPACE_END
#endif