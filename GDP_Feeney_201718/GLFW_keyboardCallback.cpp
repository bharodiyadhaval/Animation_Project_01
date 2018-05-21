#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include <iostream>

#include "cAnimationState.h"

bool isShiftKeyDown( int mods, bool bByItself = true );
bool isCtrlKeyDown( int mods, bool bByItself = true );
bool isAltKeyDown( int mods, bool bByItself = true );
bool areAllModifierKeysUp(int mods);
bool areAnyModifierKeysDown(int mods);

extern std::string g_AnimationToPlay;

extern double g_CA_CountDown;// = 0.0f;

/*static*/ void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	// Fullscreen to windowed mode on the PRIMARY monitor (whatever that is)
	if ( isAltKeyDown(mods, true) && key == GLFW_KEY_ENTER )
	{
		if ( action == GLFW_PRESS )
		{
			::g_IsWindowFullScreen = !::g_IsWindowFullScreen;

			setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );

		}//if ( action == GLFW_PRESS )
	}//if ( isAltKeyDown(...


	cGameObject* pLeftTeapot = findObjectByFriendlyName(LEFTTEAPOTNAME, ::g_vecGameObjects);





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
	brun.totalTime = 1.0f;
	brun.frameStepTime = 0.01f;

	cAnimationState::sStateDetails bjump;
	bjump.name = "assets/modelsFBX/alien/jump.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	bjump.totalTime = 2.0f;
	bjump.frameStepTime = 0.01f;


	cGameObject* pSophie = findObjectByFriendlyName( "Sophie", ::g_vecGameObjects );
	int selectcharacter = 0;

	const float CAMERASPEED = 10.0f;

	const float CAM_ACCELL_THRUST = 100.0f;

	if ( isShiftKeyDown(mods, true)  )
	{
		switch (key)
		{
		case GLFW_KEY_1:
			
			::g_pLightManager->vecLights[0].attenuation.y *= 0.99f;	// less 1%
			break;
		case GLFW_KEY_2:
		
			::g_pLightManager->vecLights[0].attenuation.y *= 1.01f; // more 1%
			if (::g_pLightManager->vecLights[0].attenuation.y <= 0.0f)
			{
				::g_pLightManager->vecLights[0].attenuation.y = 0.001f;	// Some really tiny value
			}
			break;
		case GLFW_KEY_3:	// Quad
			::g_pLightManager->vecLights[0].attenuation.z *= 0.99f;	// less 1%
			break;
		case GLFW_KEY_4:	//  Quad
			::g_pLightManager->vecLights[0].attenuation.z *= 1.01f; // more 1%
			if (::g_pLightManager->vecLights[0].attenuation.z <= 0.0f)
			{
				::g_pLightManager->vecLights[0].attenuation.z = 0.001f;	// Some really tiny value
			}
			break;

		// Lights
		// CAMERA and lighting
		case GLFW_KEY_A:		// Left
			::g_pLightManager->vecLights[0].position.x -= CAMERASPEED;
			break;
		case GLFW_KEY_D:		// Right
			::g_pLightManager->vecLights[0].position.x += CAMERASPEED;
			break;
		case GLFW_KEY_W:		// Forward (along z)
		//	::g_pLightManager->vecLights[0].position.z += CAMERASPEED;
			if (action == GLFW_PRESS)
			{
				::SW = 1;


				::keyflag = 1;
				::keytime = glfwGetTime();

			}


			if (action == GLFW_RELEASE)
			{
				::boySW = 0;
				::alSW = 0;
				::SW = 0;
			}
			break;
		case GLFW_KEY_SPACE:

			if (action == GLFW_PRESS)
			{
				::SS = 1;


				::keyflag = 1;
				::keytime = glfwGetTime();

			}


			if (action == GLFW_RELEASE)
			{
				::boySS = 0;
				::alSS = 0;
				::SS = 0;
			}
			break;
			break;
		case GLFW_KEY_S:		// Backwards (along z)
			::g_pLightManager->vecLights[0].position.z -= CAMERASPEED;
			break;
		case GLFW_KEY_Q:		// "Down" (along y axis)
			::g_pLightManager->vecLights[0].position.y -= CAMERASPEED;
			break;
		case GLFW_KEY_E:		// "Up" (along y axis)
			::g_pLightManager->vecLights[0].position.y += CAMERASPEED;
			break;
		case GLFW_KEY_G:
			{
				float angle = ::g_pLightManager->vecLights[0].getLightParamSpotPrenumAngleOuter();
				::g_pLightManager->vecLights[0].setLightParamSpotPrenumAngleOuter(angle + 0.01f);
			}
			break;
		case GLFW_KEY_H:
			{
				float angle = ::g_pLightManager->vecLights[0].getLightParamSpotPrenumAngleOuter();
				::g_pLightManager->vecLights[0].setLightParamSpotPrenumAngleOuter(angle - 0.01f);
			}
			break;
		};//switch (key)
	}//if ( isShiftKeyDown(mods, true) )


	cAnimationState::sStateDetails awalk;
	awalk.name = "assets/modelsFBX/boy/walk.fbx";
	//walk.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(walk.name);
	awalk.totalTime = 0.7f;
	awalk.frameStepTime = 0.01f;

	

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





	if (areAllModifierKeysUp(mods)  )
	{
		switch (key)
		{
		case GLFW_KEY_1:
			::selectcharacter = 0;
			::charswitch = 1;
			
			std::cout << "Camera now in " << ::g_pTheCamera->getCameraModeString() << std::endl;;
			
			break;
		case GLFW_KEY_2:
			::selectcharacter = 1;
			::charswitch = 1;
			//::g_pTheCamera->setCameraMode(cCamera::MODE_FOLLOW);
			std::cout << "Camera now in " << ::g_pTheCamera->getCameraModeString() << std::endl;;
			break;
		case GLFW_KEY_3:
			//::g_pTheCamera->setCameraMode(cCamera::MODE_MANUAL);
			std::cout << "Camera now in " << ::g_pTheCamera->getCameraModeString() << std::endl;;
			break;

		case GLFW_KEY_A:	

			if (action == GLFW_PRESS)
			{
				::A = 1;
				::keyflag = 1;
					::keytime = glfwGetTime();
			}

			if (action == GLFW_RELEASE)
			{
				::boyA = 0;
				::alA = 0;
				::A = 0;
			}

			break;
		case GLFW_KEY_D:	

			if (action == GLFW_PRESS)
			{
				::D = 1;
				
					
					::keyflag = 1;
					::keytime = glfwGetTime();
				
			}


			if (action == GLFW_RELEASE)
			{
				::boyD = 0;
				::alD = 0;
				::D = 0;
			}
			break;

		case GLFW_KEY_W:


				if (action == GLFW_PRESS)
				{
					::W = 1;
					

					::keyflag = 1;
					::keytime = glfwGetTime();
				}
			
				if (action == GLFW_RELEASE)
				{
					::boyW = 0;
					::alW = 0;
					::W = 0;

				}
			
			
				
			break;
		case GLFW_KEY_S:		

		
			if (action == GLFW_PRESS)
			{
				::S = 1;


				::keyflag = 1;
				::keytime = glfwGetTime();

			}


			if (action == GLFW_RELEASE)
			{
				::boyR = 0;
				::alR = 0;
				::S = 0;
			}
			break;


			break;
		case GLFW_KEY_UP:		
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, 0.0f, +CAMERASPEED));
			break;
		case GLFW_KEY_DOWN:		
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, 0.0f, -CAMERASPEED));		
			break;
		case GLFW_KEY_RIGHT:		
			::g_pTheCamera->FlyCamLA->move(glm::vec3(+CAMERASPEED, 0.0f, 0.0f));
			break;
		case GLFW_KEY_LEFT:		
			::g_pTheCamera->FlyCamLA->move(glm::vec3(-CAMERASPEED, 0.0f, 0.0f));
			break;
		case GLFW_KEY_Q:		
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, -CAMERASPEED, 0.0f));		
			break;
		case GLFW_KEY_E:		
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, +CAMERASPEED, 0.0f));		
			break;

		case GLFW_KEY_5:
			break;
		case GLFW_KEY_6:
			break;
		case GLFW_KEY_7:
			break;
		case GLFW_KEY_8:
			break;
		case GLFW_KEY_9:
			break;
		case GLFW_KEY_0:
			::g_CA_CountDown = 0.5f;
			break;

		case GLFW_KEY_N:
			break;

		case GLFW_KEY_J:	// Left

			if (action == GLFW_PRESS)
			{
				::J = 1;


				::keyflag = 1;
				::keytime = glfwGetTime();

			}


			if (action == GLFW_RELEASE)
			{
				::boyJ = 0;
				::alJ = 0;
				::J = 0;
			}
			break;
		case GLFW_KEY_L:	// Right
			if ( action == GLFW_PRESS )
			{
				//pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX";
				//pSophie->pAniState->vecAnimationQueue.push_back( rightAnimation );
			}
			break;
		case GLFW_KEY_I:	// Back;
