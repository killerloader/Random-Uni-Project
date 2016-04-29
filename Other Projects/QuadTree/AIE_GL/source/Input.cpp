#include "Input.h"
#include "glfw3.h"

// declare singleton variable
//------------------------------------------------------------
//------------------------------------------------------------
Input * Input::m_pInstance = nullptr;

//------------------------------------------------------------
//------------------------------------------------------------
Input::Input()
{
	auto KeyPressCallback = [](GLFWwindow *pWindow, int key, int scancode, int action, int mods)
	{
		if( action == GLFW_PRESS )
			Input::Instance()->OnKeyPressed( key );
		if( action == GLFW_RELEASE )
			Input::Instance()->OnKeyReleased( key );
		if( action == GLFW_REPEAT )
			Input::Instance()->OnKeyRepeate(key);
	};

	auto CharacterInputCallback = [](GLFWwindow *pWindow, unsigned int character)
	{
		Input::Instance()->OnCharInput(character);
	};

	auto MouseInputCallback = [](GLFWwindow *pWindow, int button, int action, int mods)
	{
		if( action == GLFW_PRESS)
			Input::Instance()->OnMousePressed( button );
		if( action == GLFW_RELEASE)
			Input::Instance()->OnMouseReleased( button );
	};

	auto MouseMoveCallback = [](GLFWwindow *pWindow, double mx, double my)
	{
		Input::Instance()->OnMouseMove( (int)mx, (int)my );
	};

	GLFWwindow *pWindow = glfwGetCurrentContext();
	glfwSetKeyCallback(pWindow, KeyPressCallback);
	glfwSetCharCallback(pWindow, CharacterInputCallback);
	glfwSetMouseButtonCallback(pWindow, MouseInputCallback);
	glfwSetCursorPosCallback(pWindow, MouseMoveCallback );

	m_mouseX = 0;
	m_mouseY = 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
Input::~Input()
{
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnKeyPressed(int keyID)
{
	m_keyStatus[keyID] = E_JUST_PRESSED;
	m_keysToUpdate.push_back( keyID );

	m_pressedKeys.push_back( keyID );
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnKeyReleased(int keyID)
{
	m_keyStatus[keyID] = E_JUST_RELEASED;
	m_keysToUpdate.push_back( keyID );
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnKeyRepeate(int key)
{
	m_pressedKeys.push_back(key);
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnCharInput(unsigned int character)
{
	m_pressedCharacters.push_back( character );
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnMouseMove(int newXPos, int newYPos)
{
	m_mouseX = newXPos;
	m_mouseY = newYPos;
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnMousePressed(int mbID)
{
	m_mouseState[mbID] = E_JUST_PRESSED;
	m_mouseToUpdate.push_back(mbID);
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::OnMouseReleased(int mbID)
{
	m_mouseState[mbID] = E_JUST_RELEASED;
	m_mouseToUpdate.push_back(mbID);
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::Update()
{
	for(unsigned int i=0; i<m_keysToUpdate.size(); i++)
	{
		m_keyStatus[ m_keysToUpdate[i] ] += 1;
	}

	for(unsigned int i=0; i<m_mouseToUpdate.size(); i++)
	{
		m_mouseState[ m_mouseToUpdate[i] ] += 1;
	}

	m_mouseToUpdate.clear();
	m_keysToUpdate.clear();

	m_pressedKeys.clear();
	m_pressedCharacters.clear();
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::IsKeyDown(int INPUT_KEY_ID)
{
	int keyState = m_keyStatus[INPUT_KEY_ID];
	return  keyState == E_JUST_PRESSED || keyState == E_DOWN;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::IsKeyUp(int INPUT_KEY_ID)
{
	int keyState = m_keyStatus[INPUT_KEY_ID];
	return  keyState == E_JUST_RELEASED || keyState == E_UP;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::WasKeyPressed(int INPUT_KEY_ID)
{
	int keyState = m_keyStatus[INPUT_KEY_ID];
	return  keyState == E_JUST_PRESSED;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::WasKeyReleased(int INPUT_KEY_ID)
{
	int keyState = m_keyStatus[INPUT_KEY_ID];
	return  keyState == E_JUST_RELEASED;
}

//------------------------------------------------------------
//------------------------------------------------------------
const std::vector<unsigned int> &Input::GetPressedKeys()
{
	return m_pressedKeys;
}

//------------------------------------------------------------
//------------------------------------------------------------
const std::vector<unsigned int> &Input::GetPressedCharacters()
{
	return m_pressedCharacters;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::IsMouseButtonDown(int INPUT_MOUSE_ID)
{
	int mbState = m_mouseState[INPUT_MOUSE_ID];
	return mbState == E_DOWN;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::IsMouseButtonUp(int INPUT_MOUSE_ID)
{
	int mbState = m_mouseState[INPUT_MOUSE_ID];
	return mbState == E_UP || mbState == E_JUST_RELEASED;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::WasMouseButtonPressed(int INPUT_MOUSE_ID)
{
	int mbState = m_mouseState[INPUT_MOUSE_ID];
	return mbState == E_JUST_PRESSED;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool Input::WasMouseButtonReleased(int INPUT_MOUSE_ID)
{
	int mbState = m_mouseState[INPUT_MOUSE_ID];
	return mbState == E_JUST_RELEASED;
}

//------------------------------------------------------------
//------------------------------------------------------------
int Input::GetMouseX()
{
	return m_mouseX;
}

//------------------------------------------------------------
//------------------------------------------------------------
int Input::GetMouseY()
{
	return m_mouseY;
}

//------------------------------------------------------------
//------------------------------------------------------------
void Input::GetMouseXY(int *x, int *y)
{
	if( x != NULL ) *x = m_mouseX;
	if( y != NULL ) *y = m_mouseY;
}