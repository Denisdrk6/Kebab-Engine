#include "ComponentMesh.h"

#include "ComponentTransform.h"

ComponentMesh::ComponentMesh(GameObject& compOwner)
{
	this->owner = &compOwner;
	this->type = ComponentType::MESH;
	this->active = true;
}

ComponentMesh::~ComponentMesh()
{
	RELEASE(vertexBuffer);
	RELEASE(indexBuffer);

	vertices.clear();
	indices.clear();
	textures.clear();
}

void ComponentMesh::Enable()
{
	this->active = true;
}

void ComponentMesh::Disable()
{
	this->active = false;
}

void ComponentMesh::Update()
{
}

void ComponentMesh::Draw(bool showNormals)
{
	BeginDraw();

	glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);

	if (showNormals)
	{
		glBegin(GL_LINES);

		// Draw vertex normals
		for (int i = 0; i < vertices.size(); ++i)
		{
			glVertex3f(vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
			float3 n;
			n.x = vertices[i].position.x + vertices[i].normal.x;
			n.y = vertices[i].position.y + vertices[i].normal.y;
			n.z = vertices[i].position.z + vertices[i].normal.z;
			n.Normalize();
			glVertex3f(n.x, n.y, n.z);
		}

		// Draw triangle normals
		for (unsigned int i = 0; i < vertices.size(); i += 3)
		{
			float3 vec1 = (vertices[i + 1].position - vertices[i + 2].position);
			float3 vec2 = -(vertices[i].position - vertices[i + 1].position);
			float3 n = math::Cross(vec1, vec2);
			n.Normalize();

			float3 cent;
			cent.x = (vertices[i].position.x + vertices[i + 1].position.x + vertices[i + 2].position.x) / 3;
			cent.y = (vertices[i].position.y + vertices[i + 1].position.y + vertices[i + 2].position.y) / 3;
			cent.z = (vertices[i].position.z + vertices[i + 1].position.z + vertices[i + 2].position.z) / 3;
			glVertex3f(cent.x, cent.y, cent.z);
			glVertex3f(cent.x + n.x, cent.y + n.y, cent.z + n.z);
		}

		glEnd();
	}

	EndDraw();
}

void ComponentMesh::SetData(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	SetUpMesh();
}

void ComponentMesh::SetUpMesh()
{
	vertexBuffer = new VertexBuffer();
	vertexBuffer->SetData(vertices);

	indexBuffer = new IndexBuffer(indices.data(), indices.size());
}

void ComponentMesh::BeginDraw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	vertexBuffer->Bind();
	indexBuffer->Bind();

	//glBindBuffer(GL_ARRAY_BUFFER, texBuffer);

	if (textures.size() > 0)
	{
		for (int i = 0; i < textures.size(); ++i)
		{
			textures[i].Bind(i);
		}
	}
	//else texture->Bind();

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
	//glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

}

void ComponentMesh::EndDraw()
{
	if (textures.size() > 0)
	{
		for (int i = 0; i < textures.size(); ++i)
		{
			textures[i].Unbind();
		}
	}
	//else texture->Unbind();

	indexBuffer->Unbind();
	vertexBuffer->Unbind();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}