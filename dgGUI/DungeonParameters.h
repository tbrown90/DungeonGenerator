#pragma once

class DungeonParameters
{
public:
	DungeonParameters(HWND dlg);
	DungeonParameters(void);
	~DungeonParameters(void);

	int width;
	int height;
	int min;
	int max;
	int deadEnd;

	int roomDensity;
	int corType;
	int mask;

	void GetData();
	void LinkToControls(HWND hdlg);
	void GetEditBoxData();

	bool initialized;

private:
	void InitRoomDensity();
	void InitCorType();
	void InitMask();
	void InitEditBoxes();

	//Handles to the dialog and various controls
	HWND	m_paramDlg;
	HWND	m_roomDensity;
	HWND	m_corType;
	HWND	m_mask;

	HWND	m_widthTxt;
	HWND	m_heightTxt;
	HWND	m_minTxt;
	HWND	m_maxTxt;
	HWND	m_deadEndTxt;
};
