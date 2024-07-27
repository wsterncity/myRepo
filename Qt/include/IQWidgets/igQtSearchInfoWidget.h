/**
 * @class   igQtSearchInfoWidget
 * @brief   igQtSearchInfoWidget's brief
 */

#pragma once
#include <ui_SearchInfo.h>
#include <iGameVector.h>
//namespace iGame {
//	class iGameIntArray;
//	class iGameFloatArray;
//}

class igQtSearchInfoWidget : public QWidget {

	Q_OBJECT

public:
	igQtSearchInfoWidget(QWidget* parent = nullptr);
	enum  ElementType {
		POINT,
		TRIANGEL,
		QUAD,
		POLYON,
		TETRA,
		HEXAHEDRON,
		PRISM,
		PYRAMID,
		ELEMTTYPESUM,
		CELL,
		ELEMENTTYPE
	};
	enum ElementItem {
		ID,
		SCALAR,
		POSITION,
		ELEMENTITEM
	};
	enum SearchWay {
		IS,
		ISINRANGE,
		ISBIGGER,
		ISSMALLER,
		ISMIN,
		ISMAX,
		ISNEARSTTO,
		SEARCHWAY
	};
public slots:
	void updateDataProducer();
	void updateElementType();
	void updateElementItem();
	void updateSearchWay();
	void updateLineEdit();
	void updateValuesData();
	SearchWay getSearchWay(QString);
	ElementItem getElementItem(QString);
	ElementType getElementType(QString);
	void searchData();
	void reset();
	void clear();
	void searchDataWithId();
	void searchDataWithPosition();
	void resetPageInfo();
	void updateShowedDataStartAndEnd();
	void updateDataToTable();
	void updateDataToTableWithPointType();
	void updateDataToTableWithCellType();

	void searchDataWithFramePlane();
	void searchDataWithRay(iGame::Vector3f startPoint, iGame::Vector3f dir);
	void drawSearchedPoint();
signals:
	//void showSearchedPoint(iGame::iGameFloatArray*);
	void showSearchedEdge();
	void showSearchedFace();
	void showSearchedVolume();
protected:

private:
	Ui::SearchInfo* ui;
	int dataProducerIndex = 0;
	ElementType elementType = POINT;
	ElementItem elementItem = ID;
	SearchWay searchWay = IS;

	int tmpDataProducerIndex = 0;
	ElementType tmpElementType = POINT;
	ElementItem tmpElementItem = ID;
	SearchWay tmpSearchWay = IS;

	float value[3] = { 0 };
	float epsilon = 0.0f;
	//iGame::iGameIntArray* searchedDataIdInfo = nullptr;
	int pageNum = 0;
	int pageIndex = 0;
	int dataNumPerPage = 50;
	int dataSt = 0;
	int dataEd = 100;

};
