//
// ColourConversions.h
//
// Header file for colour conversion functions

// Convert an RGB colour to a HSL colour
void RGBToHSL( int R, int G, int B, int& H, int& S, int& L );

// Convert a HSL colour to an RGB colour
void HSLToRGB( int H, int S, int L, int& R, int& G, int& B );
