#include "iGameScene.h"
#include "iGameCommand.h"
#include <chrono>

IGAME_NAMESPACE_BEGIN
Scene::Scene() {
    m_Camera = Camera::New();
    m_Camera->Initialize(igm::vec3{0.0f, 0.0f, 1.0f});

    m_ModelRotate = igm::mat4{};
    m_ModelMatrix = igm::mat4{};
    m_BackgroundColor = {0.5f, 0.5f, 0.5f};

    InitOpenGL();
    InitFont();
    InitAxes();
}
Scene::~Scene() {}

int Scene::AddModel(Model::Pointer model) {
    int newModelId = m_IncrementModelId++;
    m_Models.insert(std::make_pair<>(newModelId, model));
    m_CurrentModelId = newModelId;
    m_CurrentModel = model.get();
    model->m_Scene = this;

    ChangeModelVisibility(model.get(), true);
    UpdateModelsBoundingSphere();
    return newModelId;
}

Model::Pointer Scene::CreateModel(DataObject::Pointer obj) {
    Model::Pointer model = Model::New();
    model->m_DataObject = obj;
    return model;
}

void Scene::RemoveModel(int index) {
    m_Models.erase(index);
    if (index == m_CurrentModelId) {
        if (m_Models.empty()) {
            m_CurrentModelId = -1;
            m_CurrentModel = nullptr;
        } else {
            m_CurrentModelId = m_Models.begin()->first;
            m_CurrentModel = m_Models.begin()->second;
        }
    }
    UpdateModelsBoundingSphere();
}

void Scene::RemoveModel(Model* model) {
    for (auto it = m_Models.begin(); it != m_Models.end(); ++it) {
        if (it->second.get() == model) {
            m_Models.erase(it);
            if (it->first == m_CurrentModelId) {
                if (m_Models.empty()) {
                    m_CurrentModelId = -1;
                    m_CurrentModel = nullptr;
                } else {
                    m_CurrentModelId = m_Models.begin()->first;
                    m_CurrentModel = m_Models.begin()->second;
                }
            }
            break;
        }
    }
    UpdateModelsBoundingSphere();
}

void Scene::RemoveCurrentModel() {
    if (auto visibility = m_CurrentModel->GetVisibility()) {
        m_VisibleModelsCount--;
    }

    m_Models.erase(m_CurrentModelId);
    if (m_Models.empty()) {
        m_CurrentModelId = -1;
        m_CurrentModel = nullptr;
    } else {
        m_CurrentModelId = m_Models.begin()->first;
        m_CurrentModel = m_Models.begin()->second;
    }
    UpdateModelsBoundingSphere();
}

void Scene::SetCurrentModel(int index) {
    for (auto& [id, model]: m_Models) {
        if (id == index) {
            m_CurrentModelId = id;
            m_CurrentModel = model.get();
            return;
        }
        //if (obj->m_DataObject->HasSubDataObject()) {
        //    auto subObj = obj->m_DataObject->GetSubDataObject(index);
        //    if (subObj != nullptr) {
        //        m_CurrentModelId = index;
        //        m_CurrentObject = subObj.get();
        //        return true;
        //    }
        //}
    }
}

void Scene::SetCurrentModel(Model* _model) {
    for (auto& [id, model]: m_Models) {
        if (model == _model) {
            m_CurrentModelId = id;
            m_CurrentModel = model.get();
            return;
        }
    }
}

Model* Scene::GetCurrentModel() { return m_CurrentModel; }

Model* Scene::GetModelById(int index) {
    for (auto& [id, model]: m_Models) {
        if (id == index) { return model; }
        //if (obj->m_DataObject->HasSubDataObject()) {
        //    auto subObj = obj->m_DataObject->GetSubDataObject(index);
        //    if (subObj != nullptr) { return subObj.get(); }
        //}
    }
    return nullptr;
}

DataObject* Scene::GetDataObjectById(int index) {
    for (auto& [id, model]: m_Models) {
        if (id == index) { return model->m_DataObject; }
    }
    return nullptr;
}

std::map<int, Model::Pointer>& Scene::GetModelList() { return m_Models; }

void Scene::ChangeModelVisibility(int index, bool visibility) {
    auto* model = GetModelById(index);
    if (model != nullptr) { ChangeModelVisibility(model, visibility); }
}

