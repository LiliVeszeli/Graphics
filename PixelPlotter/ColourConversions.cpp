//
// ColourConversions.cpp
//
// Source file for colour conversion functions. The key functions are initially
// empty - you need to implement them for the lab exercise

// Find the minimum of three numbers
float Min( float f1, float f2, float f3 )
{
	float fMin = f1;
	if (f2 < fMin)
	{
		fMin = f2;
	}
	if (f3 < fMin)
	{
		fMin = f3;
	}
	return fMin;
}

// Find the maximum of three numbers
float Max( float f1, float f2, float f3 )
{
	float fMax = f1;
	if (f2 > fMax)
	{
		fMax = f2;
	}
	if (f3 > fMax)
	{
		fMax = f3;
	}
	return fMax;
}

// Convert an RGB colour to a HSL colour
void RGBToHSL( int R, int G, int B, int& H, int& S, int& L )
{
	// Fill in the correct code here, the functions Min and Max above will help
	// Left blank as this is now part of the assignment labs
}

// Convert a HSL colour to an RGB colour
void HSLToRGB( int H, int S, int L, int& R, int& G, int& B )
{
	// Fill in the correct code here
	// Left blank as this is now part of the assignment labs
}
