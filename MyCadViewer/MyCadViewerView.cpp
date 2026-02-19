
// MyCadViewerView.cpp: CMyCadViewerView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MyCadViewer.h"
#endif

#include "resource.h"

#include "MyCadViewerDoc.h"
#include "MyCadViewerView.h"
#include "MeasurementService.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#include <Quantity_Color.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <RWStl.hxx>
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
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
	ON_COMMAND(ID_FILE_OPEN, &CMyCadViewerView::OnFileOpen)
	ON_COMMAND(ID_VIEW_WIREFRAME, &CMyCadViewerView::OnViewWireframe)
	ON_COMMAND(ID_VIEW_SHADED, &CMyCadViewerView::OnViewShaded)
	ON_COMMAND(ID_VIEW_TOP, &CMyCadViewerView::OnViewTop)
	ON_COMMAND(ID_VIEW_BOTTOM, &CMyCadViewerView::OnViewBottom)
	ON_COMMAND(ID_VIEW_FRONT, &CMyCadViewerView::OnViewFront)
	ON_COMMAND(ID_VIEW_BACK, &CMyCadViewerView::OnViewBack)
	ON_COMMAND(ID_VIEW_RIGHT, &CMyCadViewerView::OnViewRight)
	ON_COMMAND(ID_VIEW_LEFT, &CMyCadViewerView::OnViewLeft)
	ON_COMMAND(ID_VIEW_ISO, &CMyCadViewerView::OnViewISO)
	ON_COMMAND(ID_MEASURE_DISTANCE, &CMyCadViewerView::OnMeasureDistance)
	ON_COMMAND(ID_MEASURE_CLEAR, &CMyCadViewerView::OnMeasureClear)
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

	// 측정 모드일 때
	if (myMeasureMode)
	{
		myContext->MoveTo(point.x, point.y, myView, Standard_True);

		MeasureSelection selection;
		CString typeName;
		if (!::BuildMeasureSelectionFromDetected(myContext, selection, typeName))
		{
			AfxMessageBox(_T("측정할 요소를 클릭하세요. (점/에지/면/축/평면)"));
			return;
		}

		if (!myFirstSelectionValid)
		{
			myFirstSelection = selection;
			myFirstSelectionValid = true;

			CString msg;
			msg.Format(_T("첫 번째 요소 선택: %s\n두 번째 요소를 선택하세요."), typeName.GetString());
			AfxMessageBox(msg);
		}
		else
		{
			if (myFirstSelection.distanceShape.IsNull() || selection.distanceShape.IsNull())
			{
				AfxMessageBox(_T("거리 계산에 사용할 형상 정보가 없습니다."));
				myFirstSelectionValid = false;
				return;
			}

			BRepExtrema_DistShapeShape distTool(myFirstSelection.distanceShape, selection.distanceShape);
			distTool.Perform();

			if (!distTool.IsDone() || distTool.NbSolution() < 1)
			{
				AfxMessageBox(_T("요소 간 거리 계산에 실패했습니다."));
				myFirstSelectionValid = false;
				return;
			}

			const Standard_Real minDistance = distTool.Value();
			const gp_Pnt pointOnFirst = distTool.PointOnShape1(1);
			const gp_Pnt pointOnSecond = distTool.PointOnShape2(1);

			Handle(PrsDim_LengthDimension) lengthDim = new PrsDim_LengthDimension();
			gp_Pln plane(pointOnFirst, gp::DZ());
			lengthDim->SetMeasuredGeometry(pointOnFirst, pointOnSecond, plane);

			Handle(Prs3d_DimensionAspect) dimAspect = new Prs3d_DimensionAspect();
			dimAspect->MakeArrows3d(Standard_False);
			dimAspect->MakeText3d(Standard_True);
			dimAspect->TextAspect()->SetHeight(5.0);
			dimAspect->SetCommonColor(Quantity_NOC_YELLOW);
			dimAspect->MakeUnitsDisplayed(Standard_False);
			lengthDim->SetDimensionAspect(dimAspect);

			myDimensions.Append(lengthDim);
			myContext->Display(lengthDim, Standard_True);

			Standard_Real angleDeg = 0.0;
			const bool hasAngle = ::TryComputeAngleDegrees(myFirstSelection, selection, angleDeg);
			CString report = ::FormatMeasureReport(myFirstSelection, selection, minDistance, pointOnFirst, pointOnSecond, hasAngle, angleDeg);
			AfxMessageBox(report);

			myFirstSelectionValid = false;
		}

		myView->Redraw();
		return;
	}

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
	myView->Zoom(x, y, (int)(x * factor), (int)(y * factor));
	myView->Redraw();

	return TRUE;
}