void Scene::ResetCenter() {
    igm::vec4 center = igm::vec4{m_ModelsBoundingSphere.xyz(), 1.0f};
    igm::vec3 centerInWorld = (m_ModelMatrix * center).xyz();
    float radius = m_ModelsBoundingSphere.w;
    m_Camera->SetCameraPos(centerInWorld.x, centerInWorld.y,
                           centerInWorld.z + 2.0f * radius);
}

void Scene::ChangeModelVisibility(Model* model, bool visibility) {
    UpdateModelsBoundingSphere();

    if (visibility) {
        m_VisibleModelsCount++;
        if (m_VisibleModelsCount == 1) { ResetCenter(); }
    } else {
        m_VisibleModelsCount--;
    }
}

void Scene::SetShader(IGenum type, GLShaderProgram* sp) {
    if (sp == nullptr) { return; }
    m_ShaderPrograms[type] = std::unique_ptr<GLShaderProgram>(sp);
}

GLShaderProgram* Scene::GenShader(IGenum type) {
    GLShaderProgram* sp;
    switch (type) {
        case PATCH: {
            GLShader shader_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader shader_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({shader_vert, shader_frag});
        } break;
        case NOLIGHT: {
            GLShader shader_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader shader_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shaderNoLight.frag")
                            .c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({shader_vert, shader_frag});
        } break;
        case PURECOLOR: {
            GLShader shader_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader pureColor_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/pureColor.frag")
                            .c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({shader_vert, pureColor_frag});
        } break;
        case AXES: {
            GLShader Axis_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/axis.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader Axis_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/axis.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({Axis_vert, Axis_frag});
        } break;
        case FONT: {
            GLShader font_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/font.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader font_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/font.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({font_vert, font_frag});
        } break;
        case ATTACHMENTRESOLVE: {
            GLShader font_vert = GLShader{(std::string(ASSEST_DIR) +
                                           "/Shaders/attachmentResolve.vert")
                                                  .c_str(),
                                          GL_VERTEX_SHADER};
            GLShader font_frag = GLShader{(std::string(ASSEST_DIR) +
                                           "/Shaders/attachmentResolve.frag")
                                                  .c_str(),
                                          GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({font_vert, font_frag});
        } break;
        case DEPTHREDUCE: {
            GLShader depthReduce_comp = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/depthReduce.comp")
                            .c_str(),
                    GL_COMPUTE_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({depthReduce_comp});
        } break;
        case MESHLETCULL: {
            GLShader meshletCull_comp = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/meshletCull.comp")
                            .c_str(),
                    GL_COMPUTE_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({meshletCull_comp});
        } break;
        case SCREEN: {
            GLShader screen_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/screenShader.vert")
                            .c_str(),
                    GL_VERTEX_SHADER};
            GLShader screen_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/screenShader.frag")
                            .c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({screen_vert, screen_frag});
        } break;
        default:
            break;
    }
    return sp;
}

GLShaderProgram* Scene::GetShaderWithType(IGenum type) {
    auto it = m_ShaderPrograms.find(type);
    if (it == m_ShaderPrograms.end()) { return nullptr; }
    return it->second.get();
}

GLShaderProgram* Scene::GetShader(IGenum type) {
    GLShaderProgram* sp = this->GetShaderWithType(type);
    if (sp != nullptr) { return sp; }

    sp = this->GenShader(type);
    if (sp == nullptr) {
        // std::cout << "Error for GenShader\n";
    }
    this->SetShader(type, sp);
    return sp;
}

bool Scene::HasShader(IGenum type) {
    return this->GetShaderWithType(type) != nullptr;
}

void Scene::UseShader(IGenum type) { this->GetShader(type)->use(); }

