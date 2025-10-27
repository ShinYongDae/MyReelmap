// SimpleOpengl.cpp : ���� �����Դϴ�.
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

	m_dDispPcsWidth = 0.0;
	m_dDispPcsHeight = 0.0;

	m_nItsOrgCase = 0;

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

	m_dDispPcsWidth = 0.0;
	m_dDispPcsHeight = 0.0;

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
	RemoveAllCross();
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

void CSimpleOpengl::RemoveAllCross()
{
	int nTotal = m_arCross.GetCount();
	if (nTotal > 0)
	{
		m_arCross.RemoveAll();
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



// CSimpleOpengl �޽��� ó�����Դϴ�.

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
// �׸��� ǥ���� �ȼ� ������ ����
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
��										�ǹ�
=================================================================================================================================================
PFD_DRAW_TO_WINDOW						���۴� â �Ǵ� ����̽� ȭ�鿡 �׸� �� �ֽ��ϴ�.
0x00000004

PFD_DRAW_TO_BITMAP						���۴� �޸� ��Ʈ�ʿ� �׸� �� �ֽ��ϴ�.
0x00000008

PFD_SUPPORT_GDI							���۴� GDI �׸��⸦ �����մϴ�. �� �÷��� �� PFD_DOUBLEBUFFER ���� ���׸� �������� ��ȣ ��Ÿ���Դϴ�.
0x00000010

PFD_SUPPORT_OPENGL						���۴� OpenGL �׸��⸦ �����մϴ�.
0x00000020

PFD_GENERIC_ACCELERATED					�ȼ� ������ ���׸� ������ ����ȭ�ϴ� ����̽� ����̹����� �����˴ϴ�. �� �÷��װ� ��Ȯ�ϰ� PFD_GENERIC_FORMAT �÷��װ� ������ ��� �ȼ� ������ ���׸� ���������� �����˴ϴ�.
0x00001000

PFD_GENERIC_FORMAT						�ȼ� ������ ���׸� �����̶�� �ϴ� GDI ����Ʈ���� �������� �����˴ϴ�. �� ��Ʈ�� ��Ȯ�� ��� �ȼ� ������ ����̽� ����̹� �Ǵ� �ϵ����� �����˴ϴ�.
0x00000040

PFD_NEED_PALETTE						���۴� �ȷ�Ʈ ���� ����̽����� RGBA �ȼ��� ����մϴ�. �� �ȼ� ���Ŀ� ���� �ֻ��� ����� �������� ���� �ȷ�Ʈ�� �ʿ��մϴ�. ����ǥ�� ���� cRedBits, cRedShift, cGreenBits, cGreenShift, cBluebits �� cBlueShift ����� ���� ���� �����Ǿ�� �մϴ�. wglMakeCurrent�� ȣ���ϱ� ���� ����̽� ���ؽ�Ʈ���� �ȷ�Ʈ�� ����� �����ؾ� �մϴ�.
0x00000080

PFD_NEED_SYSTEM_PALETTE					256�� ��忡���� �ϳ��� �ϵ���� �ȷ�Ʈ�� �����ϴ� �ϵ������ �ȼ� ���� �����ڿ� ���ǵ˴ϴ�. �̷��� �ý��ۿ��� �ϵ���� ������ ����Ϸ��� RGBA ��忡�� �ϵ���� �ȷ�Ʈ�� ���� ����(��: 3-3-2)�̰ų� �� �ε��� ����� �� �� �ȷ�Ʈ�� ��ġ�ؾ� �մϴ�. �� �÷��װ� �����Ǹ� ���α׷����� SetSystemPaletteUse �� ȣ���Ͽ� �� �ȷ�Ʈ�� �ý��� �ȷ�Ʈ�� �ϴ��� ������ �����ؾ� �մϴ�. OpenGL �ϵ��� ���� �ϵ���� �ȷ�Ʈ�� �����ϰ� ����̽� ����̹��� OpenGL�� ���� �ϵ���� �ȷ�Ʈ�� �Ҵ��� �� �ִ� ��� �� �÷��״� �Ϲ������� ��Ȯ�մϴ�.
0x00000100								�� �÷��״� ���׸� �ȼ� �������� �������� �ʽ��ϴ�.

PFD_DOUBLEBUFFER						���۰� ���� ���۸��˴ϴ�. �� �÷��� �� PFD_SUPPORT_GDI ���� ���׸� �������� ��ȣ ��Ÿ���Դϴ�.
0x00000001

PFD_STEREO								���۴� ��ü�Դϴ�. �� �÷��״� ���� ���׸� �������� �������� �ʽ��ϴ�.
0x00000002

PFD_SWAP_LAYER_BUFFERS					����̽��� ���� ���۸� �������� �Ǵ� ������� ����� �����ϴ� �ȼ� �������� ���� ���̾� ����� ��ȯ�� �� �ִ��� ���θ� ��Ÿ���ϴ�. �׷��� ������ ��� ���̾� ����� �׷����� �Բ� ��ȯ�˴ϴ�. �� �÷��װ� �����Ǹ� wglSwapLayerBuffers �� �����˴ϴ�.
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
			PFD_DOUBLEBUFFER,				// ���� ���� ������ (default: �̱� ���� ������)
			//PFD_DOUBLEBUFFER_DONTCARE,
			PFD_TYPE_RGBA,					// RGBA ��� (default)
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

		int nPixelFormat = ChoosePixelFormat(m_hDc, &pfd);	// DC�� �����ϴ� �־��� �ȼ� ���� ���� ��ġ�ϴ� ������ �ȼ� ������ ã�´�.
		VERIFY(SetPixelFormat(m_hDc, nPixelFormat, &pfd));	// HGLRC(GL Rendering Context Handle)�� DC�� ����

		m_hRC = wglCreateContext(m_hDc);					// Rendering Context ����
		VERIFY(wglMakeCurrent(m_hDc, m_hRC));				// ���� �����忡 Rendering Context ����

		int argc = 1;
		char *argv[1] = { (char*)"Something" };
		glutInit(&argc, argv);								// Window OS�� Session ���� / GLUT Library�� �ʱ�ȭ

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
		glFlush();											// �׸��� �� �׷����ٴ� �� �˷���.
		SwapBuffers(m_hDc);								// OpenGL�� �Ϸ��� �׸��� ���Ӱ� ȭ�鿡 �׸��� - ���� buffer�� �׷��� �Ͱ� Frame�� �׷��� ���� swap(Double buffer), �׸��� �Լ����� glFlush ��� ���
	}
}

