#include "stdafx.h"
//#include <sstream>
#include "Data.h"



CData::CData(HWND hWnd)
{


	PhWnd = &hWnd;
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = FileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(FileName);
	ofn.lpstrFilter = TEXT("All");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	zoomY = 1;
	zoomX = 1;
	scrollPos = 0;

	axesToDrawGyro.axes = 0;
	axesToDrawPos.axes = 0;
}

bool CData::Open()
{

	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (GetOpenFileName(&ofn))
	{
		//CoUninitialize();
		file.open(ofn.lpstrFile, std::ios::in);
		if (file.is_open())
		{
			SetWindowText(*PhWnd, ofn.lpstrFile);

			Read();



			MessageBox(0, ofn.lpstrFile, TEXT("File opened!"), MB_ICONINFORMATION | MB_OK);
#ifdef DEBUG

			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			freopen("CON", "w", stdout);
			std::cout << "dupa";
#endif // DEBUG

			return 0;
		}
	}
	MessageBox(0, TEXT("Fatal error!"), 0, MB_ICONERROR | MB_OK);
	return 1;
}

bool CData::DrawGrid(HDC hdc, RECT drawArea)
{
	Graphics graphics(hdc);
	Pen pen2(Color(255, 153, 153, 153));

	Rectangle(hdc, drawArea.left, drawArea.top, drawArea.right, drawArea.bottom);
	graphics.DrawLine(&pen2, (drawArea.right - drawArea.left) / 2 + drawArea.left, drawArea.top, (drawArea.right - drawArea.left) / 2 + drawArea.left, drawArea.bottom);
	graphics.DrawLine(&pen2, drawArea.left, (drawArea.bottom - drawArea.top) / 2 + drawArea.top, drawArea.right, (drawArea.bottom - drawArea.top) / 2 + drawArea.top);

	return 0;
}
void CData::DrawCurve(HDC hdc, RECT drawArea, bool gyroOrPos)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 255));
	Pen pen2(Color(255, 0, 255, 0));
	Pen pen3(Color(255, 255, 0, 0));
	Pen white(Color(255, 255, 255, 255));
	TAxesToDraw *axesToDraw;
	std::vector<Point3D> *dataToDraw;
	PointF drawPointStart, drawPointEnd;

	float drawYOffset = (drawArea.bottom - drawArea.top) / 2 + drawArea.top;
	if (gyroOrPos)
	{
		dataToDraw = &gyroData;
		axesToDraw = &axesToDrawGyro;
	}
	else
	{
		pen.SetColor(Color::Brown);
		pen2.SetColor(Color::Cyan);
		pen3.SetColor(Color::ForestGreen);
		dataToDraw = &posData;
		axesToDraw = &axesToDrawPos;
	}
	//switch (drawingMode)
	//{
	//case 1:
	//	dataToDraw = &gyroData;
	//	axesToDraw = &axesToDrawGyro;
	//	break;
	//case 2:
	//	dataToDraw = &posData;
	//	axesToDraw = &axesToDrawPos;
	//	break;
	//case 3:
	//	//both
	//	dataToDraw = &gyroData;
	//	axesToDraw = &axesToDrawGyro;
	//	break;
	//case 0:
	//	return;
	//	break;
	//}


	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	for (int i = 1; i < dataToDraw->size(); i++)
	{
		drawPointStart.X = ((i - 1)*zoomX + drawArea.left) - scrollPos * 2;
		drawPointEnd.X = (i*zoomX + drawArea.left) - scrollPos * 2;

		if (axesToDraw->X)
		{
			drawPointStart.Y = (*dataToDraw)[i - 1].x * zoomY + drawYOffset;
			drawPointEnd.Y = (*dataToDraw)[i].x * zoomY + drawYOffset;
			graphics.DrawLine(&pen, drawPointStart.X, drawPointStart.Y, drawPointEnd.X, drawPointEnd.Y);
		}
		if (axesToDraw->Y)
		{
			drawPointStart.Y = (*dataToDraw)[i - 1].y * zoomY + drawYOffset;
			drawPointEnd.Y = (*dataToDraw)[i].y * zoomY + drawYOffset;
			graphics.DrawLine(&pen2, drawPointStart.X, drawPointStart.Y, drawPointEnd.X, drawPointEnd.Y);
		}
		if (axesToDraw->Z)
		{
			drawPointStart.Y = (*dataToDraw)[i - 1].z * zoomY + drawYOffset;
			drawPointEnd.Y = (*dataToDraw)[i].z * zoomY + drawYOffset;
			graphics.DrawLine(&pen3, drawPointStart.X, drawPointStart.Y, drawPointEnd.X, drawPointEnd.Y);
		}

	}
}
bool CData::Draw(HDC hdc, RECT drawArea)
{


	DrawGrid(hdc, drawArea);
	DrawCurve(hdc, drawArea, TRUE);
	DrawCurve(hdc, drawArea, FALSE);
	//graphics.DrawRectangle(&pen, 50 + value, 400, 10, 20);
	return 0;
}

bool CData::Read()
{
	std::string line;

	double dummy;
	Point3D gyroPoint, posPoint;

	for (int i = 0; i < 3; i++)         // zerowanie punktów
	{
		gyroPoint.point3D[i] = 0;
		posPoint.point3D[i] = 0;
	}


	while (std::getline(file, line)) // read one line from file
	{
		std::istringstream iss(line); // access line as a stream

		iss >> dummy >> dummy >> dummy; //olanie polozenia
		iss >> dummy >> dummy >> dummy; //olanie akcelerometru
		iss >> dummy >> dummy >> dummy; // i magnetometru

		iss >> gyroPoint.x >> gyroPoint.y >> gyroPoint.z;

		

		//posData.push_back(posPoint);
		gyroData.push_back(gyroPoint);

	}
	posData = Integrate(gyroData);
	file.close();
	dataSize = posData.size();
	return 0;
}

void CData::DiscardSamples(std::vector<Point3D> dataVector, int amountOfSamples)
{
	dataVector.erase(dataVector.begin(), dataVector.begin() + amountOfSamples);
}

void CData::DiscardSamples(int amountOfSamples)
{
	gyroData.erase(gyroData.begin(), gyroData.begin() + amountOfSamples);
	posData.erase(posData.begin(), posData.begin() + amountOfSamples);
}

void CData::ChangeZoom(double amount, bool plusOrMinus)
{
	if (plusOrMinus) zoomY += amount;
	else zoomY -= amount;

	if (zoomY > 100) zoomY = 100;
	if (zoomY < 0)zoomY = 1;

}

std::vector<Point3D>& CData::Integrate(std::vector<Point3D> dataVector)
{
	Point3D  integratedPoint;
	std::vector<Point3D> integratedVector;

	integratedPoint.x = 0;
	integratedPoint.y = 0;
	integratedPoint.z = 0;
	for (auto index : dataVector)
	{
		integratedPoint.x += index.x * SAMPLE_TIME_RES;
		integratedPoint.y += index.y * SAMPLE_TIME_RES;
		integratedPoint.z += index.z * SAMPLE_TIME_RES;
		integratedVector.push_back(integratedPoint);
	}
	return integratedVector;
}

CData::~CData()
{
	file.close();
}
