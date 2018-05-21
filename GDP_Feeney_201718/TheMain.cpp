// Include glad and GLFW in correct order
#include "globalOpenGL_GLFW.h"


#include <iostream>			// C++ cin, cout, etc.
//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>		// "String stream"
#include <string>

#include <vector>		//  smart array, "array" in most languages
#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "sMeshDrawInfo.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"


#include "Physics/cPhysicsWorld.h"

#include "cLightManager.h"

#include "globalGameStuff.h"

#include "cCamera.h"

#include "cFBO.h" 

#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
#include "Dalek_Threaded_03.h"
#include "cDalek.h"
#include "cAnimationState.h"



std::vector <glm::vec3> velocity;


bool charswitch = 0;
int selectcharacter = 0;
iDalekManager* g_pDalekManager; 

cFBO g_FBO_Pass1_G_Buffer;
cFBO g_FBO_Pass2_Deferred;


void DrawDebugLightingSpheres(void);


cGameObject* g_pSkyBoxObject = NULL;	// (theMain.cpp)

bool boyW=0, boyA = 0, boyD = 0, alW = 0, alA = 0, alD = 0, boyJ = 0, alJ = 0, boySW = 0, alSW = 0, boySS = 0, alSS = 0, boyS=0, alS=0, boyR=0, alR=0;
bool W = 0, A = 0, D = 0, SBAR = 0, J = 0, SS = 0, SW = 0, S=0;
std::vector< cGameObject* >  g_vecGameObjects;

cCamera* g_pTheCamera = NULL;


cVAOMeshManager* g_pVAOManager = 0;		// or NULL, or nullptr

cShaderManager*		g_pShaderManager = 0;		// Heap, new (and delete)
cLightManager*		g_pLightManager = 0;

CTextureManager*		g_pTextureManager = 0;

cModelAssetLoader*		g_pModelAssetLoader = NULL;


cDebugRenderer*			g_pDebugRenderer = 0;


cAABBBroadPhase* g_terrainAABBBroadPhase = 0;

cPhysicsWorld*	g_pPhysicsWorld = NULL;	// (theMain.cpp)

cGameObject* g_ExampleTexturedQuad = NULL;

cGameObject* g_Room = NULL;
cGameObject* g_RoomMaskForStencil = NULL;



#include "cFrameBuffer.h"

bool g_IsWindowFullScreen = false;
GLFWwindow* g_pGLFWWindow = NULL;
bool keyflag = 0;
float keytime = 0;
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


float g_ChromaticAberrationOffset = 0.0f;
float g_CR_Max = 0.1f;
double g_CA_CountDown = 0.0f;

const int RENDER_PASS_0_G_BUFFER_PASS = 0;
const int RENDER_PASS_1_DEFERRED_RENDER_PASS = 1;
const int RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS = 2;

