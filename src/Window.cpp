#include <vector>
#include <algorithm>
#include "Window.h"
#include "Node.h"
#include "Transform.h"
#include "Geometry.h"
#include "Bezier.h"
#include "Skybox.h"
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
std::chrono::milliseconds prevFrame;
const char* window_title = "GLFW Starter Project";
OBJObject *dragon;
OBJObject *bunny;
OBJObject *bear;
OBJObject *cone;
OBJObject *sphere;
GLint mainShader;
GLint shadowShader;
GLint lineShader;
OBJObject *current;
Transform *sceneGraph;
Transform *sceneGraph2;
Transform *sceneFloor;
Bezier *bezier;
Skybox *skybox;
Transform* trees;

int t = 0;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "obj.vert"
#define FRAGMENT_SHADER_PATH "obj.frag"

#define VERTEX_LINE_PATH "line.vert"
#define FRAGMENT_LINE_PATH "line.frag"

#define VERTEX_SHADOW_PATH "shadow.vert"
#define FRAGMENT_SHADOW_PATH "shadow.frag"
#define GEOMETRY_SHADOW_PATH "shadow.geom"

// Default camera parameters
glm::vec3 cam_pos(0.0f, -5.0f, 0.0f);		// e	| Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d	| This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;
DirLight Window::dir;
PointLight Window::point;
SpotLight Window::spot;

int Window::mode = 5;
bool Window::activeLights[3] = {false, true, false};
bool Window::normalColor = true;

glm::vec3 Window::viewPos = cam_pos;

glm::mat4 Window::P;
glm::mat4 Window::V;
glm::mat4 cam_rot;

Transform *limbAnim;
Transform *limbOAnim;
Transform *neckAnim;

float camUpAngle;
float camAngle;
float moveVel = 0.4f;
//float moveVel = 1.0f;
//float moveVel = 0.2f;
float curveTime = 0;
float limbAng = 0;
int robotRad = 0;
static const float skyboxVertices[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
};

std::vector<std::string> skyFaces = {
	"res/right.jpg",
	"res/left.jpg",
	"res/top.jpg",
	"res/bottom.jpg",
	"res/front.jpg",
	"res/back.jpg"
};


void fixSpotlight() {
	Window::spot.direction = -normalize(cone->position);
	Window::spot.position = cone->position;
	cone->position = Window::spot.position;

	glm::vec3 direct = glm::normalize(Window::spot.direction);
	glm::vec3 side = glm::cross(direct, glm::vec3(0, 1, 0));
	if (direct.x == 0) {
		side = glm::cross(direct, glm::vec3(1, 0, 0));
	}
	side = glm::normalize(side);
	glm::vec3 top = glm::cross(side, direct);
	cone->rotation = glm::mat4(
			top[0], top[1], top[2], 0,
			-direct[0], -direct[1], -direct[2], 0,
			side[0], side[1], side[2], 0,
			0, 0, 0, 1
		);
}

int mouseX = 0;
int mouseY = 0;
bool clickRight = false;
bool clickLeft = false;
bool fboUsed = false;
GLuint velocityBuffer;
GLuint quad_programID;
GLuint velocityTexture;
GLuint velocityShader;
GLuint quad_vertexbuffer;
GLuint quad_arraybuffer;
bool ok = false;
GLuint velocityTexID;
GLuint renderTexID;
GLuint quad_vertexPosition_modelspace;

