
// MyCadViewerView.h: CMyCadViewerView 클래스의 인터페이스
//

#pragma once

#pragma warning(push)
#pragma warning(disable: 4996)
#include <Standard_Handle.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <WNT_Window.hxx>
#include <STEPControl_Reader.hxx>
#include <TopoDS_Shape.hxx>
#include <PrsDim_LengthDimension.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Vertex.hxx>
#include <NCollection_List.hxx>
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
	void LoadStepFile(const CString& filePath);
	void LoadStlFile(const CString& filePath);
	void FitAll();
	void StartMeasureDistance();

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFileOpen();
	afx_msg void OnViewWireframe();
	afx_msg void OnViewShaded();
	afx_msg void OnViewTop();
	afx_msg void OnViewBottom();
	afx_msg void OnViewFront();
	afx_msg void OnViewBack();
	afx_msg void OnViewRight();
	afx_msg void OnViewLeft();
	afx_msg void OnViewISO();
	afx_msg void OnMeasureDistance();
	afx_msg void OnMeasureClear();
	DECLARE_MESSAGE_MAP()

private:
	Handle(Aspect_DisplayConnection) myDisplay;
	Handle(OpenGl_GraphicDriver)     myDriver;
	Handle(V3d_Viewer)              myViewer;
	Handle(V3d_View)                myView;
	Handle(AIS_InteractiveContext)  myContext;
	Handle(WNT_Window)              myWntWindow;

	bool myInited = false;

	enum class DragMode { None, Rotate, Pan, Measure };
	DragMode myDragMode = DragMode::None;
	CPoint   myLastPt{};
	bool     myRotating = false;

	// 측정 관련 변수
	bool     myMeasureMode = false;
	gp_Pnt   myFirstPoint;
	bool     myFirstPointSelected = false;
	NCollection_List<Handle(PrsDim_LengthDimension)> myDimensions;

	void ApplyStandardView(V3d_TypeOfOrientation theOrientation);
};

#ifndef _DEBUG  // MyCadViewerView.cpp의 디버그 버전
inline CMyCadViewerDoc* CMyCadViewerView::GetDocument() const
   { return reinterpret_cast<CMyCadViewerDoc*>(m_pDocument); }
#endif
