#pragma once

#include <SDL3/SDL.h>

enum class Axis
{
	LeftStickVertical = SDL_GAMEPAD_AXIS_LEFTY,
	LeftStickHorizontal = SDL_GAMEPAD_AXIS_LEFTX,
	RightStickVertical = SDL_GAMEPAD_AXIS_RIGHTY,
	RightStickHorizontal = SDL_GAMEPAD_AXIS_RIGHTX,

	LeftTrigger = SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
	RightTrigger = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,

	TriggersCombined //Special case
};