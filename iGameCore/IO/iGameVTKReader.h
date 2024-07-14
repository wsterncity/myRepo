#ifndef iGameVTKReader_h
#define iGameVTKReader_h

#include "iGameVTKAbstractReader.h"

IGAME_NAMESPACE_BEGIN

class VTKReader : public VTKAbstractReader {
public:
	I_OBJECT(VTKReader);
	static Pointer New() { return new VTKReader; }

	bool Parsing() override;

	int ReadBinaryCells(int CellNum, DataArray::Pointer& CellsId, DataArray::Pointer& CellsConnect)
	{
		int offsetsSize{ 0 };
		int connSize{ 0 };
		char buffer[256];

		if (!(this->Read(&offsetsSize) && this->Read(&connSize))) {
			igError("Error while reading cell array header.");
			return 0;
		}
		if (offsetsSize < 1) {
			return 1;
		}

		if (!this->ReadString(buffer) || // "offsets"
			(strcmp(this->LowerCase(buffer, 256), "offsets") != 0) || !this->ReadString(buffer)) {// datatype
			igError("Error reading cell array offset header.");
			return 0;
		}

		this->LowerCase(buffer, 256);

		CellsId = this->ReadArray(buffer, offsetsSize, 1);
		//for (int i = 0; i < offsets->GetNumberOfValues(); i++) {
		//	std::cout << offsets->GetValue(i) << '\n';
		//}
		if (!CellsId)
		{
			igError("Error reading cell array offset data.");
			return 0;
		}

		if (!this->ReadString(buffer) || // "connectivity"
			(strcmp(this->LowerCase(buffer, 256), "connectivity") != 0) ||
			!this->ReadString(buffer)) // datatype
		{
			igError("Error reading cell array connectivity header.");
			return 0;
		}

		this->LowerCase(buffer, 256);

		CellsConnect = this->ReadArray(buffer, connSize, 1);
		//for (int i = 0; i < conn->GetNumberOfValues(); i++) {
		//	std::cout << conn->GetValue(i) << '\n';
		//}
		if (!CellsConnect)
		{
			igError("Error reading cell array connectivity data.");
			return 0;
		}
		this->UpdateReadProgress();
		return 1;
	}

	int ReadAsciiCells(int CellNum, IntArray::Pointer Cells)
	{
		if (!Cells) Cells = IntArray::New();
		int component, i, j;
		int x;
		for (i = 0; i < CellNum; i++) {
			if (!this->Read(&component)) {
				igError("Cell id error!");
				return 0;
			}
			Cells->InsertNextValue(static_cast<double>(component));
			for (j = 0; j < component; j++) {
				if (!this->Read(&x)) {
					igError("Cell id error!");
					return 0;
				}
				Cells->InsertNextValue(static_cast<double>(x));
			}
		}
		this->UpdateReadProgress();
		return 1;
	}

protected:
	VTKReader() = default;
	~VTKReader() override = default;
};

IGAME_NAMESPACE_END
#endif