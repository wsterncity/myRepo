//#include "iGameVTKReader.h"
//
//IGAME_NAMESPACE_BEGIN
//bool VTKReader::Parsing()
//{
//	int i, PointsNum = 0, CellsNum = 0;
//	char line[256];
//	int npts, size = 0, ncells = 0;
//	int piece = 0, numPieces = 0, skip1 = 0, read2 = 0, skip3 = 0, tmp = 0;
//	/*iGameCellArray* cells = nullptr;*/
//	IntArray::Pointer Types;
//	IntArray::Pointer Cells;
//	DataArray::Pointer CellsId;
//	DataArray::Pointer CellsConnect;
//
//	ReadHeader();
//	// Read unstructured grid specific stuff
//	//
//	if (!this->ReadString(line)) {
//		igError("Data file ends prematurely!");
//		return;
//	}
//	if (!strncmp(this->LowerCase(line), "dataset", 7)) {
//		// Make sure we're reading right type of geometry
//		//
//		if (!this->ReadString(line)) {
//			igError("Data file ends prematurely!");
//			return;
//		}
//		if (strncmp(this->LowerCase(line), "unstructured_grid", 17) != 0) {
//			igError("Cannot read dataset type: " << line);
//			return;
//		}
//		while (true) {
//			if (!this->ReadString(line)) {
//				break;
//			}
//			if (line[0] == '\0') {
//				continue;
//			}
//			else if (!strncmp(this->LowerCase(line), "field", 5)) {
//				this->ReadFieldData();
//			}
//			else if (!strncmp(line, "points", 6)) {
//				if (!this->Read(&PointsNum)) {
//					igError("Cannot read number of points!");
//					return;
//				}
//				iGame::iGamePoints* points = iGame::iGamePoints::New();
//				if (!this->ReadPointCoordinates(points, PointsNum)) {
//					return;
//				}
//				this->DataSet->SetPoints(points);
//			}
//			else if (!strncmp(line, "cells", 5)) {
//				if (this->FileType == IGAME_BINARY) {
//					this->ReadBinaryCells(ncells, CellsId, CellsConnect);
//					// Update the dataset
//					if (Types) {
//						this->TransferVtkCellToiGameCell(CellsId, CellsConnect, Types);
//						//this->TransferVtkCellToiGameCell(((iGameLongLongArray*)CellsId)->GetRawPointer(), ((iGameLongLongArray*)CellsConnect)->GetRawPointer(), Types);
//					}
//				}
//				else {// ascii
//					Cells = iGame::iGameIntArray::New();
//					if (!(this->Read(&ncells) && this->Read(&size))) {
//						igError("Cannot read cells!");
//						return;
//					}
//					this->ReadAsciiCells(ncells, Cells);
//					// Update the dataset
//					if (Types) {
//						this->TransferVtkCellToiGameCell(Cells, Types);
//					}
//				}
//			}
//			else if (!strncmp(line, "cell_types", 10)) {
//				if (!this->Read(&ncells)) {
//					igError("Cannot read cell types!");
//					return;
//				}
//				Types = (iGameIntArray*)this->ReadArray("int", ncells, 1);
//				// allocate array for piece cell types
//				if (this->FileType == IGAME_BINARY) {
//					iGameByteSwap::Swap4BERange(Types, read2);
//					// Update the dataset
//					if (CellsId && CellsConnect) {
//						this->TransferVtkCellToiGameCell(CellsId, CellsConnect, Types);
//						//this->TransferVtkCellToiGameCell(((iGameLongLongArray*)CellsId)->GetRawPointer(), ((iGameLongLongArray*)CellsConnect)->GetRawPointer(), Types);
//					}
//				}
//				else {// ascii
//					// Update the dataset
//					if (Cells) {
//						this->TransferVtkCellToiGameCell(Cells, Types);
//					}
//				}
//			}
//			else if (!strncmp(line, "cell_data", 9)) {
//				if (!this->Read(&CellsNum)) {
//					igError("Cannot read cell data!");
//					return;
//				}
//				if (ncells != CellsNum) {
//					igError("Number of cells don't match!");
//					return;
//				}
//				this->ReadCellData(this->DataSet, ncells);
//				break; // out of this loop
//			}
//
//			else if (!strncmp(line, "point_data", 10)) {
//				if (!this->Read(&npts)) {
//					igError("Cannot read point data!");
//					return;
//				}
//				if (npts != PointsNum) {
//					igError("Number of points don't match!");
//					return;
//				}
//				this->ReadPointData(this->DataSet, npts);
//				break; // out of this loop
//			}
//			else {
//				//igDebug("Unrecognized keyword: " << line);
//				//return;
//			}
//		}
//	}
//	else if (!strncmp(line, "point_data", 10)) {
//		igDebug("No geometry defined in data file!");
//		if (!this->Read(&PointsNum)) {
//			igError("Cannot read point data!");
//			return;
//		}
//		this->ReadPointData(this->DataSet, PointsNum);
//	}
//	else {
//		igError("Unrecognized keyword: " << line);
//	}
//	delete Cells;
//	delete Types;
//	delete CellsId;
//	delete CellsConnect;
//	this->UpdateProgress(1.0);
//	igDebug("Read " << this->DataSet->GetNumberOfPoints() << " points,"
//		<< this->DataSet->GetNumberOfCells() << " cells.\n");
//	return true;
//}
//IGAME_NAMESPACE_END