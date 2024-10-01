#include "iGameModel.h"
#include "iGameFilter.h"
#include "iGameInteractor.h"
#include "iGameScene.h"


IGAME_NAMESPACE_BEGIN
void Model::Draw(Scene* scene) {
    auto visibility = m_DataObject->m_Visibility;
    auto useColor = m_DataObject->m_UseColor;
    auto colorWithCell = m_DataObject->m_ColorWithCell;
    auto viewStyle = m_DataObject->m_ViewStyle;

    GLCheckError();
    if (!visibility) { return; }

    //if (m_DataObject.m_DrawMesh) {
    //    m_DrawMesh->SetViewStyle(m_ViewStyle);
    //    return m_DrawMesh->Draw(scene);
    //}

    // update uniform buffer
    if (useColor) {
        scene->UBO().useColor = true;
    } else {
        scene->UBO().useColor = false;
    }
    scene->UpdateUniformBuffer();

    if (useColor && colorWithCell) {
        scene->GetShader(Scene::BLINNPHONG)->use();
        m_DataObject->m_CellVAO.bind();
        glad_glDrawArrays(GL_TRIANGLES, 0, m_DataObject->m_CellPositionSize);
        m_DataObject->m_CellVAO.release();
        return;
    }

    if (viewStyle & IG_POINTS) {
        scene->GetShader(Scene::NOLIGHT)->use();

        m_DataObject->m_PointVAO.bind();
        glad_glPointSize(8);
        glad_glDepthRange(0.000001, 1);
        glad_glDrawArrays(GL_POINTS, 0,
                          m_DataObject->m_Positions->GetNumberOfValues() / 3);
        glad_glDepthRange(0, 1);
        m_DataObject->m_PointVAO.release();
    }

    if (viewStyle & IG_WIREFRAME) {
        if (useColor) {
            scene->GetShader(Scene::NOLIGHT)->use();
        } else {
            auto shader = scene->GetShader(Scene::PURECOLOR);
            shader->use();
            shader->setUniform(shader->getUniformLocation("inputColor"),
                               igm::vec3{0.0f, 0.0f, 0.0f});
        }

        m_DataObject->m_LineVAO.bind();
        glLineWidth(m_DataObject->m_LineWidth);
        glad_glDrawElements(GL_LINES,
                            m_DataObject->m_LineIndices->GetNumberOfIds(),
                            GL_UNSIGNED_INT, 0);
        m_DataObject->m_LineVAO.release();
    }

    if (viewStyle & IG_SURFACE) {
        auto shader = scene->GetShader(Scene::BLINNPHONG);
        shader->use();

        m_DataObject->m_TriangleVAO.bind();
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-0.5f, -0.5f);
        glad_glDrawElements(GL_TRIANGLES,
                            m_DataObject->m_TriangleIndices->GetNumberOfIds(),
                            GL_UNSIGNED_INT, 0);
        glDisable(GL_POLYGON_OFFSET_FILL);
        m_DataObject->m_TriangleVAO.release();
    }
    GLCheckError();

    m_Painter->Draw(scene);
    GLCheckError();
}

void Model::DrawPhase1(Scene* scene) {
#ifdef IGAME_OPENGL_VERSION_460
    // std::cout << "Draw phase 1:" << std::endl;

    auto visibility = m_DataObject->m_Visibility;
    auto useColor = m_DataObject->m_UseColor;
    auto colorWithCell = m_DataObject->m_ColorWithCell;
    auto viewStyle = m_DataObject->m_ViewStyle;
    auto meshlets = m_DataObject->m_Meshlets;

    if (!visibility) { return; }

    // update uniform buffer
    {
        if (useColor) {
            scene->UBO().useColor = true;
        } else {
            scene->UBO().useColor = false;
        }
        scene->UpdateUniformBuffer();
    }

    // draw
    if (useColor && colorWithCell) {}

    if (viewStyle & IG_POINTS) {}
    if (viewStyle & IG_WIREFRAME) {}
    if (viewStyle & IG_SURFACE) {
        scene->GetShader(Scene::BLINNPHONG)->use();
        m_DataObject->m_TriangleVAO.bind();
        unsigned int count = 0;
        meshlets->VisibleMeshletBuffer().getSubData(0, sizeof(unsigned int),
                                                    &count);
        meshlets->FinalDrawCommandBuffer().target(GL_DRAW_INDIRECT_BUFFER);
        meshlets->FinalDrawCommandBuffer().bind();
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
                                    count, 0);
        std::cout << "Draw phase 1: [render count: " << count;
        std::cout << ", meshlet count: " << meshlets->MeshletsCount() << "]"
                  << std::endl;
        m_DataObject->m_TriangleVAO.release();
    }