void Scene::InitOpenGL() {
    if (!gladLoadGL()) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // reversed-z buffer, depth range: 1.0(near plane) -> 0.0(far plane)
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

    // create empty VAO to render full-screen triangle
    m_EmptyVAO.create();

    // allocate memory
    {
        m_CameraDataBlock.create();
        m_CameraDataBlock.target(GL_UNIFORM_BUFFER);
        m_CameraDataBlock.allocate(sizeof(CameraDataBuffer), nullptr,
                                   GL_STATIC_DRAW);
        m_ObjectDataBlock.create();
        m_ObjectDataBlock.target(GL_UNIFORM_BUFFER);
        m_ObjectDataBlock.allocate(sizeof(ObjectDataBuffer), nullptr,
                                   GL_STATIC_DRAW);
        m_UBOBlock.create();
        m_UBOBlock.target(GL_UNIFORM_BUFFER);
        m_UBOBlock.allocate(sizeof(UniformBufferObjectBuffer), nullptr,
                            GL_STATIC_DRAW);

        // map shader block
        {
            auto shader = this->GetShader(PATCH);
            shader->mapUniformBlock("CameraDataBlock", 0, m_CameraDataBlock);
            shader->mapUniformBlock("ObjectDataBlock", 1, m_ObjectDataBlock);
            shader->mapUniformBlock("UniformBufferObjectBlock", 2, m_UBOBlock);
        }
        // map no light shader block
        {
            auto shader = this->GetShader(NOLIGHT);
            shader->mapUniformBlock("CameraDataBlock", 0, m_CameraDataBlock);
            shader->mapUniformBlock("ObjectDataBlock", 1, m_ObjectDataBlock);
            shader->mapUniformBlock("UniformBufferObjectBlock", 2, m_UBOBlock);
        }
        // map pure color shader block
        {
            auto shader = this->GetShader(PURECOLOR);
            shader->mapUniformBlock("CameraDataBlock", 0, m_CameraDataBlock);
            shader->mapUniformBlock("ObjectDataBlock", 1, m_ObjectDataBlock);
            shader->mapUniformBlock("UniformBufferObjectBlock", 2, m_UBOBlock);
        }
        // map culling computer shader block
        {
            auto shader = this->GetShader(MESHLETCULL);
            shader->mapUniformBlock("CameraDataBlock", 0, m_CameraDataBlock);
        }
    }

    m_UBO.useColor = false;

    // init drawculldata buffer
    m_DrawCullData.create();
    m_DrawCullData.target(GL_UNIFORM_BUFFER);
    m_DrawCullData.allocate(sizeof(DrawCullData), nullptr, GL_DYNAMIC_DRAW);

    // init framebuffer
    ResizeFrameBuffer();
}
void Scene::InitFont() {
    const wchar_t* text = L"XYZ";
    FontSet::Instance().RegisterWords(text);
}
void Scene::InitAxes() {
    auto axesShader = this->GetShader(AXES);
    GLUniform viewLocation = axesShader->getUniformLocation("view");
    GLUniform projLocation = axesShader->getUniformLocation("proj");
    GLUniform viewPosLocation = axesShader->getUniformLocation("viewPos");

    axesShader->use();
    axesShader->setUniform(viewLocation, Axes::ViewMatrix());
    axesShader->setUniform(projLocation, Axes::ProjMatrix());
    axesShader->setUniform(viewPosLocation, Axes::CameraPos());

    m_Axes = Axes::New();
}

