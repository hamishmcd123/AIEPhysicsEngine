#include "ApplicationHarness.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "Maths.h"
#include "Utilities.h"
#include <iostream>

SDL_Window* ApplicationHarness::window = nullptr;
SDL_Gamepad* ApplicationHarness::gamepad = nullptr;

ApplicationHarness::ApplicationHarness(Application* appInit) : app(appInit)
{
	if (window != nullptr)
	{
		std::cout << "Failed to create application harness - static window pointer already set!\n";
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	AppInfo appInfo = app->GetAppInfo();
	window = SDL_CreateWindow(
		appInfo.appName,
		appInfo.horizontalResolution,
		appInfo.verticalResolution,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		std::cout << "Failed to create application harness - SDL initialisation failed.\n";
		SDL_Quit();
		return;
	}
	if (app == nullptr)
	{
		std::cout << "Failed to create application harness - passed null application pointer.\n";
		return;
	}
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)
	{
		//Context creation failed for some reason.
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}

	fixedFramerate = appInfo.fixedFramerate;

	//This is where GLAD gets set up. After this point we can use openGL functions.
	if (!gladLoadGL())
	{
		return;
	}
	SDL_GL_SetSwapInterval(1);

	float pixelDensityScale = SDL_GetWindowDisplayScale(window);

	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init();
	ImGui::GetIO().FontGlobalScale = pixelDensityScale;

	simpleShader = ShaderProgram("Shaders/Simple.vsd", "Shaders/Simple.fsd");
	simpleShader.UseShader();

	lines.Initialise();
	app->lines = &lines;

	glClearColor(
		appInfo.backgroundColour.r,
		appInfo.backgroundColour.g,
		appInfo.backgroundColour.b, 1);

	glLineWidth(appInfo.lineWidth * pixelDensityScale);

	grid.Initialise();
	
	float gridPosExtreme = appInfo.grid.extent * appInfo.grid.unit;
	float gridUnit = appInfo.grid.unit;
	int gridExtent = (int)appInfo.grid.extent;
	for (int i = -gridExtent; i <= gridExtent; i++)
	{
		
		Colour colour = (i == 0) ? appInfo.grid.mainAxesColour : appInfo.grid.linesColour;
		grid.DrawLineSegment({ i * gridUnit, -gridPosExtreme }, { i * gridUnit, gridPosExtreme }, colour);
		grid.DrawLineSegment({ -gridPosExtreme, i * gridUnit }, { gridPosExtreme, i * gridUnit }, colour);
	}
	if (appInfo.grid.showBasisLines)
	{
		grid.DrawLineSegment(Vec2(), Vec2(appInfo.grid.basisLineLength, 0), appInfo.grid.positiveXLineColour);
		grid.DrawLineSegment(Vec2(), Vec2(0, appInfo.grid.basisLineLength), appInfo.grid.positiveYLineColour);
	}
	grid.Compile();

	showGrid = appInfo.grid.show;

	app->Initialise();
}

ApplicationHarness::~ApplicationHarness()
{
	delete app;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	window = nullptr;
}

void ApplicationHarness::Run()
{
	double fixedDelta = 1.0 / fixedFramerate;
	double currentTime = SDL_GetTicksNS() / 1000000000.0;
	double accumulator = 0.0;

	while (IsRunning())
	{

		double newTime = SDL_GetTicksNS() / 1000000000.0;
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= fixedDelta)
		{
			Update((float)fixedDelta);
			accumulator -= fixedDelta;
		}

		if (IsRunning())
		{
			Render();
		}
	}
}