void CSimpleOpengl::PopupMenu(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, (UINT)10001, _T("��ǥ����ȭ"));
	menu.AppendMenu(MF_STRING, (UINT)10002, _T("�ð�����ȭ"));
	menu.AppendMenu(MF_STRING, (UINT)10003, _T("��������ȭ"));
	//CRect rect;
	//::GetClientRect(m_hCtrl, &rect);
	CPoint ptMenu = point;
	ClientToScreen(&ptMenu);   //��ũ�� �������� ��ǥ ��ȯ
	UINT nFlagsForMenu = TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD;
	//UINT nFlagsForMenu = TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD;
	/*
	=======================================================================================================
	nFlags:
	=======================================================================================================
	TPM_CENTERALIGN		������ ��ġ�� �߾ӿ� �����Ѵ�.
	TPM_LEFTALIGN		������ ��ġ�� ���� ����
	TPM_RIGHTALIGN		������ ��ġ�� ������ ����
	TPM_BOTTOMALIGN		������ ��ġ�� �ٴڿ� ����.
	TPM_TOPALIGN		������ ��ġ�� ���ʿ� ����.
	TPM_VCENTERALIGN	������ ��ġ�� �����߾� ����.
	TPM_LEFTBUTTON		���콺 ���� ��ư�� ���� �˾��޴��� �����Ѵ�.
	TPM_RIGHTBUTTON		���콺 ������ ��ư�� ���� �˾��޴��� �����Ѵ�.
	TPM_RETURNCMD		TrackPopupMenu �Լ��� ��ȯ�ϴ� �ڷ����� BOOL ���̹Ƿ� ���������� int�� ó���ȴ�.
						�׷��Ƿ� TPM_RETURNCMD�� �����Ѱ�� int������ ��޴��� �����ߴ��� Ȯ���Ҽ��ִ�.
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

	// OpenGL�� ������ ��ǥ����. (default: ȭ�� �߾��� ����, ��ǥ�� ������ [-1.0 ~ 1.0])
	glViewport(0, 0, width, height);								// OpenGL��ǥ (0, 0)-���ϴܿ��� width, height �������� ������ ��. : Viewport = Screen
	glMatrixMode(GL_PROJECTION);									// ������ ��� ��带 ����.
	glLoadIdentity();												// ������ ��ȯ ����� ������ķ� ����. (�� ��ǥ�� = ���� ��ǥ�� = ���� ��ǥ��) : ���ó���� �̷�� ���� ���� ��ǥ�踦 �ʱ�ȭ ��.
	glOrtho(0.0, width, height, 0.0, -2, 2);						// Viewport ��ǥ���� ������ ���� (min_x, max_x, min_y, max_y, near, far) : (left, right, bottom, top, near, far)
	
	/*
	=============================================================================
	GLenum mode
	=============================================================================
	GL_MODELVIEW	:	�𵨸� �� ���� ��ȯ ��� (��ü �̵� ��)
						����Ʈ ���� GL_MODELVIEW �� ������.
	GL_PROJECTION	:	���� ��ȯ ��� (Ŭ���� ���A ����)
	GL_TEXTURE		:	�ؽ�ó ���� ��� (�ؽ�ó ���� ����)
	=============================================================================
	*/
	glMatrixMode(GL_MODELVIEW);										// ������ ��� ��带 ����.
	glLoadIdentity();												// ������ ��ȯ ����� ������ķ� ����. (�� ��ǥ�� = ���� ��ǥ�� = ���� ��ǥ��) : ���ó���� �̷�� ���� ���� ��ǥ�踦 �ʱ�ȭ ��.

	gluPerspective(0.0, 1.0, 1.0, 1.0);							// ���� ������ ����ϴ� ���: fovy: ���� ������ ���̴� ���� (y�� ����) , aspect: ��Ⱦ�� (������ Ŭ���� ����� ��(w)�� ����(h)�� ���� ��), zNear, zFar
}

void CSimpleOpengl::DrawBegin(int nMode, int nSize, COLORREF color)
{
	if (nMode < Opengl::modPoint || nMode > Opengl::modCircleF)
		return;

	glPushMatrix();																// Martrix���¸� ���ÿ� �ִ´�.		
	GLfloat red = GetRValue(color) / 255.0;
	GLfloat green = GetGValue(color) / 255.0;
	GLfloat blue = GetBValue(color) / 255.0;
	GLfloat alpa = 0.0;
	glColor4f(red, green, blue, alpa);		// drawing color�� �����Ѵ�.

	/* glBegin ~ glEnd ��� ���̿� ��ġ���� ������ �������� ������
	���				����
	============================================================
	GL_POINTS			�������� ��
	GL_LINE_STRIP		����� ����
	GL_LINE_LOOP		�������� ������ ���� ����
	GL_LINES			�ΰ��� �������� ���� ����
	GL_TRIANGLES		������ ������ �ﰢ��
	GL_TRIANGLE_STRIP	����� �ﰢ��
	GL_TRIANGLE_FAN		�߽��� �����ϴ� �ﰢ��
	GL_QUADS			���� 4������ �����Ͽ� �簢���� �׸���.
	GL_QUAD_STRIP		����� �簢��
	GL_POLYGON			����� ���� �ٰ���
	============================================================*/
	
	switch (nMode)
	{
	case Opengl::modPoint:
		glPointSize(nSize);				// nSize : ���� ũ�� (0.5���� 10.0���� ����, ���� ������ 0.125 �̻�, default : 1.0) - glBegin() - glEnd() �ۿ��� ���
		glBegin(GL_POINTS);
		break;
	case Opengl::modLine:
	case Opengl::modRectE:
	case Opengl::modCross:
	case Opengl::modCrossX:
		glLineWidth(nSize);				// nSize : ���� ���� (0.5���� 10.0���� ����, ���� ������ 0.125 �̻�, default : 1.0) - PointSize/LineWidth 0.0���� Ŀ����.
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
		glBegin(GL_POLYGON);			// �������� �ݽð� �������� �׸� ���� �ո�
		break;
	}
}

void CSimpleOpengl::DrawEnd()
{
	glEnd();
	glPopMatrix();	// Martrix���¸� ���ÿ��� ������
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
	glColor3f(color.R, color.G, color.B);			// ���� �����ϴ� �Լ��ν� �������� �������� �ʴ´�.
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

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
	if (CTLCOLOR_STATIC == nCtlColor)
	{
		;
	}
	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}