#endif
}

void Model::DrawPhase2(Scene* scene) {
#ifdef IGAME_OPENGL_VERSION_460
    // std::cout << "Draw phase 2:" << std::endl;
    auto visibility = m_DataObject->m_Visibility;
    auto useColor = m_DataObject->m_UseColor;
    auto colorWithCell = m_DataObject->m_ColorWithCell;
    auto viewStyle = m_DataObject->m_ViewStyle;
    auto meshlets = m_DataObject->m_Meshlets;

    if (!visibility) { return; }

    // update uniform buffer
    {
        if (useColor) {
            scene->UBO().useColor = true;
        } else {
            scene->UBO().useColor = false;
        }
        scene->UpdateUniformBuffer();
    }

    // draw
    if (useColor && colorWithCell) {}

    if (viewStyle & IG_POINTS) {}
    if (viewStyle & IG_WIREFRAME) {}
    if (viewStyle & IG_SURFACE) {
        // compute culling
        {
            auto shader = scene->GetShader(Scene::MESHLETCULL);
            shader->use();

            GLUniform workMode = shader->getUniformLocation("workMode");
            shader->setUniform(workMode, 0);

            meshlets->MeshletsBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->MeshletsBuffer().bindBase(1);

            meshlets->DrawCommandBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->DrawCommandBuffer().bindBase(2);

            unsigned int data = 0;
            meshlets->VisibleMeshletBuffer().subData(0, sizeof(unsigned int),
                                                     &data);
            meshlets->VisibleMeshletBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->VisibleMeshletBuffer().bindBase(3);

            meshlets->FinalDrawCommandBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->FinalDrawCommandBuffer().bindBase(4);

            scene->GetDrawCullDataBuffer().target(GL_UNIFORM_BUFFER);
            scene->GetDrawCullDataBuffer().bindBase(5);

            scene->DepthPyramid().active(GL_TEXTURE1);
            shader->setUniform(shader->getUniformLocation("depthPyramid"), 1);

            auto count = meshlets->MeshletsCount();
            glDispatchCompute(((count + 255) / 256), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        scene->GetShader(Scene::BLINNPHONG)->use();
        m_DataObject->m_TriangleVAO.bind();

        unsigned int count = 0;
        meshlets->VisibleMeshletBuffer().getSubData(0, sizeof(unsigned int),
                                                    &count);

        meshlets->FinalDrawCommandBuffer().target(GL_DRAW_INDIRECT_BUFFER);
        meshlets->FinalDrawCommandBuffer().bind();
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
                                    count, 0);

        std::cout << "Draw phase 2: [render count: " << count;
        std::cout << ", meshlet count: " << meshlets->MeshletsCount() << "]"
                  << std::endl;

        m_DataObject->m_TriangleVAO.release();
    }

#endif
}

void Model::TestOcclusionResults(Scene* scene) {
#ifdef IGAME_OPENGL_VERSION_460
    // std::cout << "Test Occlusion:" << std::endl;
    auto visibility = m_DataObject->m_Visibility;
    auto useColor = m_DataObject->m_UseColor;
    auto colorWithCell = m_DataObject->m_ColorWithCell;
    auto viewStyle = m_DataObject->m_ViewStyle;
    auto meshlets = m_DataObject->m_Meshlets;

    if (!visibility) { return; }

    if (useColor && colorWithCell) {}

    if (viewStyle & IG_POINTS) {}
    if (viewStyle & IG_WIREFRAME) {}
    if (viewStyle & IG_SURFACE) {
        // compute culling
        {
            auto shader = scene->GetShader(Scene::MESHLETCULL);
            shader->use();

            GLUniform workMode = shader->getUniformLocation("workMode");
            shader->setUniform(workMode, 1);

            meshlets->MeshletsBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->MeshletsBuffer().bindBase(1);

            meshlets->DrawCommandBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->DrawCommandBuffer().bindBase(2);

            unsigned int data = 0;
            meshlets->VisibleMeshletBuffer().subData(0, sizeof(unsigned int),
                                                     &data);
            meshlets->VisibleMeshletBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->VisibleMeshletBuffer().bindBase(3);

            meshlets->FinalDrawCommandBuffer().target(GL_SHADER_STORAGE_BUFFER);
            meshlets->FinalDrawCommandBuffer().bindBase(4);

            scene->GetDrawCullDataBuffer().target(GL_UNIFORM_BUFFER);
            scene->GetDrawCullDataBuffer().bindBase(5);

            scene->DepthPyramid().active(GL_TEXTURE1);
            shader->setUniform(shader->getUniformLocation("depthPyramid"), 1);

            auto count = meshlets->MeshletsCount();
            glDispatchCompute(((count + 255) / 256), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }

    unsigned int count = 0;
    meshlets->VisibleMeshletBuffer().getSubData(0, sizeof(unsigned int),
                                                &count);
    // std::cout << "Test Occlusion: [render count: " << count;
    // std::cout << ", meshlet count: " << meshlets->MeshletsCount() << "]"
    //           << std::endl;

    // std::vector<DrawElementsIndirectCommand> readBackCommands(
    //         meshlets->MeshletsCount());
    // meshlets->DrawCommandBuffer().getSubData(
    //         0, readBackCommands.size() * sizeof(DrawElementsIndirectCommand),
    //         readBackCommands.data());
    // for (const auto& cmd: readBackCommands) {
    //     //std::cout << "count: " << cmd.count << std::endl;
    //     std::cout << "primCount: " << cmd.primCount << std::endl;
    //     //std::cout << "firstIndex: " << cmd.firstIndex << std::endl;
    //     //std::cout << "baseVertex: " << cmd.baseVertex << std::endl;
    //     //std::cout << "baseInstance: " << cmd.baseInstance << std::endl;
    // }
#endif
}

void Model::Update() {
    if (m_Scene) { m_Scene->Update(); }
}

Selection* Model::GetSelection() {
    if (m_Selection == nullptr) { m_Selection = Selection::New(); }
    return m_Selection.get();
}

void Model::RequestPointSelection(Points* p, Selection* s) {
    if (m_Scene->GetInteractor() == nullptr) return;
    s->m_Points = p;
    s->m_Model = this;
    m_Scene->GetInteractor()->RequestPointSelectionStyle(s);
}

void Model::RequestDragPoint(Points* p, Selection* s) {
    if (m_Scene->GetInteractor() == nullptr) return;
    s->m_Points = p;
    s->m_Model = this;
    m_Scene->GetInteractor()->RequestDragPointStyle(s);
}

Filter* Model::GetModelFilter() { return m_Filter; }
void Model::DeleteModelFilter() { m_Filter = nullptr; }
void Model::SetModelFilter(SmartPointer<Filter> _filter) { m_Filter = _filter; }

void Model::Show() {
    m_DataObject->SetVisibility(true);
    m_Scene->ChangeModelVisibility(this, true);
    m_Painter->ShowAll();
}

void Model::Hide() {
    m_DataObject->SetVisibility(false);
    m_Scene->ChangeModelVisibility(this, false);
    m_Painter->HideAll();
}

void Model::SetBoundingBoxSwitch(bool action) {
    if (action) {
        SwitchOn(ViewSwitch::BoundingBox);

        auto& bbox = m_DataObject->GetBoundingBox();
        Vector3d p1 = bbox.min;
        Vector3d p7 = bbox.max;

        if (m_BboxHandle != 0) { m_Painter->Delete(m_BboxHandle); }
        m_Painter->SetPen(5);
        m_Painter->SetPen(Color::LightBlue);
        m_Painter->SetBrush(Color::None);
        m_BboxHandle = m_Painter->DrawCube(p1, p7);
    } else {
        SwitchOff(ViewSwitch::BoundingBox);
        m_Painter->Hide(m_BboxHandle);
    }
}

void Model::SetPickedItemSwitch(bool action) {
    if (action) {
        SwitchOn(ViewSwitch::PickedItem);
        if (m_DataObject->GetVisibility()) { m_Painter->ShowAll(); }
    } else {
        SwitchOff(ViewSwitch::PickedItem);
        m_Painter->HideAll();
    }
}

void Model::SetViewPointsSwitch(bool action) {
    if (action) {
        m_DataObject->AddViewStyle(IG_POINTS);
    } else {
        m_DataObject->RemoveViewStyle(IG_POINTS);
    }
}

void Model::SetViewWireframeSwitch(bool action) {
    if (action) {
        m_DataObject->AddViewStyle(IG_WIREFRAME);
    } else {
        m_DataObject->RemoveViewStyle(IG_WIREFRAME);
    }
}

void Model::SetViewFillSwitch(bool action) {
    if (action) {
        m_DataObject->AddViewStyle(IG_SURFACE);
    } else {
        m_DataObject->RemoveViewStyle(IG_SURFACE);
    }
}

Model::Model() {
    m_Painter = Painter::New();

    SwitchOff(ViewSwitch::BoundingBox);
    SwitchOn(ViewSwitch::PickedItem);
}
IGAME_NAMESPACE_END
