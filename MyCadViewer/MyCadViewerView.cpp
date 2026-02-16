
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