void ApplicationHarness::Update(float delta)
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	aspectRatio = width / (float)height;

	float cursorX, cursorY;
	SDL_GetMouseState(&cursorX, &cursorY);
	
	//Because we know the camera is always orthographic (and axis-aligned) we can
	//do what would normally be a matrix inverse operation more easily with some remaps.
	Vec2 cameraCentre = app->GetCameraPos();
	float cameraHeight = app->GetCameraHeight();
	float cameraWidth = cameraHeight * aspectRatio;
	float cameraLeft = cameraCentre.x - cameraWidth / 2.0f;
	float cameraRight = cameraCentre.x + cameraWidth / 2.0f;
	app->cursorPos.x = Remap((float)cursorX, 0, (float)width, cameraLeft, cameraRight);
	float cameraTop = cameraCentre.y + cameraHeight / 2.0f;
	float cameraBottom = cameraCentre.y - cameraHeight / 2.0f;
	app->cursorPos.y = Remap((float)cursorY, 0, (float)height, cameraTop, cameraBottom);

	

	app->appTime = SDL_GetTicksNS() / 1000000000.0f;
	lines.Clear();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	if (!app->GetAppInfo().camera.disable)
	{
		app->MoveCameraScaled(GetInputDirection(Key::W, Key::A, Key::S, Key::D) * delta * app->GetAppInfo().camera.cameraSpeed);
	}
	lines.UpdateWithCameraInfo(cameraCentre, cameraHeight, aspectRatio);

	app->Update(delta);

	lines.Compile();

	ImGui::Render();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_context = SDL_GL_GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		SDL_GL_MakeCurrent(backup_window, backup_context);
	}
}

void ApplicationHarness::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	float orthoMat[16];
	PopulateCameraTransform(orthoMat);
	simpleShader.SetMat4Uniform("vpMatrix", orthoMat);
	if (showGrid) grid.Draw();
	lines.Draw();


	//Render tends to get called once or twice before Update
	//gets called, so we need to make sure this info exists.
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData) ImGui_ImplOpenGL3_RenderDrawData(drawData);

	SDL_GL_SwapWindow(window);
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
		HandleEvent(event);
	}
}

bool ApplicationHarness::IsRunning() const
{
	return !tryingToClose;
}

void ApplicationHarness::OnMouseClick(int mouseButton)
{
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		switch (mouseButton)
		{
		case SDL_BUTTON_LEFT:
			app->OnLeftClick();
			break;
		case SDL_BUTTON_RIGHT:
			app->OnRightClick();
			break;
		case SDL_BUTTON_MIDDLE:
			app->OnMiddleClick();
		}
	}
}

void ApplicationHarness::OnMouseRelease(int mouseButton)
{
	switch (mouseButton)
	{
	case SDL_BUTTON_LEFT:
		app->OnLeftRelease();
		break;
	case SDL_BUTTON_RIGHT:
		app->OnRightRelease();
		break;
	case SDL_BUTTON_MIDDLE:
		app->OnMiddleRelease();
	}
}

void ApplicationHarness::OnMouseScroll(bool positive)
{
	if (!app->GetAppInfo().camera.disable)
	{
		float zoom = app->GetAppInfo().camera.zoomFactor;
		app->ScaleCameraHeight(positive ? 1.0f / zoom : zoom);
	}
	app->OnMouseScroll(positive);
}

void ApplicationHarness::OnKeyPress(Key key)
{
	app->OnKeyPress(key);
}
void ApplicationHarness::OnKeyRelease(Key key)
{
	app->OnKeyRelease(key);
}

void ApplicationHarness::OnButtonPress(Button button, int gamepadNum)
{
	app->OnButtonPress(button, gamepadNum);
}
void ApplicationHarness::OnButtonRelease(Button button, int gamepadNum)
{
	app->OnButtonRelease(button, gamepadNum);
}

bool ApplicationHarness::IsKeyDown(Key key)
{
	if (!ImGui::GetIO().WantCaptureKeyboard)
	{
		return SDL_GetKeyboardState(nullptr)[(int)key];
	}
	else
	{
		return false;
	}
}

bool ApplicationHarness::IsButtonDown(Button button, int gamepadNum)
{
	if (gamepad)
	{
		return SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)button);
	}
	return false;
}

float ApplicationHarness::GetInputAxis(Key negativeKey, Key positiveKey)
{
	float negativeValue = IsKeyDown(negativeKey) ? -1.0f : 0.0f;
	float positiveValue = IsKeyDown(positiveKey) ? 1.0f : 0.0f;
	return negativeValue + positiveValue;
}

float ApplicationHarness::GetInputAxis(Button negativeButton, Button positiveButton, int gamepadNum)
{
	float negativeValue = IsButtonDown(negativeButton) ? -1.0f : 0.0f;
	float positiveValue = IsButtonDown(positiveButton) ? 1.0f : 0.0f;
	return negativeValue + positiveValue;
}

float ApplicationHarness::GetInputAxis(Axis axis, int gamepadNum)
{
	if (gamepad)
	{
		float axisVal = Remap((float)SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)axis), -32768.0f, 32767.0f, -1.0f, 1.0f);

		return axisVal;
	}
	return 0.0f;
}

