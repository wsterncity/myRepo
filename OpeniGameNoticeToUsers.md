## OpeniGame使用手册

### 模型导入导出

1.目前支持vtk（高阶网格还没适配）、vts、pvd、obj（纯网格）、ply、off、mesh文件的读取  
2.目前支持vtk（目前高阶网格还没适配，只支持输出标量矢量）、obj（纯网格）、ply、off、mesh文件的输出  
3.如果想自行生成可视化数据（即标量），可调用this->manager->GetCurrentModel()->DataSet->GetPointData()->AddScalars(
ScalarData);
其中，manager为全局的静态管理器，需要使用manager=iGame::iGameManager::Instance()声明。主ui上点击帮助->
关于iGameMeshView可以生成一个点坐标的标量和类似于曲率的标量。目前暂时只支持点数据，因为面数据渲染规则不一样还未做适配（但是可以自己手动绘制）

### 网格数据

1.整个系统会用一个iGameManager来做管理，其内部存储了所有的模型（struct），结构体内部存储了DataSet的指针、Mesh的指针以及用于渲染和处理的数据指针，网格的存储数据（点面体这种）全都存储在DataSet中，采用数组的方式存储。Mesh是iGameMesh类，是所有网格的基类，在后续操作中，需要使用SafeDownCast转换为特定网格，比如转化为三角网格需要使用
triMesh=iGameTriMesh::SafeDownCast(Mesh);  
2.存储数组采用的是模版类，其中涉及到数据的存储采用自定义容器类iGameDataArray进行实现，在具体使用中，常常使用其子模版类iGameDataArrayTemplata，为了方便使用，我们已经define了基础数据，命名规则为iGame+数据种类(
首字母大写)+Array，比如，常用的iGameFloatArray以及iGameIntArray，其申明如下：data=iGame::iGameFloatArray::New();(
注意申明返回的是个指针)
。数据采用元组（tuple）式存储，每个元组内部有统一个数的分量，其实际效果可以等价于二维数组，比如你想申明一个容量为100*
3的二维数组，只需New()之后，调用SetNumberOfComponents(3)以及Resize(100)即可，此时空间已经开辟，数据仍旧从下标0处开始add。  
数组内部成员:
> 注意value和元组的区别，元组可能有多个value！！！！  
> NumberOfComponents:元组的分量个数  
> NumberOfValues:元素的个数  
> Size:已经分配的控件大小

常用函数：
> GetRawPointer():返回数据的buffer指针  
> SetNumberOfComponents(int n):设置元组的分量个数  
> SetNumberOfTuples(int n):设置元组的个数  
> SetNumberOfValues(int n):设置元素的个数
> Resize(int n):tuple容量重置为n，当前数据下标不变  
> Reserve(int n):tuple容量重置为n，当前数据下标重置为0  
> GetTuple(igIndex tupleIdx, ValueType* values):将tupleIdx的数据赋给values  
> SetTuple(igIndex tupleIdx, ValueType* values):把valueIdx下标处的元组更改为values  
> GetValue(igIndex valueIdx):返回valueIdx处的value数据
> AddTuple(ValueType* values):添加元组，注意数组长度大于等于元组长度  
> SetValue(igIndex valueIdx, ValueType value):把valueIdx下标处的数据更改为value  
> AddValue(ValueType value):添加value  
> 其中，少量个数的tuple的add已经定义了一些基础函数供使用，比如AddTuple3(ValueType val0, ValueType val1, ValueType val2)
> 。其余函数便不在此赘述，实际使用可以查看具体定义。

3.使用iGamePoints类存储所有的网格顶点数据，使用iGameCellArray模版类存储所有的网格cell，包括线面体等等。由于存在不固定长度的cell，比如多边形，因此内部会存储一个iGameIdArray用于存储所有cell的数据起始下标，在实际使用中，编程人员无需关注这个的具体实现，最多只需要使用下述函数即可，在实际使用中，上层已经针对网格封装好了接口，下述函数只需要明白具体功能即可。
> AddCell(igIndex* cell, int ncell):添加cell，参数一为cell内部点序号，参数二为cell点数量  
> GetCell(igIndex cellId):返回cellId的cell，但会类型为实际类，比如为Triangle类  
> GetCell(igIndex cellId, igIndex* cell):将cellId的cell内部的顶点数据赋给参数二cell数组

