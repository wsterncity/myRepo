//
// Created by m_ky on 2024/7/14.
//

/**
 * @class   iGameXMLFileReader
 * @brief   iGameXMLFileReader's brief
 */

#include "iGameXMLFileReader.h"
#include "iGameVolumeMesh.h"
#include <tinyxml.h>

IGAME_NAMESPACE_BEGIN

iGameXMLFileReader::iGameXMLFileReader() {
    SetNumberOfInputs(0);
    SetNumberOfOutputs(1);
}


void iGameXMLFileReader::SetFilePath(const std::string &filePath) {
    this->m_FilePath = filePath;
    this->m_FileName = filePath.substr(filePath.find_last_of('/') + 1, filePath.size());
}

bool iGameXMLFileReader::Execute() {
    clock_t start, end;
    start = clock();

    if(!Open()){
        return false;
    }
    if (!Parsing())
    {
        std::cerr << "Parsing failure\n";
        return false;
    }
    CreateDataObject();

    int size = m_Output->GetAttributes()->GetArrays().size();

    if (size > 0) {
        StringArray::Pointer attrbNameArray = StringArray::New();
        for (int i = 0; i < size; i++) {
            auto& data = m_Output->GetAttributes()->GetAttribute(i);
            attrbNameArray->InsertToBack(data.array->GetName());
        }
        m_Output->GetMetadata()->AddStringArray(ATTRIBUTE_NAME_ARRAY, attrbNameArray);
    }
    m_Output->SetName(m_FileName);
    SetOutput(0, m_Output);
    end = clock();
    std::cout << "Read file success!" << std::endl;
    std::cout << "   The time cost: " << end - start << "ms" << std::endl;
    delete doc;
    return true;
}

bool iGameXMLFileReader::Open() {
    if(m_FilePath.empty()) {
        printf("[XML parser]:FilePath is empty. Exiting.\n");
        return false;
    }
    doc = new TiXmlDocument((char*)m_FilePath.data());
    if (!doc->LoadFile()) {
        printf("[XML parser]:Could not load file: %s . Error='%s'. Exiting.\n", m_FilePath.c_str(), doc->ErrorDesc());
        return false;
    }
    root = doc->RootElement(); // <VTKFile>

    return true;
}

bool iGameXMLFileReader::CreateDataObject() {

    // 统计各类型元素的数量
    int numFaces = m_Data.GetNumberOfFaces();
    int numVolumes = m_Data.GetNumberOfVolumes();
    int numPoints = m_Data.GetNumberOfPoints();
    int numLines = m_Data.GetNumberOfLines();

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

        //  单独点集判断
    else if(numPoints){
        PointSet::Pointer pointSet = PointSet::New();
        pointSet->SetPoints(m_Data.GetPoints());
        pointSet->SetAttributes(m_Data.GetData());
        m_Output = pointSet;
    }
    //  单独边集判断
    else if(numLines){

    }
    if(!m_Data.GetTimeData()->GetArrays().empty())  m_Output->SetTimeFrames(m_Data.GetTimeData());
    return true;
}

TiXmlElement *iGameXMLFileReader::FindTargetItem(TiXmlElement *root, const char *itemName) {
    if(root == nullptr) return nullptr;
    TiXmlElement* res = root->FirstChildElement(itemName);
    if(res) return res;
    res = FindTargetItem(root->FirstChildElement(), itemName);
    if(res) return res;
    res = FindTargetItem(root->NextSiblingElement(), itemName);
    return res;
}

TiXmlElement * iGameXMLFileReader::FindTargetAttributeItem(TiXmlElement *root, const char *itemName, const char *attributeName,
                                            const std::string &attributeData) {
    if(root == nullptr) return nullptr;
    if(std::string(root->Value()) == std::string(itemName) && std::string(root->Attribute(attributeName)) == attributeData){
        return root;
    }
    TiXmlElement* res = FindTargetAttributeItem(root->FirstChildElement(), itemName, attributeName, attributeData);
    if(res) return res;
    res = FindTargetAttributeItem(root->NextSiblingElement(), itemName, attributeName, attributeData);
    return res;
}



IGAME_NAMESPACE_END