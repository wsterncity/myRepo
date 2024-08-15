#include "iGameMeshlet.h"
#include <format>

IGAME_NAMESPACE_BEGIN

void Meshlet::CreateBuffer() {
    m_MeshletsBuffer.create();
    m_DrawCommandBuffer.create();
    m_VisibleMeshletBuffer.create();
    m_FinalDrawCommandBuffer.create();
}

void Meshlet::BuildMeshlet(const float* vertex_positions, size_t vertex_count,
                           const int* indices, size_t index_count) {
    clock_t start, end;
    start = clock();

    // Preprocessing: Vertex Cache Optimization
    std::vector<int> optimized_indices(index_count);
    meshopt_optimizeVertexCache(optimized_indices.data(), indices, index_count,
                                vertex_count);

    // Compute the upper bound of the meshlet
    size_t max_meshlets = meshopt_buildMeshletsBound(index_count, m_MaxVertices,
                                                     m_MaxTriangles);

    // Allocate meshlet data structure
    std::vector<meshopt_Meshlet> meshlets(max_meshlets);
    // Triangle index
    m_MeshletVertices.resize(max_meshlets * m_MaxVertices);
    // Index of triangle index
    m_MeshletTriangles.resize(max_meshlets * m_MaxTriangles * 3);

    // Generate meshlet data
    size_t meshlet_count = meshopt_buildMeshlets(
            meshlets.data(), m_MeshletVertices.data(),
            m_MeshletTriangles.data(), optimized_indices.data(), index_count,
            vertex_positions, vertex_count, sizeof(float) * 3, m_MaxVertices,
            m_MaxTriangles, m_ConeWeight);

    // Resize the vector to fit the actual generated meshlet data
    const meshopt_Meshlet& last = meshlets[meshlet_count - 1];
    m_MeshletVertices.resize(last.vertex_offset + last.vertex_count);
    m_MeshletTriangles.resize(last.triangle_offset +
                              ((last.triangle_count * 3 + 3) & ~3));
    meshlets.resize(meshlet_count);

    // Optimize vertex and index data for each meshlet
    for (size_t i = 0; i < meshlet_count; ++i) {
        const meshopt_Meshlet& m = meshlets[i];
        meshopt_optimizeMeshlet(&m_MeshletVertices[m.vertex_offset],
                                &m_MeshletTriangles[m.triangle_offset],
                                m.triangle_count, m.vertex_count);
    }

    // Calculate the boundary data of meshlet for cluster culling
    std::vector<meshopt_Bounds> meshlet_bounds(meshlet_count);
    for (size_t i = 0; i < meshlet_count; ++i) {
        const meshopt_Meshlet& m = meshlets[i];
        meshlet_bounds[i] = meshopt_computeMeshletBounds(
                &m_MeshletVertices[m.vertex_offset],
                &m_MeshletTriangles[m.triangle_offset], m.triangle_count,
                vertex_positions, vertex_count, sizeof(float) * 3);
    }

    // Record indirect Command
    std::vector<MeshletData> meshletDatas(meshlet_count);
    std::vector<DrawElementsIndirectCommand> drawCommands(meshlet_count);
    m_MeshletIndices.resize(m_MeshletTriangles.size());
    for (size_t i = 0; i < meshlet_count; ++i) {
        const meshopt_Meshlet& m = meshlets[i];
        const meshopt_Bounds& b = meshlet_bounds[i];

        meshletDatas[i] = {
                igm::vec4{b.center[0], b.center[1], b.center[2], b.radius},
                igm::vec4{0.0f}};
        drawCommands[i] = {m.triangle_count * 3, 0, m.triangle_offset, 0, 0};

        for (auto j = m.triangle_offset;
             j < m.triangle_offset + m.triangle_count * 3; j++) {
            m_MeshletIndices[j] =
                    m_MeshletVertices[m.vertex_offset + m_MeshletTriangles[j]];
        }
    }

    m_MeshletsCount = meshlet_count;
    m_MeshletsBuffer.target(GL_SHADER_STORAGE_BUFFER);
    m_MeshletsBuffer.allocate(meshletDatas.size() * sizeof(MeshletData),
                              meshletDatas.data(), GL_STATIC_DRAW);

    m_DrawCommandBuffer.target(GL_SHADER_STORAGE_BUFFER);
    m_DrawCommandBuffer.allocate(drawCommands.size() *
                                         sizeof(DrawElementsIndirectCommand),
                                 drawCommands.data(), GL_STATIC_DRAW);

    m_VisibleMeshletBuffer.target(GL_SHADER_STORAGE_BUFFER);
    m_VisibleMeshletBuffer.allocate(drawCommands.size() * sizeof(unsigned int),
                                    nullptr, GL_DYNAMIC_DRAW);

    m_FinalDrawCommandBuffer.target(GL_DRAW_INDIRECT_BUFFER);
    m_FinalDrawCommandBuffer.allocate(
            drawCommands.size() * sizeof(DrawElementsIndirectCommand), nullptr,
            GL_DYNAMIC_DRAW);

    end = clock();

    std::string out;
    out.append("Build meshlets [count: ");
    out.append(std::to_string(meshlet_count));
    out.append(", time: ");
    out.append(FormatTime(end - start));
    out.append("]");
    std::cout << out << std::endl;
}

size_t Meshlet::MeshletsCount() { return m_MeshletsCount; };

const unsigned int* Meshlet::GetMeshletIndices() const {
    return m_MeshletIndices.data();
};

size_t Meshlet::GetMeshletIndexCount() { return m_MeshletIndices.size(); };

GLBuffer& Meshlet::MeshletsBuffer() { return m_MeshletsBuffer; };

GLBuffer& Meshlet::VisibleMeshletBuffer() { return m_VisibleMeshletBuffer; }

GLBuffer& Meshlet::DrawCommandBuffer() { return m_DrawCommandBuffer; };

GLBuffer& Meshlet::FinalDrawCommandBuffer() {
    return m_FinalDrawCommandBuffer;
};

IGAME_NAMESPACE_END