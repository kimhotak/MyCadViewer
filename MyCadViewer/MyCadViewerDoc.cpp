
// MyCadViewerDoc.cpp: CMyCadViewerDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MyCadViewer.h"
#endif

#include "MyCadViewerDoc.h"

#include <propkey.h>

#pragma warning(push)
#pragma warning(disable: 4996)
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Reader.hxx>
#include <StlAPI_Reader.hxx>
#pragma warning(pop)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	TCollection_AsciiString ToAsciiPath(const CString& path)
	{
		return TCollection_AsciiString(CT2A(path));
	}
}

// CMyCadViewerDoc

IMPLEMENT_DYNCREATE(CMyCadViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyCadViewerDoc, CDocument)
END_MESSAGE_MAP()


// CMyCadViewerDoc 생성/소멸

CMyCadViewerDoc::CMyCadViewerDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CMyCadViewerDoc::~CMyCadViewerDoc()
{
}

BOOL CMyCadViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	myOriginalShape.Nullify();
	myCurrentFilePath.Empty();

	return TRUE;
}

bool CMyCadViewerDoc::LoadCadFile(const CString& filePath, CString& outErrorMessage)
{
	outErrorMessage.Empty();

	const int dotPos = filePath.ReverseFind(_T('.'));
	if (dotPos < 0)
	{
		outErrorMessage = _T("파일 확장자를 확인할 수 없습니다.");
		return false;
	}

	CString extension = filePath.Mid(dotPos + 1);
	extension.MakeLower();

	TopoDS_Shape shape;
	if (extension == _T("stl"))
	{
		StlAPI_Reader reader;
		reader.Read(shape, ToAsciiPath(filePath));
		if (shape.IsNull())
		{
			outErrorMessage = _T("Failed to read STL file");
			return false;
		}
	}
	else if (extension == _T("step") || extension == _T("stp"))
	{
		STEPControl_Reader reader;
		IFSelect_ReturnStatus status = reader.ReadFile(ToAsciiPath(filePath));
		if (status != IFSelect_RetDone)
		{
			outErrorMessage = _T("Failed to read STEP file");
			return false;
		}

		const Standard_Integer nbRoots = reader.TransferRoots();
		if (nbRoots == 0)
		{
			outErrorMessage = _T("No shapes found in STEP file");
			return false;
		}

		shape = reader.OneShape();
	}
	else
	{
		outErrorMessage = _T("Unsupported file type. Please select STEP or STL.");
		return false;
	}

	if (shape.IsNull())
	{
		outErrorMessage = _T("로드된 형상이 비어 있습니다.");
		return false;
	}

	myOriginalShape = shape;
	myCurrentFilePath = filePath;
	SetModifiedFlag(FALSE);
	UpdateAllViews(nullptr);
	return true;
}

const TopoDS_Shape* CMyCadViewerDoc::GetOriginalShape() const
{
	return myOriginalShape.IsNull() ? nullptr : &myOriginalShape;
}

const CString& CMyCadViewerDoc::GetCurrentFilePath() const
{
	return myCurrentFilePath;
}




// CMyCadViewerDoc serialization

void CMyCadViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << myCurrentFilePath;
	}
	else
	{
		ar >> myCurrentFilePath;
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CMyCadViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CMyCadViewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMyCadViewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMyCadViewerDoc 진단

#ifdef _DEBUG
void CMyCadViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMyCadViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMyCadViewerDoc 명령