4.iGameSurfaceMeshBase类以及iGameSurfaceMesh类，前者为所有表面网格的基础数据存储以及邻接构造，后者继承自前者，是所有表面网格的基类，其内部定义了网格元素的读取。iGameGenericSurfaceMesh类为模板类，后续所有网格都使用该模板类定义，基础类型的表面网格已经事先申明。邻接关系由于做渲染不需要，因此在读取网格时不会建立，需要自己手动调用，具体调用会在后续讲解。  
常用函数为：
> GetPoint(igIndex pid):返回顶点，为Point，可简单理解为一维的长度为三的数组，表示下标  
> GetPoints():返回所有的点  
> GetEdge(igIndex edgeId) const = 0:返回边，为iGameEdge  
> GetEdges():返回所有的边  
> GetFace(igIndex faceId) const = 0:返回面，为iGameFace，在子类mesh中会返回具体的种类，比如三角网格中会返回Triangle  
> GetFaces():返回所有的面  
> AddVertex(const Point& v):添加点，返回点序号  
> AddEdge(igIndex vert_id1, igIndex vert_id2):添加边，返回边序号  
> AddFace(igIndex* face, igIndex nface):添加面，返回面序号  
> BuildEdges() = 0;// 构建所有且唯一的边    
> BuildVertToEdgeLink() = 0;// 构建顶点的邻接边    
> BuildVertToFaceLink() = 0;// 构建顶点的邻接面   
> BuildEdgeToFaceLink() = 0;// 构建边的邻接面  
> BuildFaceToFaceLink() = 0;// 构建面的邻接面(公共边)

5.iGameVolumeMeshBase类以及iGameVolumeMesh类，类似于表面网格，是该框架的体网格基类，其实际使用方法类似于表面网格，但是增加了体的相关函数，且iGameVolumeMesh类为模板类，基础的体网格已经事先申明，在内部已经定义好体类型和面片类型。  
常用函数：
> GetVolume(igIndex volumeId):返回体，为具体的体类型  
> GetVolumes():返回所有的体  
> BuildFaces() = 0;//构建所有且唯一的面  
> BuildEdges();// 构建所有且唯一的边  
> BuildVertToEdgeLink();// 构建顶点的邻接边  
> BuildVertToFaceLink();// 构建顶点的邻接面  
> BuildEdgeToFaceLink();// 构建边的邻接面  
> BuildVertToVolumeLink();// 构建顶点的邻接体  
> BuildFaceToFaceLink();// 构建面的邻接面  
> BuildFaceToVolumeLink();// 构建面的邻接体  
> BuildVolumeToVolumeLink();// 构建体的邻接体

6.目前已经支持三角网格、四边形网格、多边形网格、四面体网格、六面体网格、金字塔网格、三棱柱网格、混合网格，其具体使用函数除了上述的简单数据读取外，其余函数可以自行在类内部查看。

### 网格处理算法

1.文件位置、命名、调用规范化！！！！
> （1）在iGameCore的Filters文件夹下面放置自己的函数，先新建一个文件夹，并在内部放入h文件以及cpp文件，注意好命名规范。  
> （2）在Filters文件夹下的iGameFilterExport.h处导入自己的算法头文件
>
（3）在ui界面的某个成员（按钮、动作等都可以）进行该算法的调用执行，需要自己编写connect函数，具体可以查看示例。在igQtMainWindow中的initAllFilters()
> 函数内部定义具体的connect，可以使用lambda表达式。

