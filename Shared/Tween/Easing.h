#pragma once

//
//  easing.h
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.
//

namespace AHEasing
{
	typedef float(*AHEasingFunction)(float);

	// Linear interpolation (no easing)
	float LinearInterpolation(float p);

	// Quadratic easing; p^2
	float QuadraticIn(float p);
	float QuadraticOut(float p);
	float QuadraticInOut(float p);

	// Cubic easing; p^3
	float CubicIn(float p);
	float CubicOut(float p);
	float CubicInOut(float p);

	// Quartic easing; p^4
	float QuarticIn(float p);
	float QuarticOut(float p);
	float QuarticInOut(float p);

	// Quintic easing; p^5
	float QuinticIn(float p);
	float QuinticOut(float p);
	float QuinticInOut(float p);

	// Sine wave easing; sin(p * PI/2)
	float SineIn(float p);
	float SineOut(float p);
	float SineInOut(float p);

	// Circular easing; sqrt(1 - p^2)
	float CircularIn(float p);
	float CircularOut(float p);
	float CircularInOut(float p);

	// Exponential easing, base 2
	float ExponentialIn(float p);
	float ExponentialOut(float p);
	float ExponentialInOut(float p);

	// Exponentially-damped sine wave easing
	float ElasticIn(float p);
	float ElasticOut(float p);
	float ElasticInOut(float p);

	// Overshooting cubic easing; 
	float BackIn(float p);
	float BackOut(float p);
	float BackInOut(float p);

	// Exponentially-decaying bounce easing
	float BounceIn(float p);
	float BounceOut(float p);
	float BounceInOut(float p);
}