//
// Created by m_ky on 2024/7/14.
//

/**
 * @class   iGamePVDReader
 * @brief   iGamePVDReader's brief
 */

#include "iGamePVDReader.h"
#include "iGameFileReader.h"
#include "iGameVTSReader.h"
#include "iGameVTUReader.h"
#include "iGameDataObject.h"
#include "iGameStringArray.h"

#include <tinyxml2.h>

bool iGame::iGamePVDReader::Parsing() {
    std::string fileDir = this->m_FilePath.substr(0, this->m_FilePath.find_last_of('/') + 1);
    const char* existAttribute;
    tinyxml2::XMLElement* elem = root->FirstChild()->FirstChildElement("DataSet");
//    DataObject::Pointer newObj;
    std::map<float, StringArray::Pointer> child_map;

    while (elem) {

        existAttribute = elem->Attribute("timestep");
        //  判断是否有timeStep元素, 没有则默认TimeStep为0
        float t = 0.f;
        if (existAttribute)
        {
            t = mAtof(existAttribute);
        }
        existAttribute = elem->Attribute("file");
        if(!child_map.count(t))
        {
            child_map[t] = StringArray::New();
        }
        if(existAttribute)
        {
            std::string fileName(existAttribute);
            child_map[t]->AddElement(fileDir + fileName);

        }
//        if(!cht]->AddSubDataObject(newObj);
        elem = elem->NextSiblingElement("DataSet");
    }

    for(auto& [val, strArray] : child_map){
        m_Data.GetTimeData()->AddTimeStep(val, strArray);
    }
    if(!m_Data.GetTimeData()->GetArrays().empty()){
        auto& firstFrame = m_Data.GetTimeData()->GetArrays()[0];
        DataObject::Pointer  newObj;
        m_data_object = DataObject::New();
        m_data_object->SetTimeFrames(m_Data.GetTimeData());
        std::string fileName, fileSuffix;
        for(int i = 0; i < firstFrame.SubFileNames->Size(); i ++){
            fileName = firstFrame.SubFileNames->GetElement(i);
            const char* pos = strrchr(fileName.data(), '.');
            if (pos != nullptr) {
                const char *fileEnd = fileName.data() + fileName.size();
                fileSuffix = std::string(pos + 1, fileEnd);
            }
            if(fileSuffix == "vts"){
                iGameVTSReader::Pointer rd = iGameVTSReader::New();
                rd->SetFilePath(fileName);
                rd->Execute();
                newObj = rd->GetOutput();
            }
            else if(fileSuffix == "vtu"){
                iGameVTUReader::Pointer rd = iGameVTUReader::New();
                rd->SetFilePath(fileName);
                rd->Execute();
                newObj = rd->GetOutput();
            }
            m_data_object->AddSubDataObject(newObj);
        }
    }

    return true;
}

bool iGame::iGamePVDReader::CreateDataObject() {
    if(m_data_object != nullptr) {
        m_Output = m_data_object;
        m_Output->SwitchToCurrentTimeframe(0);
        return true;
    }
    return iGameXMLFileReader::CreateDataObject();
}