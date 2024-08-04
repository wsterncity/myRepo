//
// Created by m_ky on 2024/7/21.
//

/**
 * @class   iGameVTMReader
 * @brief   iGameVTMReader's brief
 */

#include "iGameVTMReader.h"
#include <tinyxml2.h>
#include "iGameVTSReader.h"
#include "iGameVTUReader.h"


IGAME_NAMESPACE_BEGIN
bool iGameVTMReader::Parsing() {
    std::string fileDir = this->m_FilePath.substr(0, this->m_FilePath.find_last_of('/') + 1);
    const char* existAttribute;


    tinyxml2::XMLElement* BlockElem = FindTargetItem(root, "vtkMultiBlockDataSet")->FirstChildElement("Block");
    tinyxml2::XMLElement* elem;

    std::vector<DataObject::Pointer> overall_multiBlock;
    DataObject::Pointer newObj;
    while(BlockElem)
    {
        elem = BlockElem->FirstChildElement();

        DataObject::Pointer curMultiBlock = DataObject::New();
        while(elem)
        {
            // TODO: Not finish Reading the Nested block.
//            std::string elemType(elem->Value());

            existAttribute = elem->Attribute("file");
            if(existAttribute)
            {
                std::string fileName(existAttribute);
                std::string fileSuffix;
                const char* pos = strrchr(fileName.data(), '.');
                if (pos != nullptr)
                {
                    const char *fileEnd = fileName.data() + fileName.size();
                    fileSuffix = std::string(pos + 1, fileEnd);
                }
//                std::cout << fileDir + std::string(existAttribute) << '\n';
                if(fileSuffix == "vts"){
                    iGameVTSReader::Pointer rd = iGameVTSReader::New();
                    rd->SetFilePath(fileDir + std::string(existAttribute));
                    rd->Execute();
                    newObj = rd->GetOutput();
                }
                else if(fileSuffix == "vtu"){
                    iGameVTUReader::Pointer rd = iGameVTUReader::New();
                    rd->SetFilePath(fileDir + std::string(existAttribute));
                    rd->Execute();
                    newObj = rd->GetOutput();
                }
            }
            if(newObj){
                curMultiBlock->AddSubDataObject(newObj);
            }
            elem = elem->NextSiblingElement();
        }

        overall_multiBlock.push_back(curMultiBlock);
        BlockElem = BlockElem->NextSiblingElement();
    }


    if(overall_multiBlock.empty()) {
        return false;
    } else if(overall_multiBlock.size() == 1) {
        parseData = overall_multiBlock.front();
        return true;
    } else {
        DataObject::Pointer rootDataObject = DataObject::New();
        for(const auto& mp : overall_multiBlock) rootDataObject->AddSubDataObject(mp);

        parseData = rootDataObject;
        return true;
    }
}

bool iGameVTMReader::CreateDataObject() {
    m_Output = parseData;

    return true;
}


IGAME_NAMESPACE_END
