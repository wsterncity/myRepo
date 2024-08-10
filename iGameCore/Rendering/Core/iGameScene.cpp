#include "iGameScene.h"
#include "iGameCommand.h"

IGAME_NAMESPACE_BEGIN
Scene::Scene() {
    m_Camera = Camera::New();
    m_Camera->Initialize(igm::vec3{0.0f, 0.0f, 1.0f});

    m_ModelRotate = igm::mat4(1.0f);
    m_BackgroundColor = {0.5f, 0.5f, 0.5f};

    InitOpenGL();
    InitFont();
    InitAxes();
}
Scene::~Scene() {}

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

Model::Pointer Scene::AddDataObject(DataObject::Pointer obj) {
    Model::Pointer model = Model::New();
    model->m_DataObject = obj;
    m_Models.insert(std::make_pair<>(obj->GetDataObjectId(), model));
    m_CurrentModelId = obj->GetDataObjectId();
    m_CurrentModel = model.get();
    m_CurrentObject = obj.get();
    model->m_Scene = this;

    ChangeDataObjectVisibility(m_CurrentModelId, true);
    UpdateModelsBoundingSphere();
    return model;
}

void Scene::RemoveDataObject(DataObject::Pointer obj) {
    for (auto it = m_Models.begin(); it != m_Models.end(); ++it) {
        if (it->second->m_DataObject == obj) {
            m_Models.erase(it);
            m_CurrentModelId = m_Models.begin()->first;
            m_CurrentModel = m_Models.begin()->second;
            m_CurrentObject = m_CurrentModel->m_DataObject;
            break;
        }
    }
    UpdateModelsBoundingSphere();
}

void Scene::RemoveCurrentDataObject() {
    if (auto visibility = m_CurrentObject->GetVisibility()) {
        m_VisibleModelsCount--;
    }

    m_Models.erase(m_CurrentModelId);
    if (m_Models.empty()) return;
    m_CurrentModelId = m_Models.begin()->first;
    m_CurrentModel = m_Models.begin()->second;
    m_CurrentObject = m_CurrentModel->m_DataObject;
    UpdateModelsBoundingSphere();
}

bool Scene::UpdateCurrentDataObject(int index) {
    for (auto& [id, obj]: m_Models) {
        if (id == index) {
            m_CurrentModelId = id;
            m_CurrentModel = obj.get();
            m_CurrentObject = m_CurrentModel->m_DataObject;
            return true;
        }
        if (obj->m_DataObject->HasSubDataObject()) {
            auto subObj = obj->m_DataObject->GetSubDataObject(index);
            if (subObj != nullptr) {
                m_CurrentModelId = index;
                m_CurrentObject = subObj.get();
                return true;
            }
        }
    }
    return false;
}

void Scene::UpdateModelsBoundingSphere() {
    // update all models bounding sphere
    igm::vec3 min(FLT_MAX);
    igm::vec3 max(-FLT_MAX);

    for (auto& [id, obj]: m_Models) {
        auto box = obj->m_DataObject->GetBoundingBox();
        Vector3f boxMin = box.min;
        Vector3f boxMax = box.max;

        min = igm::min(igm::vec3{boxMin[0], boxMin[1], boxMin[2]}, min);
        max = igm::max(igm::vec3{boxMax[0], boxMax[1], boxMax[2]}, max);
    };
    igm::vec3 center = (min + max) / 2;
    float radius = (max - min).length() / 2;

    m_ModelsBoundingSphere = igm::vec4{center, radius};

    // update camera far plane to cover all models
    auto pos = m_Camera->GetCamaraPos();
    m_Camera->SetFarPlane((pos - center).length() + m_ModelsBoundingSphere.w);
}

DataObject* Scene::GetDataObject(int index) {
    for (auto& [id, obj]: m_Models) {
        if (id == index) { return obj->m_DataObject.get(); }
        if (obj->m_DataObject->HasSubDataObject()) {
            auto subObj = obj->m_DataObject->GetSubDataObject(index);
            if (subObj != nullptr) { return subObj.get(); }
        }
    }
    return nullptr;
}

DataObject* Scene::GetCurrentObject() { return m_CurrentObject; }

Model* Scene::GetCurrentModel() { return m_CurrentModel; }

