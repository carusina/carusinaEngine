#include "GeometryGenerator.h"

#include "ModelLoader.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

std::vector<MeshData> GeometryGenerator::ReadFromFile(std::string basePath,
													  std::string fileName,
													  bool reverseNormal)
{
    ModelLoader modelLoader;
    modelLoader.Load(basePath, fileName, reverseNormal);
    vector<MeshData>& meshes = modelLoader.m_meshes;

    // Normalize vertices
    Vector3 vmin(1000, 1000, 1000);
    Vector3 vmax(-1000, -1000, -1000);
    for (MeshData& mesh : meshes)
    {
        for (Vertex& v : mesh.vertices)
        {
            vmin.x = XMMin(vmin.x, v.position.x);
            vmin.y = XMMin(vmin.y, v.position.y);
            vmin.z = XMMin(vmin.z, v.position.z);
            vmax.x = XMMax(vmin.x, v.position.x);
            vmax.y = XMMax(vmin.y, v.position.y);
            vmax.z = XMMax(vmin.z, v.position.z);
        }
    }

    float dx = vmax.x - vmin.x;
    float dy = vmax.y - vmin.y;
    float dz = vmax.z - vmin.z;
    float dl = XMMax(XMMax(dx, dy), dz);

    float cx = (vmax.x + vmin.x) * 0.5f;
    float cy = (vmax.y + vmin.y) * 0.5f;
    float cz = (vmax.z + vmin.z) * 0.5f;

    for (MeshData& mesh : meshes)
    {
        for (Vertex& v : mesh.vertices)
        {
            v.position.x = (v.position.x - cx) / dl;
            v.position.y = (v.position.y - cy) / dl;
            v.position.z = (v.position.z - cz) / dl;
        }
    }

	return meshes;
}

MeshData GeometryGenerator::MakeSquare(const float scale,
									   const DirectX::SimpleMath::Vector2 texScale)
{
    vector<Vector3> positions;
    vector<Vector3> colors;
    vector<Vector3> normals;
    vector<Vector2> texcoords;

    // �ո�
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;

    for (size_t i = 0; i < positions.size(); i++)
    {
        Vertex v;
        v.position = positions[i];
        v.normalModel = normals[i];
        v.texcoord = texcoords[i] * texScale;
        v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);

        meshData.vertices.push_back(v);
    }
    meshData.indices = { 0, 1, 2, 0, 2, 3 }; // �ո�

    return meshData;
}

MeshData GeometryGenerator::MakeSquareGrid(const int numSlices, const int numStacks, const float scale,
										   const DirectX::SimpleMath::Vector2 texScale)
{
    MeshData meshData;

    float dx = 2.0f / numSlices;
    float dy = 2.0f / numStacks;

    float y = 1.0f;
    for (int j = 0; j < numStacks + 1; j++)
    {
        float x = -1.0f;

        for (int i = 0; i < numSlices + 1; i++)
        {
            Vertex v;
            v.position = Vector3(x, y, 0.0f) * scale;
            v.normalModel = Vector3(0.0f, 0.0f, -1.0f);
            v.texcoord = Vector2(x + 1.0f, y + 1.0f) * 0.5f * texScale;
            v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);

            meshData.vertices.push_back(v);

            x += dx;
        }
        y -= dy;
    }

    for (int j = 0; j < numStacks; j++)
    {
        for (int i = 0; i < numSlices; i++)
        {
            meshData.indices.push_back((numSlices + 1) * j + i);
            meshData.indices.push_back((numSlices + 1) * j + i + 1);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.indices.push_back((numSlices + 1) * j + i + 1);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i + 1);
        }
    }

    return meshData;
}

MeshData GeometryGenerator::MakeBox(const float scale)
{
    vector<Vector3> positions;
    vector<Vector3> colors;
    vector<Vector3> normals;
    vector<Vector2> texcoords;

    // ����
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // �Ʒ���
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // �ո�
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // �޸�
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // ����
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // ������
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++)
    {
        Vertex v;
        v.position = positions[i];
        v.normalModel = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }

    meshData.indices = { 0,  1,  2,  0,  2,  3,     // ����
                         4,  5,  6,  4,  6,  7,     // �Ʒ���
                         8,  9,  10, 8,  10, 11,    // �ո�
                         12, 13, 14, 12, 14, 15,    // �޸�
                         16, 17, 18, 16, 18, 19,    // ����
                         20, 21, 22, 20, 22, 23 };  // ������

    return meshData;
}