void CSimpleOpengl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (CTLCOLOR_STATIC == nIDCtl)
	{
		;
	}

	CStatic::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSimpleOpengl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CStatic::OnPaint()��(��) ȣ������ ���ʽÿ�.

	m_bDraw = TRUE;
	//m_bDrawText = TRUE;
}

void CSimpleOpengl::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CSimpleOpengl::AddLine(stVertex v1, stVertex v2, int nSize, COLORREF color)
{
	stLine _line;
	_line.v1 = v1;
	_line.v2 = v2;
	_line.color = color;
	_line.nSize = nSize;
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

void CSimpleOpengl::AddCross(stVertex v1, stVertex v2, COLORREF color)
{
	stCross _cross;
	_cross.v1 = v1;
	_cross.v2 = v2;
	_cross.color = color;
	m_arCross.Add(_cross);
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
			DrawBegin(Opengl::modLine, _line.nSize, _line.color);
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

	nTotal = m_arCross.GetCount();
	if (nTotal > 0)
	{
		stCross _cross;
		for (i = 0; i < nTotal; i++)
		{
			_cross = m_arCross.GetAt(i);
			DrawBegin(Opengl::modLine, 2, _cross.color);
			DrawCross(_cross.v1, _cross.v2);
			DrawEnd();
		}
	}

	glFlush();											// �׸��� �� �׷����ٴ� �� �˷���. ��� ��ɾ ����ǰ� ��.
	SwapBuffers(m_hDc);									// OpenGL�� �Ϸ��� �׸��� ���Ӱ� ȭ�鿡 �׸��� - ���� buffer�� �׷��� �Ͱ� Frame�� �׷��� ���� swap(Double buffer), �׸��� �Լ����� glFlush ��� ���
	Sleep(30);

	DrawPnlDefNum();
	//DrawPnlDefNum();
	//m_arLine.RemoveAll();
}

void CSimpleOpengl::DrawClear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �÷� ����: GL_COLOR_BUFFER_BIT, ���� ����: GL_DEPTH_BUFFER_BIT, ���� ����: GL_ACCUM_BUFFER_BIT, ���ٽ� ����: GL_STENCIL_BUFFER_BIT
	glFlush();											// �׸��� �� �׷����ٴ� �� �˷���.
	SwapBuffers(m_hDc);									// OpenGL�� �Ϸ��� �׸��� ���Ӱ� ȭ�鿡 �׸��� - ���� buffer�� �׷��� �Ͱ� Frame�� �׷��� ���� swap(Double buffer), �׸��� �Լ����� glFlush ��� ���
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �÷� ����: GL_COLOR_BUFFER_BIT, ���� ����: GL_DEPTH_BUFFER_BIT, ���� ����: GL_ACCUM_BUFFER_BIT, ���ٽ� ����: GL_STENCIL_BUFFER_BIT
	glFlush();											// �׸��� �� �׷����ٴ� �� �˷���.
	SwapBuffers(m_hDc);									// OpenGL�� �Ϸ��� �׸��� ���Ӱ� ȭ�鿡 �׸��� - ���� buffer�� �׷��� �Ͱ� Frame�� �׷��� ���� swap(Double buffer), �׸��� �Լ����� glFlush ��� ���
}

void CSimpleOpengl::DrawClearColor(COLORREF color)
{
	glClearColor(GetRValue(color), GetGValue(color), GetBValue(color), 1.0);	// �������� ����, alpha �� (1.0������ ����)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();											// �׸��� �� �׷����ٴ� �� �˷���.
	SwapBuffers(m_hDc);									// OpenGL�� �Ϸ��� �׸��� ���Ӱ� ȭ�鿡 �׸��� - ���� buffer�� �׷��� �Ͱ� Frame�� �׷��� ���� swap(Double buffer), �׸��� �Լ����� glFlush ��� ���
	
	glClearColor(GetRValue(color), GetGValue(color), GetBValue(color), 1.0);	// �������� ����, alpha �� (1.0������ ����)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();											// �׸��� �� �׷����ٴ� �� �˷���.
	SwapBuffers(m_hDc);									// OpenGL�� �Ϸ��� �׸��� ���Ӱ� ȭ�鿡 �׸��� - ���� buffer�� �׷��� �Ͱ� Frame�� �׷��� ���� swap(Double buffer), �׸��� �Լ����� glFlush ��� ���
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	double dDispWidth, dDispHeight;
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	double dDispPcsScaleStY = 1.0;	// for Display Def Num and Serial Num.

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

		dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
	}
	else
	{
		dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
		dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
	}

	fData1 += dFrmOffset[0] + dDrawStPosX;						// left
	fData2 += dFrmOffset[1];									// top

	dDispWidth = fWidth * dScaleX;
	dDispHeight = fHeight * dScaleY;

	//fData3 = fData1 + fWidth * dScaleX;						// right
	//fData4 = fData2 + fHeight * dScaleY;						// bottom
	fData3 = fData1 + dDispWidth;								// right
	fData4 = fData2 + dDispHeight;								// bottom
	fData2 -= dFrmMargin[1];									// top
	fData4 += dFrmMargin[3];									// bottom

	fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE;					// for Display Total def and Serial Num
	fData4 -= nWorldH * RMAP_SERIAL_NUM_SCALE;					// for Display Total def and Serial Num

	ptLT.x = fData1;											// left
	ptLT.y = fData2;											// top
	ptRB.x = fData3;											// right
	ptRB.y = fData4;											// bottom

	return TRUE;
}

