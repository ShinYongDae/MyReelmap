// SimpleOpengl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "SimpleOpengl.h"

#include "MyReelmapDlg.h"

// CSimpleOpengl

IMPLEMENT_DYNAMIC(CSimpleOpengl, CWnd)

CSimpleOpengl::CSimpleOpengl(CWnd* pParent/*=NULL*/)
{
	m_hDc = NULL;
	m_pDc = NULL;
	m_pParent = pParent;

	cameraposmap[0] = 0.0f;
	cameraposmap[1] = 0.0f;
	cameraposmap[2] = 7000.0f;
	Angle[0] = 0.0f;
	Angle[1] = 0.0f;
	Angle[2] = 0.0f;
	m_bInit = FALSE;
	m_bFont = FALSE; 
	m_crText = RGB_WHITE;

	m_pMenu = NULL;
	m_nSerial = 0;

	m_bDraw = FALSE;
	m_bDrawText = FALSE;
	m_bDrawClear = FALSE;
	m_bDrawClearColor = FALSE;

	m_arPcr = NULL;
	m_StrPcs = NULL;

	ThreadStart();
}

CSimpleOpengl::CSimpleOpengl(HWND& hCtrl, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();
	m_hCtrl = hCtrl;
	m_pDc = NULL;
	m_bInit = FALSE;
	m_bFont = FALSE;
	m_crText = RGB_WHITE;

	m_pMenu = NULL;

	m_bDraw = FALSE;
	m_bDrawText = FALSE;
	m_bDrawClear = FALSE;
	m_bDrawClearColor = FALSE;

	m_arPcr = NULL;
	m_StrPcs = NULL;

	m_pDc = new CClientDC(FromHandle(hCtrl));
	CreateWndForm(WS_CHILD | WS_OVERLAPPED);
	ThreadStart();
}

CSimpleOpengl::~CSimpleOpengl()
{
	ThreadStop();
	Sleep(30);
	t1.join();

	RemoveAllLine();
	RemoveAllRect();
	RemoveAllText();

	if (m_bFont)
	{
		if (m_Font.DeleteObject())
			m_bFont = FALSE;
	}

	if (m_hRC)
	{
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}

	if (m_pDc)
	{
		::ReleaseDC(m_hWnd, m_pDc->GetSafeHdc());
		delete m_pDc;
		m_pDc = NULL;
	}

	if (m_pMenu)
	{
		delete m_pMenu;
		m_pMenu = NULL;
	}
}
void CSimpleOpengl::RemoveAllLine()
{
	int nTotal = m_arLine.GetCount();
	if (nTotal > 0)
	{
		m_arLine.RemoveAll();
	}
}

void CSimpleOpengl::RemoveAllRect()
{
	int nTotal = m_arRect.GetCount();
	if (nTotal > 0)
	{
		m_arRect.RemoveAll();
	}
}

void CSimpleOpengl::RemoveAllText()
{
	int nTotal = m_arText.GetCount();
	if (nTotal > 0)
	{
		m_arText.RemoveAll();
	}
}


BEGIN_MESSAGE_MAP(CSimpleOpengl, CWnd)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CSimpleOpengl 메시지 처리기입니다.

BOOL CSimpleOpengl::CreateWndForm(DWORD dwStyle)
{
	//if (!Create(NULL, _T("CSimpleOpengl"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	//{
	//	AfxMessageBox(_T("CSimpleOpengl::Create() Failed!!!"));
	//	return FALSE;
	//}
	//if (!Create(_T("CSimpleOpengl"), WS_CHILD | WS_OVERLAPPED, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	//{
	//	AfxMessageBox(_T("CSimpleOpengl::Create() Failed!!!"));
	//	return FALSE;
	//}

	return TRUE;
}

