#include "iGameFileWriter.h"
#include<windows.h>
#include<stdio.h>
#include <tchar.h>
IGAME_NAMESPACE_BEGIN

FileWriter::FileWriter()
{

}
FileWriter::~FileWriter()
{
	m_Buffers.swap(std::vector<CharArray::Pointer>());
}
bool FileWriter::WriteToFile()
{
	if (!GenerateBuffers()) {
		igError("Could not generate buffer to load.");
		return false;
	}
	return SaveBufferDataToFile();

}
bool FileWriter::WriteToFile(DataObject::Pointer dataObject, std::string filePath)
{
	this->m_DataObject = dataObject;
	this->m_FilePath = filePath;
	return WriteToFile();
}

bool FileWriter::SaveBufferDataToFile()
{
	return SaveBufferDataToFileWithWindows();
}
bool FileWriter::SaveBufferDataToFileWithWindows()
{
	clock_t time_1 = clock();
	// 打开文件
	HANDLE hFile = CreateFile(this->m_FilePath.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		_tprintf(_T("CreateFile failed with error: %lu\n"), GetLastError());
		return false;
	}

	// 计算新的文件大小
	m_FileSize = 0;
	for (int i = 0; i < m_Buffers.size(); i++) {
		if (m_Buffers[i]) {
			m_FileSize += m_Buffers[i]->GetNumberOfValues();
		}
	}

	// 创建文件映射，大小为新的文件大小
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, m_FileSize, NULL);
	if (hMapFile == NULL) {
		_tprintf(_T("CreateFileMapping failed with error: %lu\n"), GetLastError());
		CloseHandle(hFile);
		return false;
	}

	// 将文件映射到内存
	LPVOID lpBaseAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, m_FileSize);
	if (lpBaseAddress == NULL) {
		_tprintf(_T("MapViewOfFile failed with error: %lu\n"), GetLastError());
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return false;
	}

	// 将缓冲区数据写入文件
	size_t offset = 0;
	for (int i = 0; i < m_Buffers.size(); i++) {
		if (m_Buffers[i] && m_Buffers[i]->RawPointer()) {
			memcpy((char*)lpBaseAddress + offset, m_Buffers[i]->RawPointer(), m_Buffers[i]->GetNumberOfValues());
			offset += m_Buffers[i]->GetNumberOfValues();
		}
	}

	// 解除映射
	UnmapViewOfFile(lpBaseAddress);
	CloseHandle(hMapFile);
	CloseHandle(hFile);

	clock_t time_2 = clock();
	std::cout << "Write buffer to file cost " << time_2 - time_1 << "ms\n";
	return true;
}

bool FileWriter::SaveBufferDataToFileWithLinux()
{
	//用mmap就行，原理和上面的一样，实在不行用fwrite
	return true;
}
void FileWriter::SetFilePath(const std::string& filePath)
{
	this->m_FilePath = filePath;
}

void FileWriter::SetDataObject(DataObject::Pointer dataObject)
{
	this->m_DataObject = dataObject;
}
void FileWriter::AddStringToBuffer(std::string& data, CharArray::Pointer buffer)
{
	if (!buffer)return;
	for (int i = 0; i < data.size(); i++) {
		buffer->AddValue(data[i]);
	}
	return;
}
IGAME_NAMESPACE_END