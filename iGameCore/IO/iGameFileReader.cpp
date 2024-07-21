#include "iGameFileReader.h"
#include "iGameSurfaceMesh.h"
#include "iGameByteSwap.h"
#include "iGameVolumeMesh.h"

IGAME_NAMESPACE_BEGIN
FileReader::FileReader()
{
	this->SetNumberOfInputs(0);
	this->SetNumberOfOutputs(1);

	m_Buffer = CharArray::New();
}

bool FileReader::Execute()
{
	clock_t start, end;
	start = clock();

	if (!Open())
	{
		std::cerr << "Open failure\n";
		return false;
	}
	if (!ReadToBuffer())
	{
		std::cerr << "Read to buffer failure\n";
		return false;
	}
	this->IS = m_Buffer->GetRawPointer();
	if (!Parsing())
	{
		std::cerr << "Parsing failure\n";
		return false;
	}
	if (!Close())
	{
		std::cerr << "Close failure\n";
		return false;
	}
	if (!CreateDataObject())
	{
		std::cerr << "Generate DataObject failure\n";
		return false;
	}
	int size = m_Output->GetAttributes()->GetArrays().size();
	StringArray::Pointer attrbNameArray = StringArray::New();
	if (size > 0) {
		for (int i = 0; i < size; i++) {
			auto& data = m_Output->GetAttributes()->GetAttribute(i);
			attrbNameArray->InsertToBack(data.array->GetName());
		}
	}
	m_Output->GetMetadata()->AddStringArray(ATTRIBUTE_NAME_ARRAY, attrbNameArray);
	this->SetOutput(0, m_Output);
	end = clock();
	std::cout << "Read file success!" << std::endl;
	std::cout << "   The time cost: " << end - start << "ms" << std::endl;
	return true;
}

bool FileReader::Open()
{
	if (m_FilePath.empty()) 
	{
		return false;
	}
	m_File = std::make_unique<std::ifstream>(m_FilePath, std::ios::binary);

	if (!m_File->is_open()) {
		return false;
	}

	m_FileName = m_FilePath.substr(m_FilePath.find_last_of('/') + 1);
	m_FileSuffix = m_FileName.substr(m_FileName.find_last_of('.') + 1);
	m_FileSize = GetFileSize(*m_File);
	return true;
}

bool FileReader::Close()
{
	if (m_File && m_File->is_open()) {
		m_File->close();
		m_File.reset();
	}
	return true;
}

bool FileReader::ReadToBuffer() 
{
	if (!m_File || !m_File->is_open()) {
		return false;
	}

	if (m_FileSize == 0) {
		return false;
	}

	m_Buffer->SetNumberOfValues(this->m_FileSize);
	m_File->read(m_Buffer->GetRawPointer(), m_FileSize);

	return m_File->gcount() == static_cast<std::streamsize>(m_FileSize);
}

bool FileReader::CreateDataObject()
{
	// 统计各类型元素的数量
	int numFaces = m_Data.GetNumberOfFaces();
	int numVolumes = m_Data.GetNumberOfVolumes();

	// 混合网格类型判断
	if (numFaces && numVolumes) {
		VolumeMesh::Pointer mesh = VolumeMesh::New();
		mesh->SetPoints(m_Data.GetPoints());
		mesh->SetVolumes(m_Data.GetVolumes());
		mesh->SetAttributes(m_Data.GetData());
		m_Output = mesh;
	}

	// 表面网格类型判断
	else if (numFaces) {
		SurfaceMesh::Pointer mesh = SurfaceMesh::New();
		mesh->SetPoints(m_Data.GetPoints());
		mesh->SetFaces(m_Data.GetFaces());
		mesh->SetAttributes(m_Data.GetData());
		m_Output = mesh;
	}

	// 体网格类型判断
	else if (numVolumes) {
		VolumeMesh::Pointer mesh = VolumeMesh::New();
		mesh->SetPoints(m_Data.GetPoints());
		mesh->SetVolumes(m_Data.GetVolumes());
		mesh->SetAttributes(m_Data.GetData());
		m_Output = mesh;
	}

	return true;
}

