#ifndef MST_h
#define MST_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameDijistra.h"
#include <random>
#include <queue>

IGAME_NAMESPACE_BEGIN
class MST : public Filter {
public:
    I_OBJECT(MST);
    static Pointer New() { return new MST; }

    bool Execute() override {
        auto cmp=[](const EdgeWithWeight& a,const EdgeWithWeight& b)->bool{
            return a.weight>b.weight;
            };
        std::priority_queue<EdgeWithWeight, std::vector<EdgeWithWeight>, decltype(cmp)> MinHeap(cmp);
        std::queue<igIndex> waitQueue;
        std::vector<igIndex> Trace;
        std::vector<bool> isIncluded;

        SurfaceMesh::Pointer mesh = DynamicCast<SurfaceMesh>(GetInput(0));

        int psize = mesh->GetNumberOfPoints();
        int min = 0, max = psize - 1;

        
        father.resize(size, -1);
        Trace.resize(size, -1);

        if(idToIndex.empty())
        {
            std::random_device seed;
            std::ranlux48 engine(seed());
            std::uniform_int_distribution<> distrib(min, max);
            for (int i = 0; i < size; i++)
            {
                igIndex pid = distrib(engine);
                if (std::find(idToIndex.begin(), idToIndex.end(), pid) == idToIndex.end())
                    idToIndex.push_back(pid);
                else
                    i -= 1;
            }
        }

        mesh->RequestEditStatus();

        //Build all edges
        for (int i = 0; i < size; i++) 
        {
            isIncluded.resize(psize, false);
            Trace.resize(psize, -1);
            for(int j=0;j<=i;j++)
                isIncluded[idToIndex[j]] = true;
            waitQueue.push(idToIndex[i]);
            int count = size - 1 - i;
            
            //dijistra
            while(!waitQueue.empty() && count > 0)
            {
                igIndex AdjPids[64];
                igIndex cur = waitQueue.front();
                waitQueue.pop();
                int pnum = mesh->GetPointToOneRingPoints(cur, AdjPids);
                for(int j=0;j<pnum;j++)
                {
                    igIndex pid = AdjPids[j];
                    if(!isIncluded[pid])
                    {
                        isIncluded[pid] = true;
                        Trace[pid] = cur;
                        waitQueue.push(pid);
                        int k = i + 1;
                        while(k < size && idToIndex[k] != pid) k++;
                        if(k < size)
                        {
                            count--;
                            std::vector<igIndex> path;
                            int weight = 0;
                            igIndex p = pid;
                            while(p != -1)
                            {
                                path.push_back(p);
                                p = Trace[p];
                                weight ++;
                            }
                            MinHeap.push(EdgeWithWeight{i,k,path, weight});
                        }
                    }

                    if(count == 0) break;
                }
            }

        }

        //plot point
        painter->SetPen(10);
        painter->SetPen(Color::Red);
        for(int i=0;i<size;i++)
        {
            Point p = mesh->GetPoint(idToIndex[i]);
            painter->Show(painter->DrawPoint(p));
        }

        //kruskal
        painter->SetPen(6);
        painter->SetPen(Color::Blue);
        int esize = MinHeap.size();
        while(!MinHeap.empty())
        {
            auto e = MinHeap.top(); 
            MinHeap.pop();
            if (FindFather(e.id1) != FindFather(e.id2))
            {
                Union(e.id1, e.id2);
                for(int i=0; i<e.path.size() - 1; i++)
                {
                    Point p1 = mesh->GetPoint(e.path[i]);
                    Point p2 = mesh->GetPoint(e.path[i + 1]);
                    painter->Show(painter->DrawLine(p1, p2));
                }
            }
        }

        return true;
    }
    void SetSize(int pnum) { size = pnum; }
    void SetPainter(Painter::Pointer p) { painter = p; }
    void SetIds(std::vector<igIndex>& vec){idToIndex = vec;}

protected:
    
    int FindFather(int id)
    {
        if (father[id] != -1) 
        {
            father[id] = FindFather(father[id]);
            return father[id];
        }
        else return id;
    }
    
    void Union(int id1, int id2)
    {
        father[id1] = FindFather(id2);
    }

    struct EdgeWithWeight
    {
        igIndex id1;
        igIndex id2;
        std::vector<igIndex> path;
        int weight;
    };

    MST() {
        SetNumberOfInputs(1);
        SetNumberOfOutputs(1);
    }

    ~MST() noexcept override = default;

    std::vector<igIndex> idToIndex;   //Store the point index of mesh
    Painter::Pointer painter;
    std::vector<int> father;
    int size = 0;

};
IGAME_NAMESPACE_END

#endif // !MST_h
