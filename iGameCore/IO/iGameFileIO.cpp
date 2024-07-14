#include "iGameFileIO.h"

#include "iGameVTKReader.h"
//#include "iGameOFFReader.h"
//#include "iGameMESHReader.h"
#include "iGameOBJReader.h"
//#include "iGameSTLReader.h"
//#include "iGamePLYReader.h"
//#include "iGamePVDReader.h"
//#include "iGameVTSReader.h"
//#include "iGameVTMReader.h"
//#include "iGameVTUReader.h"
//#include "CGNS/iGameCGNSReader.h"
//#include "INP/iGameINPReader.h"

IGAME_NAMESPACE_BEGIN
IGenum FileIO::GetFileType(const std::string& file_name)
{
	const char* pos = strrchr(file_name.data(), '.');
	const char* fileEnd = file_name.data() + file_name.size();
	std::string FileSuffix;
	if (pos != nullptr) {
		FileSuffix = std::string(pos + 1, fileEnd);
	}
	if (FileSuffix == "vtk") {
		return VTK;
	}
	else if (FileSuffix == "obj") {
		return OBJ;
	}
	else if (FileSuffix == "mesh"|| FileSuffix == "MESH") {
		return MESH;
	}
	else if (FileSuffix == "off") {
		return OFF;
	}
	else if (FileSuffix == "stl") {
		return STL;
	}
	else if (FileSuffix == "ply") {
		return PLY;
	}
	else if (FileSuffix == "step") {
		return STEP;
	}
	else if (FileSuffix == "igs" || FileSuffix == "iges" || FileSuffix == "IGS" || FileSuffix == "IGES") {
		return IGES;
	}
    else if(FileSuffix == "pvd"){
        return PVD;
    }
    else if(FileSuffix == "vts"){
        return VTS;
    }
    else if(FileSuffix == "vtu"){
        return VTU;
    }
    else if(FileSuffix == "vtm"){
        return VTM;
    }
    else if(FileSuffix == "e" || FileSuffix == "ex2" || FileSuffix == "EX2"){
        return EX2;
    }
	else if (FileSuffix == "cgns") {
		return CGNS;
	}
	else if (FileSuffix == "inp") {
		return INP;
	}
	return NONE;
}

std::string FileIO::GetFileTypeAsString(IGenum type)
{
	switch (type)
	{
	case NONE:return "NONE";
	case VTK:return "VTK";
	case OBJ:return "OBJ";
	case OFF:return "OFF";
	case MESH:return "MESH";
	case STL:return "STL";
	case PLY:return "PLY";
	case STEP:return "STEP";
	case IGES:return "IGES";
	case PVD:return "PVD";
	case VTS:return "VTS";
	case VTM:return "VTM";
	case VTU:return "VTU";
	case CGNS:return "CGNS";
	case INP:return "INP";
	default:return "NONE";
	}
}

DataObject::Pointer FileIO::ReadFile(const std::string &file_name) 
{
	IGenum fileType = GetFileType(file_name);
    std::string out;
    out.append("Read file [type: ");
    out.append(GetFileTypeAsString(fileType));
    clock_t start, end;
    DataObject::Pointer resObj = nullptr;

    start = clock();
    switch (fileType)
    {
        case NONE:
        {
            break;
        }
        case VTK:
        {
			VTKReader::Pointer reader = VTKReader::New();
			reader->SetFilePath(file_name);
			reader->Update();
			resObj = reader->GetOutput();
            break;
        }
        case OBJ:
        {
			OBJReader::Pointer reader = OBJReader::New();
            reader->SetFilePath(file_name);
			reader->Update();
			resObj = reader->GetOutput();
            break;
        }
  //      case iGame::FileIO::OFF:
  //      {
  //          auto app = iGameOFFReader::New();
  //          resObj = app->ReadFile(file_name);
  //          delete app;
  //          break;
  //      }
  //      case iGame::FileIO::MESH:
  //      {
  //          auto app = iGameMESHReader::New();
  //          resObj = app->ReadFile(file_name);
  //          delete app;
  //          break;
  //      }
  //      case iGame::FileIO::STL:
  //      {
  //          auto app = iGameSTLReader::New();
  //          resObj = app->ReadFile(file_name);
  //          delete app;
  //          break;
  //      }
  //      case iGame::FileIO::PLY:
  //      {
  //          auto app = iGamePLYReader::New();
  //          resObj = app->ReadFile(file_name);
  //          delete app;
  //          break;
  //      }
		//case iGame::FileIO::CGNS:
		//{
		//	//auto app = iGameCGNSReader::New();
		//	//resObj = app->ReadFile(file_name);
		//	//delete app;
		//}
		//break;
		//case iGame::FileIO::INP:
		//{
		//	auto app = iGameINPReader::New();
		//	resObj = app->ReadFile(file_name);
		//	delete app;
		//}
		//break;
  //      case iGame::FileIO::STEP:
  //      {

  //      }
  //          break;
  //      case iGame::FileIO::IGES:
  //      {

  //      }
  //          break;
//        case iGame::FileIO::VTS:
//        {
//            auto app = iGameVTSReader();
//            resObj = app.ReadFile(file_name);
//            break;
//        }
  //      case iGame::FileIO::VTU:
  //      {
  //          auto app = iGameVTUReader();
  //          resObj = app.ReadFile(file_name);
  //          break;
  //      }

  //      case iGame::FileIO::PVD:
  //      {
  //          auto app = iGamePVDReader();
  //          resObj = app.ReadFile(file_name);
  //          break;
  //      }
  //      case iGame::FileIO::VTM:
  //      {
  //          auto app = iGameVTMReader::New();
  //          resObj = app->ReadFile(file_name);
  //          delete app;
  //          break;
  //      }
        default:
            break;
    }

    end = clock();
    out.append(", success: ");
    out.append(resObj != nullptr ? "true" : "false");
    out.append(", time: ");
    out.append(FormatTime(end - start));
    out.append("]");
    //igDebug(out);
	std::cout << out << std::endl;
    return resObj;
}
IGAME_NAMESPACE_END