int main(void)
{

	for (int i = 0; i < 5; i++)
	{
		velocity.push_back(glm::vec3(0, 0, 0));
	}
	//GLFWwindow* pGLFWWindow;		// Moved to allow switch from windowed to full-screen
	glfwSetErrorCallback(error_callback);



    if (!glfwInit())
	{
		// exit(EXIT_FAILURE);
		std::cout << "ERROR: Couldn't init GLFW, so we're pretty much stuck; do you have OpenGL??" << std::endl;
		return -1;
	}

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "OpenGL Rocks";

	std::ifstream infoFile("config.txt");
	if ( ! infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}   
	else
	{	// File DID open, so read it... 
		std::string a;	
		
		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"

		infoFile >> width;	// 1080

		infoFile >> a;	// "height"

		infoFile >> height;	// 768

		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if ( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading  = false;
				title = ssTitle.str();
			}
		}while( bKeepReading );


	}//if ( ! infoFile.is_open() )




    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// C++ string
	// C no strings. Sorry. char    char name[7] = "Michael\0";
    ::g_pGLFWWindow = glfwCreateWindow( width, height, 
							            title.c_str(), 
							            NULL, NULL);
    if ( ! ::g_pGLFWWindow )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback( ::g_pGLFWWindow, key_callback );
	// For the FBO to resize when the window changes
	glfwSetWindowSizeCallback( ::g_pGLFWWindow, window_size_callback );

    glfwMakeContextCurrent( ::g_pGLFWWindow );
    gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );
    glfwSwapInterval(1);



	std::cout << glGetString(GL_VENDOR) << " " 
		<< glGetString(GL_RENDERER) << ", " 
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// General error string, used for a number of items during start up
	std::string error;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";	
	//fragShader.fileName = "simpleFrag.glsl"; 
	fragShader.fileName = "simpleFragDeferred.glsl"; 

	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if ( ! ::g_pShaderManager->createProgramFromFile(
		        "mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;	
//		exit(
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;


// Triangle debug renderer test...
	::g_pDebugRenderer = new cDebugRenderer();
	if ( ! ::g_pDebugRenderer->initialize(error) )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
		std::cout << "\t" << ::g_pDebugRenderer->getLastError() << std::endl;
	}

//	const float WORLDMAX = 25.0f;
//	::g_pDebugRenderer->addTriangle( glm::vec3( -WORLDMAX, 0.0f, 0.0f ),
//									 glm::vec3(WORLDMAX, 0.0f, 0.0f ),
//									 glm::vec3( 0.0f, WORLDMAX, 0.0f),
//									 glm::vec3( 1.0f, 1.0f, 1.0f ), 20.0f );
//	for (int count = 0; count != 100; count++)
//	{
//		::g_pDebugRenderer->addTriangle(
//			glm::vec3(getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX)),
//			glm::vec3(getRandInRange(-WORLDMAX, WORLDMAX), 
//			          getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX)),
//			glm::vec3(getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX)),
//			glm::vec3( 1.0f, 1.0f, 1.0f ), 15.0f );
//	}//for ...
//	::g_pDebugRenderer->addTriangle( glm::vec3( -50.0f, -25.0f, 0.0f ),
//									 glm::vec3( 0.0, 50.0f, 100.0f ),
//									 glm::vec3( 50.0f, -25.0, 0.0f),
//									 glm::vec3( 1.0f, 1.0f, 0.0f ), 1000.0f );


//***********************************************************
//***********************************************************
//	const int NUMBER_OF_DALEKS = 1000;
//
////	::g_pDalekManager = new cDalekManager01();
////	::g_pDalekManager = new cDalekManager02();
//	::g_pDalekManager = new cDalekManager03();
//	::g_pDalekManager->Init(NUMBER_OF_DALEKS);
//
//	// Local array to point the Daleks
//	std::vector<glm::vec3> vecDalekPositions;
//	vecDalekPositions.reserve(NUMBER_OF_DALEKS);
//	for ( int count = 0; count != NUMBER_OF_DALEKS; count++ )
//	{
//		vecDalekPositions.push_back(glm::vec3(0.0f));
//	}

	//std::map< unsigned int /*Dalek Index*/, cGameObject* /*Dalek Game Object*/> mapDalekID_to_GameObject;

	//const float WORLDLIMIT = 1000.0f;
	//for ( int count = 0; count != NUMBER_OF_DALEKS; count++ )
	//{
	//	glm::vec3 position;
	//	position.x = getRandInRange<float>( -WORLDLIMIT, WORLDLIMIT );
	//	position.y = 0.0f;
	//	position.z = getRandInRange<float>( -WORLDLIMIT, WORLDLIMIT );
	//	cGameObject* pCurDalekGO = MakeDalekGameObject(position);

	//	cDalek* pCurrentDalek = NULL;
	//	::g_pDalekManager->CreateDalekThread( pCurDalekGO, pCurrentDalek );

	//	mapDalekID_to_GameObject[pCurrentDalek->getDalekID()] = pCurDalekGO;

	//	::g_vecGameObjects.push_back( pCurDalekGO );
	//}
//***********************************************************
//***********************************************************



	// Load models
	::g_pModelAssetLoader = new cModelAssetLoader();
	::g_pModelAssetLoader->setBasePath("assets/models/");

	::g_pVAOManager = new cVAOMeshManager();

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");


	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	if ( ! Load3DModelsIntoMeshManager(sexyShaderID, ::g_pVAOManager, ::g_pModelAssetLoader, error ) )
	{
		std::cout << "Not all models were loaded..." << std::endl;
		std::cout << error << std::endl;
	}

	LoadModelsIntoScene();



	// Named unifrom block
	// Now many uniform blocks are there? 
	GLint numberOfUniformBlocks = -1;
	glGetProgramiv(currentProgID, GL_ACTIVE_UNIFORM_BLOCKS, &numberOfUniformBlocks);

	// https://www.opengl.org/wiki/GLAPI/glGetActiveUniformBlock

	// Set aside some buffers for the names of the blocks
	const int BUFFERSIZE = 1000;

	int name_length_written = 0;

	char NUBName_0[BUFFERSIZE] = {0};
	char NUBName_1[BUFFERSIZE] = {0};

	glGetActiveUniformBlockName(currentProgID,
								0,
								BUFFERSIZE,
								&name_length_written,
								NUBName_0);

	glGetActiveUniformBlockName(currentProgID,
								1,
								BUFFERSIZE,
								&name_length_written,
								NUBName_1);

//	NUB_lighting
//	NUB_perFrame

	GLuint NUB_Buffer_0_ID = 0;
	GLuint NUB_Buffer_1_ID = 0;

	glGenBuffers(1, &NUB_Buffer_0_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, NUB_Buffer_0_ID );

	glGenBuffers(1, &NUB_Buffer_1_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, NUB_Buffer_1_ID);




	// Get the uniform locations for this shader
//	mvp_location = glGetUniformLocation(currentProgID, "MVP");		// program, "MVP");


//	GLint uniLoc_diffuseColour = glGetUniformLocation( currentProgID, "diffuseColour" );

	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights(10);	// There are 10 lights in the shader

	::g_pLightManager->vecLights[0].setLightParamType(cLight::POINT);		
	::g_pLightManager->vecLights[0].position = glm::vec3(500.0f, 500.0f, 1000.0f);
	::g_pLightManager->vecLights[0].attenuation.y = 0.0002f;		


	::g_pLightManager->vecLights[1].position = glm::vec3(0.0f, 570.0f, -212.0f);	
	::g_pLightManager->vecLights[1].attenuation.y = 0.000456922280f;		//0.172113f;
	::g_pLightManager->vecLights[1].setLightParamType(cLight::SPOT);		// <--- DOH! This would explain why I couldn't light up the scene!!
	// Point spot straight down at the ground
	::g_pLightManager->vecLights[1].direction = glm::vec3(0.0f, -1.0f, 0.0f );
	::g_pLightManager->vecLights[1].setLightParamSpotPrenumAngleInner( glm::radians(15.0f) );
	::g_pLightManager->vecLights[1].setLightParamSpotPrenumAngleOuter( glm::radians(45.0f) );
	::g_pLightManager->vecLights[1].position = glm::vec3(0.0f, 50.0f, 0.0f);	


	::g_pLightManager->LoadShaderUniformLocations(currentProgID);



	// Texture 
	::g_pTextureManager = new CTextureManager();

	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_TEXTURE_IMAGE_UNITS() << std::endl;
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS() << std::endl;

	::g_pTextureManager->setBasePath("assets/textures");
	if ( ! ::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Checkerboard.bmp", true) )
	{
		std::cout << "Didn't load the texture. Oh no!" << std::endl;
	}
	else
	{
		std::cout << "Texture is loaded! Hazzah!" << std::endl;
	}
	::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Enterprise.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
//	::g_pTextureManager->Create2DTextureFromBMPFile("Seamless_ground_sand_texture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("barberton_etm_2001121_lrg.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("height_map_norway-height-map-aster-30m.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("A_height_map_norway-height-map-aster-30m.bmp", true);

	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
			"space",
			"SpaceBox_right1_posX.bmp",
			"SpaceBox_left2_negX.bmp",
			"SpaceBox_top3_posY.bmp",
			"SpaceBox_bottom4_negY.bmp",
			"SpaceBox_front5_posZ.bmp",
			"SpaceBox_back6_negZ.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}


	      
///***********************************************************
	// About the generate the AABB for the terrain
	::g_terrainAABBBroadPhase = new cAABBBroadPhase();
	// Perhaps you'd like something more sophisticated than this...
	::g_terrainAABBBroadPhase->pDebugRenderer = ::g_pDebugRenderer;
	// 
	cMesh terrainMesh; 
	if (::g_pVAOManager->lookupMeshFromName("MeshLabTerrain_xyz_n_uv", terrainMesh))
	{
		std::cout << "Generating the terrain AABB grid. This will take a moment..." << std::endl;
		
		::g_terrainAABBBroadPhase->genAABBGridFromMesh(terrainMesh);

	}//if (::g_pVAOManager->lookupMeshFromName
///***********************************************************


	::g_pTheCamera = new cCamera();
//	::g_pTheCamera->setCameraMode(cCamera::FLY_CAMERA);
//	::g_pTheCamera->eye = glm::vec3(0.0f, 5.0f, -10.0f);

	//::g_pTheCamera->setCameraMode(cCamera::FOLLOW_CAMERA);
	//::g_pTheCamera->eye = glm::vec3(-100.0f, 150.0f, -300.0f);
	//::g_pTheCamera->Follow_SetMaxFollowSpeed(3.0f);
	//::g_pTheCamera->Follow_SetDistanceMaxSpeed(50.0f);	// Full speed beyond this distance
	//::g_pTheCamera->Follow_SetDistanceMinSpeed(25.0f);	// Zero speed at this distance
	//::g_pTheCamera->Follow_SetOrUpdateTarget(glm::vec3(0.0f, 0.0f, 0.0f));
	//::g_pTheCamera->Follow_SetIdealCameraLocation(glm::vec3(0.0f, 5.0f, 5.0f));

	::g_pTheCamera->setCameraMode(cCamera::MODE_FLY_USING_LOOK_AT);
	::g_pTheCamera->FlyCamLA->setEyePosition(glm::vec3(0.0f, 10.0f, 650.0f));
	::g_pTheCamera->FlyCamLA->setTargetInWorld(glm::vec3(0.0f, 20.0f, 0.0f));
	::g_pTheCamera->FlyCamLA->setUpVector(glm::vec3(0.0f, 1.0f, 0.0f));

//	::g_pTheCamera->FollowCam->SetOrUpdateTarget(glm::vec3(1.0f));


	// Follow the teapot
	//cGameObject* pLeftTeapot = findObjectByFriendlyName(LEFTTEAPOTNAME, ::g_vecGameObjects);
	//cPhysicalProperties physState;
	//pLeftTeapot->GetPhysState(physState);
	//physState.position.x = 150.0f;
	//physState.velocity.x = -10.0f;
	//physState.velocity.y = +25.0f;
	//physState.integrationUpdateType = cPhysicalProperties::DYNAMIC;	//	pLeftTeapot->bIsUpdatedInPhysics = true;
	//pLeftTeapot->SetPhysState(physState);
	//::g_pTheCamera->Follow_SetOrUpdateTarget(physState.position);


	::g_pPhysicsWorld = new cPhysicsWorld();


	// All loaded!
	std::cout << "And we're good to go! Staring the main loop..." << std::endl;

	glEnable( GL_DEPTH );


	// Create an FBO
	if ( ! ::g_FBO_Pass1_G_Buffer.init(1920, 1080, error) )
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}
	else
	{
		std::cout << "::g_FBO_Pass2_Deferred is good." << std::endl;
		std::cout << "\t::g_FBO_Pass1_G_Buffer ID = " << ::g_FBO_Pass1_G_Buffer.ID << std::endl;
		std::cout << "\tcolour texture ID = " << ::g_FBO_Pass1_G_Buffer.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << ::g_FBO_Pass1_G_Buffer.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << ::g_FBO_Pass1_G_Buffer.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << ::g_FBO_Pass1_G_Buffer.getMaxDrawBuffers() << std::endl;

	}//if ( ! ::g_FBO_Pass1_G_Buffer.init

	if ( ! ::g_FBO_Pass2_Deferred.init(1920, 1080, error) )
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}
	else
	{
		std::cout << "FBO is good." << std::endl;
		std::cout << "\t::g_FBO_Pass2_Deferred ID = " << ::g_FBO_Pass2_Deferred.ID << std::endl;
		std::cout << "\tcolour texture ID = " << ::g_FBO_Pass2_Deferred.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << ::g_FBO_Pass2_Deferred.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << ::g_FBO_Pass2_Deferred.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << ::g_FBO_Pass2_Deferred.getMaxDrawBuffers() << std::endl;

	}//if ( ! ::g_FBO_Pass2_Deferred.init




	setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );


	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();






	//// Spawn the tread to update the Dalek 
	//unsigned int DalekThreadHandle = 0;
	//unsigned threadID = 0;

	//// Creates critical section, etc.
	//InitDalekThreading();

	//DalekThreadHandle = _beginthreadex(
	//	NULL,			// Don't change the security permissions for this thread
	//	0,				// 0 = default stack
	//	DalekBrainThread,		// The function we want to call. 
	//	NULL, //pDataToPassA,			// Arguement list (or NULL if we are passing VOID)
	//	0,				// or CREATE_SUSPENDED if it's paused and has to start
	//	&threadID);


	// Allow the Daleks to update
	

	// Could also:
	// 1. Start the threads as CREATE_SUSPENDED
	// 2. Then go through all the threads and call 
	//    ResumeThread() on each one. 
	// Never call SuspendThead

	::g_pTheCamera->setCameraMode(cCamera::MODE_FLY_USING_LOOK_AT);
	// Main game or application loop







	cAnimationState::sStateDetails awalk;
	awalk.name = "assets/modelsFBX/boy/walk.fbx";
	//awalk.totalTime = g_vecGameObjects[0]->pSimpleSkinnedMesh->FindAnimationTotalTime(awalk.name);
	awalk.totalTime = 0.95f;
	awalk.frameStepTime = 0.015f;



	cAnimationState::sStateDetails adance;
	adance.name = "assets/modelsFBX/boy/dance.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	adance.totalTime = 2.0f;
	adance.frameStepTime = 0.01f;

	cAnimationState::sStateDetails asleft;
	asleft.name = "assets/modelsFBX/boy/sleft.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	asleft.totalTime = 0.35f;
	asleft.frameStepTime = 0.01f;

	cAnimationState::sStateDetails asright;
	asright.name = "assets/modelsFBX/boy/sright.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	asright.totalTime = 0.35f;
	asright.frameStepTime = 0.01f;




	cAnimationState::sStateDetails ajumpup;
	ajumpup.name = "assets/modelsFBX/boy/jumpup.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	ajumpup.totalTime = 1.5f;
	ajumpup.frameStepTime = 0.01f;


	cAnimationState::sStateDetails bwalk;
	bwalk.name = "assets/modelsFBX/alien/walk.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	bwalk.totalTime = 0.8f;
	bwalk.frameStepTime = 0.01f;



	cAnimationState::sStateDetails bdance;
	bdance.name = "assets/modelsFBX/alien/dance.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	bdance.totalTime = 2.0f;
	bdance.frameStepTime = 0.01f;

	cAnimationState::sStateDetails bsleft;
	bsleft.name = "assets/modelsFBX/alien/sleft.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	bsleft.totalTime = 0.35f;
	bsleft.frameStepTime = 0.01f;

	cAnimationState::sStateDetails bsright;
	bsright.name = "assets/modelsFBX/alien/sright.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	bsright.totalTime = 0.35f;
	bsright.frameStepTime = 0.01f;




	cAnimationState::sStateDetails bjumpup;
	bjumpup.name = "assets/modelsFBX/alien/jumpup.fbx";
	bjumpup.totalTime = 2.0f;
	bjumpup.frameStepTime = 0.015f;





	cAnimationState::sStateDetails arun;
	arun.name = "assets/modelsFBX/boy/run.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	arun.totalTime = 1.0f;
	arun.frameStepTime = 0.01f;

	cAnimationState::sStateDetails ajump;
	ajump.name = "assets/modelsFBX/boy/jump.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	ajump.totalTime = 2.0f;
	ajump.frameStepTime = 0.01f;



	cAnimationState::sStateDetails brun;
	brun.name = "assets/modelsFBX/alien/run.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	brun.totalTime = 2.0f;
	brun.frameStepTime = 0.01f;

	cAnimationState::sStateDetails bjump;
	bjump.name = "assets/modelsFBX/alien/jump.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	bjump.totalTime = 2.0f;
	bjump.frameStepTime = 0.01f;














	while ( ! glfwWindowShouldClose(::g_pGLFWWindow) )
    {
		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime =  curTime - lastTimeStep;
		lastTimeStep = curTime;


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		//Main STUFF
	




		if (::selectcharacter == 0)
		{
			::g_vecGameObjects[0]->vecMeshes[0].debugDiffuseColour = glm::vec4(1, 0, 0, 1);
			::g_vecGameObjects[1]->vecMeshes[0].debugDiffuseColour = glm::vec4(1, 1, 1, 1);
		}
		else
		{
			::g_vecGameObjects[0]->vecMeshes[0].debugDiffuseColour = glm::vec4(1, 1, 1, 1);
			::g_vecGameObjects[1]->vecMeshes[0].debugDiffuseColour = glm::vec4(1, 0, 0, 1);
		}


		if (::charswitch == 1)
		{
			if (::selectcharacter == 0)
			{
				::g_pTheCamera->FlyCamLA->setEyePosition(glm::vec3(::g_vecGameObjects[0]->getPosition().x, ::g_vecGameObjects[0]->getPosition().y + 100.0f, ::g_vecGameObjects[0]->getPosition().z - 300.0f));
				::g_pTheCamera->FlyCamLA->setTargetInWorld(::g_vecGameObjects[0]->getPosition());
			}
			else
			{
				::g_pTheCamera->FlyCamLA->setEyePosition(glm::vec3(::g_vecGameObjects[1]->getPosition().x, ::g_vecGameObjects[1]->getPosition().y + 100.0f, ::g_vecGameObjects[1]->getPosition().z - 300.0f));
				::g_pTheCamera->FlyCamLA->setTargetInWorld(::g_vecGameObjects[1]->getPosition());
			}
			::charswitch = 0;

		}


		if ( ::W==0 && ::A==0 && ::D==0 && ::J==0 && ::SBAR==0 && ::SW == 0 && ::SS==0 && ::S==0)
		{
			g_vecGameObjects[0]->pAniState->vecAnimationQueue.clear();
			g_vecGameObjects[1]->pAniState->vecAnimationQueue.clear();
		}

		if (::keyflag == 0)
		{
			/*if (::keytime + 0.7 <= glfwGetTime())
			{
				::keyflag = 0;
			}*/

			/*if (::keytime + 0.7 <= glfwGetTime())
			{
				::keyflag = 0;
			}*/
			
		}


//boyW=0, boyA = 0, boyD = 0, alW = 0, alA = 0, alD = 0, boyJ = 0, alJ = 0, boySW = 0, alSW = 0, boySS = 0, alSS = 0;

		if (::boyW == 1 && ::boySW == 0)
		{
			
				g_vecGameObjects[0]->overwritePotition(glm::vec3(g_vecGameObjects[0]->getPosition().x, g_vecGameObjects[0]->getPosition().y, g_vecGameObjects[0]->getPosition().z + 70*deltaTime  )  );
			
				//::boyW = 0;
			
		}
		else if (::boyA == 1)
		{
			
				g_vecGameObjects[0]->overwritePotition(glm::vec3(g_vecGameObjects[0]->getPosition().x + 100*deltaTime, g_vecGameObjects[0]->getPosition().y, g_vecGameObjects[0]->getPosition().z));
				//::boyA = 0;
			
		}
		else if (::boyD == 1)
		{
				g_vecGameObjects[0]->overwritePotition(glm::vec3(g_vecGameObjects[0]->getPosition().x - 100 * deltaTime, g_vecGameObjects[0]->getPosition().y, g_vecGameObjects[0]->getPosition().z));

		}
		else if (::boySW == 1)
		{
			g_vecGameObjects[0]->overwritePotition(glm::vec3(g_vecGameObjects[0]->getPosition().x, g_vecGameObjects[0]->getPosition().y, g_vecGameObjects[0]->getPosition().z + 100 * deltaTime));

		}
		else if (::boySS== 1)
		{
			g_vecGameObjects[0]->overwritePotition(glm::vec3(g_vecGameObjects[0]->getPosition().x, g_vecGameObjects[0]->getPosition().y, g_vecGameObjects[0]->getPosition().z + 150 * deltaTime));

		}
		else if (::alW == 1)
		{
			g_vecGameObjects[1]->overwritePotition(glm::vec3(g_vecGameObjects[1]->getPosition().x, g_vecGameObjects[1]->getPosition().y, g_vecGameObjects[1]->getPosition().z + 70 * deltaTime));

		}
		else if (::alA == 1)
		{
			g_vecGameObjects[1]->overwritePotition(glm::vec3(g_vecGameObjects[1]->getPosition().x + 100 * deltaTime, g_vecGameObjects[1]->getPosition().y, g_vecGameObjects[1]->getPosition().z));

		}
		else if (::alD == 1)
		{
			g_vecGameObjects[1]->overwritePotition(glm::vec3(g_vecGameObjects[1]->getPosition().x - 100 * deltaTime, g_vecGameObjects[1]->getPosition().y, g_vecGameObjects[1]->getPosition().z));

		}
		else if (::alSW == 1)
		{
			g_vecGameObjects[1]->overwritePotition(glm::vec3(g_vecGameObjects[1]->getPosition().x, g_vecGameObjects[1]->getPosition().y, g_vecGameObjects[1]->getPosition().z + 100 * deltaTime) );

		}
		else if (::alSS == 1)
		{
			g_vecGameObjects[1]->overwritePotition(glm::vec3(g_vecGameObjects[1]->getPosition().x, g_vecGameObjects[1]->getPosition().y, g_vecGameObjects[1]->getPosition().z + 150 * deltaTime));

		}
		else if (::boyR == 1)
		{
			g_vecGameObjects[0]->overwritePotition(glm::vec3(g_vecGameObjects[0]->getPosition().x, g_vecGameObjects[0]->getPosition().y, g_vecGameObjects[0]->getPosition().z - 150 * deltaTime));

		}
		else if (::alR == 1)
		{
			g_vecGameObjects[1]->overwritePotition(glm::vec3(g_vecGameObjects[1]->getPosition().x, g_vecGameObjects[1]->getPosition().y, g_vecGameObjects[1]->getPosition().z - 150 * deltaTime));

		}
		else
		{

		}
		

		

			

		if (::W == 1)
		{
			if (::selectcharacter == 0)
			{
				::boyW = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(awalk);

			}
			else
			{
				::alW = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bwalk);
			}
		}

		if (::A == 1)
		{
			if (::selectcharacter == 0)
			{
				::boyA = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(asleft);
			}
			else
			{
				::alA = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bsleft);
			}
		}



		if (::D == 1)
		{
			if (::selectcharacter == 0)
			{
				::boyD = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(asright);
			}
			else
			{
				::alD = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bsright);
			}
		}

		if (::J == 1)
		{
			if (::selectcharacter == 0)
			{
				::boyJ = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(adance);
			}
			else
			{
				::alJ = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bdance);
			}
		}


		if (::SBAR == 1)
		{
			if (::selectcharacter == 0)
			{
				::boyS = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(ajumpup);
			}
			else
			{
				::alS = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bjumpup);
			}
		}

		if (::SW == 1)
		{
			if (::selectcharacter == 0)
			{
				::boySW = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(arun);
			}
			else
			{
				::alSW = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(brun);
			}
		}

		if (::SS == 1)
		{
			if (::selectcharacter == 0)
			{
				::boySS = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(ajump);
			}
			else
			{
				::alSS = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bjump);
			}
		}
		if (::S == 1)
		{
			if (::selectcharacter == 0)
			{
				::boyR = 1;
				::g_vecGameObjects[0]->pAniState->vecAnimationQueue.push_back(awalk);
			}
			else
			{
				::alR = 1;
				::g_vecGameObjects[1]->pAniState->vecAnimationQueue.push_back(bwalk);
			}
		}




		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				if (i != j)
				{
					
					if (glm::distance(g_vecGameObjects[i]->getPosition(), g_vecGameObjects[j]->getPosition()) <= 15.0f)
					{
						velocity[i] = -g_vecGameObjects[j]->getPosition() + g_vecGameObjects[i]->getPosition();
						velocity[j] = -g_vecGameObjects[i]->getPosition() + g_vecGameObjects[j]->getPosition();


						velocity[i] /= abs(velocity[i]);
						velocity[j] /= abs(velocity[j]);


					}
					else
					{
						velocity[i] -= velocity[i] / 50.0f;
						velocity[j] -= velocity[j] / 50.0f;
					}
				}
				
			}


			if (i != 1 && i != 0 )
			{
				g_vecGameObjects[i]->overwritePotition(glm::vec3(g_vecGameObjects[i]->getPosition().x + 20*velocity[i].x * deltaTime, g_vecGameObjects[i]->getPosition().y , g_vecGameObjects[i]->getPosition().z + 20* velocity[i].y*deltaTime));
				//g_vecGameObjects[j]->overwritePotition(glm::vec3(g_vecGameObjects[j]->getPosition().x + velocity[j].x, g_vecGameObjects[j]->getPosition().y + velocity[j].y, g_vecGameObjects[j]->getPosition().z));


			}
		}

		//std::cout << " Objects 1 and ball 2 distance:" << glm::distance(g_vecGameObjects[0]->getPosition(), g_vecGameObjects[2]->getPosition()) << std::endl;


		std::cout << g_vecGameObjects[0]->getPosition().x << "   " << g_vecGameObjects[0]->getPosition().y << "     " << g_vecGameObjects[0]->getPosition().z << std::endl;

		







		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		//Main STUFF


		//::g_vecGameObjects[0]->SetPhysState.overwritePotition
		// Call the "thread" function
		//::g_DeltaTime = deltaTime;