void CSimpleOpengl::DrawStrPcs(tagStrPcs& StrPcs)
{
	// ȭ���� ���ȼ��� ���� ������ 1mm�� �����Ͽ� ������� ��.
	CMyReelmapDlg* pParent = (CMyReelmapDlg*)m_pParent;

	CString sMsg;
	COLORREF color = RGB_WHITE;
	int nSize = 1;
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
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	double dDispWidth, dDispHeight;
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	double dDispPcsScaleStY = 1.0;	// for Display Def Num and Serial Num.

	RemoveAllLine();

	dPcsScaleY = GetPcsScaleY(StrPcs);
	dDispPcsScaleStY = GetDispPcsScaleStY(StrPcs);


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

					dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
				}
				else
				{
					dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
					dPcsScaleY = (double)( (double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH );
				}
			}
			
			fData1 += dFrmOffset[0] + dDrawStPosX;						// left
			fData2 += dFrmOffset[1];									// top

			dDispWidth = fWidth * dScaleX;
			dDispHeight = fHeight * dScaleY;

			fData3 = fData1 + dDispWidth;								// right
			fData4 = fData2 + dDispHeight;								// bottom
			fData2 -= dFrmMargin[1];									// top
			fData4 += dFrmMargin[3];									// bottom

			fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE;					// for Display Total def and Serial Num
			fData4 -= nWorldH * RMAP_SERIAL_NUM_SCALE;					// for Display Total def and Serial Num

			if (k == nSelMarkingPnl || k == nSelMarkingPnl + 1)
			{
				color = RGB_RED;
				nSize = 2;
			}
			else
			{
				color = RGB_WHITE;
				nSize = 1;
			}

			fData2 -= dDispPcsScaleStY * 2.0;							// for Display Total def and Serial Num
			fData4 -= dDispPcsScaleStY;									// for Display Total def and Serial Num

			//// ITS Data�� ���� ����
			////v1.x = fData1; v1.y = fData2; v1.z = 0.0;	// (left , top) of frame
			////v2.x = fData3; v2.y = fData4; v2.z = 0.0;	// (right , bottom) of frame
			//int nNodeX = StrPcs.m_nMaxCol;
			//int nNodeY = StrPcs.m_nMaxRow / MAX_STRIP;
			//
			//// case 0 : (left , top) of frame
			//v1.x = fData1; v1.y = fData2; v1.z = 0.0;
			//v2.x = fData1 + (m_dDispPcsWidth*1.5); v2.y = fData2 + (m_dDispPcsHeight*1.5); v2.z = 0.0;
			//m_stRectOrg[0].v1 = v1;
			//m_stRectOrg[0].v2 = v2;
			//m_stRectOrg[0].color = RGB_YELLOW;
			//
			//// case 1 : (left , bottom) of frame
			//v1.x = fData1; v1.y = fData4 - (m_dDispPcsHeight*(nNodeY + 0.5)); v1.z = 0.0;
			//v2.x = fData1 + (m_dDispPcsWidth*1.5); v2.y = fData4 - (m_dDispPcsHeight*(nNodeY - 1)); v2.z = 0.0;
			//m_stRectOrg[1].v1 = v1;
			//m_stRectOrg[1].v2 = v2;
			//m_stRectOrg[1].color = RGB_YELLOW;
			//
			//// case 2 : (right , top) of frame
			//v1.x = fData3 - (m_dDispPcsWidth*1.5); v1.y = fData2 + (m_dDispPcsHeight*(nNodeY - 1)); v1.z = 0.0;
			//v2.x = fData3; v2.y = fData2 + (m_dDispPcsHeight*(nNodeY + 0.5)); v2.z = 0.0;
			//m_stRectOrg[2].v1 = v1;
			//m_stRectOrg[2].v2 = v2;
			//m_stRectOrg[2].color = RGB_YELLOW;
			//
			//// case 3 : (right , bottom) of frame
			//v1.x = fData3 - (m_dDispPcsWidth*1.5); v1.y = fData4 - (m_dDispPcsHeight*(nNodeY + 0.5)); v1.z = 0.0;
			//v2.x = fData3; v2.y = fData4; v2.z = 0.0;
			//m_stRectOrg[2].v1 = v1;
			//m_stRectOrg[2].v2 = v2;
			//m_stRectOrg[2].color = RGB_YELLOW;

			// Add Frame Region
			v1.x = fData1; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);

			v1.x = fData1; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);

			v1.x = fData3; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);

			v1.x = fData3; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);
		}


		for (i = 0; i < nTotPcs; i++)
		{
			double fNeed = 0.0;
			if(!pParent->GetMatrix(i, nR, nC))
			{
				sMsg.Format(_T("Fail to GetMatrix of PcsId(%d)!!!"), i);
				AfxMessageBox(sMsg);
				return;
			}

			fData1 = (double)StrPcs.m_stPieceRgnPix[i].iStartX * mmPxl;	// left
			fData2 = (double)StrPcs.m_stPieceRgnPix[i].iStartY * mmPxl;	// top
			fData3 = (double)StrPcs.m_stPieceRgnPix[i].iEndX * mmPxl;	// right
			fData4 = (double)StrPcs.m_stPieceRgnPix[i].iEndY * mmPxl;	// bottom

			fWidth = (fData3 - fData1) * dScaleX * RMAP_PCS_SCALE;
			fHeight = (fData4 - fData2) * dPcsScaleY * RMAP_PCS_SCALE;
			fNeed = (fData3 - fData1) - fWidth;

			dDispPcsScaleStY = (fData4 - fData2) * (1.0 - dPcsScaleY * RMAP_PCS_SCALE);
			fData2 *= dPcsScaleY;
			fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE - dDispPcsScaleStY;

			fData1 += (double)nWorldStX;
			fData1 += dFrmSpace * k;
			fData1 += dFrmOffset[0];									// left
			fData2 += dFrmOffset[1];									// top

			fData1 += dDrawStPosX - (fNeed*(double)nC);					// left
			fData1 += dFrmMargin[0] * dScaleX;							// left

			fData3 = fData1 + fWidth;									// right
			fData4 = fData2 + fHeight;									// bottom

			//fData4 = fData2 + fHeight;								// bottom
			//fData3 -= fWidth * (1.0 - RMAP_PCS_SCALE);				// for Display Total def and Serial Num
			//fData4 -= fHeight * (1.0 - RMAP_PCS_SCALE);				// for Display Total def and Serial Num
			
			if(m_dDispPcsWidth < fWidth)
				m_dDispPcsWidth = fWidth;
			if (m_dDispPcsHeight < fHeight)
				m_dDispPcsHeight = fHeight;

			// ITS Data�� ���� ����
			v1.x = fData1; v1.y = fData2; v1.z = 0.0;	// (left , top) of PCS
			v2.x = fData3; v2.y = fData4; v2.z = 0.0;	// (right , bottom) of PCS
			int nCase = SetItsOrgRgn(nR, nC, v1, v2, StrPcs);

			int nRectOrgOffset = 0;
			if (IsInRectOrg(v1, v2) && nCase > -1)
			{
				color = RGB_YELLOW;
				nSize = 4;
				nRectOrgOffset = 1;
				fData4 -= nRectOrgOffset;
			}
			else
			{
				color = RGB_WHITE;
				nSize = 1;
			}

			// Add PCS Region
			v1.x = fData1; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);	// Left

			v1.x = fData1; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData4; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);	// Bottom

			v1.x = fData3; v1.y = fData4; v1.z = 0.0;
			v2.x = fData3; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);	// Right

			v1.x = fData3; v1.y = fData2; v1.z = 0.0;
			v2.x = fData1; v2.y = fData2; v2.z = 0.0;
			AddLine(v1, v2, nSize, color);	// Top
		}


		//for (i = 0; i < nTotStrip; i++)
		//{
		//	fData1 = (double)StrPcs.m_stFrameRgnPix[i].iStartX * mmPxl;	// left
		//	fData2 = (double)StrPcs.m_stFrameRgnPix[i].iStartY * mmPxl;	// top
		//	fData3 = (double)StrPcs.m_stFrameRgnPix[i].iEndX * mmPxl;	// right
		//	fData4 = (double)StrPcs.m_stFrameRgnPix[i].iEndY * mmPxl;	// bottom
		//	fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
		//	fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);
		//
		//	fData1 += (double)nWorldStX;
		//	fData1 += dFrmSpace * k;
		//
		//	if (!i)
		//	{
		//		int nRealW = fWidth*nTotPnl + dFrmSpace*(nTotPnl - 1);// +(nWorldMargin + nWorldStX);
		//		if ((nWorldW - nRealW) < 0.0)
		//		{
		//			int nNeedX = nRealW - nWorldW;
		//			dScaleX = (double)((double)nWorldW / (double)(nWorldW + nNeedX));
		//			dFrmOffset[0] = 0.0;
		//		}
		//		else
		//			dFrmOffset[0] = (nWorldW - fWidth*nTotPnl) / 2.0;
		//
		//		dDrawStPosX = fWidth * dScaleX * k;
		//
		//		if ((nWorldH - fHeight) < 0.0)
		//		{
		//			int nNeedY = fHeight - nWorldH;
		//			dScaleY = (double)((double)nWorldH / (double)(nWorldH + nNeedY));
		//			dFrmOffset[1] = 0.0;
		//
		//			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
		//		}
		//		else
		//		{
		//			dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
		//			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
		//		}
		//	}
		//
		//	fData1 += dFrmOffset[0] + dDrawStPosX;						// left
		//	fData2 += dFrmOffset[1];									// top
		//
		//	dDispWidth = fWidth * dScaleX;
		//	dDispHeight = fHeight * dScaleY;
		//
		//	fData3 = fData1 + dDispWidth;								// right
		//	fData4 = fData2 + dDispHeight;								// bottom
		//	fData2 -= dFrmMargin[1];									// top
		//	fData4 += dFrmMargin[3];									// bottom
		//
		//	fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE;					// for Display Total def and Serial Num
		//	fData4 -= nWorldH * RMAP_SERIAL_NUM_SCALE;					// for Display Total def and Serial Num
		//
		//	fData2 -= dDispPcsScaleStY * 2.0;							// for Display Total def and Serial Num
		//	fData4 -= dDispPcsScaleStY;									// for Display Total def and Serial Num
		//
		//	if (k == nSelMarkingPnl || k == nSelMarkingPnl + 1)
		//		color = RGB_RED;
		//	else
		//		color = RGB_WHITE;
		//
		//
		//	//// ITS Data�� ���� ����
		//	////v1.x = fData1; v1.y = fData2; v1.z = 0.0;	// (left , top) of frame
		//	////v2.x = fData3; v2.y = fData4; v2.z = 0.0;	// (right , bottom) of frame
		//	//int nNodeX = StrPcs.m_nMaxCol;
		//	//int nNodeY = StrPcs.m_nMaxRow / MAX_STRIP;
		//
		//	//// case 0 : (left , top) of frame
		//	//v1.x = fData1; v1.y = fData2; v1.z = 0.0;
		//	//v2.x = fData1 + (m_dDispPcsWidth*1.5); v2.y = fData2 + (m_dDispPcsHeight*1.5); v2.z = 0.0;
		//	//m_stRectOrg[0].v1 = v1;
		//	//m_stRectOrg[0].v2 = v2;
		//	//m_stRectOrg[0].color = RGB_YELLOW;
		//
		//	//// case 1 : (left , bottom) of frame
		//	//v1.x = fData1; v1.y = fData4 - (m_dDispPcsHeight*(nNodeY + 0.5)); v1.z = 0.0;	
		//	//v2.x = fData1 + (m_dDispPcsWidth*1.5); v2.y = fData4 - (m_dDispPcsHeight*(nNodeY - 1)); v2.z = 0.0;
		//	//m_stRectOrg[1].v1 = v1;
		//	//m_stRectOrg[1].v2 = v2;
		//	//m_stRectOrg[1].color = RGB_YELLOW;
		//
		//	//// case 2 : (right , top) of frame
		//	//v1.x = fData3 - (m_dDispPcsWidth*1.5); v1.y = fData2 + (m_dDispPcsHeight*(nNodeY-1)); v1.z = 0.0;	
		//	//v2.x = fData3; v2.y = fData2 + (m_dDispPcsHeight*(nNodeY+0.5)); v2.z = 0.0;
		//	//m_stRectOrg[2].v1 = v1;
		//	//m_stRectOrg[2].v2 = v2;
		//	//m_stRectOrg[2].color = RGB_YELLOW;
		//
		//	//// case 3 : (right , bottom) of frame
		//	//v1.x = fData3 - (m_dDispPcsWidth*1.5); v1.y = fData4 - (m_dDispPcsHeight*(nNodeY+0.5)); v1.z = 0.0;	
		//	//v2.x = fData3; v2.y = fData4; v2.z = 0.0;
		//	//m_stRectOrg[2].v1 = v1;
		//	//m_stRectOrg[2].v2 = v2;
		//	//m_stRectOrg[2].color = RGB_YELLOW;
		//
		//
		//	// Add Frame Region
		//	v1.x = fData1; v1.y = fData2; v1.z = 0.0;
		//	v2.x = fData1; v2.y = fData4; v2.z = 0.0;
		//	AddLine(v1, v2, color);
		//
		//	v1.x = fData1; v1.y = fData4; v1.z = 0.0;
		//	v2.x = fData3; v2.y = fData4; v2.z = 0.0;
		//	AddLine(v1, v2, color);
		//
		//	v1.x = fData3; v1.y = fData4; v1.z = 0.0;
		//	v2.x = fData3; v2.y = fData2; v2.z = 0.0;
		//	AddLine(v1, v2, color);
		//
		//	v1.x = fData3; v1.y = fData2; v1.z = 0.0;
		//	v2.x = fData1; v2.y = fData2; v2.z = 0.0;
		//	AddLine(v1, v2, color);
		//}
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
		AfxMessageBox(_T("������ �ø��� ���� PCR ������ �����ϴ�."));
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
				AfxMessageBox(_T("PCR�� �ø��� ������ ���������� �ʽ��ϴ�."));
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
		AfxMessageBox(_T("������ �ø��� ���� PCR ������ �����ϴ�."));
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
				AfxMessageBox(_T("PCR�� �ø��� ������ ���������� �ʽ��ϴ�."));
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

	CString sMsg;
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
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	double dDispWidth, dDispHeight;
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	double dDispPcsScaleStY = 1.0;	// for Display Def Num and Serial Num.

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
		AfxMessageBox(_T("������ �ø��� ���� PCR ������ �����ϴ�."));
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

			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
		}
		else
		{
			dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
		}

		if (nDispPcrIdx >= 0)
		{
			if (nDispPcrIdx < nCount)
			{
				Pcr = arPcr[nDispPcrIdx];
				nDispPnlNum = Pcr.GetSerial();
				if (pEstimatedPnlNum[k] != nDispPnlNum)
					AfxMessageBox(_T("PCR�� �ø��� ������ ���������� �ʽ��ϴ�."));

				nTotPcs = Pcr.GetTotalDef();
				for (i = 0; i < nTotPcs; i++)
				{
					int nPcsId = Pcr.GetPcsId(i);
					int nDefCode = Pcr.GetDefCode(i);
					double fNeed = 0.0;
					if(!pParent->GetMatrix(nPcsId, nR, nC))
					{
						sMsg.Format(_T("Fail to GetMatrix of PcsId(%d)!!!"), nPcsId);
						AfxMessageBox(sMsg);
						return;
					}

					fData1 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartX * mmPxl;	// left
					fData2 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartY * mmPxl;	// top
					fData3 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndX * mmPxl;	// right
					fData4 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndY * mmPxl;	// bottom

					fWidth = (fData3 - fData1) * dScaleX * RMAP_PCS_SCALE;
					fHeight = (fData4 - fData2) * dPcsScaleY * RMAP_PCS_SCALE;
					fNeed = (fData3 - fData1) - fWidth;

					if (m_dDispPcsWidth < fWidth)
						m_dDispPcsWidth = fWidth;
					if (m_dDispPcsHeight < fHeight)
						m_dDispPcsHeight = fHeight;

					dDispPcsScaleStY = (fData4 - fData2) * (1.0 - dPcsScaleY * RMAP_PCS_SCALE); // *2.0;
					fData2 *= dPcsScaleY;
					fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE - dDispPcsScaleStY;

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

void CSimpleOpengl::DrawCross(stVertex V1, stVertex V2)
{
	glVertex3f(V1.x, V1.y, V1.z);
	glVertex3f(V2.x, V2.y, V2.z);

	glVertex3f(V1.x, V2.y, V2.z);
	glVertex3f(V2.x, V1.y, V2.z);
}

void CSimpleOpengl::DrawMarkedPcs(int nCam, int nSerial, CArPcrMark& arPcrMark, tagStrPcs& StrPcs)
{
	CMyReelmapDlg* pParent = (CMyReelmapDlg*)m_pParent;
	CString sMsg;

	int nCount = arPcrMark.GetSize();
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
	int nEstimatedPnlNum = -1;
	CPcrMark PcrMark;
	int nPcrIdx = -1, nTotDef = -1, nTotMark = 0;

	int i = 0, k = 0, nR = -1, nC = -1;
	stVertex v1, v2;
	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	double dDispWidth, dDispHeight;
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	double dDispPcsScaleStY = 1.0;	// for Display Def Num and Serial Num.

	for (i = 0; i < nCount; i++)
	{
		PcrMark = arPcrMark.GetAt(i);
		if (PcrMark.GetSerial() == nSerial)
		{
			nPcrIdx = i;
			break;
		}
	}
	if (nPcrIdx < 0)
	{
		AfxMessageBox(_T("������ �ø��� ���� PCR ������ �����ϴ�."));
		return;
	}

	int nDispPcrIdx = -1;
	//RemoveAllRect();
	for (k = 0; k < nTotPnl; k++) // Index start from left
	{
		if (nCam == 0)
		{
			nEstimatedPnlNum = nSerial - (nTotPnl - (nSelMarkingPnl + 1 + k + 1));
			nDispPcrIdx = nPcrIdx - (nTotPnl - (nSelMarkingPnl + 1 + k + 1));
		}
		else if (nCam == 1)
		{
			nEstimatedPnlNum = nSerial - (nTotPnl - (nSelMarkingPnl + k + 1));
			nDispPcrIdx = nPcrIdx - (nTotPnl - (nSelMarkingPnl + k + 1));
		}

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

			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
		}
		else
		{
			dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
		}

		if (nDispPcrIdx >= 0)
		{
			if (nDispPcrIdx < nCount)
			{
				PcrMark = arPcrMark[nDispPcrIdx];
				int nDispPnlNum = PcrMark.GetSerial();
				if (nEstimatedPnlNum != nDispPnlNum)
					AfxMessageBox(_T("PCR�� �ø��� ������ ���������� �ʽ��ϴ�."));

				nTotMark = PcrMark.GetTotalMark();
				for (i = 0; i < nTotMark; i++)
				{
					int nPcsId = PcrMark.GetMarkedPcsId(i);
					if (nPcsId < 0) break;

					double fNeed = 0.0;
					if (!pParent->GetMatrix(nPcsId, nR, nC))
					{
						sMsg.Format(_T("Fail to GetMatrix of PcsId(%d)!!!"), nPcsId);
						AfxMessageBox(sMsg);
						return;
					}
					fData1 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartX * mmPxl;	// left
					fData2 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartY * mmPxl;	// top
					fData3 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndX * mmPxl;	// right
					fData4 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndY * mmPxl;	// bottom

					fWidth = (fData3 - fData1) * dScaleX * RMAP_PCS_SCALE;
					fHeight = (fData4 - fData2) * dPcsScaleY * RMAP_PCS_SCALE;
					fNeed = (fData3 - fData1) - fWidth;

					if (m_dDispPcsWidth < fWidth)
						m_dDispPcsWidth = fWidth;
					if (m_dDispPcsHeight < fHeight)
						m_dDispPcsHeight = fHeight;

					dDispPcsScaleStY = (fData4 - fData2) * (1.0 - dPcsScaleY * RMAP_PCS_SCALE); // *2.0;
					fData2 *= dPcsScaleY;
					fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE - dDispPcsScaleStY;

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
					AddCross(v1, v2, RGB(0, 0, 0));
				}
			}
		}
		else
		{
			;
		}
	}
}

void CSimpleOpengl::DrawMarkedPcs(int nSerial, CArPcrMark& arPcrMark, tagStrPcs& StrPcs)
{
	CMyReelmapDlg* pParent = (CMyReelmapDlg*)m_pParent;

	CString sMsg;
	int nCount = arPcrMark.GetSize();
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
	int nEstimatedPnlNum = -1;
	CPcrMark PcrMark;
	int nPcrIdx = -1, nTotDef = -1, nTotMark = 0;

	int i = 0, k = 0, nR = -1, nC = -1;
	stVertex v1, v2;
	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dScaleX = 1.0, dScaleY = 1.0;// 0.85;
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmSpace = 2.0;
	double dDrawStPosX = 0.0;

	double dDispWidth, dDispHeight;
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	double dDispPcsScaleStY = 1.0;	// for Display Def Num and Serial Num.

	for (i = 0; i < nCount; i++)
	{
		PcrMark = arPcrMark.GetAt(i);
		if (PcrMark.GetSerial() == nSerial)
		{
			nPcrIdx = i;
			break;
		}
	}
	if (nPcrIdx < 0)
	{
		AfxMessageBox(_T("������ �ø��� ���� PCR ������ �����ϴ�."));
		return;
	}

	int nDispPcrIdx = -1;
	RemoveAllCross();
	for (k = 0; k < nTotPnl; k++) // Index start from left
	{
		nEstimatedPnlNum = nSerial - (nTotPnl - (nSelMarkingPnl + k + 1));
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

			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
		}
		else
		{
			dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
			dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
		}

		if (nDispPcrIdx >= 0)
		{
			if (nDispPcrIdx < nCount)
			{
				PcrMark = arPcrMark[nDispPcrIdx];
				int nDispPnlNum = PcrMark.GetSerial();
				if (nEstimatedPnlNum != nDispPnlNum)
					AfxMessageBox(_T("PCR�� �ø��� ������ ���������� �ʽ��ϴ�."));

				nTotMark = PcrMark.GetTotalMark();
				for (i = 0; i < nTotMark; i++)
				{
					int nPcsId = PcrMark.GetMarkedPcsId(i);
					if (nPcsId < 0) break;
					
					double fNeed = 0.0;
					if(!pParent->GetMatrix(nPcsId, nR, nC))
					{
						sMsg.Format(_T("Fail to GetMatrix of PcsId(%d)!!!"), nPcsId);
						AfxMessageBox(sMsg);
						return;
					}

					fData1 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartX * mmPxl;	// left
					fData2 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iStartY * mmPxl;	// top
					fData3 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndX * mmPxl;	// right
					fData4 = (double)StrPcs.m_stPieceRgnPix[nPcsId].iEndY * mmPxl;	// bottom

					fWidth = (fData3 - fData1) * dScaleX * RMAP_PCS_SCALE;
					fHeight = (fData4 - fData2) * dPcsScaleY * RMAP_PCS_SCALE;
					fNeed = (fData3 - fData1) - fWidth;

					if (m_dDispPcsWidth < fWidth)
						m_dDispPcsWidth = fWidth;
					if (m_dDispPcsHeight < fHeight)
						m_dDispPcsHeight = fHeight;

					dDispPcsScaleStY = (fData4 - fData2) * (1.0 - dPcsScaleY * RMAP_PCS_SCALE); // *2.0;
					fData2 *= dPcsScaleY;
					fData2 += nWorldH * RMAP_TOTAL_DEF_SCALE - dDispPcsScaleStY;

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
					AddCross(v1, v2, RGB(0,0,0));			
				}
			}
		}
		else
		{
			;
		}
	}
}

BOOL CSimpleOpengl::IsInRectOrg(stVertex v1, stVertex v2)
{
	int nCase = m_nItsOrgCase;
	if (v1.x > m_stRectOrg[nCase].v1.x && v1.y > m_stRectOrg[nCase].v1.y)
	{
		if (v2.x < m_stRectOrg[nCase].v2.x && v2.y < m_stRectOrg[nCase].v2.y)
			return TRUE;
	}

	return FALSE;
}

double CSimpleOpengl::GetScaleX(tagStrPcs& StrPcs)
{
	double dScaleX = 1.0;// 0.85;
	int nWorldMargin = 6;	// [mm] = [dot]
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	int nTotPnl = 6;
	double dFrmSpace = 2.0;

	CRect rtDispCtrl;
	::GetClientRect(m_hCtrl, &rtDispCtrl);
	int nWorldW = rtDispCtrl.right - rtDispCtrl.left - nWorldMargin;
	int nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;

	fData1 = (double)StrPcs.m_stFrameRgnPix[0].iStartX * mmPxl;	// left
	fData2 = (double)StrPcs.m_stFrameRgnPix[0].iStartY * mmPxl;	// top
	fData3 = (double)StrPcs.m_stFrameRgnPix[0].iEndX * mmPxl;	// right
	fData4 = (double)StrPcs.m_stFrameRgnPix[0].iEndY * mmPxl;	// bottom
	fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
	fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);

	int nRealW = fWidth*nTotPnl + dFrmSpace*(nTotPnl - 1);// +(nWorldMargin + nWorldStX);
	if ((nWorldW - nRealW) < 0.0)
	{
		int nNeedX = nRealW - nWorldW;
		dScaleX = (double)((double)nWorldW / (double)(nWorldW + nNeedX));
		dFrmOffset[0] = 0.0;
	}
	else
		dFrmOffset[0] = (nWorldW - fWidth*nTotPnl) / 2.0;

	return dScaleX;
}

