//
// Created by Sumzeek on 7/4/2024.
//

#ifndef OPENIGAME_SCENCE_H
#define OPENIGAME_SCENCE_H

#include "OpenGL/GLFramebuffer.h"
#include "OpenGL/GLIndirectCommand.h"
#include "OpenGL/GLShader.h"
#include "iGameAxes.h"
#include "iGameCamera.h"
#include "iGameFontSet.h"
#include "iGameLight.h"
#include "iGameModel.h"

IGAME_NAMESPACE_BEGIN
class Scene : public Object {
public:
    I_OBJECT(Scene);
    static Pointer New() { return new Scene; }

    /* Model Related */
    int AddModel(Model::Pointer);
    Model::Pointer CreateModel(DataObject::Pointer);
    void RemoveModel(int index);
    void RemoveModel(Model*);
    void RemoveCurrentModel();
    void SetCurrentModel(int index);
    void SetCurrentModel(Model*);

    Model* GetCurrentModel();
    Model* GetModelById(int index);
    DataObject* GetDataObjectById(int index);
    std::map<int, Model::Pointer>& GetModelList();

    void ChangeModelVisibility(int index, bool visibility);
    void ChangeModelVisibility(Model* m, bool visibility);

    /* Rendering Related */
    struct CameraDataBuffer {
        alignas(16) igm::mat4 view;
        alignas(16) igm::mat4 proj;
        alignas(16) igm::mat4 proj_view; // proj * view
    };
    struct ObjectDataBuffer {
        alignas(16) igm::mat4 model;
        alignas(16) igm::mat4 normal; // transpose(inverse(model))
        alignas(16) igm::vec4 sphereBounds;
    };
    struct UniformBufferObjectBuffer {
        alignas(16) igm::vec3 viewPos;
        alignas(4) int useColor{0};
    };

    struct DrawCullData {
        alignas(16) igm::mat4 view_model;
        alignas(4) float P00, P11, zNear; // symmetric projection parameters
        alignas(16) igm::vec4
                frustum; // data for left/right/top/bottom frustum planes
        alignas(4) float pyramidWidth,
                pyramidHeight; // depth pyramid size in texels
    };

    enum ShaderType {
        PATCH = 0,
        NOLIGHT,
        PURECOLOR,
        AXES,
        FONT,
        DEPTHREDUCE,
        MESHLETCULL,
        SCREEN,
        SHADERTYPE_COUNT
    };

    CameraDataBuffer& CameraData() { return m_CameraData; }
    ObjectDataBuffer& ObjectData() { return m_ObjectData; }
    UniformBufferObjectBuffer& UBO() { return m_UBO; }
    void UseColor();
    void UpdateUniformBuffer();

    void SetShader(IGenum type, GLShaderProgram*);
    GLShaderProgram* GenShader(IGenum type);
    GLShaderProgram* GetShaderWithType(IGenum type);
    GLShaderProgram* GetShader(IGenum type);
    bool HasShader(IGenum type);
    void UseShader(IGenum type);

    void Draw();
    void Resize(int width, int height, int pixelRatio);
    void Update();
    template<typename Functor, typename... Args>
    void SetUpdateFunctor(Functor&& functor, Args&&... args) {
        m_UpdateFunctor = std::bind(functor, args...);
    }

    GLTexture2d& HizTexture() { return m_DepthPyramid; }

    // TODO: slove z-buffer Accuracy issues
    GLBuffer& GetDrawCullDataBuffer() { return m_DrawCullData; }

    void MakeCurrent() {
        if (m_MakeCurrentFunctor) { m_MakeCurrentFunctor(); }
    }
    void DoneCurrent() {
        if (m_DoneCurrentFunctor) { m_DoneCurrentFunctor(); }
    }
    template<typename Functor, typename... Args>
    void SetMakeCurrentFunctor(Functor&& functor, Args&&... args) {
        m_MakeCurrentFunctor = std::bind(functor, args...);
    }
    template<typename Functor, typename... Args>
    void SetDoneCurrentFunctor(Functor&& functor, Args&&... args) {
        m_DoneCurrentFunctor = std::bind(functor, args...);
    }

protected:
    Scene();
    ~Scene() override;

    void UpdateModelsBoundingSphere();

    void InitOpenGL();
    void InitFont();
    void InitAxes();

    void ResizeFrameBuffer();
    void ResizeHizTexture();
    void RefreshHizTexture();
    void RefreshDrawCullDataBuffer();

    void UpdateUniformData();
    void DrawFrame();
    void DrawModels();
    void DrawAxes();


    /* Data Object Related */
    std::map<int, Model::Pointer> m_Models;
    int m_IncrementModelId{0};
    int m_CurrentModelId{-1};
    Model* m_CurrentModel{nullptr};
    //DataObject* m_CurrentObject{nullptr};

    std::function<void()> m_UpdateFunctor;
    std::function<void()> m_MakeCurrentFunctor;
    std::function<void()> m_DoneCurrentFunctor;

    Camera::Pointer m_Camera{};
    Light::Pointer m_Light{};
    Axes::Pointer m_Axes{};


    /* Rendering related */
    CameraDataBuffer m_CameraData;
    ObjectDataBuffer m_ObjectData;
    UniformBufferObjectBuffer m_UBO;

    igm::mat4 m_ModelRotate{};
    igm::vec3 m_BackgroundColor{};

    uint32_t m_VisibleModelsCount = 0;
    igm::vec4 m_ModelsBoundingSphere{0.0f, 0.0f, 0.0f, 1.0f};

    GLBuffer m_CameraDataBlock, m_ObjectDataBlock, m_UBOBlock;
    std::map<IGenum, std::unique_ptr<GLShaderProgram>> m_ShaderPrograms;

    GLVertexArray m_ScreenQuadVAO;
    GLBuffer m_ScreenQuadVBO;
    GLFramebuffer m_Framebuffer, m_FramebufferMultisampled;
    GLTexture2d m_ColorTexture, m_DepthTexture;

    GLBuffer m_DrawCullData;
    int m_DepthPyramidWidth, m_DepthPyramidHeight, m_DepthPyramidLevels;
    GLTexture2d m_DepthPyramid;

    friend class Interactor;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H