/*
// 그리기 표면의 픽셀 형식을 설명
typedef struct tagPIXELFORMATDESCRIPTOR {
	WORD  nSize;
	WORD  nVersion;
	DWORD dwFlags;
	BYTE  iPixelType;
	BYTE  cColorBits;
	BYTE  cRedBits;
	BYTE  cRedShift;
	BYTE  cGreenBits;
	BYTE  cGreenShift;
	BYTE  cBlueBits;
	BYTE  cBlueShift;
	BYTE  cAlphaBits;
	BYTE  cAlphaShift;
	BYTE  cAccumBits;
	BYTE  cAccumRedBits;
	BYTE  cAccumGreenBits;
	BYTE  cAccumBlueBits;
	BYTE  cAccumAlphaBits;
	BYTE  cDepthBits;
	BYTE  cStencilBits;
	BYTE  cAuxBuffers;
	BYTE  iLayerType;
	BYTE  bReserved;
	DWORD dwLayerMask;
	DWORD dwVisibleMask;
	DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR, *PPIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;

[dwFlags]
=================================================================================================================================================
값										의미
=================================================================================================================================================
PFD_DRAW_TO_WINDOW						버퍼는 창 또는 디바이스 화면에 그릴 수 있습니다.
0x00000004

PFD_DRAW_TO_BITMAP						버퍼는 메모리 비트맵에 그릴 수 있습니다.
0x00000008

PFD_SUPPORT_GDI							버퍼는 GDI 그리기를 지원합니다. 이 플래그 및 PFD_DOUBLEBUFFER 현재 제네릭 구현에서 상호 배타적입니다.
0x00000010

PFD_SUPPORT_OPENGL						버퍼는 OpenGL 그리기를 지원합니다.
0x00000020

PFD_GENERIC_ACCELERATED					픽셀 형식은 제네릭 구현을 가속화하는 디바이스 드라이버에서 지원됩니다. 이 플래그가 명확하고 PFD_GENERIC_FORMAT 플래그가 설정된 경우 픽셀 형식은 제네릭 구현에서만 지원됩니다.
0x00001000

PFD_GENERIC_FORMAT						픽셀 형식은 제네릭 구현이라고도 하는 GDI 소프트웨어 구현에서 지원됩니다. 이 비트가 명확한 경우 픽셀 형식은 디바이스 드라이버 또는 하드웨어에서 지원됩니다.
0x00000040

PFD_NEED_PALETTE						버퍼는 팔레트 관리 디바이스에서 RGBA 픽셀을 사용합니다. 이 픽셀 형식에 대한 최상의 결과를 얻으려면 논리적 팔레트가 필요합니다. 색상표의 색은 cRedBits, cRedShift, cGreenBits, cGreenShift, cBluebits 및 cBlueShift 멤버의 값에 따라 지정되어야 합니다. wglMakeCurrent를 호출하기 전에 디바이스 컨텍스트에서 팔레트를 만들고 실현해야 합니다.
0x00000080

PFD_NEED_SYSTEM_PALETTE					256색 모드에서만 하나의 하드웨어 팔레트를 지원하는 하드웨어의 픽셀 형식 설명자에 정의됩니다. 이러한 시스템에서 하드웨어 가속을 사용하려면 RGBA 모드에서 하드웨어 팔레트가 고정 순서(예: 3-3-2)이거나 색 인덱스 모드일 때 논리 팔레트와 일치해야 합니다. 이 플래그가 설정되면 프로그램에서 SetSystemPaletteUse 를 호출하여 논리 팔레트와 시스템 팔레트의 일대일 매핑을 강제해야 합니다. OpenGL 하드웨어가 여러 하드웨어 팔레트를 지원하고 디바이스 드라이버가 OpenGL에 예비 하드웨어 팔레트를 할당할 수 있는 경우 이 플래그는 일반적으로 명확합니다.
0x00000100								이 플래그는 제네릭 픽셀 형식으로 설정되지 않습니다.

PFD_DOUBLEBUFFER						버퍼가 이중 버퍼링됩니다. 이 플래그 및 PFD_SUPPORT_GDI 현재 제네릭 구현에서 상호 배타적입니다.
0x00000001

PFD_STEREO								버퍼는 입체입니다. 이 플래그는 현재 제네릭 구현에서 지원되지 않습니다.
0x00000002

PFD_SWAP_LAYER_BUFFERS					디바이스가 이중 버퍼링 오버레이 또는 언더레이 평면을 포함하는 픽셀 형식으로 개별 레이어 평면을 교환할 수 있는지 여부를 나타냅니다. 그렇지 않으면 모든 레이어 평면이 그룹으로 함께 교환됩니다. 이 플래그가 설정되면 wglSwapLayerBuffers 가 지원됩니다.
0x00000800
=================================================================================================================================================
*/
void CSimpleOpengl::Init()
{
	if (!m_bInit)
	{
		m_bInit = TRUE;
		static PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW	|
			PFD_SUPPORT_OPENGL	|
			PFD_SUPPORT_GDI		|
			PFD_DOUBLEBUFFER,				// 더블 버퍼 윈도우 (default: 싱글 버퍼 윈도우)
			//PFD_DOUBLEBUFFER_DONTCARE,
			PFD_TYPE_RGBA,					// RGBA 모드 (default)
			24,
			0,0,0,0,0,0,
			0,0,
			0,0,0,0,0,
			32,
			0,//1,
			0,
			PFD_MAIN_PLANE,
			0,
			0,0,0
		};

		int nPixelFormat = ChoosePixelFormat(m_hDc, &pfd);	// DC가 지원하는 주어진 픽셀 포맷 사양과 일치하는 적절한 픽셀 포맷을 찾는다.
		VERIFY(SetPixelFormat(m_hDc, nPixelFormat, &pfd));	// HGLRC(GL Rendering Context Handle)와 DC를 연결

		m_hRC = wglCreateContext(m_hDc);					// Rendering Context 생성
		VERIFY(wglMakeCurrent(m_hDc, m_hRC));				// 현재 스레드에 Rendering Context 설정

		int argc = 1;
		char *argv[1] = { (char*)"Something" };
		glutInit(&argc, argv);								// Window OS와 Session 연결 / GLUT Library를 초기화

		//SetupLight();
		//glDisable(GL_LIGHTING);

		CRect rtDispCtrl;
		::GetClientRect(m_hCtrl, &rtDispCtrl);
		//::GetWindowRect(m_hCtrl, &rtDispCtrl);
		m_rtDispCtrl = rtDispCtrl;
		m_nWorldW = rtDispCtrl.right - rtDispCtrl.left;
		m_nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;
		SetupResize(m_nWorldW, m_nWorldH);
		//SetupCamera(cameraposmap, nWorldW, nWorldH, Angle);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFlush();											// 그림이 다 그렸졌다는 걸 알려줌.
		SwapBuffers(m_hDc);								// OpenGL이 완료한 그림을 새롭게 화면에 그린다 - 현재 buffer에 그려진 것과 Frame에 그려진 것을 swap(Double buffer), 그리기 함수에서 glFlush 대신 사용
	}
}

void CSimpleOpengl::PopupMenu(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, (UINT)10001, _T("지표최적화"));
	menu.AppendMenu(MF_STRING, (UINT)10002, _T("시간최적화"));
	menu.AppendMenu(MF_STRING, (UINT)10003, _T("변수최적화"));
	//CRect rect;
	//::GetClientRect(m_hCtrl, &rect);
	CPoint ptMenu = point;
	ClientToScreen(&ptMenu);   //스크린 기준으로 좌표 변환
	UINT nFlagsForMenu = TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD;
	//UINT nFlagsForMenu = TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD;
	/*
	=======================================================================================================
	nFlags:
	=======================================================================================================
	TPM_CENTERALIGN		지정한 위치의 중앙에 오게한다.
	TPM_LEFTALIGN		지정한 위치의 좌측 정렬
	TPM_RIGHTALIGN		지정한 위치의 오른족 정렬
	TPM_BOTTOMALIGN		지정한 위치의 바닥에 정렬.
	TPM_TOPALIGN		지정한 위치의 위쪽에 정렬.
	TPM_VCENTERALIGN	지정한 위치의 수직중앙 정렬.
	TPM_LEFTBUTTON		마우스 왼쪽 버튼을 눌러 팝업메뉴를 선택한다.
	TPM_RIGHTBUTTON		마우스 오른쪽 버튼을 눌러 팝업메뉴를 선택한다.
	TPM_RETURNCMD		TrackPopupMenu 함수가 반환하는 자료형은 BOOL 형이므로 내부적으로 int형 처리된다.
						그러므로 TPM_RETURNCMD를 설정한경우 int형으로 어떤메뉴를 선택했는지 확인할수있다.
	=======================================================================================================
	*/
	int nRetValue = menu.TrackPopupMenu(nFlagsForMenu, ptMenu.x, ptMenu.y, this);
	if (!nRetValue) return;
	menu.DestroyMenu();

	CString sName = _T("Empty");
	switch (nRetValue) 
	{
	case 10001:
		sName = _T("IndexOptimizer");
		break;
	case 10002:
		sName = _T("TimeOptimizer");
		break;
	case 10003:
		sName = _T("VariableOptimizer");
		break;
	}

	AfxMessageBox(sName);
}

void CSimpleOpengl::SetHwnd(HWND hCtrlWnd, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();

	m_hCtrl = hCtrlWnd;
	m_pDc = new CClientDC(FromHandle(hCtrlWnd));
	m_hDc = m_pDc->GetSafeHdc();
}

void CSimpleOpengl::Refresh()
{
	RedrawWindow();
}

BOOL CSimpleOpengl::IsDraw()
{
	return m_bDraw;
}