double CSimpleOpengl::GetScaleY(tagStrPcs& StrPcs)
{
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	int nWorldMargin = 6;	// [mm] = [dot]
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dScaleY = 1.0;// 0.85;

	CRect rtDispCtrl;
	::GetClientRect(m_hCtrl, &rtDispCtrl);
	int nWorldW = rtDispCtrl.right - rtDispCtrl.left - nWorldMargin;
	int nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;

	fData1 = (double)StrPcs.m_stFrameRgnPix[0].iStartX * mmPxl;	// left
	fData2 = (double)StrPcs.m_stFrameRgnPix[0].iStartY * mmPxl;	// top
	fData3 = (double)StrPcs.m_stFrameRgnPix[0].iEndX * mmPxl;	// right
	fData4 = (double)StrPcs.m_stFrameRgnPix[0].iEndY * mmPxl;	// bottom
	fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
	fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);

	if ((nWorldH - fHeight) < 0.0)
	{
		int nNeedY = fHeight - nWorldH;
		dScaleY = (double)((double)nWorldH / (double)(nWorldH + nNeedY));
		dFrmOffset[1] = 0.0;

		dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
	}
	else
	{
		dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
		dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
	}

	return dScaleY;
}

double CSimpleOpengl::GetPcsScaleY(tagStrPcs& StrPcs)
{
	double dPcsScaleY = 1.0;		// for Display Def Num and Serial Num.
	int nWorldMargin = 6;	// [mm] = [dot]
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double dFrmMargin[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double dFrmOffset[4] = { 2.0, 2.0, 2.0, 2.0 }; // [mm] left(0) top(1) right(2) bottom(3)
	double fWidth, fHeight;
	double fData1, fData2, fData3, fData4;
	double dScaleY = 1.0;// 0.85;

	CRect rtDispCtrl;
	::GetClientRect(m_hCtrl, &rtDispCtrl);
	int nWorldW = rtDispCtrl.right - rtDispCtrl.left - nWorldMargin;
	int nWorldH = rtDispCtrl.bottom - rtDispCtrl.top;

	fData1 = (double)StrPcs.m_stFrameRgnPix[0].iStartX * mmPxl;	// left
	fData2 = (double)StrPcs.m_stFrameRgnPix[0].iStartY * mmPxl;	// top
	fData3 = (double)StrPcs.m_stFrameRgnPix[0].iEndX * mmPxl;	// right
	fData4 = (double)StrPcs.m_stFrameRgnPix[0].iEndY * mmPxl;	// bottom
	fWidth = (fData3 - fData1) + (dFrmMargin[0] + dFrmMargin[2]);
	fHeight = (fData4 - fData2) + (dFrmMargin[1] + dFrmMargin[3]);

	if ((nWorldH - fHeight) < 0.0)
	{
		int nNeedY = fHeight - nWorldH;
		dScaleY = (double)((double)nWorldH / (double)(nWorldH + nNeedY));
		dFrmOffset[1] = 0.0;

		dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)(nWorldH + nNeedY));
	}
	else
	{
		dFrmOffset[1] = (nWorldH - fHeight) / 2.0;
		dPcsScaleY = (double)((double)nWorldH * (1.0 - RMAP_TOTAL_DEF_SCALE - RMAP_TOTAL_DEF_SCALE) / (double)nWorldH);
	}

	return dPcsScaleY;
}