MeshData GeometryGenerator::MakeCylinder(const float bottomRadius, const float topRadius,
										 float height, int numSlices)
{
    // Texture ��ǥ�趧���� (numSlices + 1) x 2 ���� ���ؽ� ���
    const float dTheta = -XM_2PI / float(numSlices);

    MeshData meshData;

    vector<Vertex>& vertices = meshData.vertices;

    // ������ �ٴ� ���ؽ��� (�ε��� 0 �̻� numSlices �̸�)
    for (int i = 0; i <= numSlices; i++)
    {
        Vertex v;
        v.position = Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f),
                                        Matrix::CreateRotationY(dTheta * float(i)));

        v.normalModel = v.position - Vector3(0.0f, -0.5f * height, 0.0f);
        v.normalModel.Normalize();
        v.texcoord = Vector2(float(i) / numSlices, 1.0f);

        vertices.push_back(v);
    }

    // ������ �� �� ���ؽ��� (�ε��� numSlices �̻� 2 * numSlices �̸�)
    for (int i = 0; i <= numSlices; i++)
    {
        Vertex v;
        v.position = Vector3::Transform(Vector3(topRadius, 0.5f * height, 0.0f),
                                        Matrix::CreateRotationY(dTheta * float(i)));
        v.normalModel = v.position - Vector3(0.0f, 0.5f * height, 0.0f);
        v.normalModel.Normalize();
        v.texcoord = Vector2(float(i) / numSlices, 0.0f);

        vertices.push_back(v);
    }

    vector<uint32_t>& indices = meshData.indices;

    for (int i = 0; i < numSlices; i++) {
        indices.push_back(i);
        indices.push_back(i + numSlices + 1);
        indices.push_back(i + 1 + numSlices + 1);

        indices.push_back(i);
        indices.push_back(i + 1 + numSlices + 1);
        indices.push_back(i + 1);
    }

    return meshData;
}

MeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices, const int numStacks,
									   const DirectX::SimpleMath::Vector2 texScale)
{
    const float dTheta = -XM_2PI / float(numSlices);
    const float dPhi = -XM_PI / float(numStacks);

    MeshData meshData;

    vector<Vertex>& vertices = meshData.vertices;

    for (int j = 0; j <= numStacks; j++)
    {

        // ���ÿ� ���� ���� �������� x-y ��鿡�� ȸ�� ���Ѽ� ���� �ø��� ����
        Vector3 stackStartPoint = Vector3::Transform(Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * j));

        for (int i = 0; i <= numSlices; i++) {
            Vertex v;

            // �������� x-z ��鿡�� ȸ����Ű�鼭 ���� ����� ����
            v.position = Vector3::Transform(stackStartPoint,
                                            Matrix::CreateRotationY(dTheta * float(i)));

            v.normalModel = v.position; // ������ ���� �߽�
            v.normalModel.Normalize();
            v.texcoord = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks) * texScale;

            // texcoord�� ���� ������ ����
            Vector3 biTangent = Vector3(0.0f, 1.0f, 0.0f);

            Vector3 normalOrth = v.normalModel - biTangent.Dot(v.normalModel) * v.normalModel;
            normalOrth.Normalize();

            v.tangentModel = biTangent.Cross(normalOrth);
            v.tangentModel.Normalize();

            vertices.push_back(v);
        }
    }

    vector<uint32_t>& indices = meshData.indices;

    for (int j = 0; j < numStacks; j++)
    {
        const int offset = (numSlices + 1) * j;

        for (int i = 0; i < numSlices; i++)
        {
            indices.push_back(offset + i);
            indices.push_back(offset + i + numSlices + 1);
            indices.push_back(offset + i + 1 + numSlices + 1);

            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + numSlices + 1);
            indices.push_back(offset + i + 1);
        }
    }

    return meshData;
}