void Scene::ResizeFrameBuffer() {
    auto viewport = m_Camera->GetScaledViewPort();
    uint32_t width = viewport.x;
    uint32_t height = viewport.y;

    // resize multisample framebuffer
    {
        samples = 4;
        //glGetIntegerv(GL_MAX_SAMPLES, &samples);

        GLFramebuffer fbo;
        fbo.create();
        fbo.target(GL_FRAMEBUFFER);
        fbo.bind();

        GLTexture2dMultisample colorTexture;
        colorTexture.create();
        colorTexture.bind();
        colorTexture.storage(samples, GL_RGBA8, width, height, GL_TRUE);
        fbo.texture(GL_COLOR_ATTACHMENT0, colorTexture, 0);

        GLTexture2dMultisample depthTexture;
        depthTexture.create();
        depthTexture.bind();
        depthTexture.storage(samples, GL_DEPTH_COMPONENT32F, width, height,
                             GL_TRUE);
        fbo.texture(GL_DEPTH_ATTACHMENT, depthTexture, 0);

        fbo.release();

        m_ColorTextureMultisampled = std::move(colorTexture);
        m_DepthTextureMultisampled = std::move(depthTexture);
        m_FramebufferMultisampled = std::move(fbo);

        if (m_FramebufferMultisampled.checkStatus() != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                      << std::endl;
    }

    // resize resolve framebuffer(form multisamples to single sample)
    {
        auto width = m_Camera->GetScaledViewPort().x;
        auto height = m_Camera->GetScaledViewPort().y;
        int mipLevels =
                static_cast<int>(std::ceil(std::log2(std::max(width, height))));

        GLFramebuffer fbo;
        fbo.create();
        fbo.target(GL_FRAMEBUFFER);
        fbo.bind();

        GLTexture2d colorTexture;
        colorTexture.create();
        colorTexture.bind();
        colorTexture.storage(mipLevels, GL_RGBA8, width, height);
        colorTexture.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        colorTexture.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        colorTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        colorTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        fbo.texture(GL_COLOR_ATTACHMENT0, colorTexture, 0);

        GLTexture2d depthTexture;
        depthTexture.create();
        depthTexture.bind();
        depthTexture.storage(1, GL_R32F, width, height);
        depthTexture.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        depthTexture.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        depthTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        depthTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        fbo.texture(GL_COLOR_ATTACHMENT1, depthTexture, 0);

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        fbo.drawBuffers(2, buffers);

        fbo.release();

        m_ColorTextureResolved = std::move(colorTexture);
        m_DepthTextureResolved = std::move(depthTexture);
        m_FramebufferResolved = std::move(fbo);

        if (m_FramebufferResolved.checkStatus() != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                      << std::endl;
    }

    ResizeDepthPyramid();
}
void Scene::ResizeDepthPyramid() {
#ifdef IGAME_OPENGL_VERSION_460
    auto width = m_Camera->GetScaledViewPort().x;
    auto height = m_Camera->GetScaledViewPort().y;

    static auto previousPow2 = [](uint32_t v) {
        uint32_t r = 1;
        while (r * 2 < v) r *= 2;
        return r;
    };
    static auto compDepthMipLevels = [](uint32_t width, uint32_t height) {
        uint32_t result = 1;
        while (width > 1 || height > 1) {
            result++;
            width /= 2;
            height /= 2;
        }
        return result;
    };

    m_DepthPyramidWidth = previousPow2(width);
    m_DepthPyramidHeight = previousPow2(height);
    m_DepthPyramidLevels =
            compDepthMipLevels(m_DepthPyramidWidth, m_DepthPyramidHeight);

    GLTexture2d texture;
    texture.create();
    texture.bind();
    texture.storage(m_DepthPyramidLevels, GL_R32F, m_DepthPyramidWidth,
                    m_DepthPyramidHeight);
    texture.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    texture.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture.release();

    m_DepthPyramid = std::move(texture);
#endif
}

void Scene::Draw() {
    // save default framebuffer, because it is not 0 in Qt
    GLint defaultFramebuffer = GL_NONE;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    auto width = m_Camera->GetScaledViewPort().x;
    auto height = m_Camera->GetScaledViewPort().y;

    // render to multisample framebuffer
    {
        m_FramebufferMultisampled.bind();

        // reversed-z buffer, depth range: 1.0(near plane) -> 0.0(far plane)
        glClearColor(m_BackgroundColor.r, m_BackgroundColor.g,
                     m_BackgroundColor.b, 1.0f);
        glClearDepth(0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use reversed-z buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);
        DrawFrame();
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);

        m_FramebufferMultisampled.release();
    }

    // resolve to single sample framebuffer
    {
        m_FramebufferResolved.bind();
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        auto shader = GetShader(Scene::ATTACHMENTRESOLVE);
        shader->use();

        shader->setUniform(shader->getUniformLocation("numSamples"), samples);
        m_ColorTextureMultisampled.active(GL_TEXTURE1);
        shader->setUniform(shader->getUniformLocation("colorTextureMS"), 1);
        m_DepthTextureMultisampled.active(GL_TEXTURE2);
        shader->setUniform(shader->getUniformLocation("depthTextureMS"), 2);

        m_EmptyVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        m_EmptyVAO.release();

        m_FramebufferResolved.release();
    }

    // render to screen
    {
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // generate mipmap for screen texture
        m_ColorTextureResolved.generateMipmap();

        auto shader = GetShader(Scene::SCREEN);
        shader->use();

        m_ColorTextureResolved.active(GL_TEXTURE1);
        m_DepthTextureResolved.active(GL_TEXTURE2);
        m_DepthPyramid.active(GL_TEXTURE3);
        shader->setUniform(shader->getUniformLocation("screenTexture"), 1);

        m_EmptyVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        m_EmptyVAO.release();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    CalculateFrameRate();
    GLCheckError();
}

void Scene::RefreshDepthPyramid() {
#ifdef IGAME_OPENGL_VERSION_460
    auto shader = GetShader(DEPTHREDUCE);
    shader->use();
    m_DepthTextureMultisampled.active(GL_TEXTURE1);
    m_DepthPyramid.active(GL_TEXTURE2);
    shader->setUniform(shader->getUniformLocation("screenDepthMS"), 1);
    shader->setUniform(shader->getUniformLocation("myDepthPyramid"), 2);

    // generate level 0
    {
        unsigned int level = 0;
        uint32_t width = m_DepthPyramidWidth;
        uint32_t height = m_DepthPyramidHeight;
        shader->use();
        shader->setUniform(shader->getUniformLocation("level"), level);
        shader->setUniform(shader->getUniformLocation("outDepthPyramidSize"),
                           igm::uvec2{width, height});
        m_DepthPyramid.bindImage(0, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((width + 31) / 32, (height + 31) / 32, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    // generate other level
    for (unsigned int level = 1; level < m_DepthPyramidLevels; ++level) {
        uint32_t width = m_DepthPyramidWidth >> (level - 1);
        uint32_t height = m_DepthPyramidHeight >> (level - 1);
        if (width < 1) width = 1;
        if (height < 1) height = 1;

        uint32_t levelWidth = width >> 1;
        uint32_t levelHeight = height >> 1;
        if (levelWidth < 1) levelWidth = 1;
        if (levelHeight < 1) levelHeight = 1;

        shader->use();

        shader->use();
        shader->setUniform(shader->getUniformLocation("level"), level);
        shader->setUniform(shader->getUniformLocation("inDepthPyramidSize"),
                           igm::uvec2{width, height});
        m_DepthPyramid.bindImage(0, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

        glDispatchCompute((levelWidth + 31) / 32, (levelHeight + 31) / 32, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
#endif
}

void Scene::Update() {
    if (m_UpdateFunctor) { m_UpdateFunctor(); }
}

void Scene::Resize(int width, int height, int pixelRatio) {
    m_Camera->SetViewPort(width, height);
    m_Camera->SetDevicePixelRatio(pixelRatio);
    ResizeFrameBuffer();
}

void Scene::DrawFrame() {
    // update ubo data in CPU
    GLCheckError();
    UpdateUniformData();
    GLCheckError();
    DrawModels();
    GLCheckError();
    DrawAxes();
    GLCheckError();
}

void Scene::DrawModels() {
    auto viewport = m_Camera->GetScaledViewPort();
    glViewport(0, 0, viewport.x, viewport.y);

#ifdef IGAME_OPENGL_VERSION_330
    for (auto& [id, obj]: m_Models) {
        obj->m_DataObject->ConvertToDrawableData();
        obj->Draw(this);
    }
#elif IGAME_OPENGL_VERSION_460
    bool debug = false;
    if (debug) {
        //std::cout << "-------:Draw:-------" << std::endl;
        RefreshDrawCullDataBuffer();

        for (auto& [id, obj]: m_Models) {
            obj->m_DataObject->ConvertToDrawableData();
        }

        for (auto& [id, obj]: m_Models) {
            obj->m_DataObject->TestOcclusionResults(this);
        }

        // draw phase1: draw visible meshlet
        for (auto& [id, obj]: m_Models) { obj->m_DataObject->DrawPhase1(this); }

        // refresh phase1: generate loacl hierarchical z-buffer
        RefreshDepthPyramid();

        // draw phase2: draw invisible meshlet
        for (auto& [id, obj]: m_Models) { obj->m_DataObject->DrawPhase2(this); }

        // refresh phase2: generate global hierarchical z-buffer
        RefreshDepthPyramid();
    } else {
        for (auto& [id, obj]: m_Models) {
            obj->m_DataObject->ConvertToDrawableData();
            obj->Draw(this);
        }
    }
#endif
}

void Scene::UpdateUniformData() {
    // update camera data matrix
    m_CameraData.view = m_Camera->GetViewMatrix();
    m_CameraData.proj = m_Camera->GetProjectionMatrixReversedZ();
    m_CameraData.proj_view = m_Camera->GetProjectionMatrixReversedZ() *
                             m_Camera->GetViewMatrix();

    // update object data matrix
    m_ObjectData.model = m_ModelMatrix;
    m_ObjectData.normal = m_ObjectData.model.invert().transpose();

    // update other ubo
    m_UBO.viewPos = m_Camera->GetCameraPos();
}

void Scene::UseColor() { this->UBO().useColor = true; }

void Scene::UpdateUniformBuffer() {
    // update camera data matrix
    m_CameraDataBlock.subData(0, sizeof(CameraDataBuffer), &m_CameraData);

    // update object data matrix
    m_ObjectDataBlock.subData(0, sizeof(ObjectDataBuffer), &m_ObjectData);

    // update other ubo
    m_UBOBlock.subData(0, sizeof(UniformBufferObjectBuffer), &m_UBO);
}

void Scene::DrawAxes() {
    glClear(GL_DEPTH_BUFFER_BIT);

    uint32_t width = m_Camera->GetScaledViewPort().x;
    uint32_t height = m_Camera->GetScaledViewPort().y;
    float scale = static_cast<float>(std::max(width, height)) / 10.0f;
    igm::vec4 viewport = igm::vec4{0, 0, scale, scale};
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

    auto axesShader = this->GetShader(Scene::AXES);
    axesShader->use();

    GLUniform modelLocation = axesShader->getUniformLocation("model");
    axesShader->setUniform(modelLocation, m_ModelRotate);

    GLUniform isFontLocation = axesShader->getUniformLocation("isDrawFont");

    // draw Axes
    {
        axesShader->setUniform(isFontLocation, false);
        m_Axes->DrawAxes();
    }

    // draw Axes Font
    {
        axesShader->setUniform(isFontLocation, true);
        GLUniform textureUniform =
                axesShader->getUniformLocation("fontTexture");
        GLUniform colorUniform = axesShader->getUniformLocation("textColor");

        m_Axes->Update(Axes::ProjMatrix() * Axes::ViewMatrix() * m_ModelRotate,
                       {viewport.x, viewport.y, viewport.z, viewport.w});
        m_Axes->DrawXYZ(axesShader, textureUniform, colorUniform);
    }
}

void Scene::RefreshDrawCullDataBuffer() {
    igm::mat4 projection = m_Camera->GetProjectionMatrixReversedZ();
    igm::mat4 projectionT = projection.transpose();

    igm::vec4 frustumX =
            (projectionT[3] + projectionT[0]).normalized(); // x + w < 0
    igm::vec4 frustumY =
            (projectionT[3] + projectionT[1]).normalized(); // y + w < 0

    DrawCullData cullData = {};
    cullData.view_model = m_Camera->GetViewMatrix() * m_ModelMatrix;
    cullData.P00 = projection[0][0];
    cullData.P11 = projection[1][1];
    cullData.zNear = projection[3][2];
    cullData.frustum[0] = frustumX.x;
    cullData.frustum[1] = frustumX.z;
    cullData.frustum[2] = frustumY.y;
    cullData.frustum[3] = frustumY.z;
    cullData.pyramidWidth = static_cast<float>(m_DepthPyramidWidth);
    cullData.pyramidHeight = static_cast<float>(m_DepthPyramidHeight);

    m_DrawCullData.subData(0, sizeof(DrawCullData), &cullData);
}

void Scene::lookAtPositiveX() {
    auto radians = static_cast<float>(igm::radians(90.0f));
    auto matrix =
            igm::rotate(igm::mat4{}, -radians, igm::vec3{1.0f, 0.0f, 0.0f}) *
            igm::rotate(igm::mat4{}, radians, igm::vec3{0.0f, 0.0f, 1.0f});

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::lookAtNegativeX() {
    auto radians = static_cast<float>(igm::radians(90.0f));
    auto matrix =
            igm::rotate(igm::mat4{}, -radians, igm::vec3{0.0f, 0.0f, 1.0f}) *
            igm::rotate(igm::mat4{}, -radians, igm::vec3{0.0f, 1.0f, 0.0f});

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::lookAtPositiveY() {
    auto radians = static_cast<float>(igm::radians(90.0f));
    auto matrix =
            igm::rotate(igm::mat4{}, -radians, igm::vec3{1.0f, 0.0f, 0.0f});

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::lookAtNegativeY() {
    auto radians = static_cast<float>(igm::radians(90.0f));
    auto matrix =
            igm::rotate(igm::mat4{}, radians, igm::vec3{1.0f, 0.0f, 0.0f}) *
            igm::rotate(igm::mat4{}, 2 * radians, igm::vec3{0.0f, 1.0f, 0.0f});

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::lookAtPositiveZ() {
    auto radians = static_cast<float>(igm::radians(90.0f));
    auto matrix =
            igm::rotate(igm::mat4{}, 2 * radians, igm::vec3{0.0f, 1.0f, 0.0f});

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::lookAtNegativeZ() {
    auto matrix = igm::mat4{};

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::lookAtIsometric() {
    auto radians = static_cast<float>(igm::radians(45.0f));
    auto matrix =
            igm::rotate(igm::mat4{}, radians, igm::vec3{1.0f, 0.0f, 0.0f}) *
            igm::rotate(igm::mat4{}, -radians, igm::vec3{0.0f, 1.0f, 0.0f});

    m_ModelMatrix = matrix * m_ModelRotate.invert() * m_ModelMatrix;
    m_ModelRotate = matrix;
    ResetCenter();
};
void Scene::rotateNinetyClockwise() {
    auto rotateMatrix = igm::mat4{};
    auto radians = static_cast<float>(igm::radians(90.0f));
    rotateMatrix =
            igm::rotate(igm::mat4{}, -radians, igm::vec3{0.0f, 0.0f, 1.0f});

    igm::vec4 center = igm::vec4{m_ModelsBoundingSphere.xyz(), 1.0f};
    igm::vec3 centerInWorld = (m_ModelMatrix * center).xyz();
    igm::mat4 translateToOrigin = igm::translate(igm::mat4{}, -centerInWorld);
    igm::mat4 translateBack = igm::translate(igm::mat4{}, centerInWorld);
    igm::mat4 rotate = translateBack * rotateMatrix * translateToOrigin;

    m_ModelMatrix = rotate * (m_ModelMatrix);
    m_ModelRotate = rotateMatrix * (m_ModelRotate);
};
void Scene::rotateNinetyCounterClockwise() {
    auto rotateMatrix = igm::mat4{};
    auto radians = static_cast<float>(igm::radians(90.0f));
    rotateMatrix =
            igm::rotate(igm::mat4{}, radians, igm::vec3{0.0f, 0.0f, 1.0f});

    igm::vec4 center = igm::vec4{m_ModelsBoundingSphere.xyz(), 1.0f};
    igm::vec3 centerInWorld = (m_ModelMatrix * center).xyz();
    igm::mat4 translateToOrigin = igm::translate(igm::mat4{}, -centerInWorld);
    igm::mat4 translateBack = igm::translate(igm::mat4{}, centerInWorld);
    igm::mat4 rotate = translateBack * rotateMatrix * translateToOrigin;

    m_ModelMatrix = rotate * (m_ModelMatrix);
    m_ModelRotate = rotateMatrix * (m_ModelRotate);
};

void Scene::UpdateModelsBoundingSphere() {
    // update all models bounding sphere
    igm::vec3 min(FLT_MAX);
    igm::vec3 max(-FLT_MAX);

    for (auto& [id, obj]: m_Models) {
        if (!obj->m_DataObject->GetVisibility()) continue;

        auto box = obj->m_DataObject->GetBoundingBox();
        Vector3f boxMin = box.min;
        Vector3f boxMax = box.max;

        min = igm::min(igm::vec3{boxMin[0], boxMin[1], boxMin[2]}, min);
        max = igm::max(igm::vec3{boxMax[0], boxMax[1], boxMax[2]}, max);
    };
    igm::vec3 center = (min + max) / 2;
    float radius = (max - min).length() / 2;

    m_ModelsBoundingSphere = igm::vec4{center, radius};
}

void Scene::CalculateFrameRate() {
    static float framesPerSecond = 0.0f; // This will store our fps
    static auto lastTime = std::chrono::high_resolution_clock ::now();

    auto currentTime = std::chrono::high_resolution_clock ::now();
    ++framesPerSecond;

    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - lastTime)
                         .count();
    if (time > 1.0f) {
        lastTime = currentTime;
        //std::cout << framesPerSecond << std::endl;
        framesPerSecond = 0;
    }
}
IGAME_NAMESPACE_END