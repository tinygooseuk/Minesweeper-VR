//
//  easing.c
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.
//

#include "Easing.h"
#include "UnrealMath.h"

namespace AHEasing
{
	// Modeled after the line y = x
	float LinearInterpolation(float p)
	{
		return p;
	}

	// Modeled after the parabola y = x^2
	float QuadraticIn(float p)
	{
		return p * p;
	}

	// Modeled after the parabola y = -x^2 + 2x
	float QuadraticOut(float p)
	{
		return -(p * (p - 2));
	}

	// Modeled after the piecewise quadratic
	// y = (1/2)((2x)^2)             ; [0, 0.5)
	// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
	float QuadraticInOut(float p)
	{
		if (p < 0.5)
		{
			return 2 * p * p;
		}
		else
		{
			return (-2 * p * p) + (4 * p) - 1;
		}
	}

	// Modeled after the cubic y = x^3
	float CubicIn(float p)
	{
		return p * p * p;
	}

	// Modeled after the cubic y = (x - 1)^3 + 1
	float CubicOut(float p)
	{
		float f = (p - 1);
		return f * f * f + 1;
	}

	// Modeled after the piecewise cubic
	// y = (1/2)((2x)^3)       ; [0, 0.5)
	// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
	float CubicInOut(float p)
	{
		if (p < 0.5)
		{
			return 4 * p * p * p;
		}
		else
		{
			float f = ((2 * p) - 2);
			return 0.5 * f * f * f + 1;
		}
	}

	// Modeled after the quartic x^4
	float QuarticIn(float p)
	{
		return p * p * p * p;
	}

	// Modeled after the quartic y = 1 - (x - 1)^4
	float QuarticOut(float p)
	{
		float f = (p - 1);
		return f * f * f * (1 - p) + 1;
	}

	// Modeled after the piecewise quartic
	// y = (1/2)((2x)^4)        ; [0, 0.5)
	// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
	float QuarticInOut(float p)
	{
		if (p < 0.5)
		{
			return 8 * p * p * p * p;
		}
		else
		{
			float f = (p - 1);
			return -8 * f * f * f * f + 1;
		}
	}

	// Modeled after the quintic y = x^5
	float QuinticIn(float p)
	{
		return p * p * p * p * p;
	}

	// Modeled after the quintic y = (x - 1)^5 + 1
	float QuinticOut(float p)
	{
		float f = (p - 1);
		return f * f * f * f * f + 1;
	}

	// Modeled after the piecewise quintic
	// y = (1/2)((2x)^5)       ; [0, 0.5)
	// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
	float QuinticInOut(float p)
	{
		if (p < 0.5)
		{
			return 16 * p * p * p * p * p;
		}
		else
		{
			float f = ((2 * p) - 2);
			return  0.5 * f * f * f * f * f + 1;
		}
	}

	// Modeled after quarter-cycle of sine wave
	float SineIn(float p)
	{
		return FMath::Sin((p - 1) * HALF_PI) + 1;
	}

	// Modeled after quarter-cycle of sine wave (different phase)
	float SineOut(float p)
	{
		return FMath::Sin(p * HALF_PI);
	}

	// Modeled after half sine wave
	float SineInOut(float p)
	{
		return 0.5 * (1 - cos(p * PI));
	}

	// Modeled after shifted quadrant IV of unit circle
	float CircularIn(float p)
	{
		return 1 - sqrt(1 - (p * p));
	}

	// Modeled after shifted quadrant II of unit circle
	float CircularOut(float p)
	{
		return sqrt((2 - p) * p);
	}

	// Modeled after the piecewise circular function
	// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
	// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
	float CircularInOut(float p)
	{
		if (p < 0.5)
		{
			return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
		}
		else
		{
			return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
		}
	}

	// Modeled after the exponential function y = 2^(10(x - 1))
	float ExponentialIn(float p)
	{
		return (p == 0.0) ? p : pow(2, 10 * (p - 1));
	}

	// Modeled after the exponential function y = -2^(-10x) + 1
	float ExponentialOut(float p)
	{
		return (p == 1.0) ? p : 1 - pow(2, -10 * p);
	}

	// Modeled after the piecewise exponential
	// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
	// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
	float ExponentialInOut(float p)
	{
		if (p == 0.0 || p == 1.0) return p;

		if (p < 0.5)
		{
			return 0.5 * pow(2, (20 * p) - 10);
		}
		else
		{
			return -0.5 * pow(2, (-20 * p) + 10) + 1;
		}
	}

	// Modeled after the damped sine wave y = FMath::Sin(13pi/2*x)*pow(2, 10 * (x - 1))
	float ElasticIn(float p)
	{
		return FMath::Sin(13 * HALF_PI * p) * pow(2, 10 * (p - 1));
	}

	// Modeled after the damped sine wave y = FMath::Sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
	float ElasticOut(float p)
	{
		return FMath::Sin(-13 * HALF_PI * (p + 1)) * pow(2, -10 * p) + 1;
	}

	// Modeled after the piecewise exponentially-damped sine wave:
	// y = (1/2)*FMath::Sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
	// y = (1/2)*(FMath::Sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
	float ElasticInOut(float p)
	{
		if (p < 0.5)
		{
			return 0.5 * FMath::Sin(13 * HALF_PI * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
		}
		else
		{
			return 0.5 * (FMath::Sin(-13 * HALF_PI * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
		}
	}

	// Modeled after the overshooting cubic y = x^3-x*FMath::Sin(x*pi)
	float BackIn(float p)
	{
		return p * p * p - p * FMath::Sin(p * PI);
	}

	// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*FMath::Sin((1-x)*pi))
	float BackOut(float p)
	{
		float f = (1 - p);
		return 1 - (f * f * f - f * FMath::Sin(f * PI));
	}

	// Modeled after the piecewise overshooting cubic function:
	// y = (1/2)*((2x)^3-(2x)*FMath::Sin(2*x*pi))           ; [0, 0.5)
	// y = (1/2)*(1-((1-x)^3-(1-x)*FMath::Sin((1-x)*pi))+1) ; [0.5, 1]
	float BackInOut(float p)
	{
		if (p < 0.5)
		{
			float f = 2 * p;
			return 0.5 * (f * f * f - f * FMath::Sin(f * PI));
		}
		else
		{
			float f = (1 - (2 * p - 1));
			return 0.5 * (1 - (f * f * f - f * FMath::Sin(f * PI))) + 0.5;
		}
	}

	float BounceIn(float p)
	{
		return 1 - BounceOut(1 - p);
	}

	float BounceOut(float p)
	{
		if (p < 4 / 11.0)
		{
			return (121 * p * p) / 16.0;
		}
		else if (p < 8 / 11.0)
		{
			return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
		}
		else if (p < 9 / 10.0)
		{
			return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
		}
		else
		{
			return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
		}
	}

	float BounceInOut(float p)
	{
		if (p < 0.5)
		{
			return 0.5 * BounceIn(p * 2);
		}
		else
		{
			return 0.5 * BounceOut(p * 2 - 1) + 0.5;
		}
	}
}