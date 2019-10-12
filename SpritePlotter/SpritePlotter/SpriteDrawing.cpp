/*******************************************
	SpriteDrawing.cpp

	2D bitmap and sprite drawing functions
********************************************/

#include "stdafx.h"             // Precompiled headers for Visual Studio
#include "SpritePlotterForm.h"  // Class header file

// All application definitions are contained in a 'namespace'. Namespaces primarily protect against
// variable name clashes when using many libraries together, they also promote good encapsulation
namespace SpritePlotter         
{

////////////////////////////////////
// Instructions
////////////////////////////////////
//
//	Below are several methods of the SpritePlotterForm class that draw bitmaps and sprites. All the
//	methods are variations of the same process - draw a bitmap at the given coordinates in the
//	viewport. Only the drawing style varies. The methods are initially empty - you need to implement
//	them for the lab exercise.
//
//	The following member functions are already provided to access *viewport* pixels:
//
//		// Set the colour of the given pixel in the viewport (same as DrawPixel from last week)
//		void SetViewportPixel( int X, int Y, Color colour )
//
//		// Get the colour of the given pixel in the viewport
//		Color GetViewportPixel( int X, int Y )
//
// To access the *bitmap* pixels you must use member functions from the 'SrcBitmap' parameter.
// This is a Microsoft .NET 'Bitmap' class, you need to look up the documentation on this class
// Unfortunately documentation in the Visual Studio 2010 is terrible. So go to msdn.microsoft.com
// and search for "Bitmap Class" to get the "Bitmap Class (System.Drawing)" documentation page
// Here you will find the properties (variables) and methods (function) that you can use. Scroll
// down to see example code - ensure you select the C++ tab. 
//
// Later in the exercise you need to look up "Color Structure" in a similar way.
//
// We will only use the .NET libraries in these first couple of weeks, we will use DirectX shortly.

//***** IMPORTANT *********
// This application is a managed C++ CLR application. You will cover such applications in other
// modules, but there is an impact here. Microsoft has extended the syntax of C++ for CLR apps,
// adding features similar to C#. One such extension is the reference ^ which is used to specify
// objects that will be automatically garbage collected. A variable such as:
//     Bitmap^ b;
// is used in a similar way to the more familiar:
//     Bitmap* b;
// E.g.
//     Bitmap^ b;  int w = b->Width;
	

// Copy given bitmap to position (X, Y) on the viewport. Read the note above regarding the ^ symbol
void SpritePlotterForm::DrawBitmap( int X, int Y, Bitmap^ SrcBitmap )
{
	for (int x = 0; x < SrcBitmap->Width; x++)
	{
		for (int y = 0; y < SrcBitmap->Height; y++)
		{
			Color pixelColor = SrcBitmap->GetPixel(x, y);
			SetViewportPixel(x+X, y+Y, pixelColor);
		}
	}
}

// Draw 'cutout' sprite to position (X,Y) on the viewport, using the alpha channel to
// determine the cutout. If a pixel has a zero alpha then it should not be copied
void SpritePlotterForm::DrawSprite( int X, int Y, Bitmap^ SrcBitmap )
{
	for (int x = 0; x < SrcBitmap->Width; x++)
	{
		for (int y = 0; y < SrcBitmap->Height; y++)
		{
			Color pixelColor = SrcBitmap->GetPixel(x, y);
			if (pixelColor.A != 0)
			{
				SetViewportPixel(x + X, y + Y, pixelColor);
			}
		}
	}
}

// Blend sprite onto position (X,Y) on the viewport, using the alpha channel as a blending factor
// Pseudo code:
//     FinalViewportRed = (BitmapRed * BitmapAlpha + ViewportRed * (255 - BitmapAlpha)) / 255
//     Same for green and blue
//
// Tip: Use three variables (int) to calculate the red green and blue components then create a
//      single colour object with a line like this:
//           Color ResultColor = Color::FromArgb( R, G, B );
void SpritePlotterForm::DrawSpriteAlpha( int X, int Y, Bitmap^ SrcBitmap )
{
for (int x = 0; x < SrcBitmap->Width; x++)
{
	for (int y = 0; y < SrcBitmap->Height; y++)
	{
		Color pixelColor = SrcBitmap->GetPixel(x, y);
		
		Color viewportColor = GetViewportPixel(x+X, y+Y);
		int red = (pixelColor.R * pixelColor.A + viewportColor.R * (255 - pixelColor.A)) / 255;
		int blue = (pixelColor.B * pixelColor.A + viewportColor.B * (255 - pixelColor.A)) / 255;
		int green = (pixelColor.G * pixelColor.A + viewportColor.G * (255 - pixelColor.A)) / 255;
		Color ResultColor = Color::FromArgb(red, green, blue);
		SetViewportPixel(x + X, y + Y, ResultColor);
		
	}
}
	
}

// Special sprite blending techniques - see the worksheet
void SpritePlotterForm::DrawSpriteAdd( int X, int Y, Bitmap^ SrcBitmap )
{
	for (int x = 0; x < SrcBitmap->Width; x++)
	{
		for (int y = 0; y < SrcBitmap->Height; y++)
		{
			Color pixelColor = SrcBitmap->GetPixel(x, y);

			Color viewportColor = GetViewportPixel(x + X, y + Y);
			int red = pixelColor.R + viewportColor.R;
			int blue = pixelColor.B + viewportColor.B;
			int green = pixelColor.G + viewportColor.G;
			if (red > 255)
			{
				red = 255.0f;
			}
			if (blue > 255)
			{
				blue = 255.0f;
			}
			if (green > 255)
			{
				green = 255.0f;
			}
			Color ResultColor = Color::FromArgb(red, green, blue);
			SetViewportPixel(x + X, y + Y, ResultColor);

		}
	}
}

void SpritePlotterForm::DrawSpriteMultiply( int X, int Y, Bitmap^ SrcBitmap )
{
	for (int x = 0; x < SrcBitmap->Width; x++)
	{
		for (int y = 0; y < SrcBitmap->Height; y++)
		{
			Color pixelColor = SrcBitmap->GetPixel(x, y);

			Color viewportColor = GetViewportPixel(x + X, y + Y);
			int red = pixelColor.R * viewportColor.R / 255;
			int blue = pixelColor.B * viewportColor.B / 255;
			int green = pixelColor.G * viewportColor.G / 255;
			Color ResultColor = Color::FromArgb(red, green, blue);
			SetViewportPixel(x + X, y + Y, ResultColor);

		}
	}
}

void SpritePlotterForm::DrawSpriteLuminosity( int X, int Y, Bitmap^ SrcBitmap )
{

	for (int x = 0; x < SrcBitmap->Width; x++)
	{
		for (int y = 0; y < SrcBitmap->Height; y++)
		{
			Color pixelColor = SrcBitmap->GetPixel(x, y);

			Color viewportColor = GetViewportPixel(x + X, y + Y);
			int red = (pixelColor.R * pixelColor.R + viewportColor.R * (255 - pixelColor.R)) / 255;
			int blue = (pixelColor.B * pixelColor.B + viewportColor.B * (255 - pixelColor.B)) / 255;;
			int green = (pixelColor.G * pixelColor.G + viewportColor.G * (255 - pixelColor.G)) / 255;;
			Color ResultColor = Color::FromArgb(red, green, blue);
			SetViewportPixel(x + X, y + Y, ResultColor);

		}
	}

}


} // end namespace SpritePlotter
