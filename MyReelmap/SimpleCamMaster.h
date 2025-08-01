#pragma once
#include <thread>

#define MAX_PIECE_NODE_X_NUM	600		// 130319 jsy edit
#define MAX_PIECE_NODE_Y_NUM	600

#define MAX_FRAME_RGN_NUM		1000		// Maximum number of Strip regions
#define MAX_STRIP_X_NUM			10			//2012.08.07 hyk
#define MAX_STRIP_Y_NUM			10			//2012.08.07 hyk
#define MAX_PIECE_RGN_NUM		(MAX_PIECE_NODE_X_NUM * MAX_PIECE_NODE_Y_NUM)		// Maximum number of Cell regions

#ifndef MAX_STRIP
#define MAX_STRIP	4
#endif

typedef struct tagREGIONS_FRAME 
{
	int iStartX, iStartY;
	int iEndX, iEndY;
	int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌우미러
	int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
	tagREGIONS_FRAME()
	{
		iStartX = 0;
		iStartY = 0;
		iEndX = 0;
		iEndY = 0;
		FMirror = -1;	//0 : 원본 1 : 상하미러  2 : 좌우미러
		FRotate = -1;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
	}
} REGIONS_FRAME;


typedef struct tagREGIONS_FRAME_ID 
{
	int nId;
	int Col;
	int Row;
	tagREGIONS_FRAME_ID()
	{
		nId = -1;
		Col = -1;
		Row = -1;
	}
} REGIONS_FRAME_ID;


typedef struct tagREGIONS_PIECE
{
	int iStartX, iStartY;
	int iEndX, iEndY;
	int FMirror;	//0 : Defult 1 : Up to Down Mirroring  2 : Left to Right Mirroring
	int FRotate;	//0 : 0  1 : 90  2 : 180  3 : 270 [Degree]
	tagREGIONS_PIECE()
	{
		iStartX = 0;
		iStartY = 0;
		iEndX = 0;
		iEndY = 0;
		FMirror = -1;	//0 : Defult 1 : Up to Down Mirroring  2 : Left to Right Mirroring
		FRotate = -1;	//0 : 0  1 : 90  2 : 180  3 : 270 [Degree]
	}
} REGIONS_PIECE;

typedef struct tagREGIONS_PIECE_2
{
	int nId;
	int iStartX, iStartY;
	int iEndX, iEndY;
	int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌퓖E肩?
	int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
	int Row;
	int Col;
	tagREGIONS_PIECE_2()
	{
		nId = -1;
		iStartX = 0;
		iStartY = 0;
		iEndX = 0;
		iEndY = 0;
		FMirror = -1;	//0 : Defult 1 : Up to Down Mirroring  2 : Left to Right Mirroring
		FRotate = -1;	//0 : 0  1 : 90  2 : 180  3 : 270 [Degree]
		Col = -1;
		Row = -1;
	}
} REGIONS_PIECE_2;


typedef struct tagStrPcs
{
	short m_nFrameRgnNum;
	int m_nPieceRgnNum;
	int m_nMaxRow, m_nMaxCol;
	REGIONS_FRAME_ID m_stFrameRgnID[MAX_FRAME_RGN_NUM];
	REGIONS_FRAME m_stFrameRgnPix[MAX_FRAME_RGN_NUM];
	int m_nPieceNum[MAX_STRIP];
	REGIONS_PIECE_2 m_stPieceRgnPix[MAX_PIECE_RGN_NUM];

	tagStrPcs()
	{
		m_nFrameRgnNum = 0;
		m_nPieceRgnNum = 0;
		m_nMaxRow = 0;
		m_nMaxCol = 0;
		m_nPieceNum[0] = 0;
		m_nPieceNum[1] = 0;
		m_nPieceNum[2] = 0;
		m_nPieceNum[3] = 0;
	}

	~tagStrPcs()
	{
		;
	}
}stStrPcs;

// CSimpleCamMaster

class CSimpleCamMaster : public CWnd
{
	DECLARE_DYNAMIC(CSimpleCamMaster)

	CWnd* m_pParent;
	HWND m_hParent;

	CString m_sPath;
	BOOL CreateWndForm(DWORD dwStyle);

	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;
	void ThreadStart();
	void ThreadStop();

	stStrPcs m_stStrPcs;

	void AllocPolygonRgnData(CPoint**& PolygonPoints, int nCornerNum, int PieceRgnNum);
	void FreePolygonRgnData(CPoint** PolygonPoints);

public:
	CSimpleCamMaster();
	CSimpleCamMaster(CWnd* pParent);
	virtual ~CSimpleCamMaster();
	static void ProcThrd(const LPVOID lpContext);

	BOOL LoadStrpcs(CString sPath);
	tagStrPcs& GetAddrStrPcs();

protected:
	void ThreadEnd();
	BOOL ProcCamMaster();
	BOOL ThreadIsAlive();

protected:
	DECLARE_MESSAGE_MAP()
};


