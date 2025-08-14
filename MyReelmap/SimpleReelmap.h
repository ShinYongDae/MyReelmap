#pragma once
#include <thread>

#include "Pcr.h"
#include "PcrYield.h"
#include "PcrMark.h"

#define PATH_CONFIG				_T("C:\\R2RSet\\Config.ini")
#define PATH_SAPP3				_T("C:\\R2RSet\\Sapp3.ini")
#define PATH_ITS_FOLDER			_T("D:\\Its")
#define PATH_SAPP3_FOLDER		_T("C:\\LOSS")

#ifndef MAX_STRIP
#define MAX_STRIP	4
#endif

#ifndef MAX_DEF
#define MAX_DEF		30
#endif

#ifndef MAX_SAPP3
#define MAX_SAPP3	10
#endif

#ifndef DEF_DEFINE
#define DEF_DEFINE		
#define DEF_NONE					0		// NONE 	
#define DEF_NICK					1		// Nick 	
#define DEF_PROTRUSION				2		// Excessive line width
#define DEF_SPACE					3		// Minimum space violation
#define DEF_OPEN					4		// Pattern open
#define DEF_SHORT					5		// Pattern short
#define DEF_USHORT					6		// Micro short
#define DEF_PINHOLE					7		// Void, pinhole
#define DEF_HOLE_MISS				8		// Missing Hole
#define DEF_EXTRA					9		// Island
#define DEF_PAD						10		// Precision Pattern Error
#define DEF_HOLE_POSITION			11      // Location Hole 
#define DEF_POI						12		// Point of interest (Virtual defect by operator specfied)
#define DEF_VH_POSITION			    13		// Via hole positionning
#define DEF_VH_MISS					14		// Via hole missing
#define DEF_HOLE_DEFECT		    	15		// Hole burr..
#define DEF_HOLE_OPEN				16		// Hole open
#define DEF_VH_OPEN					17	    // VH open
#define DEF_VH_DEF					18	    // Reserved
#define DEF_EDGE_NICK				19		// Edge Nick
#define DEF_EDGE_PROT				20	    // Edge Protrusion
#define DEF_EDGE_SPACE				21	    // Edge Space
#define DEF_USER_DEFINE_1			22	    // User Define 1
#define DEF_NARROW					23	    // User Define 2
#define DEF_WIDE					24	    // User Define 3
#define DEF_FIXED_DEF				25	    // User Define 3
#define DEF_VH_SIZE					26	    // User Define 3
#define DEF_VH_EDGE					27	    // User Define 3
#define DEF_LIGHT					28	
#define DEF_INNER					29	
#endif

#ifndef RGB_DEFINE
#define RGB_DEFINE
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
#endif

typedef enum tagDefCode {
	None = 0, Nick, Prot, Space, Open, Short, UShort, PinHole, HoleMiss, Extra, Pad, HolePos, Poi, VhPos, VhMiss, HoleDef,
	HoleOpen, VhOpen, VhDef, EdgeNick, EdgeProt, EdgeSpace, Define1, Narrow, Wide, FixedDef, VhSize, VhEdge, Light, Inner
}DefCode;


typedef enum tagRMap {
	RMapNone = -1, RMapUp, RMapDn, RMapAll, RMapUpInner, RMapDnInner, RMapAllInner, RMapUpOutter, RMapDnOutter, RMapAllOutter, RMapIts
}RMap;

typedef enum tagSapp3 {
	Sapp3Open = 0, Sapp3Short, Sapp3Nick, Sapp3SpaceExtraProt, Sapp3PinHole, Sapp3HOpen, 
	Sapp3HMissHPosHBad, Sapp3UShort, Sapp3Pad, Sapp3VHOpenVHAlignVHDefNoVH
}Sapp3;


typedef struct tagRmapInfo
{
	CString m_sMcName;
	CString m_sUserName;
	CString m_sModel;
	CString m_sLot;
	CString m_sLayer;
	CString m_sLayerUp;
	CString m_sLayerDn;
	CString m_sItsCode;
	CString m_sProcessCode;
	double m_dEntireSpeed;
	COleDateTime m_timeLotStart;
	COleDateTime m_timeLotRun;
	COleDateTime m_timeLotEnd;
	int m_nLastSerial;
	int m_nActionCode;
	int m_nMaxRow;
	int m_nMaxCol;
	int m_nMaxStrip;
	int m_nMaxDefCode;

	tagRmapInfo()
	{
		m_sMcName = _T("");
		m_sUserName = _T("");
		m_sModel = _T("");
		m_sLot = _T("");
		m_sLayer = _T("");
		m_sLayerUp = _T("");
		m_sLayerDn = _T("");
		m_sItsCode = _T("");
		m_sProcessCode = _T("");
		m_dEntireSpeed = 0.0;
		m_timeLotStart = COleDateTime::GetCurrentTime();;
		m_timeLotRun = COleDateTime::GetCurrentTime();;
		m_timeLotEnd = COleDateTime::GetCurrentTime();;
		m_nLastSerial = 0;
		m_nActionCode = 0;
		m_nMaxRow = 0;
		m_nMaxCol = 0;
		m_nMaxStrip = 4;
		m_nMaxDefCode = 30;
	}
}stRmapInfo;


