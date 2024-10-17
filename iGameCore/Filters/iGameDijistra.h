#ifndef Dijistra_h
#define Dijistra_h


#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <random>
#include <queue>

IGAME_NAMESPACE_BEGIN
//We assume that every edge has the same cost, so the process of finding single source shortest path is the same with the process of BFS
class Digistra : public Filter {
public:
    I_OBJECT(Digistra);
    static Pointer New() { return new Digistra; }

    bool Execute() override {
        
        
        igIndex AdjPids[64];
        
        std::queue<igIndex> waitQueue;
        std::vector<igIndex> Trace;
        std::vector<bool> isIncluded;
        SurfaceMesh::Pointer mesh = DynamicCast<SurfaceMesh>(GetInput(0));

        if (mesh == nullptr) { return false; }
        int psize = mesh->GetNumberOfPoints();

        mesh->RequestEditStatus();

        Trace.resize(psize, -1);
        isIncluded.resize(psize, false);

        if (source == -1 || target == -1)
        {
            int min = 0, max = psize;
            std::random_device seed;
            std::ranlux48 engine(seed()); 
            std::uniform_int_distribution<> distrib(min, max); 
            source = distrib(engine); 
            target = distrib(engine);
        }

        isIncluded[source] = true;
        waitQueue.push(source);
        
        bool isFind = false;
        //std::cout << "Source: " << source << " Target: " << target <<std::endl;
        while (!isFind && !waitQueue.empty()) 
        { 
            igIndex cur = waitQueue.front();
            waitQueue.pop();
            //std::cout << "Pop pid: " << cur << std::endl;

            int size = mesh->GetPointToOneRingPoints(cur, AdjPids);
            for (int i = 0; i < size; i++)
            { 
                igIndex pid = AdjPids[i];
                if (!isIncluded[pid]) 
                { 
                    waitQueue.push(pid); 
                    //std::cout << "Push pid: " << pid << std::endl;
                    Trace[pid] = cur;
                    isIncluded[pid] = true;
                    if (pid == target) {
                        isFind = true;
                        break;
                    }
                }
            }
        }

        IGuint handle;
        painter->SetPen(3);
        painter->SetPen(Color::Red);
        painter->SetBrush(None);
        handle = painter->DrawPoint(mesh->GetPoint(source));
        painter->Show(handle);
        handle = painter->DrawPoint(mesh->GetPoint(target));
        painter->Show(handle);
        painter->SetPen(Color::Green);
        

        igIndex cursor = target;
        while (cursor != source)
        { 
            Point p1 = mesh->GetPoint(cursor);
            Point p2 = mesh->GetPoint(Trace[cursor]);
            handle = painter->DrawLine(p1, p2);
            painter->Show(handle);
            cost++;
            cursor = Trace[cursor];
        }


        return true;
    }

    void SetSource(igIndex s){ source = s; }
    igIndex GetSource() { return source; }

    void SetTarget(igIndex t) { target = t;}
    igIndex GetTarget() { return target; }

    int GetWeight() { return cost; }

    void SetPainter(Painter::Pointer p){painter = p;}

protected:

    Digistra() 
    {
        SetNumberOfInputs(1);
        SetNumberOfOutputs(1);
    }

    ~Digistra() noexcept override = default;

    SurfaceMesh::Pointer mesh;
    Painter::Pointer painter;
    igIndex source = -1, target = -1;
    int cost = 0;
    
};

IGAME_NAMESPACE_END
#endif // Dijistra_h