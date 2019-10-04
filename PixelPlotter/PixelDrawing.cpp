/*******************************************
	PixelDrawing.cpp

	2D pixel based drawing functions
********************************************/

#include "stdafx.h"            // Precompiled headers for Visual Studio
#include "PixelPlotterForm.h"  // Class header file
#include <math.h>              // Need standard math functions for drawing circles

// All application definitions are contained in a 'namespace'. Namespaces primarily protect against
// variable name clashes when using many libraries together, they also promote good encapsulation
// Here the namespace is needed for the main drawing application - you can safely ignore it
namespace PixelPlotter
{

	////////////////////////////////////
	// Instructions
	////////////////////////////////////
	//
	// The functions below are initially empty, you need to implement them for the lab exercise.
	// Note that they are all member functions of the class PixelPlotterForm, so they begin with 
	// "PixelPlotterForm::". This does not affect how you write the functions
	//
	// The following function has already been declared:
	//     SetViewportPixel( int X, int Y, Color PixelColour );
	// To write the functions you only need to call this function once for each pixel in your shape
	//
	// Where necessary, use the definition of PI below
	// Also note that <math.h> has already been included

	const float PI = 3.14159f;


	// Draw a rectangle with top-left at (X, Y) with given width, height and colour
	void PixelPlotterForm::DrawRectangle(int X, int Y, int Width, int Height, Color PixelColour)
	{
		// Fill in the correct code here
		for (int i = 0; i < Width; i++)
		{
			SetViewportPixel(X + i, Y, PixelColour);
			SetViewportPixel(X + i, Y + Height - 1, PixelColour);
		}

		for (int i = 0; i < Height; i++)
		{
			SetViewportPixel(X, Y + i, PixelColour);
			SetViewportPixel(X + Width - 1, Y + i, PixelColour);
		}
	}


	// Draw a line from (X1, Y1) to (X2, Y2) in given colour
	void PixelPlotterForm::DrawLine(int X1, int Y1, int X2, int Y2, Color PixelColour)
	{
		// Fill in the correct code here

		int a = abs(X2 - X1);
		int b = abs(Y2 - Y1);
		int MaxDist = a - b;

		float XStep = (X2 - X1);
		float YStep = (Y2 - Y1);

		XStep /= MaxDist;
		YStep /= MaxDist;

		float X = X1 + 0.5;
		float Y = Y1 + 0.5;

		while (MaxDist >= 0)
		{
			SetViewportPixel(X, Y, PixelColour);
			X += XStep;
			Y += YStep;
			MaxDist--;
		}
	}


	// Draw a regular polygon with the given number of sides. The centre of the polygon will be at
	// (X, Y) and the points are on a circle of radius R
	void PixelPlotterForm::DrawPolygon(int Sides, int X, int Y, int R, Color PixelColour)
	{
		// Fill in the correct code here
	}


	// Draw a regular star with the given number of points. The centre of the star will be at (X, Y)
	// and it the points are on a circle of radius R. The number of points will always be odd.
	void PixelPlotterForm::DrawStar(int Sides, int X, int Y, int R, Color PixelColour)
	{
		// Fill in the correct code here
	}


	// Draw a circle with centre (X, Y) and radiuses RX (horizontal) and RY (vertical)
	void PixelPlotterForm::DrawCircle(int X, int Y, int R, Color PixelColour)
	{
		// Fill in the correct code here
	}


} // end namespace PixelPlotter