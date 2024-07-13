#include "iGameMetadata.h"
IGAME_NAMESPACE_BEGIN
void Metadata::AddInt(const std::string& name, int value)
{
	AddEntry(name, value);
}

bool Metadata::GetInt(const std::string& name, int& value) const
{
	return GetEntry(name, value);
}

int Metadata::GetInt(const std::string& name) const
{
	int value{};
	GetEntry(name, value);
	return value;
}

void Metadata::AddIntArray(const std::string& name, IntArray2::Pointer value)
{
	AddEntry(name, value);
}

bool Metadata::GetIntArray(const std::string& name, IntArray2::Pointer& value) const
{
	return GetEntry(name, value);
}

IntArray2::Pointer Metadata::GetIntArray(const std::string& name) const
{
	IntArray2::Pointer value;
	if (GetEntry(name, value)) {
		return value;
	}
	return nullptr;
}

void Metadata::AddFloat(const std::string& name, float value)
{
	AddEntry(name, value);
}

bool Metadata::GetFloat(const std::string& name, float& value) const
{
	return GetEntry(name, value);
}

float Metadata::GetFloat(const std::string& name) const
{
	float value{};
	GetEntry(name, value);
	return value;
}

void Metadata::AddFloatArray(const std::string& name, FloatArray2::Pointer value)
{
	AddEntry(name, value);
}

bool Metadata::GetFloatArray(const std::string& name, FloatArray2::Pointer& value) const
{
	return GetEntry(name, value);
}

FloatArray2::Pointer Metadata::GetFloatArray(const std::string& name) const
{
	FloatArray2::Pointer value;
	if (GetEntry(name, value)) {
		return value;
	}
	return nullptr;
}

void Metadata::AddDouble(const std::string& name, double value)
{
	AddEntry(name, value);
}

bool Metadata::GetDouble(const std::string& name, double& value) const
{
	return GetEntry(name, value);
}

double Metadata::GetDouble(const std::string& name) const
{
	double value{};
	GetEntry(name, value);
	return value;
}

void Metadata::AddDoubleArray(const std::string& name, DoubleArray2::Pointer value)
{
	AddEntry(name, value);
}

bool Metadata::GetDoubleArray(const std::string& name, DoubleArray2::Pointer& value) const
{
	return GetEntry(name, value);
}

DoubleArray2::Pointer Metadata::GetDoubleArray(const std::string& name) const
{
	DoubleArray2::Pointer value;
	if (GetEntry(name, value)) {
		return value;
	}
	return nullptr;
}

void Metadata::AddString(const std::string& name, const std::string& value)
{
	AddEntry(name, value);
}

bool Metadata::GetString(const std::string& name, std::string& value) const
{
	return GetEntry(name, value);
}

std::string Metadata::GetString(const std::string& name) const
{
	std::string value{};
	GetEntry(name, value);
	return value;
}

void Metadata::AddStringArray(const std::string& name, StringArray::Pointer value)
{
	AddEntry(name, value);
}

bool Metadata::GetStringArray(const std::string& name, StringArray::Pointer& value) const
{
	return GetEntry(name, value);
}

StringArray::Pointer Metadata::GetStringArray(const std::string& name) const
{
	StringArray::Pointer value;
	if (GetEntry(name, value)) {
		return value;
	}
	return nullptr;
}

bool Metadata::AddSubMetadata(const std::string& name, Pointer sub_metadata)
{
	auto sub_ptr = m_SubMetadatas.find(name);
	// Avoid accidentally writing over a sub-metadata with the same name.
	if (sub_ptr != m_SubMetadatas.end()) {
		return false;
	}
	m_SubMetadatas[name] = std::move(sub_metadata);
	return true;
}

Metadata::Pointer Metadata::GetSubMetadata(const std::string& name)
{
	auto sub_ptr = m_SubMetadatas.find(name);
	if (sub_ptr == m_SubMetadatas.end()) {
		return nullptr;
	}
	return sub_ptr->second.get();
}

void Metadata::RemoveEntry(const std::string& name)
{
	auto entry_ptr = m_Entries.find(name);
	if (entry_ptr != m_Entries.end()) {
		m_Entries.erase(entry_ptr);
	}
}
IGAME_NAMESPACE_END