//		::g_SetDeltaTime(deltaTime);
//		DalekBrainThread(NULL);
//
//		// Copy the Dalek information to the actual game object
//		cGameObject* pDalekGO = findObjectByFriendlyName("Big D", ::g_vecGameObjects);
//		cPhysProps state;
//		pDalekGO->GetPhysState(state);
////		state.position = ::g_DalekPosition;
//		state.position = ::g_GetDalekPosition();
//		pDalekGO->SetPhysState( state );
//		std::cout 
//			<< state.position.x << ", " 
//			<< state.position.y << ", " 
//			<< state.position.z << std::endl;
//
		// ************************************************************
//		mapDalekID_to_GameObject;
//			virtual bool getDalekPositionsAtIndexRange( unsigned int startIndex, unsigned int endIndex, 
//									             );
		//::g_pDalekManager->getAllDalekPositions( vecDalekPositions );
		//for ( unsigned int index = 0; index != vecDalekPositions.size(); index++ )
		//{
		//	cPhysProps DalekPhysState;
		//	cGameObject* pDalekGO = mapDalekID_to_GameObject[index];
		//	pDalekGO->GetPhysState( DalekPhysState );
		//	DalekPhysState.position = vecDalekPositions[index];
		//	pDalekGO->SetPhysState( DalekPhysState );
		//}

		//// For lockless, swich buffers
		//::g_pDalekManager->SwitchBuffers();

		// ************************************************************



		::g_pPhysicsWorld->IntegrationStep(deltaTime);

		::g_pTheCamera->updateTick(deltaTime);

		// *********************************************
		//    ___ _        ___              __     ___                          
		//   / __| |___  _| _ ) _____ __  __\ \   / __|__ _ _ __  ___ _ _ __ _  
		//   \__ \ / / || | _ \/ _ \ \ / |___> > | (__/ _` | '  \/ -_) '_/ _` | 
		//   |___/_\_\\_, |___/\___/_\_\    /_/   \___\__,_|_|_|_\___|_| \__,_| 
		//            |__/                                                      
		cPhysicalProperties skyBoxPP;
		::g_pSkyBoxObject->GetPhysState(skyBoxPP);
		skyBoxPP.position = ::g_pTheCamera->getEyePosition();
		::g_pSkyBoxObject->SetPhysState(skyBoxPP);