float ApplicationHarness::GetInputAxis(Axis negativeAxis, Axis positiveAxis, int gamepadNum)
{
	return GetInputAxis(positiveAxis, gamepadNum) - GetInputAxis(negativeAxis, gamepadNum);
}

Vec2 ApplicationHarness::GetInputDirection(Button upButton, Button leftButton, Button downButton, Button rightButton, bool normalise, int gamepadNum)
{
	Vec2 output(GetInputAxis(leftButton, rightButton), GetInputAxis(downButton, upButton));
	if (normalise && (output.x != 0.0f || output.y != 0.0f))
	{
		output.Normalise();
	}
	return output;
}

Vec2 ApplicationHarness::GetInputDirection(Axis horizontalAxis, Axis verticalAxis, bool flipVertical, int gamepadNum)
{
	return Vec2(GetInputAxis(horizontalAxis, gamepadNum), GetInputAxis(verticalAxis, gamepadNum) * (flipVertical ? -1.0f : 1.0f));
}

Vec2 ApplicationHarness::GetInputDirection(Key upKey, Key leftKey, Key downKey, Key rightKey, bool normalise)
{
	Vec2 output(GetInputAxis(leftKey, rightKey), GetInputAxis(downKey, upKey));
	if (normalise && (output.x != 0.0f || output.y != 0.0f))
	{
		output.Normalise();
	}
	return output;
}

void ApplicationHarness::SetPixelDensityScale(float scale)
{
	glLineWidth(app->GetAppInfo().lineWidth * scale);
	ImGui::GetIO().FontGlobalScale = scale;
}

void ApplicationHarness::PopulateCameraTransform(float* matrix) const
{
	//Mildly hacky creation of an orthographic matrix (near/far clip at -1 to 1).
	//Not really worth setting up a Mat4 class just for this.
	memset(matrix, 0, sizeof(float) * 16);

	float cameraHeight = app->GetCameraHeight();
	float cameraWidth = aspectRatio * cameraHeight;
	Vec2 cameraCentre = app->GetCameraPos();
	matrix[0] = 2.0f / cameraWidth;
	matrix[5] = 2.0f / cameraHeight;
	matrix[10] = -1.0f;
	matrix[12] = -2.0f * cameraCentre.x / cameraWidth;
	matrix[13] = -2.0f * cameraCentre.y / cameraHeight;
	matrix[15] = 1.0f;
}

void ApplicationHarness::HandleEvent(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_EVENT_QUIT:
		tryingToClose = true;
		break;
	case SDL_EVENT_KEY_DOWN:
		if (!event.key.repeat) OnKeyPress((Key)event.key.scancode);
		break;
	case SDL_EVENT_KEY_UP:
		OnKeyRelease((Key)event.key.scancode);
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		if (!ImGui::GetIO().WantCaptureMouse)
		{
			OnMouseClick(event.button.button);
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				app->leftMouseDown = true;
			}
			else if (event.button.button == SDL_BUTTON_RIGHT)
			{
				app->rightMouseDown = true;
			}
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		OnMouseRelease(event.button.button);
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			app->leftMouseDown = false;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			app->rightMouseDown = false;
		}
		break;
	case SDL_EVENT_MOUSE_WHEEL:
		if (event.wheel.integer_y != 0)
		{
			OnMouseScroll(event.wheel.integer_y > 0);
		}
		break;
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		OnButtonPress((Button)event.gbutton.button, 0);
		//NOTE: Haven't implemented multiple gamepads properly
		break;
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
		OnButtonRelease((Button)event.gbutton.button, 0);
		break;
	case SDL_EVENT_GAMEPAD_ADDED:
		if (!gamepad)
		{
			gamepad = SDL_OpenGamepad(event.gdevice.which);
		}
		break;
	case SDL_EVENT_GAMEPAD_REMOVED:
		if (gamepad && SDL_GetGamepadID(gamepad) == event.gdevice.which)
		{
			SDL_CloseGamepad(gamepad);
			gamepad = nullptr;
		}
		break;
	case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
		SetPixelDensityScale(SDL_GetWindowDisplayScale(window));
		break;
	case SDL_EVENT_WINDOW_RESIZED:
		int width, height; 
		SDL_GetWindowSizeInPixels(window, &width, &height);
		glViewport(0, 0, width, height);
		break;
	};
}