void CSimpleOpengl::ProcThrd(const LPVOID lpContext)
{
	CSimpleOpengl* pSimpleOpengl = reinterpret_cast<CSimpleOpengl*>(lpContext);

	while (pSimpleOpengl->ThreadIsAlive())
	{
		if (!pSimpleOpengl->ProcOpengl())
			break;
		Sleep(30);
	}

	pSimpleOpengl->ThreadEnd();
}

BOOL CSimpleOpengl::ProcOpengl()
{
	if (m_bDraw)
	{
		m_bDraw = FALSE;
		Init();
		Draw();
	}
	else if (m_bDrawText)
	{
		m_bDrawText = FALSE;
		//DrawTestText();
	}
	else if (m_bDrawClear)
	{
		m_bDrawClear = FALSE;
		DrawClear();
	}
	else if (m_bDrawClearColor)
	{
		m_bDrawClearColor = FALSE;
		COLORREF color = RGB(0, 0, 0);
		DrawClearColor(color);
	}
	return TRUE;
}

void CSimpleOpengl::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(ProcThrd, this);
}

void CSimpleOpengl::ThreadStop()
{
	m_bThreadAlive = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000 * 60 * 3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(30);
	while (!m_bThreadStateEnd)
	{
		if (GetTickCount() >= (dwStartTick + dwTimeOut))
		{
			AfxMessageBox(_T("WaitUntilThreadEnd() Time Out!!!", NULL, MB_OK | MB_ICONSTOP));
			return;
		}
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(30);
	}
}

void CSimpleOpengl::ThreadEnd()
{
	m_bThreadStateEnd = TRUE;
}

BOOL CSimpleOpengl::ThreadIsAlive()
{
	return m_bThreadAlive;
}

void CSimpleOpengl::SetupResize(int width, int height)
{
	GLfloat fAspect;
	if (height == 0)
		height = 1;

	// OpenGL은 오른손 좌표계임. (default: 화면 중앙이 원점, 좌표계 범위는 [-1.0 ~ 1.0])
	glViewport(0, 0, width, height);								// OpenGL좌표 (0, 0)-좌하단에서 width, height 영역에만 뷰잉을 함. : Viewport = Screen
	glMatrixMode(GL_PROJECTION);									// 현재의 행렬 모드를 설정.
	glLoadIdentity();												// 현재의 변환 행렬을 단위행렬로 설정. (모델 좌표계 = 전역 좌표계 = 시점 좌표계) : 행렬처리가 이루어 지기 전에 좌표계를 초기화 함.
	glOrtho(0.0, width, height, 0.0, -2, 2);						// Viewport 좌표계의 범위를 설정 (min_x, max_x, min_y, max_y, near, far) : (left, right, bottom, top, near, far)
	
	/*
	=============================================================================
	GLenum mode
	=============================================================================
	GL_MODELVIEW	:	모델링 및 뷰잉 변환 모드 (물체 이동 시)
						디폴트 모드로 GL_MODELVIEW 로 설정됨.
	GL_PROJECTION	:	투영 변환 모드 (클리핑 공갂 정의)
	GL_TEXTURE		:	텍스처 매핑 모드 (텍스처 매핑 정의)
	=============================================================================
	*/
	glMatrixMode(GL_MODELVIEW);										// 현재의 행렬 모드를 설정.
	glLoadIdentity();												// 현재의 변환 행렬을 단위행렬로 설정. (모델 좌표계 = 전역 좌표계 = 시점 좌표계) : 행렬처리가 이루어 지기 전에 좌표계를 초기화 함.

	gluPerspective(0.0, 1.0, 1.0, 1.0);							// 원근 투영을 사용하는 경우: fovy: 수직 방향의 보이는 각도 (y축 방향) , aspect: 종횡비 (앞쪽의 클리핑 평면의 폭(w)을 높이(h)로 나눈 값), zNear, zFar
}

void CSimpleOpengl::DrawBegin(int nMode, int nSize, COLORREF color)
{
	if (nMode < Opengl::modPoint || nMode > Opengl::modCircleF)
		return;

	glPushMatrix();																// Martrix상태를 스택에 넣는다.		
	GLfloat red = GetRValue(color) / 255.0;
	GLfloat green = GetGValue(color) / 255.0;
	GLfloat blue = GetBValue(color) / 255.0;
	GLfloat alpa = 0.0;
	glColor4f(red, green, blue, alpa);		// drawing color를 설정한다.

	/* glBegin ~ glEnd 블록 사이에 위치만을 가지는 정점만을 정의함
	모드				설명
	============================================================
	GL_POINTS			독립적인 점
	GL_LINE_STRIP		연결된 선분
	GL_LINE_LOOP		시작점과 끝점을 이은 선분
	GL_LINES			두개의 정점들을 이은 선분
	GL_TRIANGLES		세개씩 연결한 삼각형
	GL_TRIANGLE_STRIP	연결된 삼각형
	GL_TRIANGLE_FAN		중심을 공유하는 삼각형
	GL_QUADS			정점 4개씩을 연결하여 사각형을 그린다.
	GL_QUAD_STRIP		연결된 사각형
	GL_POLYGON			연결된 볼록 다각형
	============================================================*/
	
	switch (nMode)
	{
	case Opengl::modPoint:
		glPointSize(nSize);				// nSize : 점의 크기 (0.5부터 10.0까지 지정, 설정 간격은 0.125 이상, default : 1.0) - glBegin() - glEnd() 밖에서 사용
		glBegin(GL_POINTS);
		break;
	case Opengl::modLine:
	case Opengl::modRectE:
	case Opengl::modCross:
	case Opengl::modCrossX:
		glLineWidth(nSize);				// nSize : 선의 굵기 (0.5에서 10.0까지 지정, 설정 간격은 0.125 이상, default : 1.0) - PointSize/LineWidth 0.0보다 커야함.
		glBegin(GL_LINES);
		break;
	case Opengl::modRectF:
		glLineWidth(nSize);
		glBegin(GL_QUADS);
		break;
	case Opengl::modCircleE:
		glLineWidth(nSize);
		glBegin(GL_LINE_LOOP);
		break;
	case Opengl::modCircleF:
		glLineWidth(nSize);
		glBegin(GL_POLYGON);			// 꼭지점을 반시계 방향으로 그린 쪽이 앞면
		break;
	}
}

void CSimpleOpengl::DrawEnd()
{
	glEnd();
	glPopMatrix();	// Martrix상태를 스택에서 꺼낸다
}

void CSimpleOpengl::DrawRect(stVertex V1, stVertex V2)
{
	glVertex3f(V1.x, V1.y, V1.z);
	glVertex3f(V2.x, V1.y, V2.z);

	glVertex3f(V2.x, V1.y, V2.z);
	glVertex3f(V2.x, V2.y, V2.z);

	glVertex3f(V2.x, V2.y, V2.z);
	glVertex3f(V1.x, V2.y, V1.z);

	glVertex3f(V1.x, V2.y, V1.z);
	glVertex3f(V1.x, V1.y, V1.z);
}