//    ___                _             _           ___       _            __   __           
//   | _ \ ___  _ _   __| | ___  _ _  | |_  ___   / __| ___ | |__  _  _  / _| / _| ___  _ _ 
//   |   // -_)| ' \ / _` |/ -_)| '_| |  _|/ _ \ | (_ ||___|| '_ \| || ||  _||  _|/ -_)| '_|
//   |_|_\\___||_||_|\__,_|\___||_|    \__|\___/  \___|     |_.__/ \_,_||_|  |_|  \___||_|  
//                                                                        
// In this pass, we render all the geometry to the "G buffer"
// The lighting is NOT done here. 
// 
		::g_pShaderManager->useShaderProgram("mySexyShader");

		// Direct everything to the FBO
		GLint renderPassNumber_LocID = glGetUniformLocation(sexyShaderID, "renderPassNumber");
		glUniform1i( renderPassNumber_LocID, RENDER_PASS_0_G_BUFFER_PASS );

		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass1_G_Buffer.ID );
		// Clear colour AND depth buffer
		g_FBO_Pass1_G_Buffer.clearBuffers();

//glDisable(GL_DEPTH_TEST);		// Check what's already in the depth buffer
//glDepthMask(GL_FALSE);			// Written to the depth buffer

		// 1. Drawing the "mask" object (that "stencil")

		// Note the addition to the stencil buffer clear
		// BIG NOTE: This clears the FINAL frame buffer! 
		// (Keep in mind that when the stencil is enabled, it's enabled for ALL frame buffers)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

