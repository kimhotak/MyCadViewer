#pragma once

#include <afxstr.h>

#include <TopoDS_Shape.hxx>

struct MeshParams
{
	double deflection;
	double angleRad;
	bool relative;
	bool parallel;
	double minSize;
	bool cleanBeforeMesh;
};

struct StlExportParams
{
	MeshParams mesh;
	bool binary;
};

MeshParams LoadMeshParamsFromIni(const CString& sectionName, const MeshParams& defaults);

StlExportParams LoadStlExportParamsFromIni();

bool ApplyMeshToShape(TopoDS_Shape& shape, const MeshParams& p);
bool ExportStlWithIniParams(const TopoDS_Shape& shape, const CString& outPath, const StlExportParams& p);
