#pragma once
#include <thread>

#include <gl/freeglut_std.h>
#pragma comment (lib, "freeglut.lib")

#define WM_THREAD_END	(WM_USER + 0x73FF)	// 0x7FFF (Range of WM_USER is 0x400 ~ 0x7FFF)

#define RGB_BLACK       RGB(0,0,0)
#define RGB_WHITE       RGB(255,255,255)
#define RGB_GRAY        RGB(128, 128, 128)
#define RGB_RED         RGB(255,0,0)
#define RGB_DARKRED     RGB(207,0,0)
#define RGB_YELLOW      RGB(255,255,0)
#define RGB_DARKYELLOW  RGB(128,128,0)
#define RGB_GREEN       RGB(0,255,0)
#define RGB_LTGREEN     RGB(0,255,128)
#define RGB_CYAN        RGB(0,255,255)
#define RGB_BLUE        RGB(0,0,255)
#define RGB_LTBLUE		RGB(82, 200, 200)
#define RGB_CLOUDBLUE	RGB(128,184,223)
#define RGB_MAGENTA     RGB(255,0,255)
#define RGB_DARKMAGENTA RGB(193,0,130)
#define RGB_WHGRAY      RGB(218,218,218)
#define RGB_LTGRAY      RGB(192,192,192)
#define RGB_DARKCYAN    RGB(0,128,128)
#define RGB_LTCYAN      RGB(128,255,255)
#define RGB_BOON        RGB(255,0,128)
#define RGB_DARKBOON    RGB(64,0,128)
#define RGB_DARKBLUE    RGB(0,0,128)
#define RGB_PINK        RGB(255,0,128)
#define RGB_DARKPINK	RGB(227,64,128)
#define RGB_SKYBLUE		RGB(128,255,255)
#define RGB_DKSKYBLUE	RGB(82,200,200)
#define RGB_PURPLE		RGB(128,0,255)
#define RGB_YELLOWGREEN RGB(128,255,0)
#define RGB_WHYELLOW    RGB(255,255,128)
#define RGB_ORANGE      RGB(255,128,0)
#define RGB_LTDKORANGE  RGB(255,172,89)
#define RGB_DARKORANGE  RGB(220,70,0)
#define RGB_CHESTNUT	RGB(128,64,64)
#define RGB_NAVY		RGB(0,0,128)
#define RGB_LLTGREEN    RGB(128,255,128)
#define RGB_LT_ORANGE   RGB(255,128,64)
#define RGB_DARKFINGER	RGB(128,0,128)
#define RGB_DLG_FRM		RGB(212,208,200)
#define RGB_DLG_FRM2	RGB(240,240,240)
#define RGB_DARKBROWN	RGB(64,0,0)
#define RGB_DIALOG		RGB(200,200,192)
#define RGB_POSTIT		RGB(240,233,134)
#define RGB_DARK_BLUE	RGB(68,136,204)
#define RGB_FRAME		RGB(212,208,200)
#define RGB_LTRED       RGB(255,128,64)
#define RGB_LTPURPLE	RGB(226,233,251)
#define RGB_LTMAGENTA   RGB(255,64,255)
#define RGB_LTYELLOW    RGB(255,255,128)
#define RGB_LTPINK      RGB(255,64,128)


#define RGB_DARK_GREEN		RGB(0,125,60)
#define RGB_LT_PURPLE		RGB(179,179,255)
#define RGB_LT_YELLOW		RGB(255,255,150)
#define RGB_LT_PINK			RGB(255,200,200)
#define RGB_LT_GREEN		RGB(195,255,195)
#define RGB_LT_BLUE			RGB(210,255,255)
#define RGB_WH_ORANGE		RGB(255,220,190)
#define RGB_PURPLE			RGB(147,112,216)
#define RGB_DARKPURPLE		RGB(64,0,64)
#define RGB_LT_DARKMAGENTA	RGB(204,50,153)
#define RGB_MUSTARD			RGB(217,217,25)
#define RGB_SKY				RGB(173, 234, 234)
#define RGB_LT_BROWN		RGB(209, 146, 117)
#define RGB_PEACOCK_GREEN	RGB(100, 186, 146)
#define RGB_FOREST_GREEN	RGB(85, 158, 124)
#define RGB_EVER_GREEN		RGB(74, 118, 98)
#define	RGB_PCS_OUT			RGB_LTGRAY


#define RMAP_PCS_SCALE				1.0		// 0.85
#define RMAP_TOTAL_DEF_SCALE		0.1
#define RMAP_SERIAL_NUM_SCALE		0.1
#define CAMMST_PIXEL_RESOLUTION		5.0		//  [um]


static GLfloat ambient[] = { 0.1f,0.1f,0.1f,1.0f };			//이것은 자연광 1.0이 제일쎄고 지금은 0.1로했군..
static GLfloat diffuse[] = { 0.7f,0.7f,0.7f,1.0f };			//이것은 반사광
static GLfloat specular[] = { 1.0f,1.0f,1.0f,1.0f };		//반짝임효과(재질때문에쓰인다
static GLfloat specreff[] = { 1.0f,1.0f,1.0f,1.0f };		//위랑 똑같음
static GLfloat pos[] = { 200.0f,300.0f,400.0f,1.0f };		//이것은 빛의 위치  x로500y로500z는0 


namespace Opengl
{
	typedef enum Mode { modPoint = 0, modLine = 1, modRectE = 2, modRectF = 3, modCross = 4, modCrossX = 5, modCircleE = 6, modCircleF = 7 };
}