MeshData GeometryGenerator::MakeTetrahedron()
{
    const float a = 1.0f;
    const float x = sqrt(3.0f) / 3.0f * a;
    const float d = sqrt(3.0f) / 6.0f * a; // = x / 2
    const float h = sqrt(6.0f) / 3.0f * a;

    vector<Vector3> points = { {0.0f, x, 0.0f},
                               {-0.5f * a, -d, 0.0f},
                               {+0.5f * a, -d, 0.0f},
                               {0.0f, 0.0f, h} };

    Vector3 center = Vector3(0.0f);

    for (int i = 0; i < 4; i++)
    {
        center += points[i];
    }
    center /= 4.0f;

    for (int i = 0; i < 4; i++)
    {
        points[i] -= center;
    }

    MeshData meshData;

    for (int i = 0; i < points.size(); i++)
    {

        Vertex v;
        v.position = points[i];
        v.normalModel = v.position; // �߽��� ����
        v.normalModel.Normalize();

        meshData.vertices.push_back(v);
    }

    meshData.indices = { 0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3 };

    return meshData;
}

MeshData GeometryGenerator::MakeIcosahedron()
{
    const float X = 0.525731f;
    const float Z = 0.850651f;

    MeshData newMesh;

    vector<Vector3> pos = { Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),   Vector3(-X, 0.0f, -Z),
                            Vector3(X, 0.0f, -Z), Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
                            Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X), Vector3(Z, X, 0.0f),
                            Vector3(-Z, X, 0.0f), Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f) };

    for (size_t i = 0; i < pos.size(); i++)
    {
        Vertex v;
        v.position = pos[i];
        v.normalModel = v.position;
        v.normalModel.Normalize();

        newMesh.vertices.push_back(v);
    }

    newMesh.indices = { 1,  4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1,  8, 4,
                        1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5,  3,  7, 2,
                        3,  10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
                        10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9,  11, 2, 7 };

    return newMesh;
}

MeshData GeometryGenerator::SubdivideToSphere(const float radius, MeshData meshData)
{
    // ������ �߽��̶�� ����
    for (auto& v : meshData.vertices)
    {
        v.position = v.normalModel * radius;
    }

    // ���� ǥ������ �ű�� ��ְ� texture ��ǥ ���
    auto ProjectVertex = [&](Vertex& v) {
        v.normalModel = v.position;
        v.normalModel.Normalize();
        v.position = v.normalModel * radius; };

    auto UpdateFaceNormal = [](Vertex& v0, Vertex& v1, Vertex& v2) {
        auto faceNormal = (v1.position - v0.position).Cross(v2.position - v0.position);
        faceNormal.Normalize();
        v0.normalModel = faceNormal;
        v1.normalModel = faceNormal;
        v2.normalModel = faceNormal; };

    // ���ؽ��� �ߺ��Ǵ� ������ ����
    MeshData newMesh;
    uint32_t count = 0;
    for (size_t i = 0; i < meshData.indices.size(); i += 3)
    {
        size_t i0 = meshData.indices[i];
        size_t i1 = meshData.indices[i + 1];
        size_t i2 = meshData.indices[i + 2];

        Vertex v0 = meshData.vertices[i0];
        Vertex v1 = meshData.vertices[i1];
        Vertex v2 = meshData.vertices[i2];

        Vertex v3;
        v3.position = (v0.position + v2.position) * 0.5f;
        v3.texcoord = (v0.texcoord + v2.texcoord) * 0.5f;
        ProjectVertex(v3);

        Vertex v4;
        v4.position = (v0.position + v1.position) * 0.5f;
        v4.texcoord = (v0.texcoord + v1.texcoord) * 0.5f;
        ProjectVertex(v4);

        Vertex v5;
        v5.position = (v1.position + v2.position) * 0.5f;
        v5.texcoord = (v1.texcoord + v2.texcoord) * 0.5f;
        ProjectVertex(v5);

        newMesh.vertices.push_back(v4);
        newMesh.vertices.push_back(v1);
        newMesh.vertices.push_back(v5);

        newMesh.vertices.push_back(v0);
        newMesh.vertices.push_back(v4);
        newMesh.vertices.push_back(v3);

        newMesh.vertices.push_back(v3);
        newMesh.vertices.push_back(v4);
        newMesh.vertices.push_back(v5);

        newMesh.vertices.push_back(v3);
        newMesh.vertices.push_back(v5);
        newMesh.vertices.push_back(v2);

        for (uint32_t j = 0; j < 12; j++)
        {
            newMesh.indices.push_back(j + count);
        }
        count += 12;
    }

    return newMesh;
}
