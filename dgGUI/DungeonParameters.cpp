#include "StdAfx.h"
#include "DungeonParameters.h"
#include "Resource.h"

DungeonParameters::DungeonParameters(void)
{
	m_paramDlg = NULL;
	m_roomDensity = NULL;
	m_corType = NULL;
	m_mask = NULL;

	m_widthTxt = NULL;
	m_heightTxt = NULL;
	m_minTxt = NULL;
	m_maxTxt = NULL;
	m_deadEndTxt = NULL;

	width = 20;
	height = 20;
	min = 3;
	max = 7;
	deadEnd = 50;

	roomDensity = 1;
	corType = 1;
	mask = 0;

	initialized = false;
}

DungeonParameters::~DungeonParameters(void) { }

DungeonParameters::DungeonParameters(HWND dlg)
{
	m_paramDlg = dlg;
	m_roomDensity = GetDlgItem(m_paramDlg, IDC_ROOMDENSITY);
	m_corType = GetDlgItem(m_paramDlg, IDC_CORTYPE);
	m_mask = GetDlgItem(m_paramDlg, IDC_MASK);

	m_widthTxt = GetDlgItem(m_paramDlg, IDC_WIDTHTXT);
	m_heightTxt = GetDlgItem(m_paramDlg, IDC_HEIGHTTXT);
	m_minTxt = GetDlgItem(m_paramDlg, IDC_MINTXT);
	m_maxTxt = GetDlgItem(m_paramDlg, IDC_MAXTXT);
	m_deadEndTxt = GetDlgItem(m_paramDlg, IDC_DEADENDTXT);

	width = 20;
	height = 20;
	min = 3;
	max = 7;
	deadEnd = 50;

	roomDensity = 1;
	corType = 1;
	mask = 0;

	InitRoomDensity();
	InitCorType();
	InitMask();
	InitEditBoxes();

	initialized = true;
}

void DungeonParameters::InitRoomDensity()
{
	ComboBox_AddString(m_roomDensity, "SPARSE");
	ComboBox_AddString(m_roomDensity, "SCATTERED");
	ComboBox_AddString(m_roomDensity, "DENSE");

	ComboBox_SetCurSel(m_roomDensity, roomDensity);
}

void DungeonParameters::InitCorType()
{
	ComboBox_AddString(m_corType, "STRAIGHT");
	ComboBox_AddString(m_corType, "MIXED");
	ComboBox_AddString(m_corType, "LABYRINTH");

	ComboBox_SetCurSel(m_corType, corType);
}

void DungeonParameters::InitMask()
{
	ComboBox_AddString(m_mask, "SQUARE");
	ComboBox_AddString(m_mask, "CIRCLE");
	ComboBox_AddString(m_mask, "CASTLE");
	ComboBox_AddString(m_mask, "TRIANGLE");
	ComboBox_AddString(m_mask, "CROSS");

	ComboBox_SetCurSel(m_mask, mask);
}

void DungeonParameters::InitEditBoxes()
{
	char* temp = new char[4];

	temp = _itoa(width, temp, 10);
	Edit_SetText(m_widthTxt, temp);

	temp = _itoa(height, temp, 10);
	Edit_SetText(m_heightTxt, temp);

	temp = _itoa(min, temp, 10);
	Edit_SetText(m_minTxt, temp);

	temp = _itoa(max, temp, 10);
	Edit_SetText(m_maxTxt, temp);

	temp = _itoa(deadEnd, temp, 10);
	Edit_SetText(m_deadEndTxt, temp);
}

void DungeonParameters::GetEditBoxData()
{
	char* temp = new char[4];
	Edit_GetText(m_widthTxt, temp, 4);
	width = atoi(temp);

	Edit_GetText(m_heightTxt, temp, 4);
	height = atoi(temp);

	Edit_GetText(m_minTxt, temp, 4);
	min = atoi(temp);

	Edit_GetText(m_maxTxt, temp, 4);
	max = atoi(temp);

	Edit_GetText(m_deadEndTxt, temp, 4);
	int de = atoi(temp);
	if (de <= 100)
		deadEnd = atoi(temp);
	else
	{
		deadEnd = 100;
		Edit_SetText(m_deadEndTxt, "100");
	}
}

void DungeonParameters::GetData()
{
	GetEditBoxData();

	roomDensity = ComboBox_GetCurSel(m_roomDensity);
	corType = ComboBox_GetCurSel(m_corType);
	mask = ComboBox_GetCurSel(m_mask);
}

void DungeonParameters::LinkToControls(HWND dlg)
{
	m_paramDlg = dlg;
	m_roomDensity = GetDlgItem(m_paramDlg, IDC_ROOMDENSITY);
	m_corType = GetDlgItem(m_paramDlg, IDC_CORTYPE);
	m_mask = GetDlgItem(m_paramDlg, IDC_MASK);

	m_widthTxt = GetDlgItem(m_paramDlg, IDC_WIDTHTXT);
	m_heightTxt = GetDlgItem(m_paramDlg, IDC_HEIGHTTXT);
	m_minTxt = GetDlgItem(m_paramDlg, IDC_MINTXT);
	m_maxTxt = GetDlgItem(m_paramDlg, IDC_MAXTXT);
	m_deadEndTxt = GetDlgItem(m_paramDlg, IDC_DEADENDTXT);


	InitRoomDensity();
	InitMask();
	InitCorType();
	InitEditBoxes();

	initialized = true;
}