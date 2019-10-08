// SpritePlotter.cpp : main project file.

#include "stdafx.h"
#include "SpritePlotterForm.h"

using namespace SpritePlotter;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew SpritePlotterForm());
	return 0;
}