// Maths.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include <iomanip>
#include <math.h>
using namespace std;

void Min(float a, float b)
{
	float min = 0;
	if (a < b)
	{
		 min = a;
	}
	else
	{
		b = min;
	}
	cout << min;
}

int main()
{
  
	/*int geza = 10;
	float gizella = 16.0f;
	double arabella = 88.9;

	cout << sizeof(geza) << endl;
	cout << sizeof(gizella) << endl;
	cout << sizeof(arabella) << endl;*/

	//float irina = 0.7f;
	//if (irina == 0.7f)
	//{
	//	cout << setprecision(17);
	//	cout << irina << endl;
	//}
	//else
	//{
	//	cout << "no";
	//}

	//double enrico = 0.7;
	//cout << setprecision(17);
	//cout << enrico;
	//cout << endl;
	//enrico -= 0.6999999;
	//enrico *= 10000000;
	//cout << enrico;

	/*float enrico = 0.7f;
	cout << setprecision(17);
	cout << enrico;
	cout << endl;
	enrico -= 0.6999999;
	enrico *= 10000000;
	cout << enrico;*/

	//float enrico = 0.6999999f;
	//cout << setprecision(17);
	//cout << enrico;
	//cout << endl;
	//enrico -= 0.6999999;
	//enrico *= 10000000;
	//cout << enrico;

	/*int antonia = 100000;
	short int ant = antonia;
	cout << ant;
	cout << endl;
	cout << endl;

	unsigned char ursula = 245;
	char uninia = ursula;
	cout << static_cast<int>(uninia);*/

	/*float egert = 3.55f;
	float erencia = 3.54f;

	int iaura = 5;
	int ferenc = 6;

	Min(egert, erencia);*/ //dosent work if the floats are close to each other

	float x1 = 123.5f;
	float x2 = 12.0f;

	float result = abs(x1-x2);
	result = fmod(result, 32.0);
	int rresult = result;

	cout << rresult;



	cout << endl;
	system("pause");
}

