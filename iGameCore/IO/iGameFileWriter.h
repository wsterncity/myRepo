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
	// static Pointer New() { return new FileWriter; }

	// Pure virtual function to generate buffers. Subclasses must implement this.
	virtual bool GenerateBuffers() = 0;

	// Write the current data object to a file at the path specified by m_FilePath.
	bool WriteToFile();

	// Write the specified data object to a file at the given file path.
	bool WriteToFile(DataObject::Pointer dataobject, std::string filePath);

	// Save the buffer data to a file using a general method.
	bool SaveBufferDataToFile();

	// Save the buffer data to a file using Windows-specific methods.
	bool SaveBufferDataToFileWithWindows();

	// Save the buffer data to a file using Linux-specific methods.
	bool SaveBufferDataToFileWithLinux();

	// Save the buffer data to a file using Mac-specific methods.
	bool SaveBufferDataToFileWithMac();

	// Set the file path where the file will be saved.
	void SetFilePath(const std::string& filePath);

	// Set the file type to either ASCII or binary.
	void SetFileType(IGenum fileType) {
		if (fileType == IGAME_BINARY) {
			m_FileType = IGAME_BINARY;
		}
		else {
			m_FileType = IGAME_ASCII;
		}
	}

	// Set the data object to be written to the file.
	void SetDataObject(DataObject::Pointer dataobject);

	// Add a string to the buffer for writing to the file.
	void AddStringToBuffer(std::string& data, CharArray::Pointer buffer);

protected:
	// Constructor for FileWriter.
	FileWriter();

	// Destructor for FileWriter.
	~FileWriter() override;

	// Data object that will be written to the file.
	DataObject::Pointer m_DataObject{ nullptr };

	// Path to the file that will be written.
	std::string m_FilePath = "default.txt";

	// Size of the file to be written.
	size_t m_FileSize;

	/*
	The char buffer used to load data into the file.
	Regardless of how different types of files are written
	in parallel to a bunch of buffers, the buffers that end up
	in m_Buffers must be in order.
	*/
	std::vector<CharArray::Pointer> m_Buffers;

	/*
	File type, may be either ASCII or binary.
	*/
	IGenum m_FileType{ IGAME_ASCII };
};

IGAME_NAMESPACE_END
#endif