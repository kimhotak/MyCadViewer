
// MyCadViewerView.h: CMyCadViewerView 클래스의 인터페이스
//

#pragma once

#pragma warning(push)
#pragma warning(disable: 4996)
#include <Standard_Handle.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <WNT_Window.hxx>
#pragma warning(pop)


class CMyCadViewerView : public CView
{
protected: // serialization에서만 만들어집니다.
	CMyCadViewerView() noexcept;
	DECLARE_DYNCREATE(CMyCadViewerView)

// 특성입니다.
public:
	CMyCadViewerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMyCadViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	virtual void OnInitialUpdate();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

private:
	Handle(Aspect_DisplayConnection) myDisplay;
	Handle(OpenGl_GraphicDriver)     myDriver;
	Handle(V3d_Viewer)              myViewer;
	Handle(V3d_View)                myView;
	Handle(AIS_InteractiveContext)  myContext;
	Handle(WNT_Window)              myWntWindow;

	bool myInited = false;
};

#ifndef _DEBUG  // MyCadViewerView.cpp의 디버그 버전
inline CMyCadViewerDoc* CMyCadViewerView::GetDocument() const
   { return reinterpret_cast<CMyCadViewerDoc*>(m_pDocument); }
#endif