typedef struct tagResultInfo
{
	int nTotalPcr;
	int nTotStrip;
	int nTotStOut;
	int nTotPcs, nTotGood, nTotBad;
	int nStripOut[MAX_STRIP], nStripDef[MAX_STRIP], nTotDef[MAX_DEF];
	int nDefPerStrip[MAX_STRIP][MAX_DEF];

	tagResultInfo()
	{
		nTotalPcr = 0;
		nTotStrip = 0;
		nTotStOut = 0;
		nTotPcs = 0;
		nTotGood = 0;
		nTotBad = 0;
		nStripOut[0] = 0; nStripOut[1] = 0; nStripOut[2] = 0; nStripOut[3] = 0;
		nStripDef[0] = 0; nStripDef[1] = 0; nStripDef[2] = 0; nStripDef[3] = 0; 
		memset(nTotDef, 0, sizeof(nTotDef));
		memset(nDefPerStrip, 0, sizeof(nDefPerStrip));
	}
}stResultInfo;


// CSimpleReelmap

class CSimpleReelmap : public CWnd
{
	DECLARE_DYNAMIC(CSimpleReelmap)

	CWnd* m_pParent;
	HWND m_hParent;

	CString m_sPathInfo, m_sPathRmap, m_sPathYield, m_sPathMark;
	BOOL CreateWndForm(DWORD dwStyle);

	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;
	void ThreadStart();
	void ThreadStop();

	CArPcr m_arPcr;				// PCR파일 Array
	CArPcrYield m_arPcrYield;	// PCR수율 Array
	CArPcrMark m_arPcrMark;		// PCS 마킹 Array
	BOOL m_bLock;

	stResultInfo m_stResult;
	stRmapInfo m_stInfo;
	COLORREF m_rgbDef[MAX_DEF];
	CString m_sKorDef[MAX_DEF], m_sEngDef[MAX_DEF];
	char m_cBigDef[MAX_DEF], m_cSmallDef[MAX_DEF];
	int m_nOdr[MAX_DEF];
	int m_nBkColor[3]; //RGB
	int m_nSapp3Code[MAX_SAPP3];

	BOOL IsShare(int &nSerial);
	BOOL Add(int nSerial);
	BOOL ShiftToBuffer(int nSerial);

	BOOL CopyPcr(CString sSrc, CString sDest);
	CString ParseFolderName(CString sPathName, CString &sRemainPathName);
	BOOL Copy(CString sSource, CString sDest);
	BOOL DelPcr(CString sPath);
	int MirrorLR(int nPcsId); // 좌우 미러링
	int MirrorUD(int nPcsId); // 상하 미러링
	int Rotate180(int nPcsId); // 180도 회전
	void StringToChar(CString str, char* pCh); // char* returned must be deleted... 
	CString CharToString(const char *szStr);
	void Free();

	int m_nCMstTotPcs;
	BOOL LoadSapp3CodeIni();
	BOOL LoadDefectTableIni();
	void InitColor();
	void InitDef();
	BOOL LoadInfo();
	BOOL LoadRmap();
	BOOL LoadYield();
	BOOL LoadMark();
	BOOL SaveInfo();
	BOOL SaveRmap();
	BOOL SaveYield();
	BOOL SaveMark();

	int m_nLayer;
	int m_nDispPnl[2]; // Left(0), Right(1)
	int m_nMkedPcs[2]; // Left(0), Right(1)
	int GetPcrIdx(int nCam);
	BOOL GetResult();
	CString GetTimeIts();
	CString GetPathIts(int nSerial);
	CString GetTextIts(int nSerial);
	CString GetPathSapp3();
	CString GetTextSapp3();
	int GetItsDefCode(int nDefCode);

public:
	CSimpleReelmap(int nLayer, CString m_sPathInfo, CString sPathRmap, CString sPathYield, CString sPathMark, CWnd* pParent = NULL);
	virtual ~CSimpleReelmap();
	static void ProcThrd(const LPVOID lpContext);

	void Init(stRmapInfo stInfo);//int nMaxRow, int nMaxCol, int nActionCode = 0
	BOOL GetMatrix(int nPcsId, int &nR, int &nC);
	BOOL Save();
	BOOL Load();

	CArPcr& GetAddrArPcr();
	CArPcrMark& GetAddrArPcrMark();
	COLORREF GetDefColor(int nDefCode);
	char GetCodeBigDef(int nIdx);
	char GetCodeSmallDef(int nIdx);

	CString GetTextResult();
	CString GetTextConverse();
	CString GetTextArPcr();
	CString GetTextArPcrYield();
	CString GetTextListItsFile(int nIdx = -1); // Default : All
	CString GetTextItsFile(int nIdx);
	CString GetTextListSapp3File(int nIdx = -1); // Default : All
	CString GetTextSapp3File(int nIdx);

	void SetDispPnl(int nSerial);
	void SetPcsMkOut(int nCam); // 0: Left Cam Or 1: Right Cam , 불량 피스 인덱스 [ 0 ~ (Total Pcs - 1) ]  // (피스인덱스는 CamMaster에서 정한 것을 기준으로 함.)
	BOOL MakeIts(int nSerial);
	BOOL MakeSapp3();

protected:
	void ThreadEnd();
	BOOL ProcReelmap();
	BOOL ThreadIsAlive();

protected:
	DECLARE_MESSAGE_MAP()
};


