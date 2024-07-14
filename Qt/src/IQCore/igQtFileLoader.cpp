﻿//
// Created by m_ky on 2024/4/22.
//
/**
 * @class   igQtFileLoader
 * @brief   igQtFileLoader's brief
 */
#include <cstring>

#include "iGameFileIO.h"
#include "iGameScene.h"
#include "iGamePointSet.h"
#include "iGameDrawableObject.h"

#include <IQCore/igQtFileLoader.h>
#include <QCoreApplication.h>
#include <qsettings.h>
#include <qdebug.h>
#include <qaction.h>
#include <iostream>

igQtFileLoader::igQtFileLoader(QObject* parent) : QObject(parent) 
{
	InitRecentFilePaths();

}

igQtFileLoader::~igQtFileLoader() {
}

void igQtFileLoader::LoadFile() 
{
	std::string filePath = QFileDialog::getOpenFileName(nullptr, "Load file", "", "ALL FIle(*.obj *.off *.stl *.ply *.vtk *.mesh *.pvd *.vts *.vtu *.vtm)").toStdString();
	this->OpenFile(filePath);
}

void igQtFileLoader::OpenFile(const std::string& filePath)
{
	using namespace iGame;
	if (filePath.empty() || strrchr(filePath.data(), '.') == nullptr)return;
	

	Q_EMIT EmitMakeCurrent();
	DataObject::Pointer multiData = DataObject::New();
	multiData->SetUniqueDataObjectId();

	auto obj = iGame::FileIO::ReadFile(filePath);
	//obj->SetUniqueDataObjectId();
	SurfaceMesh::Pointer mesh = DynamicCast<SurfaceMesh>(obj);

	FloatArray::Pointer points = mesh->GetPoints()->ConvertToDataArray();
	FloatArray::Pointer pointScalar = FloatArray::New();
	FloatArray::Pointer cellScalar = FloatArray::New();
	pointScalar->SetName("pointScalar");
	cellScalar->SetName("cellScalar");
	float tu[3];
	for (int i = 0; i < points->GetNumberOfTuples(); i++)
	{
		points->GetTuple(i, tu);
		pointScalar->InsertNextValue(tu[0]);
	}

	for (int i = 0; i < mesh->GetNumberOfFaces(); i++)
	{
		Face* face = mesh->GetFace(i);
		cellScalar->InsertNextValue(face->Points->GetPoint(0)[0]);
	}
	mesh->GetAttributes()->AddScalars(IG_POINT, pointScalar);
	mesh->GetAttributes()->AddScalars(IG_CELL, cellScalar);

	mesh->GetMetadata()->AddString(FILE_PATH, filePath);
	mesh->GetMetadata()->AddString(FILE_NAME, filePath.substr(filePath.find_last_of('/') + 1));
	mesh->GetMetadata()->AddString(FILE_SUFFIX, filePath.substr(filePath.find_last_of('.') + 1));

//	auto obj1 = iGame::FileIO::ReadFile("H:/iGameProjects/model/obj/horse.obj");
	auto obj1 = iGame::FileIO::ReadFile("C:\\Users\\m_ky\\Desktop\\Resource\\Model\\bunny.obj");
	obj1->SetUniqueDataObjectId();
//	auto obj1 = iGame::FileIO::ReadFile(filePath);
	SurfaceMesh::Pointer mesh1 = DynamicCast<SurfaceMesh>(obj1);

	points = mesh1->GetPoints()->ConvertToDataArray();
	pointScalar = FloatArray::New();
	cellScalar = FloatArray::New();
	pointScalar->SetName("pointScalar");
	cellScalar->SetName("cellScalar");
	for (int i = 0; i < points->GetNumberOfTuples(); i++)
	{
		points->GetTuple(i, tu);
		pointScalar->InsertNextValue(tu[0]);
	}

	for (int i = 0; i < mesh1->GetNumberOfFaces(); i++)
	{
		Face* face = mesh1->GetFace(i);
		cellScalar->InsertNextValue(face->Points->GetPoint(0)[0]);
	}
	mesh1->GetAttributes()->AddScalars(IG_POINT, pointScalar);
	mesh1->GetAttributes()->AddScalars(IG_CELL, cellScalar);
	

	
	multiData->AddSubDataObject(mesh);
	multiData->AddSubDataObject(obj1);
	multiData->SetViewStyle(IG_SURFACE);

	StringArray::Pointer attrbNameArray = StringArray::New();
	attrbNameArray->InsertToBack("pointScalar");
	attrbNameArray->InsertToBack("cellScalar");
	multiData->GetMetadata()->AddStringArray(ATTRIBUTE_NAME_ARRAY, attrbNameArray);

	//for (int i = 0; i < mesh1->GetNumberOfFaces(); i++)
	//{
	//	Face* face = mesh1->GetFace(i);
	//	cellScalar->InsertNextValue(face->Points->GetPoint(0)[0]);
	//}
	//mesh1->GetAttributes()->AddScalars(IG_POINT, pointScalar);
	//mesh1->GetAttributes()->AddScalars(IG_CELL, cellScalar);
	m_SceneManager->GetCurrentScene()->AddDataObject(multiData);


	//auto obj2 = iGame::FileIO::ReadFile("H:/iGameProjects/model/obj/bunny.obj");
	//SurfaceMesh::Pointer mesh2 = DynamicCast<SurfaceMesh>(obj2);

	//points = mesh2->GetPoints()->ConvertToDataArray();
	//pointScalar = FloatArray::New();
	//cellScalar = FloatArray::New();
	//pointScalar->SetName("pointScalar");
	//cellScalar->SetName("cellScalar");
	//for (int i = 0; i < points->GetNumberOfTuples(); i++)
	//{
	//	points->GetTuple(i, tu);
	//	pointScalar->InsertNextValue(tu[0]);
	//}

	//for (int i = 0; i < mesh2->GetNumberOfFaces(); i++)
	//{
	//	Face* face = mesh2->GetFace(i);
	//	cellScalar->InsertNextValue(face->Points->GetPoint(0)[0]);
	//}
	//mesh2->GetAttributes()->AddScalars(IG_POINT, pointScalar);
	//mesh2->GetAttributes()->AddScalars(IG_CELL, cellScalar);
	//m_SceneManager->GetCurrentScene()->AddDataObject(mesh2);

	Q_EMIT EmitDoneCurrent();
	

	this->SaveCurrentFileToRecentFile(QString::fromStdString(filePath));
	Q_EMIT AddFileToModelList(QString(filePath.substr(filePath.find_last_of('/') + 1).c_str()));
	Q_EMIT FinishReading();

}

