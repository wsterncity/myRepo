#include "iGameINPReader.h"

IGAME_NAMESPACE_BEGIN
bool INPReader::Parsing()
{
	Points::Pointer Points = m_Data.GetPoints();
	CellArray::Pointer Faces = m_Data.GetFaces();
	CellArray::Pointer Volumes = m_Data.GetVolumes();
	char buffer[256];

	while (1) {
		if (!this->ReadString(buffer))break;
		this->LowerCase(buffer);
		if (!strncmp(buffer, "*part,", 6)) {
			if (!this->ReadString(buffer) || strncmp(LowerCase(buffer), "name=", 5)) {
				igError("read part name error!");
				return false;
			}
			char partName[256];
			for (int i = 5;; i++) {
				partName[i - 5] = buffer[i];
				if (buffer[i] == '\0')break;
			}
			std::cout << "Read part with name = " << partName << '\n';
		}
		//read nodes
		else if (!strncmp(buffer, "*node", 5)) {
			igIndex id = 0;
			Point p;
			while (this->Read(&id))
			{
				for (int i = 0; i < 3; i++) {
					this->IS++;
					this->Read(&p[i]);
				}
				Points->AddPoint(p);
				while (*this->IS == ' ' || *this->IS == '\n' || *this->IS == '\r')this->IS++;
				if (*this->IS == '*')break;
			}
		}
		//read elements
		else if (!strncmp(buffer, "*element,", 9)) {
			if (!this->ReadString(buffer) || strncmp(LowerCase(buffer), "type=", 5)) {
				igError("read part elements type error!");
				return false;
			}
			char Type[256];
			for (int i = 5;; i++) {
				Type[i - 5] = buffer[i];
				if (buffer[i] == '\0')break;
			}
			this->IS -= 5;
			this->ReadLine(buffer);
			igIndex id = 0;
			igIndex vhs[64];
			int cellVcnt = 0;
			while (this->Read(&id))
			{
				std::cout << id << '\n';
				if (!strncmp(Type, "s4", 2))
				{
					cellVcnt = 4;
					for (int i = 0; i < cellVcnt; i++) {
						this->IS++;
						this->Read(&vhs[i]);
						vhs[i]--;
					}
					Faces->InsertNextCell(vhs, 4);
				}
				else if (!strncmp(Type, "c3d8r", 5))
				{
					cellVcnt = 8;
					for (int i = 0; i < cellVcnt; i++) {
						this->IS++;
						this->Read(&vhs[i]);
						vhs[i]--;
					}
					Volumes->InsertNextCell(vhs, 8);
				}
				while (*this->IS == ' ' || *this->IS == '\n' || *this->IS == '\r')this->IS++;
				if (*this->IS == '*')break;
			}
		}
	}
	return true;
}
IGAME_NAMESPACE_END