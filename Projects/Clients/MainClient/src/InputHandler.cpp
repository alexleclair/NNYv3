#include "InputHandler.h"
#include "MainClient.h"
#include "WorldManager.h"
#include "Camera.h"

CInputHandler::CInputHandler(CWorldManager& World, CCamera& Camera, Ogre::RenderWindow *window, Ogre::SceneManager *SceneMgr, Ogre::RaySceneQuery *RaySceneQuery) :
	mWorld(World), mCamera(Camera), mWindow(window), mSceneMgr(SceneMgr), mRaySceneQuery(RaySceneQuery),
		mInputManager(0), mKeyboard(0), mMouse(0), mGUISystem(CEGUI::System::getSingletonPtr())
{
	size_t windowHnd = 0;
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	
	std::ostringstream windowHndStr;
	windowHndStr << windowHnd;
	
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	
	mInputManager = OIS::InputManager::createInputSystem( pl );

	//Create the keyboard and mouse device in buffered mode
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);

	//Register this class as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
}

CInputHandler::~CInputHandler(void)
{
	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	//This function destroys the input objects
	windowClosed(mWindow);
}

bool CInputHandler::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mKeyboard->capture();
	mMouse->capture();
	return true;
}

bool CInputHandler::keyPressed(const OIS::KeyEvent &arg)
{
	if( mGUISystem ){
		mGUISystem->injectKeyDown(arg.key);
		mGUISystem->injectChar(arg.text);
	}

	switch (arg.key){
		case OIS::KC_ESCAPE:
			CMainClient::getSingleton().SendThreadMessage(Message_Quit);
            break;
		case OIS::KC_GRAVE:
			CUIMain::getSingleton().ToggleConsole();
			break;
		default:
			break;
	}

	return true;
}

bool CInputHandler::keyReleased(const OIS::KeyEvent &arg)
{
	if( mGUISystem ) mGUISystem->injectKeyUp(arg.key);
	return true;
}

bool CInputHandler::mouseMoved(const OIS::MouseEvent &arg)
{
	if( mGUISystem ){
		mGUISystem->injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
		// Scroll wheel.
		if (arg.state.Z.rel) mGUISystem->injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	}
	//Zoom using scroll wheel
	if( arg.state.Z.rel ){
		mCamera.mCamZoomSpeed -= 1.2 * Ogre::Real(arg.state.Z.rel);
	}
	//Look around when right mouse button is down (regardless of wether the mouse is on a window)
	if (arg.state.buttonDown(OIS::MB_Right))
	{
		mCamera.mCamYawSpeed -= 1.2 * Ogre::Real(arg.state.X.rel);
		mCamera.mCamPitchSpeed -= 1.2 * Ogre::Real(arg.state.Y.rel);
	}
	return true;
}

//CEGUI and OIS do not use the same constants for mouse buttons so we have to convert them
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
		case OIS::MB_Left:		return CEGUI::LeftButton;
		case OIS::MB_Right:		return CEGUI::RightButton;
		case OIS::MB_Middle:	return CEGUI::MiddleButton;
		case OIS::MB_Button3:	return CEGUI::X1Button;
		default:				return CEGUI::LeftButton;
    }
}

bool CInputHandler::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	bool ProcessedByGUI = false;
	if( mGUISystem ) ProcessedByGUI = mGUISystem->injectMouseButtonDown(convertButton(id));
	if( ProcessedByGUI ) return true;

	if( id == OIS::MB_Left ){
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getDisplayIndependantPosition();
		Ogre::Ray ray(mCamera.GetCamera()->getCameraToViewportRay(mousePos.d_x, mousePos.d_y));
		mRaySceneQuery->setRay(ray);
		Ogre::RaySceneQueryResult& qryResult = mRaySceneQuery->execute();
		for( Ogre::RaySceneQueryResult::iterator it = qryResult.begin(); it != qryResult.end(); ++it ){
			//TODO: Test whether it is an entity or the ground.
			Ogre::Vector3 Collision = ray.getPoint(it->distance);
			if( mWorld.LocalPlayer ) mWorld.LocalPlayer->SetSingleDestination(Collision);
			break;
		}
	}
	return true;
}

bool CInputHandler::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if( mGUISystem ) mGUISystem->injectMouseButtonUp(convertButton(id));
	return true;
}