void CSimpleOpengl::DrawLine(stVertex V1, stVertex V2)
{
	glVertex3f(V1.x, V1.y, V1.z);
	glVertex3f(V2.x, V2.y, V2.z);
}

void CSimpleOpengl::SetTextOpenGL(CString str, stVertex pos, int size, stColor color, int line_width) //  Text
{
	int nLen = str.GetLength();
	char* pData = new char[nLen + 1]; // for '\0'
	StringToChar(str, pData);

	char *c;
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glLineWidth(line_width);
	glColor3f(color.R, color.G, color.B);			// 색을 선택하는 함수로써 불투명도는 지정하지 않는다.
	glScalef(0.01f*size, 0.01f*size, 0.01f*size);

	for (c = pData; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();

	delete pData;
}


HBRUSH CSimpleOpengl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if (CTLCOLOR_STATIC == nCtlColor)
	{
		;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CSimpleOpengl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (CTLCOLOR_STATIC == nIDCtl)
	{
		;
	}

	CStatic::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSimpleOpengl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.

	m_bDraw = TRUE;
	//m_bDrawText = TRUE;
}

void CSimpleOpengl::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CSimpleOpengl::AddLine(stVertex v1, stVertex v2, COLORREF color)
{
	stLine _line;
	_line.v1 = v1;
	_line.v2 = v2;
	_line.color = color;
	m_arLine.Add(_line);
}

void CSimpleOpengl::AddText(CString str, CPoint pos, COLORREF color)
{
	stText _text;
	_text.str = str;
	_text.pos = pos;
	_text.color = color;
	m_arText.Add(_text);
}

void CSimpleOpengl::AddRect(stVertex v1, stVertex v2, COLORREF color)
{
	stRect _rect;
	_rect.v1 = v1;
	_rect.v2 = v2;
	_rect.color = color;
	m_arRect.Add(_rect);
}

void CSimpleOpengl::Draw()
{
	DrawClear();

	int i, nTotal;

	nTotal = m_arLine.GetCount();
	if (nTotal > 0)
	{
		stLine _line;
		for (i = 0; i < nTotal; i++)
		{
			_line = m_arLine.GetAt(i);
			DrawBegin(Opengl::modLine, 1, _line.color);
			DrawLine(_line.v1, _line.v2);
			DrawEnd();
		}
	}
	else
		return;

	nTotal = m_arRect.GetCount();
	if (nTotal > 0)
	{
		stRect _rect;
		for (i = 0; i < nTotal; i++)
		{
			_rect = m_arRect.GetAt(i);
			DrawBegin(Opengl::modRectF, 1, _rect.color);
			DrawRect(_rect.v1, _rect.v2);
			DrawEnd();
		}
	}

	glFlush();											// 그림이 다 그렸졌다는 걸 알려줌. 모든 명령어를 실행되게 함.
	SwapBuffers(m_hDc);									// OpenGL이 완료한 그림을 새롭게 화면에 그린다 - 현재 buffer에 그려진 것과 Frame에 그려진 것을 swap(Double buffer), 그리기 함수에서 glFlush 대신 사용
	Sleep(10);

	DrawPnlDefNum();
	//DrawPnlDefNum();
	//m_arLine.RemoveAll();
}

void CSimpleOpengl::DrawClear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 컬러 버퍼: GL_COLOR_BUFFER_BIT, 깊이 버퍼: GL_DEPTH_BUFFER_BIT, 누적 버퍼: GL_ACCUM_BUFFER_BIT, 스텐실 버퍼: GL_STENCIL_BUFFER_BIT
	glFlush();											// 그림이 다 그렸졌다는 걸 알려줌.
	SwapBuffers(m_hDc);									// OpenGL이 완료한 그림을 새롭게 화면에 그린다 - 현재 buffer에 그려진 것과 Frame에 그려진 것을 swap(Double buffer), 그리기 함수에서 glFlush 대신 사용
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 컬러 버퍼: GL_COLOR_BUFFER_BIT, 깊이 버퍼: GL_DEPTH_BUFFER_BIT, 누적 버퍼: GL_ACCUM_BUFFER_BIT, 스텐실 버퍼: GL_STENCIL_BUFFER_BIT
	glFlush();											// 그림이 다 그렸졌다는 걸 알려줌.
	SwapBuffers(m_hDc);									// OpenGL이 완료한 그림을 새롭게 화면에 그린다 - 현재 buffer에 그려진 것과 Frame에 그려진 것을 swap(Double buffer), 그리기 함수에서 glFlush 대신 사용
}

void CSimpleOpengl::DrawClearColor(COLORREF color)
{
	glClearColor(GetRValue(color), GetGValue(color), GetBValue(color), 1.0);	// 바탕색을 지정, alpha 값 (1.0값으로 고정)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();											// 그림이 다 그렸졌다는 걸 알려줌.
	SwapBuffers(m_hDc);									// OpenGL이 완료한 그림을 새롭게 화면에 그린다 - 현재 buffer에 그려진 것과 Frame에 그려진 것을 swap(Double buffer), 그리기 함수에서 glFlush 대신 사용
	
	glClearColor(GetRValue(color), GetGValue(color), GetBValue(color), 1.0);	// 바탕색을 지정, alpha 값 (1.0값으로 고정)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();											// 그림이 다 그렸졌다는 걸 알려줌.
	SwapBuffers(m_hDc);									// OpenGL이 완료한 그림을 새롭게 화면에 그린다 - 현재 buffer에 그려진 것과 Frame에 그려진 것을 swap(Double buffer), 그리기 함수에서 glFlush 대신 사용
}

void CSimpleOpengl::SetClear()
{
	m_bDrawClear = TRUE;
}

void CSimpleOpengl::SetClearColor()
{
	m_bDrawClearColor = TRUE;
}

void CSimpleOpengl::SetDraw()
{
	m_bDraw = TRUE;
	//m_bDrawText = TRUE;
}

void CSimpleOpengl::SetFont(CString srtFntName, int nSize, BOOL bBold)
{
	if (m_bFont)
	{
		if (m_Font.DeleteObject())
			m_bFont = FALSE;
	}

	LOGFONT lfCtrl = { 0 };
	lfCtrl.lfOrientation = 0;
	lfCtrl.lfEscapement = 0;

	lfCtrl.lfHeight = nSize;
	lfCtrl.lfWeight = bBold ? FW_BOLD : FW_NORMAL;

	lfCtrl.lfItalic = FALSE;
	lfCtrl.lfUnderline = FALSE;
	lfCtrl.lfStrikeOut = FALSE;

	lfCtrl.lfCharSet = DEFAULT_CHARSET;
	lfCtrl.lfQuality = DEFAULT_QUALITY;
	lfCtrl.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lfCtrl.lfPitchAndFamily = DEFAULT_PITCH;
	_tcscpy(lfCtrl.lfFaceName, srtFntName);

	if (!m_bFont)
	{
		BOOL bCr = m_Font.CreateFontIndirect(&lfCtrl);
		if (bCr)
			m_bFont = TRUE;
	}
}


