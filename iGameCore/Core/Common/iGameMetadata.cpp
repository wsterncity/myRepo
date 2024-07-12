#include "iGameMetadata.h"
IGAME_NAMESPACE_BEGIN
EntryValue::EntryValue(const EntryValue& value)
{
	m_Data.resize(value.m_Data.size());
	memcpy(&m_Data[0], &value.m_Data[0], value.m_Data.size());
}

EntryValue::EntryValue(const std::string& value)
{
	m_Data.resize(value.size());
	memcpy(&m_Data[0], &value[0], value.size());
}

void Metadata::AddInt(const std::string& name, int value)
{
	AddEntry(name, value);
}

bool Metadata::GetInt(const std::string& name, int* value) const
{
	return GetEntry(name, value);
}

void Metadata::AddIntArray(const std::string& name, const std::vector<int>& value)
{
	AddEntry(name, value);
}

bool Metadata::GetIntArray(const std::string& name, std::vector<int>* value) const
{
	return GetEntry(name, value);
}

void Metadata::AddFloat(const std::string& name, float value)
{
	AddEntry(name, value);
}

bool Metadata::GetFloat(const std::string& name, float* value) const
{
	return GetEntry(name, value);
}

void Metadata::AddFloatArray(const std::string& name, const std::vector<float>& value)
{
	AddEntry(name, value);
}

bool Metadata::GetFloatArray(const std::string& name, std::vector<float>* value) const
{
	return GetEntry(name, value);
}

void Metadata::AddDouble(const std::string& name, double value)
{
	AddEntry(name, value);
}

bool Metadata::GetDouble(const std::string& name, double* value) const
{
	return GetEntry(name, value);
}

void Metadata::AddDoubleArray(const std::string& name, const std::vector<double>& value)
{
	AddEntry(name, value);
}

bool Metadata::GetDoubleArray(const std::string& name, std::vector<double>* value) const
{
	return GetEntry(name, value);
}

void Metadata::AddString(const std::string& name, const std::string& value)
{
	AddEntry(name, value);
}

bool Metadata::GetString(const std::string& name, std::string* value) const
{
	return GetEntry(name, value);
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

const Metadata* Metadata::GetSubMetadata(const std::string& name) const
{
	auto sub_ptr = m_SubMetadatas.find(name);
	if (sub_ptr == m_SubMetadatas.end()) {
		return nullptr;
	}
	return sub_ptr->second.get();
}

Metadata* Metadata::GetSubMetadata(const std::string& name)
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