//------------------------------------------------------------------------------
// Internal function to read in an integer value.
// Returns zero if there was an error.
int FileReader::Read(char* result)
{
	//int intData;
	//*this->IS >> intData;
	//if (this->IS->fail())
	//{
	//	return 0;
	//}
	//*result = (char)intData;
	return 1;
}
//------------------------------------------------------------------------------
int FileReader::Read(char* data, size_t n)
{
	memcpy(data, this->IS, n);
	this->IS += n;
	return 1;
}
//------------------------------------------------------------------------------
int FileReader::Read(unsigned char* result)
{
	//int intData;
	//*this->IS >> intData;
	//if (this->IS->fail())
	//{
	//	return 0;
	//}
	//*result = (unsigned char)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(short* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (short)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(unsigned short* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (unsigned short)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(int* result)
{
	this->IS = mAtoi(this->IS, *result);
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(unsigned int* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (unsigned int)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(long* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (long)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(unsigned long* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (unsigned long)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(long long* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (long long)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(unsigned long long* result)
{
	int intData;
	this->IS = mAtoi(this->IS, intData);
	*result = (unsigned long long)intData;
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(float* result)
{
	this->IS = mAtof(this->IS, *result);
	return 1;
}

//------------------------------------------------------------------------------
int FileReader::Read(double* result)
{
	this->IS = mAtof(this->IS, *result);
	return 1;
}

//------------------------------------------------------------------------------
// Internal function to read in a line up to 256 characters.
// Returns zero if there was an error.
int FileReader::ReadLine(char result[256])
{
	const char* lineEnd = strchr(this->IS, '\n');
	if (!lineEnd) lineEnd = m_Buffer->GetRawPointer() + m_Buffer->GetNumberOfValues() - 1;
	if (!this->IS || this->IS > lineEnd) {
		result[0] = '\0';
		return 0;
	}
	size_t slen = lineEnd - this->IS;
	std::memcpy(result, this->IS, slen);
	result[slen] = '\0';
	// remove '\r', if present.
	if (slen > 0 && result[slen - 1] == '\r') {
		result[slen - 1] = '\0';
	}
	this->IS = lineEnd + 1;
	return 1;
}

//------------------------------------------------------------------------------
// Internal function to read in a string up to 256 characters.
// Returns zero if there was an error.
int FileReader::ReadString(char result[256])
{
	// Force the parameter to be seen as a 256-byte array rather than a decayed
	// pointer.

	while (*this->IS == ' ' || *this->IS == '\r' || *this->IS == '\n')this->IS++;
	const char* op = strchr(this->IS, ' ');
	const char* lineEnd = strchr(this->IS, '\n');
	if (!op || op > lineEnd) {
		op = lineEnd;
	}
	size_t slen = 0;
	if (!op) {
		op = m_Buffer->GetRawPointer() + m_Buffer->GetNumberOfValues() - 1;
		slen = op - this->IS + 1;
	}
	else {
		slen = op - this->IS;
	}
	if (this->IS > op) {
		result[0] = '\0';
		return 0;
	}
	std::memcpy(result, this->IS, slen);
	result[slen] = '\0';
	if (slen > 0 && result[slen - 1] == '\r') {
		result[slen - 1] = '\0';
	}
	this->IS = op + 1;
	return 1;
}
int FileReader::ReadString(std::string& result)
{
	while (*this->IS == ' ' || *this->IS == '\r' || *this->IS == '\n' || *this->IS == '\0')this->IS++;
	const char* op = strchr(this->IS, ' ');
	const char* lineEnd = strchr(this->IS, '\n');
	if (!op || op > lineEnd) {
		op = lineEnd;
	}
	if (!op) op = m_Buffer->GetRawPointer() + m_Buffer->GetNumberOfValues();

	if (this->IS > op) {
		result[0] = '\0';
		return 0;
	}
	result = std::string(this->IS, op);
	size_t slen = result.length();
	if (slen > 0 && result[slen - 1] == '\r') {
		result = std::string(this->IS, op - 1);
	}
	this->IS = op + 1;
	return 1;
}


char* FileReader::LowerCase(char* str, const size_t len)
{
	size_t i;
	char* s;

	for (i = 0, s = str; *s != '\0' && i < len; s++, i++)
	{
		*s = tolower(*s);
	}
	return str;
}

// General templated function to read data of various types.
template <class T>
int iGameReadBinaryData(FileReader::Pointer self, T* data, int numTuples, int numComp)
{
	if (numTuples == 0 || numComp == 0)
	{
		// nothing to read here.
		return 1;
	}
	char line[256];

	// suck up newline
	//IS->getline(line, 256);
	//IS->read((char*)data, sizeof(T) * numComp * numTuples);
	self->ReadLine(line);
	if (!self->Read((char*)data, sizeof(T) * numComp * numTuples))
	{
		igDebug("Error reading binary data!");
		return 0;
	}
	data = (T*)data;
	return 1;
}

// General templated function to read data of various types.
template <class T>
int iGameReadASCIIData(FileReader::Pointer self, T* data, int numTuples, int numComp)
{
	int i, j;

	for (i = 0; i < numTuples; i++) {
		for (j = 0; j < numComp; j++) {
			if (!self->Read(data)) {
				igDebug("Error reading ascii data. Possible mismatch of "
					"datasize with declaration.");
				return 0;
			}
			data++;
		}
	}
	return 1;
}

DataArray::Pointer FileReader::ReadArray(const char* dataType, int numTuples, int numComp)
{
	char* type = strdup(dataType);
	type = this->LowerCase(type);

	DataArray::Pointer array;
	if (!strncmp(type, "bit", 3))
	{
		/*	array = vtkBitArray::New();
			array->SetNumberOfComponents(numComp);
			if (numTuples != 0 && numComp != 0)
			{
				unsigned char* ptr = ((vtkBitArray*)array)->WritePointer(0, numTuples * numComp);
				if (this->FileType == IGAME_BINARY)
				{
					char line[256];
					this->IS->getline(line, 256);
					this->IS->read((char*)ptr, sizeof(unsigned char) * (numTuples * numComp + 7) / 8);
					if (this->IS->eof())
					{
						igError( "Error reading binary bit array!");
						free(type);
						array->Delete();
						return nullptr;
					}
				}
				else
				{
					int b;
					for (int i = 0; i < numTuples; i++)
					{
						for (int j = 0; j < numComp; j++)
						{
							if (!this->Read(&b))
							{
								vtkErrorMacro("Error reading ascii bit array! tuple: " << i << ", component: " << j);
								free(type);
								array->Delete();
								return nullptr;
							}
							else
							{
								((vtkBitArray*)array)->SetValue(i * numComp + j, b);
							}
						}
					}
				}
			}*/
	}

	else if (!strcmp(type, "char") || !strcmp(type, "signed_char"))
	{
		CharArray::Pointer arr = CharArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		char* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY)
		{
			iGameReadBinaryData(this, ptr, numTuples, numComp);
		}
		else
		{
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}

	else if (!strncmp(type, "unsigned_char", 13))
	{
		UnsignedCharArray::Pointer arr = UnsignedCharArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		unsigned char* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY)
		{
			iGameReadBinaryData(this, ptr, numTuples, numComp);
		}
		else
		{
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}

	else if (!strncmp(type, "short", 5))
	{
		ShortArray::Pointer arr = ShortArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		short* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap2BERange(ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}

	else if (!strncmp(type, "unsigned_short", 14))
	{
		UnsignedShortArray::Pointer arr = UnsignedShortArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		unsigned short* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap2BERange((short*)ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}
	else if (!strncmp(type, "vtkidtype", 9)) {
		// currently writing vtkidtype as int.
		// may be long long,need to transfer
		IntArray::Pointer arr = IntArray::New();
		arr->SetNumberOfComponents(numComp);
		std::vector<int> buffer(numTuples * numComp);
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, buffer.data(), numTuples, numComp);
			ByteSwap::Swap4BERange(buffer.data(), numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, buffer.data(), numTuples, numComp);
		}
		array->SetNumberOfTuples(numTuples);
		int* ptr2 = arr->GetRawPointer();
		for (int idx = 0; idx < numTuples * numComp; idx++) {
			ptr2[idx] = buffer[idx];
		}
		array = arr;
	}
	else if (!strncmp(type, "int", 3)) {
		IntArray::Pointer arr = IntArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		int* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap4BERange(ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}
	else if (!strncmp(type, "unsigned_int", 12)) {
		UnsignedIntArray::Pointer arr = UnsignedIntArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		unsigned int* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap4BERange((int*)ptr, numTuples * numComp);
		}
		else
		{
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}

	else if (!strncmp(type, "vtktypeint64", 12)) {
		LongLongArray::Pointer arr = LongLongArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		long long* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap8BERange(ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}
	else if (!strncmp(type, "vtktypeuint64", 13)) {
		UnsignedLongLongArray::Pointer arr = UnsignedLongLongArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		unsigned long long* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap8BERange(ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}

	else if (!strncmp(type, "float", 5)) {
		FloatArray::Pointer arr = FloatArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		float* ptr = arr->GetRawPointer();
		if (m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap4BERange(ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}

	else if (!strncmp(type, "double", 6)) {
		DoubleArray::Pointer arr = DoubleArray::New();
		arr->SetNumberOfComponents(numComp);
		arr->SetNumberOfTuples(numTuples);
		double* ptr = arr->GetRawPointer();
		if (this->m_FileType == IGAME_BINARY) {
			iGameReadBinaryData(this, ptr, numTuples, numComp);
			ByteSwap::Swap8BERange(ptr, numTuples * numComp);
		}
		else {
			iGameReadASCIIData(this, ptr, numTuples, numComp);
		}
		array = arr;
	}
	else if (!strncmp(type, "variant", 7))
	{
		/*	array = iGameVariantArray::New();
			array->SetNumberOfComponents(numComp);
			for (int i = 0; i < numTuples; i++)
			{
				for (int j = 0; j < numComp; j++)
				{
					int t;
					std::string str;
					this->Read(&t);
					this->ReadString(str);
					std::vector<char> decoded(str.length() + 1);
					int decodedLength = this->DecodeString(decoded.data(), str.c_str());
					std::string decodedStr(decoded.data(), decodedLength);
					iGameVariant sv(decodedStr);
					iGameVariant v;
					switch (t)
					{
					case IG_CHAR:
						v = sv.ToChar();
						break;
					case IG_UNSIGNED_CHAR:
						v = sv.ToUnsignedChar();
						break;
					case IG_SHORT:
						v = sv.ToShort();
						break;
					case IG_UNSIGNED_SHORT:
						v = sv.ToUnsignedShort();
						break;
					case IG_INT:
						v = sv.ToInt();
						break;
					case IG_UNSIGNED_INT:
						v = sv.ToUnsignedInt();
						break;
					case IG_FLOAT:
						v = sv.ToFloat();
						break;
					case IG_DOUBLE:
						v = sv.ToDouble();
						break;
					case IG_LONG_LONG:
						v = sv.ToLongLong();
						break;
					case IG_UNSIGNED_LONG_LONG:
						v = sv.ToUnsignedLongLong();
						break;
					default:
						igError("Unknown variant type " << t);
					}
					((iGameVariantArray*)array)->InsertNextValue(v);
				}
			}*/
	}
	else {
		igError("Unsupported data type: " << type);
		free(type);
		return nullptr;
	}
	free(type);
	//this->UpdateReadProgress();
	return array;
}

    void FileReader::SetFilePath(const std::string &filePath) {
		 this->m_FilePath = filePath;
		 this->m_FileName = filePath.substr(filePath.find_last_of('/') + 1, filePath.size());;
    }


IGAME_NAMESPACE_END