//////////		glEnable(GL_STENCIL_TEST);
//////////
//////////		// Always SUCCEED (the stencil test).
//////////		// Don't mask anything (0xFF means don't "mask")
//////////		// Write "ref" to buffer if succeeds.
//////////		// NOTE: 
//////////		// - Stencil is currently all zeros. 
//////////		// - Whatever we draw will be written to the stencil as a value of 1
//////////		// - The mask of 0xFF (1111 1111) means nothing will be masked (prevented)
////////////		glStencilFunc(GL_ALWAYS, 1, 0xFF);		// All 1s: 1111 1111
//////////		glStencilFunc(GL_GEQUAL, 1, 0xFF);		// All 1s: 1111 1111
//////////		// Stencil will ALWAYS pass...
//////////		// Depth will pass, too (since this is the only thing we are drawing)
//////////		// If the Stencil AND the Depth PASS, then REPLACE the stencil value with the 
//////////		// ...value in the StencilFun(), which is the value 1, in this case
//////////		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//////////		glStencilMask(0xFF);		// Control of writing to the buffer
//////////		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//////////		// Render the doorway mask
////////////	glDisable(GL_DEPTH_TEST);
////////////	glDepthMask(GL_FALSE);
//////////
//////////		std::vector< cGameObject* > vecOnlyTheRoomMask;
//////////		vecOnlyTheRoomMask.push_back( ::g_RoomMaskForStencil);
//////////		RenderScene(vecOnlyTheRoomMask, ::g_pGLFWWindow, deltaTime );
//////////		// So, at this point the stencil buffer will be zero (0), except where we 
//////////		// ...drew the door mask object, where it will be one (1)
//////////
//////////
//////////		// 2. 
//////////		// Clear colour buffer ONLY
//////////		// Keep depth and stencil: 
//////////		//  - depth because we don't want to draw what's behind the mask
//////////		//  - stencil is untouched
//////////		//  - clear colour to "erase" the masking object.
////////////		glClear(GL_COLOR_BUFFER_BIT );
////////////		glClear(GL_DEPTH_BUFFER_BIT );
//////////
//////////		// Draw only the room
//////////		// Where it's 0, draw the room (where we DIDN'T draw the masking object)
//////////		// (Note the bit mask is 0xFF, meaning we aren't selecting a particular stencil)
//////////		glStencilFunc(GL_ALWAYS, 0, 0xFF);
//////////		// Stencil passes (always) - keep original
//////////		// AND Depth FAILS - Keep stencil (this is where the mask is IN FRONT OF portions of the room, like the far side)
//////////		// Stencil AND depth pass - REPLACE with a value of zero (meaning part of the room is IN FRONT OF the door mask)
//////////		// Parts of the room are in front of the door mask, but since the door mask was drawn 1st, the stencil
//////////		//	buffer will have 1s at those locations. These "in front of the mask" will now be set back to zeros.
//////////		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//////////		// Draw ONLY the room
//////////		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
////////////	glEnable(GL_DEPTH_TEST);
////////////	glDepthMask(GL_TRUE);
//////////		std::vector< cGameObject* > vecOnlyTheRoom;
//////////		vecOnlyTheRoom.push_back( ::g_Room );
//////////		RenderScene( vecOnlyTheRoom, ::g_pGLFWWindow, deltaTime );


















































			//::g_vecGameObjects[0]->overwritePotition(::g_vecGameObjects[0]->getPosition(), 1);




		// 3. Draw the rest of the scene.
		// Clear the depth buffer, too (where the door mask was)
		glClear( GL_DEPTH_BUFFER_BIT );
