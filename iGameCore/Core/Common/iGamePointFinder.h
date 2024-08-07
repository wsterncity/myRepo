#ifndef iGamePointFinder_h
#define iGamePointFinder_h

#include "iGameObject.h"
#include "iGameBoundingBox.h"
#include "iGamePoints.h"
#include "iGameElementArray.h"
#include "iGameIdArray.h"
#include "iGameFlexArray.h"

IGAME_NAMESPACE_BEGIN
class PointFinder : public Object {
public:
    I_OBJECT(PointFinder);
    static Pointer New() { return new PointFinder; }
    

    void SetPoints(Points::Pointer p) {
        m_Points = p;
    }

    void Initialize() {
        if (!m_Points) { 
            return;
        }
        m_NumberOfPointsPerBox = m_Threshold * m_Points->GetNumberOfPoints();
        int numBoxes = m_Points->GetNumberOfPoints() / m_NumberOfPointsPerBox;
        m_Depth = std::ceil(std::log10(numBoxes) / std::log10(8));
        m_Depth = std::max(1, std::min(4, m_Depth));
        m_Size = 2 ^ m_Depth;
        m_SizeSquared = m_Size * m_Size;
        m_NumberOfBoxes = 8 ^ m_Depth;

        m_Bounding.reset();
        for (int i = 0; i < m_Points->GetNumberOfPoints(); i++) {
            m_Bounding.add(m_Points->GetPoint(i));
        }
        
        double max = m_Bounding.diagVector().maxCoeff();
        m_Bounding.min -= 0.005 * max;
        m_Bounding.max += 0.005 * max;

        Vector3d diag = m_Bounding.diagVector();
        m_BoxSize = diag / m_Size;
        m_ReciprocalBoxSize[0] = 1.0 / m_BoxSize[0];
        m_ReciprocalBoxSize[1] = 1.0 / m_BoxSize[1];
        m_ReciprocalBoxSize[2] = 1.0 / m_BoxSize[2];

        m_Buffer->Resize(m_NumberOfBoxes);

        for (IGsize i = 0; i < m_Points->GetNumberOfPoints(); i++) {
            auto& x = m_Points->GetPoint(i);
            IGsize index = this->GetBoxIndex(x);
            auto& box = m_Buffer->ElementAt(index);
            if (box = nullptr) {
                box = IdArray::New();
                //box->Reserve(m_NumberOfPointsPerBox);
            }
            box->AddId(i);
        }
    }
	
    igIndex FindClosestPoint(const Vector3d& x) {

    }

protected:
    PointFinder() { }
    ~PointFinder() override = default;

    void GetBoxNeighbors(FlexArray<int>* boxes, const Vector3i ijk,
                         const Vector3i ndivs, int level) {
        int i, j, k, min, max;
        Vector3i minLevel, maxLevel, nei;

        boxes->reset();

        // If at this box
        if (level == 0) {
            //boxes->push_back(ijk);
            return;
        }

        // 找level邻域内所有的box
        for (i = 0; i < 3; i++) {
            min = ijk[i] - level;
            max = ijk[i] + level;
            minLevel[i] = (min > 0 ? min : 0);
            maxLevel[i] = (max < (ndivs[i] - 1) ? max : (ndivs[i] - 1));
        }

        // 仅找level层的box
        for (i = minLevel[0]; i <= maxLevel[0]; i++) {
            for (j = minLevel[1]; j <= maxLevel[1]; j++) {
                for (k = minLevel[2]; k <= maxLevel[2]; k++) {
                    if (i == (ijk[0] + level) || i == (ijk[0] - level) ||
                        j == (ijk[1] + level) || j == (ijk[1] - level) ||
                        k == (ijk[2] + level) || k == (ijk[2] - level)) {
                        nei[0] = i;
                        nei[1] = j;
                        nei[2] = k;
                        //boxes->InsertNextPoint(nei);
                    }
                }
            }
        }
    }

    IGsize GetBoxIndex(const Vector3d& x) const {
        Vector3i ijk = Interize(x);
        return static_cast<IGsize>(ijk[0]) +
               static_cast<IGsize>(ijk[1]) * m_Size +
               static_cast<IGsize>(ijk[2]) * m_SizeSquared;
    }

    Vector3i Interize(const Vector3d& x) const {
        assert(m_Bounding.isIn(x));
        Vector3i pi;

        pi[0] = int((x[0] - m_Bounding.min[0]) * m_ReciprocalBoxSize[0]);
        pi[1] = int((x[1] - m_Bounding.min[1]) * m_ReciprocalBoxSize[1]);
        pi[2] = int((x[2] - m_Bounding.min[2]) * m_ReciprocalBoxSize[2]);

        pi[0] = std::max(0, std::min(m_Size - 1, pi[0]));
        pi[1] = std::max(0, std::min(m_Size - 1, pi[1]));
        pi[2] = std::max(0, std::min(m_Size - 1, pi[2]));

        return pi;
    }

    Vector3d DeInterize(const Vector3i& pi) const {
        Vector3d pf;

        assert(pi[0] >= 0 && pi[0] < m_Size);
        assert(pi[1] >= 0 && pi[1] < m_Size);
        assert(pi[2] >= 0 && pi[2] < m_Size);

        pf[0] = pi[0] * m_BoxSize[0] + m_Bounding.min[0];
        pf[1] = pi[1] * m_BoxSize[1] + m_Bounding.min[1];
        pf[2] = pi[2] * m_BoxSize[2] + m_Bounding.min[2];

        return pf;
    }

    Points::Pointer m_Points{};
    ElementArray<IdArray::Pointer>::Pointer m_Buffer{};
    Vector3d m_BoxSize, m_ReciprocalBoxSize;
    BoundingBox m_Bounding;

    int m_NumberOfBoxes{1};
    int m_NumberOfPointsPerBox{1};
    double m_Threshold{0.01};
    
    int m_Size{}, m_SizeSquared{}; // m_Size = 2 ^ m_Depth
    int m_Depth{};                 // Tree's depth
};
IGAME_NAMESPACE_END
#endif