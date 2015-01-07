// FFTExample.h : main header file for the FFTExample application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CFFTExampleApp:
// See FFTExample.cpp for the implementation of this class
//

class CFFTExampleApp : public CWinApp
{
public:
	CFFTExampleApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFFTExampleApp theApp;