void CMyCadViewerView::OnFileOpen()
{
	CFileDialog dlg(TRUE, _T("step"), NULL,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("CAD Files (*.step;*.stp;*.stl)|*.step;*.stp;*.stl|STEP Files (*.step;*.stp)|*.step;*.stp|STL Files (*.stl)|*.stl|All Files (*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString extension = dlg.GetFileExt();
		extension.MakeLower();

		if (extension == _T("stl"))
		{
			LoadStlFile(filePath);
		}
		else if (extension == _T("step") || extension == _T("stp"))
		{
			LoadStepFile(filePath);
		}
		else
		{
			AfxMessageBox(_T("Unsupported file type. Please select STEP or STL."));
		}
	}
}

void CMyCadViewerView::LoadStepFile(const CString& filePath)
{
	if (myContext.IsNull())
	{
		AfxMessageBox(_T("OCCT context not initialized"));
		return;
	}

	STEPControl_Reader reader;
	IFSelect_ReturnStatus status = reader.ReadFile(CT2A(filePath));

	if (status != IFSelect_RetDone)
	{
		AfxMessageBox(_T("Failed to read STEP file"));
		return;
	}

	Standard_Integer nbRoots = reader.TransferRoots();
	if (nbRoots == 0)
	{
		AfxMessageBox(_T("No shapes found in STEP file"));
		return;
	}

	TopoDS_Shape shape = reader.OneShape();

	myContext->RemoveAll(Standard_False);

	Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
	myContext->Display(aisShape, Standard_True);

	FitAll();
}

void CMyCadViewerView::LoadStlFile(const CString& filePath)
{
	if (myContext.IsNull())
	{
		AfxMessageBox(_T("OCCT context not initialized"));
		return;
	}

	TopoDS_Shape shape = RWStl::ReadFile(CT2A(filePath));
	if (shape.IsNull())
	{
		AfxMessageBox(_T("Failed to read STL file"));
		return;
	}


	
	
	
	
	
	myContext->RemoveAll(Standard_False);

	Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
	myContext->Display(aisShape, Standard_True);

	FitAll();
}

void CMyCadViewerView::FitAll()
{
	if (!myView.IsNull())
	{
		myView->FitAll();
		myView->ZFitAll();
		myView->Redraw();
	}
}

void CMyCadViewerView::OnViewWireframe()
{
	if (myContext.IsNull())
		return;

	myContext->SetDisplayMode(AIS_WireFrame, Standard_True);
	myView->Redraw();
}

void CMyCadViewerView::OnViewShaded()
{
	if (myContext.IsNull())
		return;

	myContext->SetDisplayMode(AIS_Shaded, Standard_True);
	myView->Redraw();
}

void CMyCadViewerView::ApplyStandardView(V3d_TypeOfOrientation theOrientation)
{
	if (myView.IsNull())
		return;

	myView->SetProj(theOrientation);
	myView->FitAll();
	myView->ZFitAll();
	myView->Redraw();
}

void CMyCadViewerView::OnViewTop()
{
	ApplyStandardView(V3d_Zpos);
}

void CMyCadViewerView::OnViewBottom()
{
	ApplyStandardView(V3d_Zneg);
}

void CMyCadViewerView::OnViewFront()
{
	ApplyStandardView(V3d_Yneg);
}

void CMyCadViewerView::OnViewBack()
{
	ApplyStandardView(V3d_Ypos);
}

void CMyCadViewerView::OnViewRight()
{
	ApplyStandardView(V3d_Xpos);
}

void CMyCadViewerView::OnViewLeft()
{
	ApplyStandardView(V3d_Xneg);
}

void CMyCadViewerView::OnViewISO()
{
	ApplyStandardView(V3d_XposYposZpos);
}

void CMyCadViewerView::OnMeasureDistance()
{
	if (myContext.IsNull())
		return;

	myMeasureMode = !myMeasureMode;
	myFirstSelectionValid = false;

	if (myMeasureMode)
	{
		AfxMessageBox(_T("측정 모드가 활성화되었습니다. 첫 번째 요소(점/에지/면/축/평면)를 클릭하세요.\n(회전/이동을 하려면 측정 모드를 다시 토글하세요)"));
	}
	else
	{
		AfxMessageBox(_T("측정 모드가 비활성화되었습니다."));
	}
}

void CMyCadViewerView::OnMeasureClear()
{
	if (myContext.IsNull())
		return;

	// 모든 치수 제거
	for (NCollection_List<Handle(PrsDim_LengthDimension)>::Iterator it(myDimensions); it.More(); it.Next())
	{
		myContext->Remove(it.Value(), Standard_False);
	}
	myDimensions.Clear();

	myMeasureMode = false;
	myFirstSelectionValid = false;

	myView->Redraw();
	AfxMessageBox(_T("모든 측정 치수가 제거되었습니다."));
}

void CMyCadViewerView::StartMeasureDistance()
{
	myMeasureMode = true;
	myFirstSelectionValid = false;
}
