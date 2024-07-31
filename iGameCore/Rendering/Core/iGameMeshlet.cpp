#include "iGameMeshlet.h"
#include <format>

IGAME_NAMESPACE_BEGIN

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
        drawCommands[i] = {m.triangle_count * 3, 1, m.triangle_offset, 0, 0};

        for (auto i = m.triangle_offset;
             i < m.triangle_offset + m.triangle_count * 3; i++) {
            m_MeshletIndices[i] =
                    m_MeshletVertices[m.vertex_offset + m_MeshletTriangles[i]];
        }
    }

    m_MeshletsCount = meshlet_count;
    m_MeshletsBuffer.create();
    m_MeshletsBuffer.allocate(meshletDatas.size() * sizeof(MeshletData),
                              meshletDatas.data(), GL_DYNAMIC_DRAW);
    m_DrawCommandBuffer.create();
    m_DrawCommandBuffer.allocate(drawCommands.size() *
                                         sizeof(DrawElementsIndirectCommand),
                                 drawCommands.data(), GL_DYNAMIC_DRAW);

    end = clock();

    std::string out;
    out.append("Build meshlets [count: ");
    out.append(std::to_string(meshlet_count));
    out.append(", time: ");
    out.append(FormatTime(end - start));
    out.append("]");
    std::cout << out << std::endl;
}

const size_t Meshlet::MeshletsCount() const { return m_MeshletsCount; };

const unsigned int* Meshlet::GetMeshletIndices() const {
    return m_MeshletIndices.data();
};

const size_t Meshlet::GetMeshletIndexCount() const {
    return m_MeshletIndices.size();
};

const GLBuffer& Meshlet::MeshletsBuffer() const { return m_MeshletsBuffer; };

const GLBuffer& Meshlet::DrawCommandBuffer() const {
    return m_DrawCommandBuffer;
};

IGAME_NAMESPACE_END