void CSimpleOpengl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	PopupMenu(nFlags, point);

	CStatic::OnRButtonDown(nFlags, point);
}
BOOL CSimpleOpengl::GetRngDrawPnl(int nDrawPnlIdx, tagStrPcs& StrPcs, CPoint& ptLT, CPoint& ptRB)
{
	int nWorldMargin = 6;	// [mm] = [dot]
	int nWorldStX = 3;		// [mm] = [dot]

	CRect rtDispCtrl;
	::GetClientRect(m_hCtrl, &rtDispCtrl);
	int nWorldW = rtDispCtrl.right - rtDispCtrl.left - nWorldMargin;
	int nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;

	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dPixelSize = 2.5; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	int i = 0, k = 0, nR = -1, nC = -1;
	int nTotPnl = 6, nSelMarkingPnl = 2;

	stVertex v1, v2;
	fData1 = (double)StrPcs.m_stFrameRgnPix[0].iStartX * mmPxl;	// left
	fData2 = (double)StrPcs.m_stFrameRgnPix[0].iStartY * mmPxl;	// top
	fData3 = (double)StrPcs.m_stFrameRgnPix[0].iEndX * mmPxl;	// right
	fData4 = (double)StrPcs.m_stFrameRgnPix[0].iEndY * mmPxl;	// bottom
	fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
	fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);

	fData1 += (double)nWorldStX;
	fData1 += dFrmSpace * nDrawPnlIdx;

	int nRealW = fWidth*nTotPnl + dFrmSpace*(nTotPnl - 1);// +(nWorldMargin + nWorldStX);
	if ((nWorldW - nRealW) < 0.0)
	{
		int nNeedX = nRealW - nWorldW;
		dScaleX = (double)((double)nWorldW / (double)(nWorldW + nNeedX));
		dFrmOffset[0] = 0.0;
	}
	else
		dFrmOffset[0] = (nWorldW - fWidth*nTotPnl) / 2.0;

	dDrawStPosX = fWidth * dScaleX * nDrawPnlIdx;

	if ((nWorldH - fHeight) < 0.0)
	{
		int nNeedY = fHeight - nWorldH;
		dScaleY = (double)((double)nWorldH / (double)(nWorldH + nNeedY));
		dFrmOffset[1] = 0.0;
	}
	else
		dFrmOffset[1] = (nWorldH - fHeight) / 2.0;

	fData1 += dFrmOffset[0] + dDrawStPosX;						// left
	fData2 += dFrmOffset[1];									// top

	fData3 = fData1 + fWidth * dScaleX;							// right
	fData4 = fData2 + fHeight * dScaleY;						// bottom
	fData2 -= dFrmMargin[1];									// top
	fData4 += dFrmMargin[3];									// bottom

	ptLT.x = fData1;											// left
	ptLT.y = fData2;											// top
	ptRB.x = fData3;											// right
	ptRB.y = fData4;											// bottom

	return TRUE;
}

void CSimpleOpengl::DrawStrPcs(tagStrPcs& StrPcs)
{
	CMyReelmapDlg* pParent = (CMyReelmapDlg*)m_pParent;

	COLORREF color = RGB_WHITE;
	int nWorldMargin = 6;	// [mm] = [dot]
	int nWorldStX = 3;		// [mm] = [dot]

	CRect rtDispCtrl;
	::GetClientRect(m_hCtrl, &rtDispCtrl);
	int nWorldW = rtDispCtrl.right - rtDispCtrl.left - nWorldMargin;
	int nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;

	int i = 0, k = 0, nR = -1, nC = -1;
	int nTotPnl = 6, nSelMarkingPnl = 2;
	//int nMkedPnl = nTotPnl - nSelMarkingPnl - 1; // 6-2-1 = 3
	//int nFromPnl = nSerial - nMkedPnl - 1; // 6-3-1 = 2

	int nTotStrip = StrPcs.m_nFrameRgnNum;
	int nTotPcs = StrPcs.m_nPieceRgnNum;

	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dPixelSize = 2.5; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;
	
	RemoveAllLine();
	for (k = 0; k < nTotPnl; k++) // Draw Panel from Left to Right. 
	{
		stVertex v1, v2;
		for (i = 0; i < nTotStrip; i++)
		{
			fData1 = (double)StrPcs.m_stFrameRgnPix[i].iStartX * mmPxl;	// left
			fData2 = (double)StrPcs.m_stFrameRgnPix[i].iStartY * mmPxl;	// top
			fData3 = (double)StrPcs.m_stFrameRgnPix[i].iEndX * mmPxl;	// right
			fData4 = (double)StrPcs.m_stFrameRgnPix[i].iEndY * mmPxl;	// bottom
			fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
			fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);

			fData1 += (double)nWorldStX;
			fData1 += dFrmSpace * k;

			if (!i)
			{
				int nRealW = fWidth*nTotPnl + dFrmSpace*(nTotPnl - 1);// +(nWorldMargin + nWorldStX);
				if ((nWorldW - nRealW) < 0.0)
				{
					int nNeedX = nRealW - nWorldW;
					dScaleX = (double)((double)nWorldW / (double)(nWorldW + nNeedX));
					dFrmOffset[0] = 0.0;
				}
				else
					dFrmOffset[0] = (nWorldW - fWidth*nTotPnl) / 2.0;

				dDrawStPosX = fWidth * dScaleX * k;

				if ((nWorldH - fHeight) < 0.0)
				{
					int nNeedY = fHeight - nWorldH;
					dScaleY = (double)((double)nWorldH / (double)(nWorldH + nNeedY));
					dFrmOffset[1] = 0.0;
				}
				else
					dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
			}
			
			fData1 += dFrmOffset[0] + dDrawStPosX;						// left
			fData2 += dFrmOffset[1];									// top

			fData3 = fData1 + fWidth * dScaleX;							// right
			fData4 = fData2 + fHeight * dScaleY;						// bottom
			fData2 -= dFrmMargin[1];									// top
			fData4 += dFrmMargin[3];									// bottom

			if (k == nSelMarkingPnl || k == nSelMarkingPnl+1)
				color = RGB_RED;
			else
				color = RGB_WHITE;

			v1.x = fData1; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2, color);

			v1.x = fData1; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2, color);

			v1.x = fData3; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2, color);

			v1.x = fData3; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2, color);
		}

		for (i = 0; i < nTotPcs; i++)
		{
			double fNeed = 0.0;
			pParent->GetMatrix(i, nR, nC);
			fData1 = (double)StrPcs.m_stPieceRgnPix[i].iStartX * mmPxl;	// left
			fData2 = (double)StrPcs.m_stPieceRgnPix[i].iStartY * mmPxl;	// top
			fData3 = (double)StrPcs.m_stPieceRgnPix[i].iEndX * mmPxl;	// right
			fData4 = (double)StrPcs.m_stPieceRgnPix[i].iEndY * mmPxl;	// bottom
			fWidth = (fData3 - fData1) * dScaleX;
			fHeight = (fData4 - fData2) * dScaleY;
			fNeed = (fData3 - fData1) - fWidth;

			fData1 += (double)nWorldStX;
			fData1 += dFrmSpace * k;
			fData1 += dFrmOffset[0];									// left
			fData2 += dFrmOffset[1];									// top

			fData1 += dDrawStPosX - (fNeed*(double)nC);					// left
			fData1 += dFrmMargin[0] * dScaleX;							// left

			fData3 = fData1 + fWidth;									// right
			fData4 = fData2 + fHeight;									// bottom

			v1.x = fData1; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2);

			v1.x = fData1; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2);

			v1.x = fData3; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2);

			v1.x = fData3; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2);
		}
	}
	Refresh();
}