2.具体实现，编写一个算法类继承自iGameMeshAlgorithm，并且必须重写GetAlgorithmName()以及RequestData(iGameInformationMap*
inInfo, iGameInformationMap* outInfo)，前者为函数名，后者为函数具体实现。
> iGameInformationMap类:是信息映射类，用于传递算法需要的数据的指针，基础数据为mesh以及dataset，这两个可以使用inInfo->
> GetObject(MESH_KEY())以及inInfo->GetObject(DATASET_KEY())
>
得到，得到网格指针后，便可以进行算法操作，同时需要将处理后的网格指针（如果网格没有新建则为同一个指针）传递给outInfo，使用outInfo->
> Add(MESH_KEY(), this->Mesh)即可。同时，算法内部需要的参数，比如epsilon，lambda都可以在qt组件处传递给inInfo，比如使用info->
> Add("lambda", 0.05);设置0.05的lambda，在算法内部调用 double lambda = inInfo->GetDouble("lambda");即可得到。  
> 进度条：算法总调用this->UpdateProgres(double amount)即可，amout属于0-1，主ui的进度条会自动变化。

3.算法编写的注意事项，尤其是预处理(其中删除操作的预处理千万别忘记)，必须执行，否则无法执行算法！！！！！！！！！！！！！！！！！
> 	//如果需要执行删除操作，切记一定要先调用以下函数:  

		RequestCellStatus();    
		// 表面网格初始化网格，表面网格对象初始只有点和面，需要自己手动建立边以及需要
		// 的连接关系
		if (this->Mesh->EdgeNotBuild()) {
			this->Mesh->BuildEdges();
		}
		if (this->Mesh->Vert2EdgeNotBuild()) {
			this->Mesh->BuildVertToEdgeLink();
		}
		if (this->Mesh->Vert2FaceNotBuild()) {
			this->Mesh->BuildVertToFaceLink();
		}
		if (this->Mesh->Edge2FaceNotBuild()) {
			this->Mesh->BuildEdgeToFaceLink();
		}
		//体网格
		//构建所有的面
		virtual void BuildFaces() = 0;
        // 构建所有且唯一的边
        void BuildEdges();
        // 构建顶点的邻接边
        void BuildVertToEdgeLink();
        // 构建顶点的邻接面
        void BuildVertToFaceLink();
        // 构建边的邻接面
        void BuildEdgeToFaceLink();
        // 构建顶点的邻接体
        void BuildVertToVolumeLink();
        // 构建面的邻接面
        void BuildFaceToFaceLink();
        // 构建面的邻接体
        void BuildFaceToVolumeLink();
        // 构建体的邻接体
        void BuildVolumeToVolumeLink();  

4.算法示例，可以参考iGameLaplacianSmoothing，里面有具体的示例和注释。

### 渲染

如果有渲染需求，在iGameModelDraw中，构建一个VertexInputStateInfo，并放置自己的数据，VertexInputStateInfo的数据格式如下：

```c++
struct VertexInputStateInfo {
    PointInputStateInfo pointInfo;
    LineInputStateInfo lineInfo;
    TriangleInputStateInfo triangleInfo;
};

// 采用VBO绘制还是EBO绘制
typedef enum DataType {
    POSITION_NULL = 0x00000000,
    POSITION_ARRAY = 0x00000001,
    POSITION_ELEMENT = 0x00000002
} DataType;

struct PointInputStateInfo {
    DataType dataType = DataType::POSITION_ARRAY;

    float* positions = nullptr;
    float* colors = nullptr;
    uint64_t positionSize = 0;
    float unifiedColor[3]{-1.0f, -1.0f, -1.0f};

    uint32_t* indices = nullptr;
    uint32_t indexSize = 0;
};

struct LineInputStateInfo {
    DataType dataType = DataType::POSITION_ARRAY;

    float* positions = nullptr;
    float* colors = nullptr;
    uint64_t positionSize = 0;
    float unifiedColor[3]{-1.0f, -1.0f, -1.0f};

    uint32_t* indices = nullptr;
    uint32_t indexSize = 0;
};

struct TriangleInputStateInfo {
    DataType dataType = DataType::POSITION_ARRAY;

    float* positions = nullptr;
    float* colors = nullptr;
    float* normals = nullptr;
    float* textures = nullptr;
    uint64_t positionSize = 0;

    uint32_t* indices = nullptr;
    uint32_t indexSize = 0;

    // quad may be removed in a future version
    DataType quadDataType = DataType::POSITION_NULL;
    uint32_t* quadIndices = nullptr;
    uint32_t quadIndexSize = 0;
};
```

其中输入数据结构中的Size（positionSize、indexSize）均为数组的大小，例如float position[3]，传入给Size的值即为3。

