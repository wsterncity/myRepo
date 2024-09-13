#ifndef iGameARAPTest_h
#define iGameARAPTest_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameModel.h"
#include <random>

IGAME_NAMESPACE_BEGIN
class ARAPTest : public Filter {
public:
	I_OBJECT(ARAPTest);
	static Pointer New() { return new ARAPTest; }

	bool Initialize() {
		mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		model = m_Model;
		if (mesh == nullptr) { return false; }

		// �����������ѡ��
		Points* ps = mesh->GetPoints();
		fixed = Selection::New();
		fixed->SetFilterEvent(&ARAPTest::CallbackEvent, this, std::placeholders::_1);
		model->RequestPointSelection(ps, fixed);

		// ִ���㷨��ʼ��

		return true;
	}

	bool Begin() {
		// ���������϶���
		Points* ps = mesh->GetPoints();
		moved = Selection::New();
		moved->SetFilterEvent(&ARAPTest::CallbackEvent, this, std::placeholders::_1);
		model->RequestDragPoint(ps, moved);

		return true;
	}

	bool Execute() override {
		// TODO: ִ���㷨

		mesh->SetPoint(dragId, dragNew);



		mesh->Modified();
		model->Update(); // ����ģ��
		return true;
    }

	void CallbackEvent(Selection::Event _event) {
		switch (_event.type)
		{
		case Selection::Event::PickPoint:
			// ѡ�����̶���, ����������
			std::cout << "Pick point id: " << _event.pickId << std::endl;
			break;
		case Selection::Event::PickFace:
			std::cout << "Pick face id: " << _event.pickId << std::endl;
			break;
		case Selection::Event::DragPoint:
			std::cout << "Drag point id: " << _event.pickId << " " << _event.pos << std::endl;
			dragId = _event.pickId;
			dragNew = _event.pos;
			this->Execute();
			break;
		default:
			break;
		}
	}

protected:
	ARAPTest()
	{
		SetNumberOfInputs(1);
	}
	~ARAPTest() override = default;

	igIndex dragId{ -1 };
	Vector3f dragNew{};

	Selection::Pointer fixed{};
	Selection::Pointer moved{};
	SurfaceMesh::Pointer mesh{};
	Model::Pointer model{};
};
IGAME_NAMESPACE_END
#endif