void CSimpleOpengl::DrawTestText()
{
	// Draw the text
	CRect rect;
	::GetClientRect(m_hCtrl, &rect);
	CDC* pDC = m_pDc;
	CString strWnd, strText, strPars, strRem;
	GetWindowText(strWnd);
	strWnd = _T("OpenGL Test");
	strText = _T("OpenGL Test");

	HFONT hOldFont = NULL;
	//Sets the font
	if (m_Font.GetSafeHandle() != NULL)
	{
		hOldFont = (HFONT)pDC->SelectObject(m_Font.GetSafeHandle());
	}

	CPoint pt;
	CSize Extent = pDC->GetTextExtent(strText);
	pt.y = rect.CenterPoint().y - Extent.cy / 2;

	int nMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF crTextOld = pDC->SetTextColor(m_crText);

	int nPos, nLine = 0;
	CString strLineText[MAX_LINE];
	CPoint LinePos[MAX_LINE];

	nLine++;

	do
	{
		nPos = strText.Find(_T("_"), 0);
		if (nPos == 0)
		{
			strText.SetAt(nPos, _T(' '));
		}

		nPos = strText.Find(_T("&&"), 0);
		if (nPos >= 0)
			strText.Delete(nPos);

		nPos = strText.Find(_T("\\r"), 0);
		if (nPos > 0)
		{
			strPars = strText.Left(nPos);
			strRem = strText.Right(strText.GetLength() - nPos - 2);
			strLineText[nLine - 1] = strPars;
			strText = strRem;
			nLine++;
		}
		else
		{
			nPos = strText.Find(_T("\r"), 0);
			if (nPos > 0)
			{
				strPars = strText.Left(nPos);
				strRem = strText.Right(strText.GetLength() - nPos - 1);
				strLineText[nLine - 1] = strPars;
				strText = strRem;
				nLine++;
			}
		}

	} while (nPos > 0);

	strLineText[nLine - 1] = strText;
	LinePos[0].y = pt.y - (Extent.cy / 2 * (nLine - 1)) - (LINE_SPACE * (nLine - 1) / 2);

	for (int nL = 0; nL < nLine; nL++)
	{
		Extent = pDC->GetTextExtent(strLineText[nL]);
		LinePos[nL].x = rect.CenterPoint().x - Extent.cx / 2;

		if (nL > 0)
			LinePos[nL].y = LinePos[nL - 1].y + Extent.cy + LINE_SPACE;

		pDC->TextOut(LinePos[nL].x, LinePos[nL].y, strLineText[nL]);
	}

	pDC->SetBkMode(nMode);

	//Reset the old font
	if (hOldFont != NULL)
	{
		pDC->SelectObject(hOldFont);
	}
}

void CSimpleOpengl::DrawText(CString sText, CPoint ptPnt, COLORREF rgb)
{
	// Draw the text
	CRect rect;
	::GetClientRect(m_hCtrl, &rect);
	CDC* pDC = m_pDc;
	CString strPars, strRem;

	HFONT hOldFont = NULL;
	//Sets the font
	if (m_Font.GetSafeHandle() != NULL)
	{
		hOldFont = (HFONT)pDC->SelectObject(m_Font.GetSafeHandle());
	}

	int nMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF crTextOld = pDC->SetTextColor(rgb);
	int nLen = sText.GetLength();

	pDC->TextOut(ptPnt.x, ptPnt.y, sText);
	pDC->SetBkMode(nMode);

	//Reset the old font
	if (hOldFont != NULL)
	{
		pDC->SelectObject(hOldFont);
	}
}

void CSimpleOpengl::EraseText(CRect rect)
{
	CDC* pDC = m_pDc;
	CBrush brush, *Oldbrush;
	brush.CreateSolidBrush(RGB(0, 0, 0));
	Oldbrush = pDC->SelectObject(&brush);
	pDC->FillRect(rect, &brush);
	pDC->SelectObject(Oldbrush);
	brush.DeleteObject();
}

