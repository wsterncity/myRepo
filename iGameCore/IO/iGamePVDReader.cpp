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

#include <tinyxml.h>

bool iGame::iGamePVDReader::Parsing() {
    std::string fileDir = this->m_FilePath.substr(0, this->m_FilePath.find_last_of('/') + 1);
    const char* existAttribute;
    TiXmlElement* elem = root->FirstChild()->FirstChildElement("DataSet");
    DataObject::Pointer newObj;

    while (elem) {

        existAttribute = elem->Attribute("timestep");
        //  判断是否有timeStep元素, 没有则默认TimeStep为0
        float t = 0.f;
        if (existAttribute)
        {
            t = mAtof(existAttribute);
        }
        existAttribute = elem->Attribute("file");
        if(existAttribute)
        {
            std::string fileName(existAttribute);
            std::string fileSuffix;
            const char* pos = strrchr(fileName.data(), '.');
            if (pos != nullptr) {
                const char *fileEnd = fileName.data() + fileName.size();
                fileSuffix = std::string(pos + 1, fileEnd);
            }
            if(fileSuffix == "vts"){
                iGameVTSReader::Pointer rd = iGameVTSReader::New();
                rd->SetFilePath(fileDir + std::string(existAttribute));
                rd->Update();
                newObj = rd->GetOutput();
            }
            else if(fileSuffix == "vtu"){
                iGameVTUReader::Pointer rd = iGameVTUReader::New();
                rd->SetFilePath(fileDir + std::string(existAttribute));
                rd->Update();
                newObj = rd->GetOutput();
            }
        }
        if(!child_map.count(t))
        {
            child_map[t] = DataObject::New();
        }
        child_map[t]->AddSubDataObject(newObj);
        elem = elem->NextSiblingElement("DataSet");
    }

    return true;
}

bool iGame::iGamePVDReader::CreateDataObject() {
//    if(child_map.size() == 1) m_Output = child_map.begin()->second;
    m_Output = child_map.begin()->second;


    return true;
}