//		glDisable(GL_STENCIL_TEST);
		// Where it's 1, draw the scene 
		// - remember, wherever we drew the masking object, it's 1
		// - the rest of the scene is sill zero 
		// So this passes where every the 0 is less than the stencil buffer
		// - The stencil has 1 where the mask is, and zero everywhere else
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);		// Make sure we can't write to the stencil buffer
		// BUT, we DON'T want to change what's already on the stencil buffer
		glStencilOp(GL_KEEP,		// Stencil fail
					GL_KEEP,		// Depth fail
					GL_KEEP);		// Stencil AND Depth PASS

		RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );

		for (int i=0 ; i <= 1; i++)
		{
			if (g_vecGameObjects[i]->getPosition().z >= 800)
			{
				g_vecGameObjects[i]->overwritePotition(glm::vec3(g_vecGameObjects[i]->getPosition().x, g_vecGameObjects[i]->getPosition().y - 150*deltaTime, g_vecGameObjects[i]->getPosition().z));
			}
		}
		


	//	std::cout << g_vecGameObjects[0]->getPosition().x << "  " << g_vecGameObjects[0]->getPosition().y << g_vecGameObjects[0]->getPosition().z<<std::endl;


//// *********************************************************************************
//		// NOTE: The RenderScene eventually updates the skinned mesh info, 
//		//			which is now stored into the skinned mesh information
//		// When I call this, if it's a skinned mesh, the extents will be calculated, too
//		// (like for that pose on this frame)
//		// Find one of the skinned meshes
//		cGameObject* pSophieT = findObjectByFriendlyName( "Sophie", ::g_vecGameObjects );
//
//		// This box is "object relative" (i.e. around origin of the model, not in the world)
//		glm::vec3 minXYZ = pSophieT->vecMeshes[0].minXYZ_from_SM_Bones;
//		glm::vec3 maxXYZ = pSophieT->vecMeshes[0].maxXYZ_from_SM_Bones;
//
////	// Transform this based on where the character is in the world...
//	// BUT HOW, you? If only there was a matrix that described where the object was!!!
//	{
//		cPhysicalProperties sophPhysState;
//		pSophieT->GetPhysState(sophPhysState);
//
//		glm::mat4 matSophieWorld = glm::mat4(1.0f);		// identity
//		glm::mat4 trans = glm::mat4x4(1.0f);
//		matSophieWorld = glm::translate(matSophieWorld, sophPhysState.position );
//		glm::mat4 postRotQuat = glm::mat4(pSophieT->getFinalMeshQOrientation(sophPhysState.get));
//		matSophieWorld = matSophieWorld * postRotQuat;
//		glm::mat4 matScale = glm::mat4x4(1.0f);
//		matScale = glm::scale(matScale,
//							  glm::vec3(pSophieT->GetPhysState,
//							  theMesh.scale,
//							  theMesh.scale));
//		matSophieWorld = matSophieWorld * matScale;
//	}
//
//		//pSophieT->vecMeshes[0].vecObjectBoneTransformation[boneIndex]
//
//		g_pDebugRenderer->addTriangle( minXYZ, maxXYZ, minXYZ,
//									   glm::vec3(0.0,0.0,0.0) );
//		g_pDebugRenderer->addTriangle( maxXYZ, minXYZ,maxXYZ, 
//									   glm::vec3(0.0,0.0,0.0) );
//
//		// Draw a triangle at each bone... 
//		// Note we have to translate the location for each debug triangle.
//		// (but the "boneLocationXYZ" is the centre of the bone)
////		for (unsigned int boneIndex = 0; 
////			 boneIndex != pSophieT->vecMeshes[0].vecObjectBoneTransformation.size();
////			 boneIndex++)
////		{
////			glm::mat4 boneLocal = pSophieT->vecMeshes[0].vecObjectBoneTransformation[boneIndex];
////	
////			float scale = pSophieT->vecMeshes[0].scale;
////			boneLocal = glm::scale(boneLocal, glm::vec3(scale, scale, scale));
////	
////	
////			//glm::vec4 boneBallLocation = boneLocal * GameObjectLocalOriginLocation;
////			glm::vec4 boneLocationXYZ = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
////			boneLocationXYZ *= scale;
////	
////			// Draw a triangle at each bone...
////			glm::vec3 v[3];
////			v[0].x = boneLocationXYZ.x;
////			v[0].y = boneLocationXYZ.y;
////			v[0].z = boneLocationXYZ.z;
////			v[1] = v[0] + glm::vec3(2.0f, 0.0f, 0.0f);
////			v[2] = v[0] - glm::vec3(2.0f, 0.0f, 0.0f);
////	
////			g_pDebugRenderer->addTriangle( v[0], v[1], v[2], glm::vec3(1.0f, 1.0f, 1.0f) ); 
////		}
//
//		int indexOfFingerBone = pSophieT->pSimpleSkinnedMesh->m_mapBoneNameToBoneIndex["B_L_Finger02"];// = 32
//
//		// Transform to get the location in world space... 
//
//		// This vector of bones has the final location of that bone (this frame)
//		glm::mat4 boneLocal = pSophieT->vecMeshes[0].vecObjectBoneTransformation[indexOfFingerBone];
//
//		float scale = pSophieT->vecMeshes[0].scale;
//		boneLocal = glm::scale(boneLocal, glm::vec3(scale, scale, scale));
//
//		glm::vec4 B_L_Finger02_XYZ = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//		B_L_Finger02_XYZ *= scale;
//		// And transform based on object world...
//
//		// matWorld (or matObject) of the object
//
//		glm::vec3 fingerXYZ = glm::vec3(B_L_Finger02_XYZ);
//
//		g_pDebugRenderer->addTriangle( fingerXYZ,
//									   fingerXYZ + glm::vec3(1.0,0,0),
//									   fingerXYZ - glm::vec3(1.0,0,0),
//									   glm::vec3(1.0f, 1.0f, 1.0f) );
//// *********************************************************************************

