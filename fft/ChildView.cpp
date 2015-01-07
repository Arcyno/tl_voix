// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "FFTExample.h"
#include "ChildView.h"
#include "Fourier.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	//Creates a CFourier object
	//The CFourier class has the FFT function and a couple of usefull variables
	CFourier fft;

	//sample rate of the signal (must be 2^n)
	long sample_rate=8192;

	//number of samples you want to send for processing in the fft (any)
	//for example 100 samples
	long captured_samples=500;

	//frequency of the signal (has to be smaller than sample_rate/2)
	//for example 46
	int frequency=250;

	float data[5000];


	//example of a sin signal(you can try to add some more signals to see the
	//fourier change

	for(int i=0; i<captured_samples; i++)
	{
			double temp=(double)(2*fft.pi*frequency*((float)i/sample_rate));		
			data[i]=((double)30*sin(temp));	
	}

	//aply the FFT to the signal
	fft.ComplexFFT(data,captured_samples,sample_rate,1);
	
	//do the drawing of the signal
	//the fft object has two usefull variables
	//one is the fft.fundamental_frequency which contains the value of
	//the fundamental frequency
	//the second one is the fft.vector which contains the values of the
	//Fourier signal we will use to do the drawing

	CRect rcClient;
	GetClientRect(rcClient);

	CClientDC dcc(this);
	
	CBitmap Bitmap;
	CBitmap* pbmOld = NULL;

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dcc);

	Bitmap.CreateCompatibleBitmap(&dcc,rcClient.right,rcClient.bottom);
	pbmOld = dcMem.SelectObject(&Bitmap);

	dcMem.PatBlt(0, 0,rcClient.right, rcClient.bottom, WHITENESS);
	char buffer[25];
	itoa(fft.fundamental_frequency,buffer,10);
	dcMem.TextOut(rcClient.right-280,20,"The fundamental frequency is:");
	dcMem.TextOut(rcClient.right-50,20,buffer);
	int x,y, x_temp, y_temp;
	for(x=0 ; x<rcClient.right; x++)
	{
		//this temp variables are used to ajust the sign to the wiindow
		//if you want to see more of the signal ajust these fields
		x_temp=((x*(sample_rate/2))/rcClient.right);
		y_temp=(int)((rcClient.bottom*(pow(fft.vector[2*x_temp],2)+pow(fft.vector[2*x_temp+1],2)))/((double)pow(fft.vector[2*fft.fundamental_frequency],2)+pow(fft.vector[2*fft.fundamental_frequency+1],2)));
		dcMem.MoveTo(x,rcClient.bottom);
		y=rcClient.bottom-y_temp;
		dcMem.LineTo(x,y);
	}



	dcc.BitBlt(0,0,rcClient.right,rcClient.bottom,
				&dcMem, 0, 0, SRCCOPY);
	
	
	dcMem.SelectObject(pbmOld);
	dcMem.DeleteDC();
	// Do not call CWnd::OnPaint() for painting messages
}