void CSimpleOpengl::DrawPnlDefNum(int nSerial, CArPcr& arPcr, tagStrPcs& StrPcs)
{
	int i, k;
	CString sPnlDefNum, sPnlNum;

	int nCount = arPcr.GetSize();

	if (nCount < 1)
		return;

	int nTotPnl = 6;
	int nSelMarkingPnl = 2; // Index start from right
	int *pEstimatedPnlNum = new int[nTotPnl];
	int *pPnlNum = new int[nTotPnl];
	int *pPnlDefNum = new int[nTotPnl];
	CPcr Pcr;
	int nPcrIdx = -1;
	for (i = 0; i < nCount; i++)
	{
		Pcr = arPcr.GetAt(i);
		if (Pcr.GetSerial() == nSerial)
		{
			nPcrIdx = i;
			break;
		}
	}
	if (nPcrIdx < 0)
	{
		AfxMessageBox(_T("선택한 시리얼에 대한 PCR 정보가 없습니다."));
		delete[] pEstimatedPnlNum;
		delete[] pPnlNum;
		delete[] pPnlDefNum;
		return;
	}

	CDC* pDC = m_pDc;
	CRect rt;
	int nDispPnlNum = -1;
	int nDispPcrIdx = -1;
	for (k = 0; k < nTotPnl; k++) // Index start from left
	{
		pEstimatedPnlNum[k] = nSerial - (nTotPnl - (nSelMarkingPnl + k + 1));
		nDispPcrIdx = nPcrIdx - (nTotPnl - (nSelMarkingPnl + k + 1));
		if (nDispPcrIdx >= 0 && nDispPcrIdx < nCount)
		{
			Pcr = arPcr[nDispPcrIdx];
			nDispPnlNum = Pcr.GetSerial();
			if (pEstimatedPnlNum[k] != nDispPnlNum)
				AfxMessageBox(_T("PCR의 시리얼 정보가 연속적이지 않습니다."));
			// Array for Disp PnlNum, PnlDefNum.
			pPnlNum[k] = nDispPnlNum;
			pPnlDefNum[k] = Pcr.GetTotalDef();
		}
		else
		{
			pPnlNum[k] = -1;
			pPnlDefNum[k] = -1;
		}

		if (pPnlDefNum[k] < 0)
		{
			sPnlDefNum.Format(_T(""));
			sPnlNum.Format(_T(""));
		}
		else
		{
			sPnlDefNum.Format(_T("Total: %d"), pPnlDefNum[k]);
			sPnlNum.Format(_T("%d"), pPnlNum[k]);
		}
		CSize Extent = pDC->GetTextExtent(sPnlDefNum);
		CPoint ptPnlDefLt, ptPnlDefRb, ptPnlDefPnt;
		CPoint ptPnlLt, ptPnlRb, ptPnlPnt;
		GetRngDrawPnl(k, StrPcs, ptPnlLt, ptPnlRb);
		ptPnlDefPnt.x = ptPnlLt.x + ((ptPnlRb.x - ptPnlLt.x) - Extent.cx) / 2;
		ptPnlDefPnt.y = ptPnlLt.y - Extent.cy - LINE_SPACE;

		Extent = pDC->GetTextExtent(sPnlNum);
		ptPnlPnt.x = ptPnlLt.x + ((ptPnlRb.x - ptPnlLt.x) - Extent.cx) / 2;
		ptPnlPnt.y = ptPnlRb.y + LINE_SPACE;

		rt.left = ptPnlLt.x; rt.top = ptPnlDefPnt.y;
		rt.right = ptPnlRb.x; rt.bottom = ptPnlDefPnt.y + Extent.cy;
		EraseText(rt);
		rt.left = ptPnlLt.x; rt.top = ptPnlPnt.y ;
		rt.right = ptPnlRb.x; rt.bottom = ptPnlRb.y + Extent.cy + LINE_SPACE;
		EraseText(rt);

		if (nSelMarkingPnl + 1 == nTotPnl - (k + 1))
		{
			DrawText(sPnlDefNum, ptPnlDefPnt, RGB_RED);
			DrawText(sPnlNum, ptPnlPnt, RGB_RED);
		}
		else if (nSelMarkingPnl == nTotPnl - (k + 1))
		{
			DrawText(sPnlDefNum, ptPnlDefPnt, RGB_RED);
			DrawText(sPnlNum, ptPnlPnt, RGB_RED);
		}
		else
		{
			DrawText(sPnlDefNum, ptPnlDefPnt, RGB_WHITE);
			DrawText(sPnlNum, ptPnlPnt, RGB_WHITE);
		}
	}

	delete[] pEstimatedPnlNum;
	delete[] pPnlNum;
	delete[] pPnlDefNum;

	m_nSerial = nSerial;
	m_arPcr = &arPcr;
	m_StrPcs = &StrPcs;
}

void CSimpleOpengl::DrawPnlDefNum()
{
	if (!m_arPcr || !m_StrPcs)
		return;

	int i, k;
	CString sPnlDefNum, sPnlNum;

	int nCount = m_arPcr->GetSize();

	if (nCount < 1)
		return;

	int nTotPnl = 6;
	int nSelMarkingPnl = 2; // Index start from right
	int *pEstimatedPnlNum = new int[nTotPnl];
	int *pPnlNum = new int[nTotPnl];
	int *pPnlDefNum = new int[nTotPnl];
	CPcr Pcr;
	int nPcrIdx = -1;
	for (i = 0; i < nCount; i++)
	{
		Pcr = m_arPcr->GetAt(i);
		if (Pcr.GetSerial() == m_nSerial)
		{
			nPcrIdx = i;
			break;
		}
	}
	if (nPcrIdx < 0)
	{
		AfxMessageBox(_T("선택한 시리얼에 대한 PCR 정보가 없습니다."));
		delete[] pEstimatedPnlNum;
		delete[] pPnlNum;
		delete[] pPnlDefNum;
		return;
	}

	CDC* pDC = m_pDc;
	CRect rt;
	int nDispPnlNum = -1;
	int nDispPcrIdx = -1;
	for (k = 0; k < nTotPnl; k++) // Index start from left
	{
		pEstimatedPnlNum[k] = m_nSerial - (nTotPnl - (nSelMarkingPnl + k + 1));
		nDispPcrIdx = nPcrIdx - (nTotPnl - (nSelMarkingPnl + k + 1));
		if (nDispPcrIdx >= 0 && nDispPcrIdx < nCount)
		{
			Pcr = m_arPcr->GetAt(nDispPcrIdx);
			nDispPnlNum = Pcr.GetSerial();
			if (pEstimatedPnlNum[k] != nDispPnlNum)
				AfxMessageBox(_T("PCR의 시리얼 정보가 연속적이지 않습니다."));
			// Array for Disp PnlNum, PnlDefNum.
			pPnlNum[k] = nDispPnlNum;
			pPnlDefNum[k] = Pcr.GetTotalDef();
		}
		else
		{
			pPnlNum[k] = -1;
			pPnlDefNum[k] = -1;
		}

		if (pPnlDefNum[k] < 0)
		{
			sPnlDefNum.Format(_T(""));
			sPnlNum.Format(_T(""));
		}
		else
		{
			sPnlDefNum.Format(_T("Total: %d"), pPnlDefNum[k]);
			sPnlNum.Format(_T("%d"), pPnlNum[k]);
		}
		CSize Extent = pDC->GetTextExtent(sPnlDefNum);
		CPoint ptPnlDefLt, ptPnlDefRb, ptPnlDefPnt;
		CPoint ptPnlLt, ptPnlRb, ptPnlPnt;
		GetRngDrawPnl(k, (tagStrPcs&)*m_StrPcs, ptPnlLt, ptPnlRb);
		ptPnlDefPnt.x = ptPnlLt.x + ((ptPnlRb.x - ptPnlLt.x) - Extent.cx) / 2;
		ptPnlDefPnt.y = ptPnlLt.y - Extent.cy - LINE_SPACE;

		Extent = pDC->GetTextExtent(sPnlNum);
		ptPnlPnt.x = ptPnlLt.x + ((ptPnlRb.x - ptPnlLt.x) - Extent.cx) / 2;
		ptPnlPnt.y = ptPnlRb.y + LINE_SPACE;

		rt.left = ptPnlLt.x; rt.top = ptPnlDefPnt.y;
		rt.right = ptPnlRb.x; rt.bottom = ptPnlDefPnt.y + Extent.cy;
		EraseText(rt);
		rt.left = ptPnlLt.x; rt.top = ptPnlPnt.y;
		rt.right = ptPnlRb.x; rt.bottom = ptPnlRb.y + Extent.cy + LINE_SPACE;
		EraseText(rt);

		if (nSelMarkingPnl + 1 == nTotPnl - (k + 1))
		{
			DrawText(sPnlDefNum, ptPnlDefPnt, RGB_RED);
			DrawText(sPnlNum, ptPnlPnt, RGB_RED);
		}
		else if (nSelMarkingPnl == nTotPnl - (k + 1))
		{
			DrawText(sPnlDefNum, ptPnlDefPnt, RGB_RED);
			DrawText(sPnlNum, ptPnlPnt, RGB_RED);
		}
		else
		{
			DrawText(sPnlDefNum, ptPnlDefPnt, RGB_WHITE);
			DrawText(sPnlNum, ptPnlPnt, RGB_WHITE);
		}
	}

	delete[] pEstimatedPnlNum;
	delete[] pPnlNum;
	delete[] pPnlDefNum;
}