//    ___         __                         _   ___                _             ___               
//   |   \  ___  / _| ___  _ _  _ _  ___  __| | | _ \ ___  _ _   __| | ___  _ _  | _ \ __ _  ___ ___
//   | |) |/ -_)|  _|/ -_)| '_|| '_|/ -_)/ _` | |   // -_)| ' \ / _` |/ -_)| '_| |  _// _` |(_-<(_-<
//   |___/ \___||_|  \___||_|  |_|  \___|\__,_| |_|_\\___||_||_|\__,_|\___||_|   |_|  \__,_|/__//__/
//   
// In this pass, we READ from the G-buffer, and calculate the lighting. 
// In this example, we are writing to another FBO, now. 
// 


		// Render it again, but point the the FBO texture... 
//		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass2_Deferred.ID );
//		g_FBO_Pass2_Deferred.clearBuffers();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		glDisable(GL_STENCIL_TEST);

		::g_pShaderManager->useShaderProgram("mySexyShader");

		glUniform1i( renderPassNumber_LocID, RENDER_PASS_1_DEFERRED_RENDER_PASS );
		
		//uniform sampler2D texFBONormal2D;
		//uniform sampler2D texFBOVertexWorldPos2D;

		GLint texFBOColour2DTextureUnitID = 10;
		GLint texFBOColour2DLocID = glGetUniformLocation(sexyShaderID, "texFBOColour2D");
		GLint texFBONormal2DTextureUnitID = 11;
		GLint texFBONormal2DLocID = glGetUniformLocation(sexyShaderID, "texFBONormal2D");
		GLint texFBOWorldPosition2DTextureUnitID = 12;
		GLint texFBOWorldPosition2DLocID = glGetUniformLocation(sexyShaderID, "texFBOVertexWorldPos2D");

		// Pick a texture unit... 
		glActiveTexture(GL_TEXTURE0 + texFBOColour2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.colourTexture_0_ID);
		glUniform1i(texFBOColour2DLocID, texFBOColour2DTextureUnitID);

		glActiveTexture(GL_TEXTURE0 + texFBONormal2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.normalTexture_1_ID);
		glUniform1i(texFBONormal2DLocID, texFBONormal2DTextureUnitID);
		
		glActiveTexture(GL_TEXTURE0 + texFBOWorldPosition2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.vertexWorldPos_2_ID);
		glUniform1i(texFBOWorldPosition2DLocID, texFBOWorldPosition2DTextureUnitID);
		
		// Set the sampler in the shader to the same texture unit (20)

		glfwGetFramebufferSize(::g_pGLFWWindow, &width, &height);

		GLint screenWidthLocID = glGetUniformLocation(sexyShaderID, "screenWidth");
		GLint screenHeightLocID = glGetUniformLocation(sexyShaderID, "screenHeight");
		glUniform1f(screenWidthLocID, (float)width);
		glUniform1f(screenHeightLocID, (float)height);

		// Adjust the CA:
		//float g_ChromaticAberrationOffset = 0.0f;
		// 0.1
		if ( ::g_CA_CountDown > 0.0 )
		{
			float CRChange = getRandInRange( (-g_CR_Max/10.0f), (::g_CR_Max/10.0f) );
			::g_ChromaticAberrationOffset += CRChange;
			::g_CA_CountDown -= deltaTime;
		}
		else
		{
			::g_ChromaticAberrationOffset = 0.0f;
		}

		GLint CROffset_LocID = glGetUniformLocation(sexyShaderID, "CAoffset" );
		glUniform1f( CROffset_LocID, g_ChromaticAberrationOffset);

		std::vector< cGameObject* >  vecCopy2ndPass;
		// Push back a SINGLE quad or GIANT triangle that fills the entire screen
		// Here we will use the skybox (as it fills the entire screen)
		vecCopy2ndPass.push_back( ::g_pSkyBoxObject );