添加渲染数据主要有以下函数：

```c++
// 实例，注意在QT中操作实例时需要用makeCurrent();和doneCurrent();包围
iGameRenderer::OpenGLRenderer *igOpenGLRenderer;
```

1. 添加一个新模型，在QT中的包装类为``igQtRenderWidget::AddRendererData``

   ```c++
   igOpenGLRenderer->addActor(VertexInputStateInfo& vertexInputInfo);
   ```

2. 更新模型数据，在QT中的包装类为``igQtRenderWidget::UpdateRendererData``

   ```c++
   // actorIndex为需要更改数据模型的索引，也就是左边模型列表的索引
   igOpenGLRenderer->updateActorData(int actorIndex, VertexInputStateInfo& vertexInputInfo);
   ```

   ``updateActorData``函数只修改提供的数据，例如只改变了三角面片的索引，则只需要提供索引数据即可

   ```c++
   // 构造更改数据的结构体
   iGameRenderer::TriangleInputStateInfo triInfo{};
   triInfo.indices = triIndices;
   triInfo.indexSize = 3;
   
   // 打包为VertexInputStateInfo结构体
   iGameRenderer::VertexInputStateInfo info{};
   info.triangleInfo = triInfo;
   
   // 执行updateActorData函数更改数据
   makeCurrent();
   igOpenGLRenderer->updateActorData(int actorIndex, VertexInputStateInfo& vertexInputInfo);
   doneCurrent();
   ```

3. 在模型中添加/删除点线面

   ```c++
   // actorIndex为需要更改数据模型的索引，也就是左边模型列表的索引
   
   // 添加点线面数据
   // 下列函数返回一个int值，为所添加点线面的标识符，后续需要通过该标识符删除指定的点线面数据
   igOpenGLRenderer->addPointSet(int actorIndex, PointInputStateInfo pointInputInfo);
   igOpenGLRenderer->addLineSet(int actorIndex, LineInputStateInfo lineInputInfo);
   igOpenGLRenderer->addTriangleSet(int actorIndex, TriangleInputStateInfo triangleInputInfo);
   // 删除点线面数据
   // 通过add操作获取的标识符进行删除
   igOpenGLRenderer->removePointSet(int actorIndex, int pointSetIndex);
   igOpenGLRenderer->removeLineSet(int actorIndex, int lineSetIndex);
   igOpenGLRenderer->removeTriangleSet(int actorIndex, int triangleSetIndex);
   ```

   示例：

   ```cpp
   // 渲染数据
   float pointPosition[6] = {1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
   uint32_t pointIndices[2] = {0, 1};
   
   // 构造点集结构体
   iGameRenderer::PointInputStateInfo pointInfo{};
   pointInfo.dataType = iGameRenderer::DataType::POSITION_ELEMENT;
   pointInfo.positions = pointPosition;
   pointInfo.positionSize = 6;
   pointInfo.indices = pointIndices;
   pointInfo.indexSize = 2;
   
   // 添加点集
   makeCurrent();
   auto pointIndex = igOpenGLRenderer->addPointSet(manager->GetCurrentModelIdx(), pointInfo);
   doneCurrent();
   
   // 删除添加的点集
   makeCurrent();
   igOpenGLRenderer->removePointSet(manager->GetCurrentModelIdx(), pointIndex);
   doneCurrent();
   ```

4. 一些具体的例子可以参考如下函数写法
   ```cpp
   文件路径: Qt/src/IQWidgets/igQtModelDrawWidget.cpp
   1. void igQtModelDrawWidget::DrawSelectedPoint(iGame::iGameFloatArray* points)
   2. void igQtModelDrawWidget::DrawStreamline();
   3. void igQtModelDrawWidget::UpdateStreamline();
   4. void igQtModelDrawWidget::UpdateCurrentModel();
   ```

### About

由于Openigame还在不断的完善，因此在使用过程中可能会遇到些许问题，所有遇到的bug都可以给我们提issue，如果有解决办法就更好了。如果有具体需求，可以自行实现，然后告诉我们，我们把他移植到正式版本中，如果该需求会很适用，也可以告知我们我们在后续事先并添加。