void igQtFileLoader::SaveFile() {
	//auto currentModel = iGame::iGameManager::Instance()->GetCurrentModel();
	//if (currentModel == nullptr)return;
	//if (!iGame::iGameFileIO::WriteDataSetToFile(currentModel->DataSet, currentModel->filePath)) {
	//	igDebug("Save File Error\n");
	//}

}
void igQtFileLoader::SaveFileAs() {
	//auto currentModel = iGame::iGameManager::Instance()->GetCurrentModel();
	//if (currentModel == nullptr)return;
	//std::string filePath = QFileDialog::getSaveFileName(nullptr, "Save file as ", "", "Surface Mesh(*.obj *.off *.stl *.vtk);;Volume Mesh(*.mesh *.vtk *.ex2 *.e *.pvd *.vts)").toStdString();
	//if (filePath.empty()) {
	//	igDebug("Could not save file with error file path\n");
	//	return;
	//}
	//if (!iGame::iGameFileIO::WriteDataSetToFile(currentModel->DataSet, filePath)) {
	//	igDebug("Save File Error\n");
	//}
}

void igQtFileLoader::SaveCurrentFileToRecentFile(QString path)
{
	if (path.isEmpty())
		return;
	for (int i = 0; i < recentFileActionList.size(); i++) {
		if (recentFileActionList.at(i)->data() == path) {
			delete recentFileActionList.at(i);
			recentFileActionList.removeAt(i);
			break;
		}
	}
	AddCurrentFileToRecentFilePath(path);
	UpdateIniFileInfo();
	return;
}
void igQtFileLoader::AddCurrentFileToRecentFilePath(QString filePath)
{
	auto recentFileActions = this->GetRecentActionList();
	QAction* recentFileAction = nullptr;
	recentFileAction = new QAction(this);
	recentFileAction->setText(filePath);
	recentFileAction->setData(filePath);
	recentFileAction->setVisible(true);
	connect(recentFileAction, &QAction::triggered, this, [=]() {
		this->OpenFile(filePath.toStdString());
		});
	this->recentFileActionList.append(recentFileAction);
	UpdateRecentActionList();
}
void igQtFileLoader::UpdateIniFileInfo()
{
	//为了能记住上次打开的路径
	QSettings setting(QCoreApplication::applicationDirPath() + "/config/savePath.ini", QSettings::IniFormat);
	int num = this->recentFileActionList.size();
	int idx = 0;
	for (int i = 0; i < num; i++) {
		if (recentFileActionList.at(i)->isVisible()) {
			idx++;
			QString name = "LastFilePath" + QString::fromStdString(std::to_string(idx));
			setting.setValue(name, this->recentFileActionList[i]->data());
		}
	}
}
void igQtFileLoader::InitRecentFilePaths()
{
	QString path = QCoreApplication::applicationDirPath() + "/config/savePath.ini";
	QFile* file = new QFile(this);
	std::vector<QString>FilePaths;
	file->setFileName(path);
	if (!file->open(QIODevice::ReadOnly)) {
		return;
	}
	while (!file->atEnd()) {
		QString str = file->readLine();
		//std::cout << str.toStdString()<< std::endl;
		if (str.toStdString().find('=') == std::string::npos)continue;
		QStringList list = str.split("=");
		if (!list.isEmpty()) {
			FilePaths.emplace_back(list.at(1).trimmed());
		}
	}
	file->close();
	delete file;
	InitRecentFileActions(FilePaths);
}


void igQtFileLoader::InitRecentFileActions(std::vector<QString> FilePaths)
{
	QAction* recentFileAction = nullptr;
	for (int i = 0; i < FilePaths.size(); i++) {
		recentFileAction = new QAction(this);
		recentFileAction->setText(FilePaths[i]);
		recentFileAction->setData(FilePaths[i]);
		recentFileAction->setVisible(false);
		connect(recentFileAction, &QAction::triggered, this, [=]() {
			this->OpenFile(FilePaths[i].toStdString());
			});
		this->recentFileActionList.append(recentFileAction);
	}
	UpdateRecentActionList();
	return;
}
void igQtFileLoader::UpdateRecentActionList() {
	int st = this->recentFileActionList.size() - 1;;
	int ed = std::max(st - maxFileNr + 1, 0);
	for (int i = st; i >= ed; i--) {
		this->recentFileActionList.at(i)->setVisible(true);

	}
	for (int i = ed - 1; i >= 0; i--) {
		this->recentFileActionList.at(i)->setVisible(false);
	}
	return;
}
