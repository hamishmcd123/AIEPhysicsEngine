#pragma once
#include "Graphics.h"
#include "Maths.h"
#include "LineRenderer.h"
#include "ShaderProgram.h"
#include "Application.h"
#include "Key.h"
#include "Button.h"
#include "Axis.h"

class ApplicationHarness
{
private:
	static SDL_Window* window;
	SDL_GLContext glContext;
	static SDL_Gamepad* gamepad;

	Application* app = nullptr;

	ShaderProgram simpleShader;
	float aspectRatio;

	LineRenderer grid;
	LineRenderer lines;

	unsigned int fixedFramerate;
	bool showGrid;

	bool tryingToClose = false;

	void PopulateCameraTransform(float* matrix) const;


public:
	ApplicationHarness(Application* appInit);
	~ApplicationHarness();

	ApplicationHarness(const ApplicationHarness& other) = delete;
	ApplicationHarness& operator=(const ApplicationHarness& other) = delete;
	
	void Run();

	void Update(float delta);
	void Render();

	bool IsRunning() const;

	//1 for left, 2 for right, 3 for middle. Other button codes will get sent for other buttons but I'm not sure how they'd map to a particular mouse.
	void OnMouseClick(int mouseButton);
	void OnMouseRelease(int mouseButton);
	void OnMouseScroll(bool positive);

	void OnKeyPress(Key key);
	void OnKeyRelease(Key key);

	void OnButtonPress(Button button, int gamepadNum);
	void OnButtonRelease(Button button, int gamepadNum);

	static bool IsKeyDown(Key key);
	static bool IsButtonDown(Button button, int gamepadNum = 0);
	static float GetInputAxis(Key negativeKey, Key positiveKey);
	static float GetInputAxis(Button negativeButton, Button positiveButton, int gamepadNum = 0);
	static float GetInputAxis(Axis axis, int gamepadNum = 0);
	static float GetInputAxis(Axis negativeAxis, Axis positiveAxis, int gamepadNum = 0);
	static Vec2 GetInputDirection(Button upButton, Button leftButton, Button downButton, Button rightButton, bool normalise = true, int gamepadNum = 0);
	static Vec2 GetInputDirection(Axis horizontalAxis, Axis verticalAxis, bool flipVertical = false, int gamepadNum = 0);
	static Vec2 GetInputDirection(Key upKey, Key leftKey, Key downKey, Key rightKey, bool normalise = true);


	void SetPixelDensityScale(float scale);

	void HandleEvent(const SDL_Event& event);
};