void Scene::ChangeViewStyle(IGenum mode) {
    m_CurrentObject->SetViewStyle(mode);
}

void Scene::ChangeDataObjectVisibility(int index, bool visibility) {
    auto* obj = GetDataObject(index);
    obj->SetVisibility(visibility);

    if (visibility) {
        m_VisibleModelsCount++;
        if (m_VisibleModelsCount == 1) {
            Vector3f center = obj->GetBoundingBox().center();
            float radius = obj->GetBoundingBox().diag() / 2;
            m_Camera->SetCamaraPos(center[0], center[1],
                                   center[2] + 2.0f * radius);
        }
    } else {
        m_VisibleModelsCount--;
    }
}

std::map<int, Model::Pointer>& Scene::GetModelList() {
    return m_Models;
}

void Scene::InitOpenGL() {
    if (!gladLoadGL()) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        auto patchShader = this->GetShader(PATCH);
        patchShader->mapUniformBlock("CameraDataBlock", 0, m_CameraDataBlock);
        patchShader->mapUniformBlock("ObjectDataBLock", 1, m_ObjectDataBlock);
        patchShader->mapUniformBlock("UniformBufferObjectBlock", 2, m_UBOBlock);

        auto noLightShader = this->GetShader(NOLIGHT);
        noLightShader->mapUniformBlock("CameraDataBlock", 0, m_CameraDataBlock);
        noLightShader->mapUniformBlock("ObjectDataBlock", 1, m_ObjectDataBlock);
        noLightShader->mapUniformBlock("UniformBufferObjectBlock", 2,
                                       m_UBOBlock);

        auto cullComputeShader = this->GetShader(MESHLETCULL);
        cullComputeShader->mapUniformBlock("CameraDataBlock", 0,
                                           m_CameraDataBlock);
    }

    // init screen quad VAO
    {
        m_ScreenQuadVAO.create();
        m_ScreenQuadVBO.create();
        m_ScreenQuadVBO.target(GL_ARRAY_BUFFER);

        float quadVertices[] = {// positions   // texCoords
                                -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                                0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,
                                -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                                1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
        m_ScreenQuadVBO.allocate(sizeof(quadVertices), quadVertices,
                                 GL_STATIC_DRAW);

        // bind VBO to VAO
        m_ScreenQuadVAO.vertexBuffer(GL_VBO_IDX_0, m_ScreenQuadVBO, 0,
                                     4 * sizeof(float));
        // position
        GLSetVertexAttrib(m_ScreenQuadVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 2,
                          GL_FLOAT, GL_FALSE, 0);
        // texture coord
        GLSetVertexAttrib(m_ScreenQuadVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_0, 2,
                          GL_FLOAT, GL_FALSE, 2 * sizeof(float));
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
    uint32_t width = m_Camera->GetViewPort().x;
    uint32_t height = m_Camera->GetViewPort().y;

    // multisample framebuffer
    GLint samples = 4;
    //glGetIntegerv(GL_MAX_SAMPLES, &samples);

    {
        GLFramebuffer fbo;
        fbo.create();
        fbo.target(GL_FRAMEBUFFER);
        fbo.bind();

        GLTexture2dMultisample colorTexture;
        colorTexture.create();
        colorTexture.bind();
        colorTexture.storage(samples, GL_RGB8, width, height, GL_TRUE);
        fbo.texture(GL_COLOR_ATTACHMENT0, colorTexture, 0);

        GLTexture2dMultisample depthTexture;
        depthTexture.create();
        depthTexture.bind();
        depthTexture.storage(samples, GL_DEPTH_COMPONENT32F, width, height,
                             GL_TRUE);
        fbo.texture(GL_DEPTH_ATTACHMENT, depthTexture, 0);

        fbo.release();

        m_FramebufferMultisampled = std::move(fbo);

        if (m_FramebufferMultisampled.checkStatus() != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                      << std::endl;
    }

    // second post-processing framebuffer(resolve)
    {
        GLFramebuffer fbo;
        fbo.create();
        fbo.target(GL_FRAMEBUFFER);
        fbo.bind();

        GLTexture2d colorTexture;
        colorTexture.create();
        colorTexture.bind();
        colorTexture.storage(1, GL_RGB8, width, height);
        colorTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        colorTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        fbo.texture(GL_COLOR_ATTACHMENT0, colorTexture, 0);

        GLTexture2d depthTexture;
        depthTexture.create();
        depthTexture.bind();
        depthTexture.storage(1, GL_DEPTH_COMPONENT32F, width, height);
        depthTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        depthTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        fbo.texture(GL_DEPTH_ATTACHMENT, depthTexture, 0);

        fbo.release();

        m_Framebuffer = std::move(fbo);
        m_ColorTexture = std::move(colorTexture);
        m_DepthTexture = std::move(depthTexture);

        if (m_Framebuffer.checkStatus() != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                      << std::endl;
    }

#ifndef __APPLE__
    ResizeHizTexture();
#endif
}
void Scene::ResizeHizTexture() {
    uint32_t width = m_Camera->GetViewPort().x;
    uint32_t height = m_Camera->GetViewPort().y;

    m_DepthPyramidWidth = width;
    m_DepthPyramidHeight = height;
    m_DepthPyramidLevels = 1 + static_cast<unsigned int>(std::floor(
                                       std::log2(std::max(width, height))));

    GLTexture2d texture;
    texture.create();
    texture.bind();
    texture.storage(m_DepthPyramidLevels, GL_R32F, width, height);
    texture.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    texture.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture.release();

    m_DepthPyramid = std::move(texture);
    m_DepthPyramid.getTextureHandle().makeResident();
}

void Scene::Draw() {
    // save default framebuffer, because it is not 0 in Qt
    GLint defaultFramebuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    auto width = m_Camera->GetViewPort().x;
    auto height = m_Camera->GetViewPort().y;

    // render to framebuffer
    {
        m_FramebufferMultisampled.bind();
        glClearColor(m_BackgroundColor.r, m_BackgroundColor.g,
                     m_BackgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawFrame();
        m_FramebufferMultisampled.release();
        //return;
    }

    // blit multisampled buffer(s) to normal colorbuffer of intermediate FBO.
    {
        GLFramebuffer::blit(m_FramebufferMultisampled, m_Framebuffer, 0, 0,
                            width, height, 0, 0, width, height,
                            GL_COLOR_BUFFER_BIT, GL_LINEAR);
        GLFramebuffer::blit(m_FramebufferMultisampled, m_Framebuffer, 0, 0,
                            width, height, 0, 0, width, height,
                            GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    // render to screen
    {
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        auto shader = GetShader(Scene::SCREEN);
        shader->use();

        GLUniform textureUniform = shader->getUniformLocation("screenTexture");
        m_ColorTexture.active(GL_TEXTURE1);
        m_DepthTexture.active(GL_TEXTURE2);
        m_DepthPyramid.active(GL_TEXTURE3);
        shader->setUniform(textureUniform, 1);

        m_ScreenQuadVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_ScreenQuadVAO.release();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //GLCheckError();
}

void Scene::RefreshHizTexture() {
    // blit multisampled z-buffer to normal z-buffer
    {
        GLFramebuffer::blit(m_FramebufferMultisampled, m_Framebuffer, 0, 0,
                            m_DepthPyramidWidth, m_DepthPyramidHeight, 0, 0,
                            m_DepthPyramidWidth, m_DepthPyramidHeight,
                            GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    // copy level 0
    GLTexture2d::copyImageSubData(m_DepthTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
                                  m_DepthPyramid, GL_TEXTURE_2D, 0, 0, 0, 0,
                                  m_DepthPyramidWidth, m_DepthPyramidHeight, 1);

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

        auto shader = GetShader(DEPTHREDUCE);
        shader->use();

        shader->setUniform(shader->getUniformLocation("inImage"),
                           m_DepthPyramid.getTextureHandle());

        shader->setUniform(shader->getUniformLocation("inImageSize"),
                           igm::uvec2{width, height});

        shader->setUniform(shader->getUniformLocation("level"), level);

        m_DepthPyramid.bindImage(0, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

        glDispatchCompute((levelWidth + 15) / 16, (levelHeight + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
};

void Scene::Update() {
    if (m_UpdateFunctor) { 
        m_UpdateFunctor();
    }
}

void Scene::Resize(int width, int height, int pixelRatio) {
    m_Camera->SetViewPort(width, height);
    m_Camera->SetDevicePixelRatio(pixelRatio);
    ResizeFrameBuffer();
}

void Scene::DrawFrame() {
    // update ubo data in CPU
    UpdateUniformData();

    DrawModels();
    DrawAxes();
}

void Scene::DrawModels() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, m_Camera->GetViewPort().x, m_Camera->GetViewPort().y);

#ifdef __APPLE__
    for (auto& [id, obj]: m_Models) {
        obj->ConvertToDrawableData();
        obj->Draw(this);
    }
#else
    bool debug = false;
    if (debug) {
        std::cout << "-------:Draw:-------" << std::endl;
        for (auto& [id, obj]: m_Models) { obj->m_DataObject->ConvertToDrawableData(); }

        // phase1: draw visible meshlet
        for (auto& [id, obj]: m_Models) { obj->m_DataObject->DrawPhase1(this); }

        // phase2: draw invisible meshlet
        RefreshHizTexture();
        for (auto& [id, obj]: m_Models) { obj->m_DataObject->DrawPhase2(this); }

        // phase3: record all visible meshlet
        RefreshHizTexture();
        for (auto& [id, obj]: m_Models) { obj->m_DataObject->DrawPhase3(this); }

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
    m_CameraData.proj = m_Camera->GetProjectionMatrix();
    m_CameraData.projview =
            m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix();

    // update object data matrix
    m_ObjectData.model = m_ModelRotate;
    m_ObjectData.normal = m_ObjectData.model.invert().transpose();

    // update other ubo
    m_UBO.viewPos = m_Camera->GetCamaraPos();
}

void Scene::UpdateUniformBuffer() {
    // update camera data matrix
    m_CameraDataBlock.subData(0, sizeof(CameraDataBuffer), &m_CameraData);

    // update object data matrix
    m_ObjectDataBlock.subData(0, sizeof(ObjectDataBuffer), &m_ObjectData);

    // update other ubo
    m_UBOBlock.subData(0, sizeof(UniformBufferObjectBuffer), &m_UBO);
}

void Scene::DrawAxes() {
    //glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, 200, 200);

    //draw Axes
    auto axesShader = this->GetShader(Scene::AXES);
    GLUniform modelLocation = axesShader->getUniformLocation("model");

    axesShader->use();
    axesShader->setUniform(modelLocation, m_ModelRotate);

    m_Axes->DrawAxes();

    // draw Axes Font
    auto fontShader = this->GetShader(Scene::FONT);
    GLUniform projLocation = fontShader->getUniformLocation("proj");
    auto mvp = Axes::ProjMatrix() * Axes::ViewMatrix() * m_ModelRotate;

    fontShader->use();
    fontShader->setUniform(projLocation, mvp);

    m_Axes->Update(mvp, {0, 0, 200, 200});

    GLUniform textureUniform = fontShader->getUniformLocation("fontTexture");
    GLUniform colorUniform = fontShader->getUniformLocation("textColor");
    m_Axes->DrawXYZ(fontShader, textureUniform, colorUniform);
}

GLBuffer& Scene::GetDrawCullDataBuffer() {
    igm::mat4 projection = m_Camera->GetProjectionMatrix();
    igm::mat4 projectionT = projection.transpose();

    auto normalizePlane = [](igm::vec4 p) { return p / igm::vec3(p).length(); };
    igm::vec4 frustumX =
            normalizePlane(projectionT[3] + projectionT[0]); // x + w < 0
    igm::vec4 frustumY =
            normalizePlane(projectionT[3] + projectionT[1]); // y + w < 0

    DrawCullData cullData = {};
    cullData.modelview = m_Camera->GetViewMatrix() * m_ModelRotate;
    cullData.P00 = projection[0][0];
    cullData.P11 = projection[1][1];
    cullData.znear = m_Camera->GetNearPlane();
    cullData.zfar = m_Camera->GetFarPlane();
    cullData.frustum[0] = frustumX.x;
    cullData.frustum[1] = frustumX.z;
    cullData.frustum[2] = frustumY.y;
    cullData.frustum[3] = frustumY.z;
    cullData.pyramidWidth = static_cast<float>(m_DepthPyramidWidth);
    cullData.pyramidHeight = static_cast<float>(m_DepthPyramidHeight);

    m_DrawCullData.subData(0, sizeof(DrawCullData), &cullData);

    return m_DrawCullData;
}

IGAME_NAMESPACE_END