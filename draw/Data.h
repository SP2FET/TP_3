#pragma once
//#include <fstream>

#define SAMPLE_TIME_RES 1/25

union Point3D
{
	double point3D[3];
	struct
	{
		double x;
		double y;
		double z;
	};
};

union TAxesToDraw
{
	int axes;
	struct
	{
		bool X;
		bool Y;
		bool Z;
	};
};

enum DrawingMode {none = 0,gyro = 1, pos = 2, both = 3};

class CData
{
private:
	OPENFILENAME ofn;       // common dialog box structure
	
	//LPWSTR szFileName;
	wchar_t FileName[512] = L"";
	std::fstream file;
	HWND *PhWnd;
	std::vector<Point3D> gyroData;
	std::vector<Point3D> posData;
	

public:
	double zoomY;
	double zoomX;
	int scrollPos;
	int dataSize;
	TAxesToDraw axesToDrawGyro, axesToDrawPos;
	int drawingMode;
	CData(HWND hWnd);
	bool Open();
	bool Read();
	bool Draw(HDC hdc, RECT drawArea);
	bool DrawGrid(HDC hdc, RECT drawArea);
	void DrawCurve(HDC hdc, RECT drawArea, bool gyroOrPos);
	void DiscardSamples(std::vector<Point3D> dataVector, int amountOfSamples);
	void DiscardSamples(int amountOfSamples);
	void ChangeZoom(double amount, bool plusOrMinus);
	~CData();
};