//			pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx";
			break;
		case GLFW_KEY_K:	// Forward:
//			pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Forward(FBX2013).FBX";
			break;
		case GLFW_KEY_M:	// Walk
//			pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX";
			break;
		case GLFW_KEY_SPACE:	// Jump

			if (action == GLFW_PRESS)
			{
				::SBAR = 1;


				::keyflag = 1;
				::keytime = glfwGetTime();

			}


			if (action == GLFW_RELEASE)
			{
				::boyS = 0;
				::alS = 0;
				::SBAR = 0;
			}
			
			break;
		default:
			/*::g_vecGameObjects[::selectcharacter]->pAniState->defaultAnimation.name = "assets/modelsFBX/working/Idling.fbx";
			::g_vecGameObjects[::selectcharacter]->pAniState->defaultAnimation.totalTime = 0.5;*/
			
			break;	

		}//switch
	}//if (areAllModifierKeysUp(mods))






	// HACK: print output to the console
//	std::cout << "Light[0] linear atten: "
//		<< ::g_pLightManager->vecLights[0].attenuation.y << ", "
//		<< ::g_pLightManager->vecLights[0].attenuation.z << std::endl;
	return;
}



// Helper functions
bool isShiftKeyDown( int mods, bool bByItself /*=true*/ )
{
	if ( bByItself )
	{	// shift by itself?
		if ( mods == GLFW_MOD_SHIFT )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if ( ( mods & GLFW_MOD_SHIFT ) == GLFW_MOD_SHIFT )	{	return true;	}
		else												{	return false;	}
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isCtrlKeyDown( int mods, bool bByItself /*=true*/ )
{
	if ( bByItself )
	{	// shift by itself?
		if ( mods == GLFW_MOD_CONTROL )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if ( ( mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)	{	return true;	}
		else												{	return false;	}
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isAltKeyDown( int mods, bool bByItself /*=true*/ )
{
	if (bByItself)
	{	// shift by itself?
		if (mods == GLFW_MOD_ALT) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool areAllModifierKeysUp(int mods)
{
	if ( isShiftKeyDown(mods) )	{	return false;	}
	if ( isCtrlKeyDown(mods) ) 	{	return false;	}
	if ( isAltKeyDown(mods) )	{	return false;	}

	// All of them are up
	return true;
}//areAllModifierKeysUp()

bool areAnyModifierKeysDown(int mods)
{
	if ( isShiftKeyDown(mods) )		{	return true;	}
	if ( isCtrlKeyDown(mods) )		{	return true;	}
	if ( isAltKeyDown(mods) )		{	return true;	}
	// None of them are down
	return false;
}