void CSimpleOpengl::DrawPnlDef(int nSerial, CArPcr& arPcr, tagStrPcs& StrPcs)
{
	CMyReelmapDlg* pParent = (CMyReelmapDlg*)m_pParent;

	int nCount = arPcr.GetSize();
	if (nCount < 1)
		return;

	CRect rtDispCtrl;
	::GetClientRect(m_hCtrl, &rtDispCtrl);
	int nWorldMargin = 6;	// [mm] = [dot]
	int nWorldStX = 3;		// [mm] = [dot]
	int nWorldW = rtDispCtrl.right - rtDispCtrl.left - nWorldMargin;
	int nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;

	int nTotPnl = 6;
	int nSelMarkingPnl = 2; // Index start from right
	int *pEstimatedPnlNum = new int[nTotPnl];
	int *pPnlNum = new int[nTotPnl];
	int *pPnlDefNum = new int[nTotPnl];
	CPcr Pcr;
	int nPcrIdx = -1 , nTotPcs = -1;

	int i = 0, k = 0, nR = -1, nC = -1;
	stVertex v1, v2;
	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dPixelSize = 2.5; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	for (i = 0; i < nCount; i++)
	{
		Pcr = arPcr.GetAt(i);
		if (Pcr.GetSerial() == nSerial)
		{
			nPcrIdx = i;
			break;
		}
	}
	if (nPcrIdx < 0)
	{
		AfxMessageBox(_T("선택한 시리얼에 대한 PCR 정보가 없습니다."));
		delete[] pEstimatedPnlNum;
		delete[] pPnlNum;
		delete[] pPnlDefNum;
		return;
	}

	int nDispPnlNum = -1;
	int nDispPcrIdx = -1;
	RemoveAllRect();
	for (k = 0; k < nTotPnl; k++) // Index start from left
	{
		pEstimatedPnlNum[k] = nSerial - (nTotPnl - (nSelMarkingPnl + k + 1));
		nDispPcrIdx = nPcrIdx - (nTotPnl - (nSelMarkingPnl + k + 1));

		fData1 = (double)StrPcs.m_stFrameRgnPix[0].iStartX * mmPxl;	// left
		fData2 = (double)StrPcs.m_stFrameRgnPix[0].iStartY * mmPxl;	// top
		fData3 = (double)StrPcs.m_stFrameRgnPix[0].iEndX * mmPxl;	// right
		fData4 = (double)StrPcs.m_stFrameRgnPix[0].iEndY * mmPxl;	// bottom
		fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
		fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);

		fData1 += (double)nWorldStX;
		fData1 += dFrmSpace * k;

		int nRealW = fWidth*nTotPnl + dFrmSpace*(nTotPnl - 1);
		if ((nWorldW - nRealW) < 0.0)
		{
			int nNeedX = nRealW - nWorldW;
			dScaleX = (double)((double)nWorldW / (double)(nWorldW + nNeedX));
			dFrmOffset[0] = 0.0;
		}
		else
			dFrmOffset[0] = (nWorldW - fWidth*nTotPnl) / 2.0;

		dDrawStPosX = fWidth * dScaleX * k;

		if ((nWorldH - fHeight) < 0.0)
		{
			int nNeedY = fHeight - nWorldH;
			dScaleY = (double)((double)nWorldH / (double)(nWorldH + nNeedY));
			dFrmOffset[1] = 0.0;
		}
		else
			dFrmOffset[1] = (nWorldH - fHeight) / 2.0;

		if (nDispPcrIdx >= 0)
		{
			if (nDispPcrIdx < nCount)
			{
				Pcr = arPcr[nDispPcrIdx];
				nDispPnlNum = Pcr.GetSerial();
				if (pEstimatedPnlNum[k] != nDispPnlNum)
					AfxMessageBox(_T("PCR의 시리얼 정보가 연속적이지 않습니다."));

				nTotPcs = Pcr.GetTotalDef();
				for (i = 0; i < nTotPcs; i++)
				{
					int nPcsId = Pcr.GetPcsId(i);
					int nDefCode = Pcr.GetDefCode(i);
					double fNeed = 0.0;
					pParent->GetMatrix(nPcsId, nR, nC);
					fData1 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartX * mmPxl;	// left
					fData2 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartY * mmPxl;	// top
					fData3 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndX * mmPxl;	// right
					fData4 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndY * mmPxl;	// bottom
					fWidth = (fData3 - fData1) * dScaleX;
					fHeight = (fData4 - fData2) * dScaleY;
					fNeed = (fData3 - fData1) - fWidth;

					fData1 += (double)nWorldStX;
					fData1 += dFrmSpace * k;
					fData1 += dFrmOffset[0];									// left
					fData2 += dFrmOffset[1];									// top

					fData1 += dDrawStPosX - (fNeed*(double)nC);					// left
					fData1 += dFrmMargin[0] * dScaleX;							// left

					fData3 = fData1 + fWidth;									// right
					fData4 = fData2 + fHeight;									// bottom

					v1.x = fData1; v1.y = fData2; v1.z = 0.0;
					v2.x = fData3; v2.y = fData4; v2.z = 0.0;
					AddRect(v1, v2, pParent->GetDefColor(nDefCode));			//m_rgbDef[nDefCode]);
				}
			}
		}
		else
		{
			pPnlNum[k] = -1;
			pPnlDefNum[k] = -1;
		}
	}

	m_nSerial = nSerial;
	m_arPcr = &arPcr;
	m_StrPcs = &StrPcs;

	delete[] pEstimatedPnlNum;
	delete[] pPnlNum;
	delete[] pPnlDefNum;
}