double  CSimpleOpengl::GetDispPcsScaleStY(tagStrPcs& StrPcs)
{
	double dDispPcsScaleStY = 1.0;	// for Display Def Num and Serial Num.
	double dPixelSize = CAMMST_PIXEL_RESOLUTION; // [um/pixel]
	double mmPxl = dPixelSize / 1000.0; // [mm]
	double fWidth, fHeight;

	double dScaleX = GetScaleX(StrPcs);
	double dPcsScaleY = GetPcsScaleY(StrPcs);

	double fData1, fData2, fData3, fData4;
	fData1 = (double)StrPcs.m_stPieceRgnPix[0].iStartX * mmPxl;	// left
	fData2 = (double)StrPcs.m_stPieceRgnPix[0].iStartY * mmPxl;	// top
	fData3 = (double)StrPcs.m_stPieceRgnPix[0].iEndX * mmPxl;	// right
	fData4 = (double)StrPcs.m_stPieceRgnPix[0].iEndY * mmPxl;	// bottom

	fWidth = (fData3 - fData1) * dScaleX * RMAP_PCS_SCALE;
	fHeight = (fData4 - fData2) * dPcsScaleY * RMAP_PCS_SCALE;

	if (m_dDispPcsWidth < fWidth)
		m_dDispPcsWidth = fWidth;
	if (m_dDispPcsHeight < fHeight)
		m_dDispPcsHeight = fHeight;

	dDispPcsScaleStY = (fData4 - fData2) * (1.0 - dPcsScaleY * RMAP_PCS_SCALE);

	return dDispPcsScaleStY;
}

