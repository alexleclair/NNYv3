#include "GUIHandler.h"
#include "MainClient.h"

CGUIHandler::CGUIHandler(CEGUI::WindowManager *WindowManager) :
	mWindowManager(WindowManager), mRootWindow(0)
{
}

CGUIHandler::~CGUIHandler(void)
{
}

void CGUIHandler::Setup(void)
{
	//All windows have a parent/child structure.
	//Its common to name windows as follows: NNYv3/TabCtrl/Page2/ObjectTypeList
	//Create the root window (fullscreen)
	mRootWindow = mWindowManager->createWindow("DefaultWindow", "NNYv3");

	//Create the quit button
	CEGUI::Window *btnQuit = mWindowManager->createWindow("TaharezLook/Button", "NNYv3/QuitButton");
	btnQuit->setText("Quit");
	//CEGUI::UDim - Taken from tutorial:
	//When setting the size you must create a UDim object to tell it what size it should be.
	//The first parameter is the relative size of the object in relation to its parent.
	//The second parameter is the absolute size of the object (in pixels).
	//The important thing to realize is that you are only supposed to set one of the two parameters to UDim.
	//The other parameter must be 0. So in this case we have made a button which is 15% as wide as its parent and 5% as tall.
	//If we wanted to specify that it should be 20 pixels by 5 pixels,
	//we would do that by setting the second parameter in both of the UDim calls to be 20 and 5 respectively.
	btnQuit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
	btnQuit->setPosition(CEGUI::UVector2(CEGUI::UDim(0.85, 0), CEGUI::UDim(0,0)));

	//Set the function that handles the quit button
	btnQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CGUIHandler::QuitBtnClick, this));

	mRootWindow->addChildWindow(btnQuit);

	CEGUI::System::getSingleton().setGUISheet(mRootWindow);
}


bool CGUIHandler::QuitBtnClick(const CEGUI::EventArgs &e)
{
	CMainClient::getSingleton().SendNotify(CMainClient::Message_Quit);
	return true;
}

bool CGUIHandler::MessageBoxBtnOkClick(const CEGUI::EventArgs &e)
{
	CEGUI::WindowEventArgs& arg = *(CEGUI::WindowEventArgs*)&e;
	CEGUI::Window *MsgBox = arg.window->getParent();
	mWindowManager->destroyWindow(MsgBox);
	return true;
}

CEGUI::Window* CGUIHandler::MsgBox(std::string Text, std::string Title, std::string WindowName)
{
	CEGUI::Window *MsgBox = 0;
	try{
		MsgBox = mWindowManager->createWindow("TaharezLook/FrameWindow", WindowName);
	}catch( CEGUI::AlreadyExistsException ){
		return 0;
	}
	CEGUI::Window *Label = mWindowManager->createWindow("TaharezLook/StaticText");
	CEGUI::Window *ButtonOk = mWindowManager->createWindow("TaharezLook/Button");

	Label->setProperty("HorzFormatting", "WordWrapLeftAligned");
	Label->setArea(CEGUI::URect(CEGUI::UDim(0.03,0), CEGUI::UDim(0.15,0), CEGUI::UDim(0.97,0), CEGUI::UDim(0.80,0)));
	ButtonOk->setSize(CEGUI::UVector2(CEGUI::UDim(0.40,0), CEGUI::UDim(0.14,0)));
	ButtonOk->setPosition(CEGUI::UVector2(CEGUI::UDim(0.30,0), CEGUI::UDim(0.83,0)));
	MsgBox->setSize(CEGUI::UVector2(CEGUI::UDim(0.40,0), CEGUI::UDim(0.30,0)));
	MsgBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.30,0), CEGUI::UDim(0.35,0)));

	MsgBox->setText(Title);
	Label->setText(Text);
	ButtonOk->setText("Ok");
	ButtonOk->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CGUIHandler::MessageBoxBtnOkClick, this));

	MsgBox->addChildWindow(Label);
	MsgBox->addChildWindow(ButtonOk);
	mRootWindow->addChildWindow(MsgBox);

	return MsgBox;
}

//===================
// Login section
//===================
int	CGUIHandler::DisplayLoginScreen(void)
{
	//Load the window layout from file
	CEGUI::Window *LoginWindow = mWindowManager->loadWindowLayout("loginscreen.layout");
	//Set the password box to be masked
	CEGUI::Editbox *Password = static_cast<CEGUI::Editbox*>(mWindowManager->getWindow("LoginWindow/Password"));
	Password->setMaskCodePoint('*');
	Password->setTextMasked(true);
	//Subscribe the handler for the login and about button
	CEGUI::Window *LoginButton = 0;
	CEGUI::Window *AboutButton = 0;
	try{
		LoginButton = mWindowManager->getWindow("LoginWindow/ButtonLogin");
	}catch(CEGUI::UnknownObjectException){}
	try{
		AboutButton = mWindowManager->getWindow("LoginWindow/ButtonAbout");
	}catch(CEGUI::UnknownObjectException){}
	if( LoginButton ) LoginButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CGUIHandler::LoginBtnClick, this));
	if( AboutButton ) AboutButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CGUIHandler::AboutBtnClick, this));
	//Attach the window to the root window so it's visible
	mRootWindow->addChildWindow(LoginWindow);
	return 1;
}

bool CGUIHandler::LoginBtnClick(const CEGUI::EventArgs &e)
{
	CMainClient::getSingleton().SendNotify(CMainClient::Message_Login);
	return true;
}

bool CGUIHandler::AboutBtnClick(const CEGUI::EventArgs &e)
{
	MsgBox("NNYv3 stands for No Name Yet version 3.\nNNYv3 is an open source MMORPG client made mainly by nitrix and Tombana.", "About NNYv3", "NNYv3/Aboutbox");
	return true;
}