struct stVertex
{
	float x;
	float y;
	float z;
};
typedef CArray<stVertex, stVertex> CArVertex;

struct stLine
{
	stVertex v1, v2;
	COLORREF color;
	stLine()
	{
		v1.x = 0; v1.y = 0; v1.z = 0;
		v2.x = 0; v2.y = 0; v2.z = 0;
		color = RGB(255, 255, 255);
	}
};
typedef CArray<stLine, stLine> CArLine;

struct stRect
{
	stVertex v1, v2;
	COLORREF color;
	stRect()
	{
		v1.x = 0; v1.y = 0; v1.z = 0;
		v2.x = 0; v2.y = 0; v2.z = 0;
		color = RGB(255, 255, 255);
	}
};
typedef CArray<stRect, stRect> CArRect;

struct stCross
{
	stVertex v1, v2;
	COLORREF color;
	stCross()
	{
		v1.x = 0; v1.y = 0; v1.z = 0;
		v2.x = 0; v2.y = 0; v2.z = 0;
		color = RGB(255, 255, 255);
	}
};
typedef CArray<stCross, stCross> CArCross;

struct stText
{
	CString str;
	CPoint pos;
	COLORREF color;
};
typedef CArray<stText, stText> CArText;

struct stColor
{
	GLfloat R;
	GLfloat G;
	GLfloat B;
	GLfloat A;
};

#define MAX_LINE	10
#define LINE_SPACE	4

// CSimpleOpengl
#include "SimpleCamMaster.h"
#include "SimpleReelmap.h"

class CSimpleOpengl : public CStatic //public CWnd
{
	DECLARE_DYNAMIC(CSimpleOpengl)

	CWnd* m_pParent;
	HWND m_hParent;
	HWND m_hCtrl;
	HDC m_hDc;
	CDC *m_pDc;
	HGLRC m_hRC;
	CRect m_rtDispCtrl;
	int m_nWorldW, m_nWorldH;
	CArLine m_arLine;
	CArRect m_arRect;
	CArCross m_arCross;
	CArText m_arText;

	BOOL m_bInit;
	BOOL m_bDraw, m_bDrawText, m_bDrawClear, m_bDrawClearColor;
	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;

	GLfloat cameraposmap[3];
	GLfloat Angle[3];

	BOOL m_bFont;
	CFont m_Font;
	COLORREF m_crText;

	CMenu *m_pMenu;
	int m_nSerial;
	CArPcr* m_arPcr;
	tagStrPcs* m_StrPcs;

	BOOL CreateWndForm(DWORD dwStyle);

	void ThreadStart();
	void ThreadStop();

	void StringToChar(CString str, char* szStr);

	void PopupMenu(UINT nFlags, CPoint point);

	void DrawBegin(int mode, int size, COLORREF color);
	void DrawEnd();
	void DrawRect(stVertex V1, stVertex V2);
	void DrawCross(stVertex V1, stVertex V2);
	void DrawLine(stVertex V1, stVertex V2);
	void Draw();
	void DrawClear();
	void DrawClearColor(COLORREF color);
	void DrawTestText();
	void DrawText(CString sText, CPoint ptPnt, COLORREF rgb);
	void DrawPnlDefNum(); // 릴맵 Reflash 후 다시 GDI 객체를 드로잉하는 함수.

	void RemoveAllLine();
	void RemoveAllRect();
	void RemoveAllCross();
	void RemoveAllText();

	BOOL GetRngDrawPnl(int nDrawPnlIdx, tagStrPcs& StrPcs, CPoint& ptLT, CPoint& ptRB);
	void EraseText(CRect rect);

public:
	CSimpleOpengl(CWnd* pParent);
	CSimpleOpengl(HWND& hCtrl, CWnd* pParent = NULL);
	virtual ~CSimpleOpengl();
	void SetHwnd(HWND hCtrlWnd, CWnd* pParent=NULL);
	void Refresh();

	static void ProcThrd(const LPVOID lpContext);

	void SetTextOpenGL(CString str, stVertex pos, int size=10, stColor color = { 1,1,1 }, int line_width=1);
	void SetClear();
	void SetClearColor();
	void SetDraw();
	void SetFont(CString srtFntName, int nSize = 10, BOOL bBold = FALSE);

	void SetupResize(int width, int height);
	void Init();
	BOOL IsDraw();

public:
	void AddLine(stVertex v1, stVertex v2, COLORREF color = RGB(255, 255, 255));
	void AddRect(stVertex v1, stVertex v2, COLORREF color = RGB(255, 255, 255));
	void AddCross(stVertex v1, stVertex v2, COLORREF color = RGB(0, 0, 0));
	void AddText(CString str, CPoint pos = {0,0}, COLORREF color = RGB(255, 255, 255)); // DWORD COLORREF 0x00bbggrr

	void DrawStrPcs(tagStrPcs& StrPcs);
	void DrawPnlDefNum(int nSerial, CArPcr& arPcr, tagStrPcs& StrPcs);
	void DrawPnlDef(int nSerial, CArPcr& arPcr, tagStrPcs& StrPcs);
	void DrawMarkedPcs(int nSerial, CArPcrMark& arPcrMark, tagStrPcs& StrPcs);				// 릴맵이 셋팅될 때 위치 표시
	void DrawMarkedPcs(int nCam, int nSerial, CArPcrMark& arPcrMark, tagStrPcs& StrPcs);	// 펀칭 중에 펀칭 위치 표시


protected:
	void ThreadEnd();
	BOOL ThreadIsAlive();
	BOOL ProcOpengl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