GLuint renderBuffer;
GLuint depthRenderBuffer;
GLuint stencilRenderBuffer;
GLuint renderTexture;
void Window::initialize_objects()
{

	const aiScene* bunnyImp =aiImportFile("res/Lowpoly_tree_sample.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	std::cout << "Number of meshes found in file: " << bunnyImp->mNumMeshes << std::endl;
	std::cout << "Number of vertices in mesh 1: " << bunnyImp->mMeshes[0]->mNumVertices << std::endl;



	prevFrame = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	);
	cam_look_at = glm::vec3(cam_pos.x + sin(camAngle), cam_pos.y + sin(camUpAngle), cam_pos.z + cos(camAngle));
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);

	dragon = new OBJObject("res/bear.obj", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(4.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f);

	bunny = new OBJObject("res/bunny.obj", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-4.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

	bear = new OBJObject("res/bear.obj", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(4.0f, -1.0f, 0.0f),
		glm::vec3(0.3f, 0.2f, 0.1f), glm::vec3(0.1f, 0.05f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 6.0f);

	Window::dir = { glm::vec3(1.0, -1.0, 0.0), glm::vec3(1.0, 1.0, 1.0) };
	Window::point = { glm::vec3(0.0, 2.0, 0.0), glm::vec3(1.0, 1.0, 1.0), 0.1f };

	sphere = new OBJObject("res/sphere.obj", glm::vec3(1.0f, 1.0f, 1.0f), point.position,
		glm::vec3(0.0f, 0.0f, 0.0f), Window::point.color, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	sphere->scale = glm::vec3(0.25, 0.25, 0.25);

	Window::spot = { glm::vec3(0.0, -2.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0), 2, 0.5 };

	cone = new OBJObject("res/cone.obj", glm::vec3(0.0f, 1.0f, 0.0f), spot.position,
		glm::vec3(0.0f, 0.0f, 0.0f), Window::spot.color, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	cone->scale = glm::vec3(0.25, 0.25, 0.25);
	fixSpotlight();

	current = bunny;

	// Load the shader program. Make sure you have the correct filepath up top
	mainShader = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	shadowShader = LoadShaders(VERTEX_SHADOW_PATH, FRAGMENT_SHADOW_PATH, GEOMETRY_SHADOW_PATH);
	lineShader = LoadShaders(VERTEX_LINE_PATH, FRAGMENT_LINE_PATH);
	quad_programID = LoadShaders("texture.vert", "texture.frag");
	velocityShader = LoadShaders("velocity.vert", "velocity.frag");

	/*Transform *robot = new Transform(glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(1, 0, 0)));
	robot->addChild(new Geometry("res/robot-parts/body.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, mainShader, shadowShader, velocityShader, true));

	Geometry *limb = new Geometry("res/robot-parts/head.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, mainShader, shadowShader, velocityShader, false);
	Transform *temp = new Transform(glm::rotate(glm::rotate(
		glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.6)),
		glm::pi<float>(), glm::vec3(1, 0, 0)),
		glm::pi<float>(), glm::vec3(0, 0, 1)));
	neckAnim = new Transform(glm::mat4(1.0f));
	robot->addChild(temp);
	temp->addChild(neckAnim);
	neckAnim->addChild(limb);

	limb = new Geometry("res/robot-parts/antenna.obj", glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, mainShader, shadowShader, velocityShader, false);
	Transform *temp2 = new Transform(glm::scale(glm::translate(glm::mat4(1.0f),
		glm::vec3(-0.5f, 0, 0.5f)), glm::vec3(0.3f, 0.3f, 0.3f)));
	neckAnim->addChild(temp2);
	temp2->addChild(limb);

	Transform *temp3 = new Transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0, 0, 1)));
	neckAnim->addChild(temp3);
	temp3->addChild(temp2);

	limb = new Geometry("res/robot-parts/eyeball.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, mainShader, shadowShader, velocityShader, false);
	temp3 = new Transform(glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0, 0, 1)));
	temp2 = new Transform(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, 0.8f, -0.1f)), glm::vec3(0.2f, 0.2f, 0.2f)));
	neckAnim->addChild(temp2);
	temp3->addChild(limb);
	temp2->addChild(temp3);

	temp2 = new Transform(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.8f, -0.1f)), glm::vec3(0.2f, 0.2f, 0.2f)));
	neckAnim->addChild(temp2);
	temp2->addChild(temp3);

	limb = new Geometry("res/robot-parts/limb.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, mainShader, shadowShader, velocityShader, false);
	temp = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(1.3, 0, -0.8)));
	robot->addChild(temp);
	limbAnim = new Transform(glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1, 0, 0)));
	temp->addChild(limbAnim);
	temp2 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.8)));
	limbAnim->addChild(temp2);
	temp2->addChild(limb);

	temp = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-1.3, 0, -0.8)));
	robot->addChild(temp);
	limbOAnim = new Transform(glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1, 0, 0)));
	temp->addChild(limbOAnim);
	limbOAnim->addChild(temp2);

	temp = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-0.4, 0, 0.8)));
	temp->addChild(limbAnim);
	robot->addChild(temp);

	temp = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.4, 0, 0.8)));
	temp->addChild(limbOAnim);
	robot->addChild(temp);*/
	trees = new Transform(glm::mat4(1.0f));
	sceneGraph = new Transform(glm::mat4(1.0f));
	
	sceneFloor = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(100, 1, 100)));
	Geometry *werewolf = new Geometry("res/werewolf.obj", glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 0.0f), 6.0f, mainShader, shadowShader, velocityShader, true);
	Transform *scaleWolf = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(5.0, 5.0, 5.0)));
	scaleWolf->addChild(werewolf);

	sceneGraph->addChild(scaleWolf);
	Geometry *box = new Geometry("res/cube.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, mainShader, shadowShader, velocityShader, true);
	Geometry *treeObj = new Geometry("res/treenoleaves.obj", glm::vec3(0.3f, 0.2f, 0.1f), glm::vec3(0.1f, 0.05f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 6.0f, mainShader, shadowShader, velocityShader, true);

	Transform *scaleTree = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 30.0, 20.0)));
	scaleTree->addChild(treeObj);
	
	Transform *treeVert = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 13.0, 0.0)));
	treeVert->addChild(scaleTree);

	Transform * treeTrans = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(10.0, 0.0, 10.0)));
	treeTrans->addChild(treeVert);

	Transform * treeTrans1 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(20.0, 0.0, 40.0)));
	treeTrans1->addChild(treeVert);

	Transform * treeTrans2 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(20.0, 0.0, 20.0)));
	treeTrans2->addChild(treeVert);

	Transform * treeTrans3 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(50.0, 0.0, 10.0)));
	treeTrans3->addChild(treeVert);

	Transform * treeTrans4 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-20.0, 0.0, -20.0)));
	treeTrans4->addChild(treeVert);

	Transform * treeTrans5 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-20.0, 0.0, 30.0)));
	treeTrans5->addChild(treeVert);

	Transform * treeTrans6 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(30.0, 0.0, -10.0)));
	treeTrans6->addChild(treeVert);

	Transform * treeTrans7 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(30.0, 0.0, -20.0)));
	treeTrans7->addChild(treeVert);

	Transform * treeTrans8 = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(30.0, 0.0, 30.0)));
	treeTrans8->addChild(treeVert);


	trees->addChild(treeTrans);
	trees->addChild(treeTrans1);
	trees->addChild(treeTrans2);
	trees->addChild(treeTrans3);
	trees->addChild(treeTrans4);
	trees->addChild(treeTrans5);
	trees->addChild(treeTrans6);
	trees->addChild(treeTrans7);
	trees->addChild(treeTrans8);

	
	sceneFloor->addChild(box);
	//sceneGraph->addChild(box);
	/*for (int i = -robotRad; i <= robotRad; i++) {
		Transform *treeTrans = new Transform(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * i, 0, 0))));
		treeTrans->addChild(tree);
		//robTrans->addChild(box);
		sceneGraph->addChild(treeTrans);
	}*/
	//Transform* tree1 = new Transform(glm::translate)
	/*Transform *robRow = new Transform(glm::mat4(1.0f));
	for (int i = -robotRad; i <= robotRad; i++) {
		Transform *robTrans = new Transform(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * i, 0, 0))));
		robTrans->addChild(robot);
		//robTrans->addChild(box);
		robRow->addChild(robTrans);
	}

	for (int i = -robotRad; i <= robotRad; i++) {
		Transform *robTrans = new Transform(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 4.0f * i))));
		robTrans->addChild(robRow);
		sceneGraph->addChild(robTrans);
	}*/

	skybox = new Skybox(skyFaces);

	bezier = new Bezier(
		glm::vec3(0, 0, 0),
		glm::vec3(10, 0, 0),
		glm::vec3(15, 0, -10),
		glm::vec3(10, 0, -20),
		lineShader
	);
	bezier->addControl(
		glm::vec3(0, 10, -30),
		glm::vec3(0, 10, -40)
	);
	bezier->addControl(
		glm::vec3(-10, 0, -10),
		glm::vec3(-20, 0, -20)
	);
	bezier->addControl(
		glm::vec3(-25, -5, -15),
		glm::vec3(-30, 0, -5)
	);
	bezier->addControl(
		glm::vec3(-10, 0, 0),
		glm::vec3(0, 0, 0)
	);
	bezier->updateBuffer();

	glGenFramebuffers(1, &velocityBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, velocityBuffer);

	glGenTextures(1, &velocityTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, velocityTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, velocityTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		ok = true;
	}


	glGenFramebuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);

	/*glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);


	glGenRenderbuffers(1, &stencilRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, stencilRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRenderBuffer);*/

	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, Window::width, Window::height);

	//Create stencil RenderBuffer
	/*glGenRenderbuffers(1, &stencilRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, stencilRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, Window::width, Window::height);*/

	// bind renderbuffers to framebuffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRenderBuffer);

	glGenTextures(1, &renderTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);

	// Set the list of draw buffers.
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		ok = true;
	}





	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	
	glGenVertexArrays(1, &quad_arraybuffer);
	glGenBuffers(1, &quad_vertexbuffer);
	glBindVertexArray(quad_arraybuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		quad_vertexPosition_modelspace, // attribute
		3,                              // size
		GL_FLOAT,                       // type
		GL_FALSE,                       // normalized?
		0,                              // stride
		(void*)0                        // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	quad_vertexPosition_modelspace = glGetAttribLocation(quad_programID, "aPos");
	velocityTexID = glGetUniformLocation(quad_programID, "velocityTexture");
	renderTexID = glGetUniformLocation(quad_programID, "renderTexture");
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(bunny);
	delete(dragon);
	delete(bear);
	glDeleteShader(mainShader);
	glDeleteShader(lineShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#elif __linux__
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	}


}

