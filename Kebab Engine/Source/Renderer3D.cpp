#include "Application.h"
#include "Renderer3D.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Math/float4x4.h"

#include "mmgr/mmgr.h"

Renderer3D::Renderer3D(bool startEnabled) : Module(true)
{
	name = "renderer";
}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Init(JSON_Object* root)
{
	LOG_CONSOLE("Creating 3D Renderer context");
	
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(app->window->window);
	if(context == NULL)
	{
		LOG_CONSOLE("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG_CONSOLE("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glLoadMatrixf(app->camera->GetProjectionMatrix());

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG_CONSOLE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG_CONSOLE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.5f, 0.5f, 0.5f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG_CONSOLE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		LOG_CONSOLE("OpenGL initialization correct. Version %s", glGetString(GL_VERSION));

		// GLEW initialization
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			LOG_CONSOLE("Error loading GLEW: %s", glewGetErrorString(err));
		}
		else LOG_CONSOLE("GLEW initialization correct. Version %s", glewGetString(GLEW_VERSION));
	}

	Load(root);

	SetDepth();
	SetCullFace();
	lights[0].Active(true);
	SetLighting();
	SetColorMaterial();
	SetTexture2D();
	SetWireframe();

	drawVertexNormals = false;
	drawTriangleNormals = false;

	int w, h;
	app->window->GetWindowSize(w, h);
	OnResize(w, h);

	/*vertexArray = new VertexArray();
	indexBuffer = new IndexBuffer();
	vertexBuffer = new VertexBuffer();*/

	FrameBufferProperties props;
	props.width = w;
	props.height = h;
	frameBuffer = new FrameBuffer(props);

	return ret;
}

// PreUpdate: clear buffer
bool Renderer3D::PreUpdate(float dt)
{
	glClearColor(0.05f, 0.05f, 0.05f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();

	//glMatrixMode(GL_MODELVIEW);
	
	glLoadMatrixf(app->camera->GetViewMatrix());

	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		wireframe = !wireframe;
		wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// light 0 on cam pos
	lights[0].SetPos(app->camera->position.x, app->camera->position.y, app->camera->position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return true;
}

// Draw present buffer to screen
bool Renderer3D::Draw(float dt)
{
	app->editor->OnImGuiRender(dt, frameBuffer);

	frameBuffer->Bind();

	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawGrid();

	for (const auto& g : geometries)
		g->Draw(drawVertexNormals, drawTriangleNormals);

	for (const auto& mesh : meshes)
		mesh->Draw(drawVertexNormals, drawTriangleNormals);

	frameBuffer->Unbind();


	SDL_GL_SwapWindow(app->window->window);
	return true;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	LOG_CONSOLE("Destroying 3D Renderer");

	geometries.clear();
	/*delete vertexArray;
	delete vertexBuffer;
	delete indexBuffer;*/

	RELEASE(vertexArray);
	RELEASE(vertexBuffer);
	RELEASE(indexBuffer);
	RELEASE(frameBuffer);

	SDL_GL_DeleteContext(context);

	return true;
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	float ratio = (float)width / (float)height;
	app->camera->SetRatio(ratio);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf(app->camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Renderer3D::SetDepth()
{
	depth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	LOG_CONSOLE("-- GL_DEPTH_TEST -- set to %d", depth);
}

void Renderer3D::SetCullFace()
{
	cullFace ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
	LOG_CONSOLE("-- GL_CULL_FACE -- set to %d", cullFace);
}

void Renderer3D::SetLighting()
{
	lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	LOG_CONSOLE("-- GL_LIGHTING -- set to %d", lighting);
}

void Renderer3D::SetColorMaterial()
{
	colorMaterial ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
	LOG_CONSOLE("-- GL_COLOR_MATERIAL -- set to %d", colorMaterial);
}

void Renderer3D::SetTexture2D()
{
	texture2D ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
	LOG_CONSOLE("-- GL_TEXTURE_2D -- set to %d", texture2D);
}

void Renderer3D::SetWireframe()
{
	wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	LOG_CONSOLE("-- WIREFRAME -- set to %d", wireframe);
}

void Renderer3D::Save(JSON_Object* root)
{
	json_object_set_value(root, name.c_str(), json_value_init_object());
	JSON_Object* renObj = json_object_get_object(root, name.c_str());

	json_object_set_boolean(renObj, "depth", depth);
	json_object_set_boolean(renObj, "cullface", cullFace);
	json_object_set_boolean(renObj, "lighting", lighting);
	json_object_set_boolean(renObj, "color material", colorMaterial);
	json_object_set_boolean(renObj, "texture2D", texture2D);
	json_object_set_boolean(renObj, "wireframe", wireframe);
}

void Renderer3D::Load(JSON_Object* root)
{
	json_object_set_value(root, name.c_str(), json_object_get_value(root, name.c_str()));
	JSON_Object* renObj = json_object_get_object(root, name.c_str());

	depth = json_object_get_boolean(renObj, "depth");
	cullFace = json_object_get_boolean(renObj, "cullface");
	lighting = json_object_get_boolean(renObj, "lighting");
	colorMaterial = json_object_get_boolean(renObj, "color material");
	texture2D = json_object_get_boolean(renObj, "texture2D");
	wireframe = json_object_get_boolean(renObj, "wireframe");
	drawVertexNormals = json_object_get_boolean(renObj, "showNormals");
}

void Renderer3D::Submit(KbGeometry* geometry)
{
	geometries.push_back(geometry);

}

void Renderer3D::Submit(const std::vector<KbGeometry>& geos)
{
	//geometries.insert(geometries.end(), geos.begin(), geos.end());
	for (auto g : geos)
		Submit(&g);
}

// TODO: Still need to check if the childs do have more childs
void Renderer3D::Submit(GameObject* go)
{
	for (const auto& child : go->GetChilds())
	{
		ComponentMesh* m = (ComponentMesh*)child->GetComponent(ComponentType::MESH);
		meshes.push_back(m);
	}

	if (go->GetComponent(ComponentType::MESH))
		meshes.push_back((ComponentMesh*)go->GetComponent(ComponentType::MESH));
}

void Renderer3D::DrawGrid()
{
	glLineWidth(1.5f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

	float d = 200.0f;

	glColor4f(1.0f, 1.0f, 1.0f, 0.65f);
	glLineWidth(1.0f);

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}