//		cGameObject* pBunny = findObjectByFriendlyName("bugs", ::g_vecGameObjects);
//		vecCopy2ndPass.push_back(pBunny);

		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );




		// Set the scissor buffer
//// Example to render only INSIDE the scissor buffer (square) area
//		glScissor(500, 500, 1080, 600);
//		glEnable(GL_SCISSOR_TEST );
//		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
//		glDisable(GL_SCISSOR_TEST );
//// 
//
//// Example to render only OUTSIDE of the scissor area:
//		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
//		glScissor(500, 500, 1080, 600);
//		glEnable(GL_SCISSOR_TEST );
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glDisable(GL_SCISSOR_TEST );
//// 


//    ___  _              _   ___  ___    ___               
//   | __|(_) _ _   __ _ | | |_  )|   \  | _ \ __ _  ___ ___
//   | _| | || ' \ / _` || |  / / | |) | |  _// _` |(_-<(_-<
//   |_|  |_||_||_|\__,_||_| /___||___/  |_|  \__,_|/__//__/
//                                                          	
// Here, we read from the off screen buffer, the one that 
// has all the lighting, etc. 
// This is where we can do the "2nd pass effects", so the 
// full-screen 2D effects.
//
// NOTE: In this example, we are only using this to render to an offscreen object

		// Now the final pass (in this case, only rendering to a quad)
		//RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS
///////	glBindFramebuffer(GL_FRAMEBUFFER, 0);
///////
///////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
///////
///////	::g_pShaderManager->useShaderProgram("mySexyShader");
///////
///////	glUniform1i(renderPassNumber_LocID, RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS );
///////
///////	/// The "deferred pass" FBO has a colour texture with the entire rendered scene
///////	/// (including lighting, etc.)
///////	GLint fullRenderedImage2D_LocID = glGetUniformLocation(sexyShaderID, "fullRenderedImage2D");
///////
///////	/// Pick a texture unit... 
///////	unsigned int pass2unit = 50;
///////	glActiveTexture( GL_TEXTURE0 + pass2unit);
///////	glBindTexture(GL_TEXTURE_2D, ::g_FBO_Pass2_Deferred.colourTexture_0_ID);
///////	glUniform1i(fullRenderedImage2D_LocID, pass2unit);
///////
///////
///////	std::vector< cGameObject* >  vecCopySingleLonelyQuad;
///////	/// Push back a SINGLE quad or GIANT triangle that fills the entire screen
///////	vecCopySingleLonelyQuad.push_back( ::g_ExampleTexturedQuad );
///////
///////	cGameObject* pTheShip = findObjectByFriendlyName( "NCC-1701", ::g_vecGameObjects );
///////	vecCopySingleLonelyQuad.push_back( pTheShip );
///////
///////	RenderScene(vecCopySingleLonelyQuad, ::g_pGLFWWindow, deltaTime);





//		RenderScene(::g_vecTransparentObjectsOnly, ::g_pGLFWWindow, deltaTime);

		std::stringstream ssTitle;
//		ssTitle << "Camera (xyz): " 
//			<< g_cameraXYZ.x << ", " 
//			<< g_cameraXYZ.y << ", " 
//			<< g_cameraXYZ.z;
		// 
//		glm::vec4 EulerAngle;
		//g_pTheCamera->qOrientation.eularAngles(EulerAngle);
		//ssTitle << "Camera (xyz): "
		//	<< EulerAngle.x << ", "
		//	<< EulerAngle.y << ", "
		//	<< EulerAngle.z;

		glfwSetWindowTitle( ::g_pGLFWWindow, ssTitle.str().c_str() );

		// "Presents" what we've drawn
		// Done once per scene 
        glfwSwapBuffers(::g_pGLFWWindow);
        glfwPollEvents();


    }// while ( ! glfwWindowShouldClose(window) )


    glfwDestroyWindow(::g_pGLFWWindow);
    glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;


	

//    exit(EXIT_SUCCESS);
	return 0;
}


void DrawDebugLightingSpheres(void)
{
	//DEBUG sphere
	::g_pDebugRenderer->addDebugSphere( glm::vec3( 0.0f, 0.0f, 0.0f ), 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), 1.0f );
	// Light at 95% 
	float scaleAt99 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.99f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt99 );

	// Light at 50% 
	float scaleAt50 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.5f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt50 );

	// Light at 25% 
	float scaleAt25 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.25f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt25 );

	// Light at 1% 
	float scaleAt01 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.01f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt01 );
	return;
}



void setWindowFullScreenOrWindowed( GLFWwindow* pTheWindow, bool IsFullScreen )
{
	// Set full-screen or windowed
	if ( ::g_IsWindowFullScreen )
	{	
		// Find the size of the primary monitor
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );
		// Set this window to full screen, matching the size of the monitor:
		glfwSetWindowMonitor( pTheWindow, pPrimaryScreen, 
							  0, 0,				// left, top corner 
							  pPrimMonVidMode->width, pPrimMonVidMode->height, 
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now fullscreen at ( " 
			<< pPrimMonVidMode->width << " x " 
			<< pPrimMonVidMode->height << " )" << std::endl;
	}
	else
	{
		// Make the screen windowed. (i.e. It's CURRENTLY full-screen)
		// NOTE: We aren't saving the "old" windowed size - you might want to do that...
		// HACK: Instead, we are taking the old size and mutiplying it by 75% 
		// (the thinking is: the full-screen switch always assumes we want the maximum
		//	resolution - see code above - but when we make that maximum size windowed,
		//  it's going to be 'too big' for the screen)
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );

		// Put the top, left corner 10% of the size of the full-screen
		int topCornerTop = (int)( (float)pPrimMonVidMode->height * 0.1f );
		int topCornerLeft = (int)( (float)pPrimMonVidMode->width * 0.1f );
		// Make the width and height 75% of the full-screen resolution
		int height = (int)( (float)pPrimMonVidMode->height * 0.75f );
		int width = (int)( (float)pPrimMonVidMode->width * 0.75f );

		glfwSetWindowMonitor( pTheWindow, NULL,		// This NULL makes the screen windowed
							  topCornerLeft, topCornerTop,				// left, top corner 
							  width, height, 
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now windowed at ( " 
			<< width << " x " << height << " )" 
			<< " and offset to ( "
			<< topCornerLeft << ", " << topCornerTop << " )"
			<< std::endl;
	}
	return;
}
























