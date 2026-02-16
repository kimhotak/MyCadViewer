
// MyCadViewerView.cpp: CMyCadViewerView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MyCadViewer.h"
#endif

#include "MyCadViewerDoc.h"
#include "MyCadViewerView.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#include <Quantity_Color.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <V3d_TypeOfVisualization.hxx>
#pragma warning(pop)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyCadViewerView

IMPLEMENT_DYNCREATE(CMyCadViewerView, CView)

BEGIN_MESSAGE_MAP(CMyCadViewerView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMyCadViewerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CMyCadViewerView 생성/소멸

CMyCadViewerView::CMyCadViewerView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMyCadViewerView::~CMyCadViewerView()
{
}

BOOL CMyCadViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMyCadViewerView 초기화

void CMyCadViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if (myInited)
		return;

	// 1) Display / Driver
	myDisplay = new Aspect_DisplayConnection();
	myDriver = new OpenGl_GraphicDriver(myDisplay);

	// 2) Viewer / Context
	myViewer = new V3d_Viewer(myDriver);
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();

	myContext = new AIS_InteractiveContext(myViewer);

	// 3) View 생성
	myView = myViewer->CreateView();

	// 4) MFC View의 HWND를 OCCT Window로 래핑
	myWntWindow = new WNT_Window(GetSafeHwnd());
	myView->SetWindow(myWntWindow);
	if (!myWntWindow->IsMapped())
		myWntWindow->Map();

	// 5) 기본 배경/리사이즈/좌표축(선택)
	myView->SetBackgroundColor(Quantity_NOC_GRAY50);
	myView->MustBeResized();

	// 좌하단 좌표축(tryedron) 표시 - 화면이 제대로 붙었는지 확인용
	myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.08, V3d_ZBUFFER);

	myView->Redraw();

	myInited = true;
}

// CMyCadViewerView 그리기

void CMyCadViewerView::OnDraw(CDC* /*pDC*/)
{
	CMyCadViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMyCadViewerView 인쇄


void CMyCadViewerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMyCadViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMyCadViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMyCadViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMyCadViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMyCadViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMyCadViewerView 진단

#ifdef _DEBUG
void CMyCadViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CMyCadViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyCadViewerDoc* CMyCadViewerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyCadViewerDoc)));
	return (CMyCadViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyCadViewerView 메시지 처리기

void CMyCadViewerView::OnPaint()
{
	CPaintDC dc(this);

	if (!myView.IsNull())
		myView->Redraw();
}

void CMyCadViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (!myView.IsNull())
	{
		myView->MustBeResized();
		myView->Redraw();
	}
}

void CMyCadViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
	if (myView.IsNull()) return;

	SetCapture();
	myDragMode = DragMode::Rotate;
	myLastPt = point;

	myView->StartRotation(point.x, point.y);
	myRotating = true;
}

void CMyCadViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);
	if (GetCapture() == this) ReleaseCapture();

	myDragMode = DragMode::None;
	myRotating = false;
}

void CMyCadViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CView::OnMButtonDown(nFlags, point);
	if (myView.IsNull()) return;

	SetCapture();
	myDragMode = DragMode::Pan;
	myLastPt = point;
}

void CMyCadViewerView::OnMButtonUp(UINT nFlags, CPoint point)
{
	CView::OnMButtonUp(nFlags, point);
	if (GetCapture() == this) ReleaseCapture();

	myDragMode = DragMode::None;
}

void CMyCadViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);
	if (myView.IsNull()) return;

	if (myDragMode == DragMode::Rotate && myRotating)
	{
		myView->Rotation(point.x, point.y);
		myView->Redraw();
	}
	else if (myDragMode == DragMode::Pan)
	{
		const int dx = point.x - myLastPt.x;
		const int dy = point.y - myLastPt.y;

		myView->Pan(dx, -dy);
		myView->Redraw();

		myLastPt = point;
	}
}

BOOL CMyCadViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (myView.IsNull()) return CView::OnMouseWheel(nFlags, zDelta, pt);

	ScreenToClient(&pt);

	const Standard_Real factor = (zDelta > 0) ? 0.9 : 1.1;

	const int x = pt.x, y = pt.y;
	myView->Zoom(x, y, (int)(x + 100 * factor), (int)(y + 100 * factor));
	myView->Redraw();

	return TRUE;
}