void Window::idle_callback()
{
	float rate = 0.05f;
	glm::vec4 rotated = glm::rotate(glm::mat4(1.0f), rate, glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::vec4(Window::dir.direction.x, Window::dir.direction.y, Window::dir.direction.z, 1.0f);
	Window::dir.direction = glm::vec3(rotated.x, rotated.y, rotated.z);

	// std::cout<<cone->rotation[0][0]<<" "<<cone->rotation[0][1]<<" "<<cone->rotation[0][2]<<" "<<cone->rotation[0][3]<<"	";
	// std::cout<<cone->rotation[1][0]<<" "<<cone->rotation[1][1]<<" "<<cone->rotation[1][2]<<" "<<cone->rotation[1][3]<<"	";
	// std::cout<<cone->rotation[2][0]<<" "<<cone->rotation[2][1]<<" "<<cone->rotation[2][2]<<" "<<cone->rotation[2][3]<<"	";
	// std::cout<<cone->rotation[3][0]<<" "<<cone->rotation[3][1]<<" "<<cone->rotation[3][2]<<" "<<cone->rotation[3][3]<<"\n";
	// std::cout<<direct[0]<<" "<<direct[1]<<" "<<direct[2]<<"\n";
	cone->position = Window::spot.position;
	//cone->rotation = glm::transpose(cone->rotation);
}

void Window::display_callback(GLFWwindow* window)
{
	std::chrono::milliseconds frameTime = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
		);
	std::this_thread::sleep_for(std::max(std::chrono::milliseconds(0), 
		std::chrono::milliseconds(16) - (frameTime - prevFrame)));
	prevFrame = frameTime;
	/*limbAng += moveVel;
	limbAnim->M = glm::rotate(glm::mat4(1.0f), 0.8f*std::sin(limbAng), glm::vec3(1, 0, 0));
	limbOAnim->M = glm::rotate(glm::mat4(1.0f), 0.8f*std::sin(-limbAng), glm::vec3(1, 0, 0));
	neckAnim->M = glm::rotate(glm::mat4(1.0f), 0.2f*std::sin(-limbAng), glm::vec3(0, 0, 1));*/

	curveTime += moveVel / 20;
	std::pair<glm::vec3, glm::vec3> inter = bezier->interpolate(curveTime);

	glm::vec3 up(0, 1, 0);
	if (inter.second == glm::vec3(0, 1, 0)) {
		up = glm::vec3(1, 0, 0);
	}
	Window::point.position = glm::vec3(glm::inverse(glm::lookAt(inter.first, inter.first - inter.second, up)) * glm::vec4(1.0f));
	glBindFramebuffer(GL_FRAMEBUFFER, velocityBuffer);
	glViewport(0, 0, Window::width, Window::height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(velocityShader);
	trees->draw(glm::mat4(1.0f), true);
	sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first - inter.second, up)), true);
	//bunny->draw(velocityShader);
	//dragon->draw(velocityShader);
	//bear->draw(velocityShader);

	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
	glViewport(0, 0, Window::width, Window::height);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glDepthMask(GL_TRUE);

	// Use the shader of programID
	glUseProgram(mainShader);
	trees->draw(glm::mat4(1.0f), true);
	sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first - inter.second, up)), true);
	//bunny->draw(mainShader);
	//dragon->draw(mainShader);
	//bear->draw(mainShader);
	trees->update();
	sceneGraph->update();
	Window::normalColor = !Window::normalColor;
	sceneFloor->draw(glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, 0)), false);
	//bunny->update();
	//dragon->update();
	//bear->update();

	
	/*glUseProgram(mainShader);
	Window::normalColor = !Window::normalColor;
	sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first-inter.second, up)), false);
	
	Window::normalColor = !Window::normalColor;
	glDrawBuffer(GL_NONE);*/
	Window::normalColor = !Window::normalColor;
	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_CLAMP);        
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_ALWAYS, 0, 0xff);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);       

	glUseProgram(shadowShader);
	sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first-inter.second, up)), false);

	glDisable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);                  

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glStencilFunc(GL_EQUAL, 0x0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

	P = glm::perspective(45.0f, (float)width / (float)height, 0.10f, 1005.0f);
	glUseProgram(mainShader);
	sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first-inter.second, up)), false);
	sceneFloor->draw(glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, 0)), false);
	P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

    glDisable(GL_BLEND);

	glDisable(GL_STENCIL_TEST);
	glDepthMask(GL_TRUE);
	glUseProgram(mainShader);

	//sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first-inter.second, up)));
	//sceneGraph->draw(glm::mat4(1.0f));
	//sceneGraph->draw(glm::translate(glm::mat4(1.0f), inter.first));
	//sceneGraph->draw(glm::translate(glm::mat4(1.0f), inter.first) * glm::mat4(
	//				xAxis[0], xAxis[1], xAxis[2], 0, 
	//				yAxis[0], yAxis[1], yAxis[2], 0, 
	//				zAxis[0], zAxis[1], zAxis[2], 0, 
	//				0, 0, 0, 1
	//));
	//sceneGraph->draw(glm::mat4(1.0f));

	/*if (Window::activeLights[1]) {
		sphere->draw(mainShader);
	}
	if (Window::activeLights[2]) {
		cone->draw(mainShader);
	}*/

	bezier->draw();

	//sceneGraph->update();
	//sceneFloor->update();

	if (!Window::normalColor) {
		glUseProgram(shadowShader);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sceneGraph->draw(glm::inverse(glm::lookAt(inter.first, inter.first-inter.second, up)), false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	glUseProgram(mainShader);
	//sceneGraph2->draw(glm::mat4(1.0f));
	skybox->draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::width, Window::height);

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(quad_programID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityTexture);
	glUniform1i(velocityTexID, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glUniform1i(renderTexID, 1);

	glBindVertexArray(quad_arraybuffer);
	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glBindVertexArray(0);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
	
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		switch (key)
		{
			case GLFW_KEY_ESCAPE:
				// Close the window. This causes the program to also terminate.
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_F1:
				current = dragon;
				break;
			case GLFW_KEY_F2:
				current = bunny;
				break;
			case GLFW_KEY_F3:
				current = bear;
				break;
		
			
			case GLFW_KEY_E:
				if (mods & GLFW_MOD_SHIFT) {
					if (Window::spot.dropoff > 1 / 128.0f) {
						Window::spot.dropoff /= 2;
					}
				} else {
					if (Window::spot.dropoff < 128) {
						Window::spot.dropoff *= 2;
					}
				}
				break;
			case GLFW_KEY_N:
				Window::normalColor = !Window::normalColor;
				break;
			case GLFW_KEY_0:
				Window::mode = 0;
				break;
			case GLFW_KEY_1:
				Window::mode = 1;
				Window::activeLights[1] = false;
				Window::activeLights[2] = false;
				break;
			case GLFW_KEY_2:
				Window::mode = 2;
				Window::activeLights[1] = true;
				Window::activeLights[2] = false;
				break;
			case GLFW_KEY_3:
				Window::mode = 3;
				Window::activeLights[1] = false;
				Window::activeLights[2] = true;
				break;
			case GLFW_KEY_4:
				Window::activeLights[0] = !Window::activeLights[0];
				break;
			case GLFW_KEY_5:
				Window::mode = 5;
				break;
			case GLFW_KEY_W:
				cam_pos = glm::vec3(cam_pos.x + sin(camAngle), cam_pos.y, cam_pos.z + cos(camAngle));
				break;
			case GLFW_KEY_A:
				cam_pos = glm::vec3(cam_pos.x + cos(camAngle), cam_pos.y, cam_pos.z + sin(camAngle));
				break;
			case GLFW_KEY_S:
				cam_pos = glm::vec3(cam_pos.x - sin(camAngle), cam_pos.y, cam_pos.z - cos(camAngle));
				break;

			case GLFW_KEY_D:
				cam_pos = glm::vec3(cam_pos.x - cos(camAngle), cam_pos.y, cam_pos.z - sin(camAngle));
				break;

		}

		cam_look_at = glm::vec3(cam_pos.x + sin(camAngle), cam_pos.y + sin(camUpAngle), cam_pos.z + cos(camAngle));
		Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
	if (action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_LEFT:
			camAngle = camAngle + 0.05;
			break;
		
		case GLFW_KEY_RIGHT:
			camAngle = camAngle - 0.05;
			break;
		
		case GLFW_KEY_UP:
			camUpAngle = camUpAngle + 0.05;
			break;

		case GLFW_KEY_DOWN:
			camUpAngle = camUpAngle - 0.05;
			break;
	}

		//cam_look_at = glm::vec3(cam_pos.x + sin(camAngle), cam_pos.y+ sin(camUpAngle), cam_pos.z + cos(camAngle));
		
		//Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		
	}
}