int CSimpleOpengl::SetItsOrgRgn(int nR, int nC, stVertex v1, stVertex v2, tagStrPcs& StrPcs)
{
	int nCase = -1; // Is not Its Origin Region.
	int nNodeX = StrPcs.m_nMaxCol;
	int nNodeY = StrPcs.m_nMaxRow / MAX_STRIP;

	stVertex Org_v1, Org_v2;

	if (nR == 0 && nC == 0)
	{
		nCase = 0;
		Org_v1.x = v1.x - 1; Org_v1.y =  v1.y - 1; Org_v1.z = 0.0;
		Org_v2.x = v2.x + 1; Org_v2.y = v2.y + 1; Org_v2.z = 0.0;
		m_stRectOrg[0].v1 = Org_v1;
		m_stRectOrg[0].v2 = Org_v2;
		m_stRectOrg[0].color = RGB_GREEN;
	}
	else if(nR == StrPcs.m_nMaxRow - nNodeY && nC == 0)
	{
		nCase = 1;
		Org_v1.x = v1.x - 1; Org_v1.y = v1.y - 1; Org_v1.z = 0.0;
		Org_v2.x = v2.x + 1; Org_v2.y = v2.y + 1; Org_v2.z = 0.0;
		m_stRectOrg[1].v1 = Org_v1;
		m_stRectOrg[1].v2 = Org_v2;
		m_stRectOrg[1].color = RGB_GREEN;
	}
	else if (nR == nNodeY - 1 && nC == nNodeX - 1)
	{
		nCase = 2;
		Org_v1.x = v1.x - 1; Org_v1.y = v1.y - 1; Org_v1.z = 0.0;
		Org_v2.x = v2.x + 1; Org_v2.y = v2.y + 1; Org_v2.z = 0.0;
		m_stRectOrg[2].v1 = Org_v1;
		m_stRectOrg[2].v2 = Org_v2;
		m_stRectOrg[2].color = RGB_GREEN;
	}
	else if (nR == StrPcs.m_nMaxRow - 1 && nC == nNodeX - 1)
	{
		nCase = 3;
		Org_v1.x = v1.x - 1; Org_v1.y = v1.y - 1; Org_v1.z = 0.0;
		Org_v2.x = v2.x + 1; Org_v2.y = v2.y + 1; Org_v2.z = 0.0;
		m_stRectOrg[3].v1 = Org_v1;
		m_stRectOrg[3].v2 = Org_v2;
		m_stRectOrg[3].color = RGB_GREEN;
	}

	return nCase;
}

void CSimpleOpengl::SetItsOrgCase(int nCase)
{
	m_nItsOrgCase = nCase;
}