float bound(float val, float left, float right) {
	float width = right - left;
	return std::max(-1.0f, std::min(1.0f, (2 * val - width) / width));
}
void Window::cursor_pos_callback(GLFWwindow * window, double posX, double posY) {
	bool leftPressed = GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	bool rightPressed = GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (leftPressed) {
		float startX = -bound(mouseX, 0, Window::width);
		float startY = bound(mouseY, 0, Window::height);
		float startLen = sqrt(startX * startX + startY * startY);
		if (startLen > 1) {
			startX /= startLen + 0.01;
			startY /= startLen + 0.01;
			startLen = 1;
		}

		float stopX = -bound(posX, 0, Window::width);
		float stopY = bound(posY, 0, Window::height);
		float stopLen = sqrt(stopX * stopX + stopY * stopY);
		if (stopLen > 1) {
			stopX /= stopLen + 0.01;
			stopY /= stopLen + 0.01;
			stopLen = 1;
		}

		//std::cout<<stopX<<"\t "<<stopY<<"\t "<<stopLen<<"\n";

		glm::vec3 start = glm::vec3(
			startX, startY, -sqrt(1 - startLen * startLen)
		);
		//std::cout<<start[0]<<" "<<start[1]<<" "<<start[2]<<"\n";

		glm::vec3 stop = glm::vec3(
			stopX, stopY, -sqrt(1 - stopLen * stopLen)
		);

		glm::vec3 crossProd = glm::cross(start, stop);
		//std::cerr<<"cross "<<crossProd[0]<<" "<<crossProd[1]<<" "<<crossProd[2]<<"\n";
		//std::cerr<<"cos "<<glm::dot(start, stop) <<" "<< (glm::length(start) * glm::length(stop))<<"\n";
		glm::vec3 axis = glm::cross(start, stop);
		float angle = acos(glm::dot(start, stop) / (glm::length(start) * glm::length(stop)));

		//axis = glm::vec3(glm::mat3(glm::lookAt(cam_pos, cam_look_at, cam_up)) * glm::vec4(axis, 1.0f));
		switch (Window::mode) {
			case 0:
				dragon->rotate(axis, angle);
				bunny->rotate(axis, angle);
				bear->rotate(axis, angle);
				break;
			case 2:
				sphere->rotate(axis, angle);
				Window::point.position = sphere->position;
				break;
			case 3:
				cone->rotate(axis, angle);
				fixSpotlight();
				break;
			case 5:
				/*if (glm::length(axis) < 0.001) {
					return;
				}*/
				/*axis = glm::normalize(axis);
				glm::mat4 camTransform = glm::inverse(glm::mat4(glm::mat3(glm::lookAt(cam_look_at, cam_pos, cam_up))));
				camTransform[0][0] = -camTransform[0][0];
				camTransform[0][1] = -camTransform[0][1];
				camTransform[0][2] = -camTransform[0][2];
				axis = glm::vec3(camTransform * glm::vec4(axis, 1.0f));
				glm::mat4 rotator = glm::rotate(glm::mat4(1.0f), -angle, axis);
				//cam_pos = glm::vec3(rotator * glm::vec4(cam_pos, 1.0f));
				//cam_up = glm::vec3(rotator * glm::vec4(cam_up, 1.0f));
				cam_look_at = cam_look_at + glm::vec3(rotator * glm::vec4(cam_look_at, 1.0f));*/
				float diffX = startX - stopX;
				float diffY = startY - stopY;
				camAngle += diffX;
				camUpAngle -= diffY;
				if (camAngle < -360 || camAngle > 360) {
					camAngle = 0;
				}
				cam_look_at = glm::vec3(cam_pos.x + sin(camAngle), cam_pos.y + sin(camUpAngle), cam_pos.z + cos(camAngle));
				Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
				break;
		}

	}
	if (rightPressed) {
		float diffX = 20.0f  * (posX - mouseX) / Window::height;
		float diffY = -20.0f * (posY - mouseY) / Window::height;
		switch (Window::mode) {
			case 0:
				dragon->move(diffX, diffY, 0, 1);
				bunny->move(diffX, diffY, 0, 1);
				bear->move(diffX, diffY, 0, 1);
				break;
			case 2:
				sphere->move(diffX, diffY, 0, 1);
				Window::point.position = sphere->position;
				break;
			case 3:
				cone->move(diffX, diffY, 0, 1);
				fixSpotlight();
				break;
			case 5:
				glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(diffX, diffY, 0));
				Window::V = trans * Window::V;
				glm::mat4 cam = glm::inverse(Window::V);
				Window::viewPos = glm::vec3(cam[3][0], cam[3][1], cam[3][2]);
				cam_look_at += viewPos - cam_pos;
				cam_pos = viewPos;
				Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
				break;
		}
	}
	mouseX = posX;
	mouseY = posY;
}

void Window::scroll_callback(GLFWwindow *window, double offX, double offY) {
	switch (Window::mode) {
		case 0:
			dragon->move(0, 0, -offY / 4.0f, 1);
			bunny->move(0, 0, -offY / 4.0f, 1);
			bear->move(0, 0, -offY / 4.0f, 1);
			break;
		case 2:
			sphere->move(0, 0, -offY / 4.0f, 1);
			Window::point.position = sphere->position;
			break;
		case 3:
			cone->move(0, 0, -offY / 4.0f, 1);
			fixSpotlight();
			break;
		case 5:
			float scale = ((float)pow(2, -offY / 10.0f));
			if (scale > 1 || glm::distance(cam_pos, cam_look_at) > 1) {
				Window::viewPos = cam_look_at + (scale * (cam_pos - cam_look_at));
				cam_pos = viewPos;
				Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			}
			break;
	}
}
