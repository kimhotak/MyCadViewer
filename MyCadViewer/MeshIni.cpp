#include "pch.h"

#include "MeshIni.h"

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <IMeshTools_Parameters.hxx>
#include <StlAPI_Writer.hxx>

namespace
{
	constexpr double kPi = 3.1415926535897932384626433832795;

	CString FormatDouble(double v)
	{
		CString s;
		s.Format(_T("%.15g"), v);
		return s;
	}

	CString FormatInt(int v)
	{
		CString s;
		s.Format(_T("%d"), v);
		return s;
	}

	CString GetIniPath()
	{
		TCHAR exePath[MAX_PATH] = {};
		::GetModuleFileName(nullptr, exePath, MAX_PATH);

		CString path(exePath);
		const int pos = path.ReverseFind(_T('\\'));
		if (pos >= 0)
			path = path.Left(pos + 1);
		path += _T("CadViewer.ini");
		return path;
	}

	CString ReadIniStringOrEmpty(const CString& section, const CString& key)
	{
		TCHAR buf[256] = {};
		::GetPrivateProfileString(section, key, _T(""), buf, (DWORD)_countof(buf), GetIniPath());
		return CString(buf);
	}

	void WriteIniString(const CString& section, const CString& key, const CString& value)
	{
		::WritePrivateProfileString(section, key, value, GetIniPath());
	}

	double ReadIniDouble(const CString& section, const CString& key, double defaultValue)
	{
		const CString s = ReadIniStringOrEmpty(section, key);
		if (s.IsEmpty())
		{
			WriteIniString(section, key, FormatDouble(defaultValue));
			return defaultValue;
		}
		return _tstof(s);
	}

	int ReadIniInt(const CString& section, const CString& key, int defaultValue)
	{
		const CString s = ReadIniStringOrEmpty(section, key);
		if (s.IsEmpty())
		{
			WriteIniString(section, key, FormatInt(defaultValue));
			return defaultValue;
		}
		return _ttoi(s);
	}

	bool ReadIniBool(const CString& section, const CString& key, bool defaultValue)
	{
		return ReadIniInt(section, key, defaultValue ? 1 : 0) != 0;
	}
}

MeshParams LoadMeshParamsFromIni(const CString& sectionName, const MeshParams& defaults)
{
	MeshParams p = defaults;

	p.deflection = ReadIniDouble(sectionName, _T("Deflection"), defaults.deflection);

	const double defaultAngleDeg = defaults.angleRad * (180.0 / kPi);
	const double angleDeg = ReadIniDouble(sectionName, _T("AngleDeg"), defaultAngleDeg);
	p.angleRad = angleDeg * (kPi / 180.0);

	p.relative = ReadIniBool(sectionName, _T("Relative"), defaults.relative);
	p.parallel = ReadIniBool(sectionName, _T("Parallel"), defaults.parallel);
	p.minSize = ReadIniDouble(sectionName, _T("MinSize"), defaults.minSize);
	p.cleanBeforeMesh = ReadIniBool(sectionName, _T("CleanBeforeMesh"), defaults.cleanBeforeMesh);

	return p;
}

StlExportParams LoadStlExportParamsFromIni()
{
	MeshParams meshDefaults{};
	meshDefaults.deflection = 0.01;
	meshDefaults.angleRad = 12.0 * (kPi / 180.0);
	meshDefaults.relative = true;
	meshDefaults.parallel = true;
	meshDefaults.minSize = 0.0;
	meshDefaults.cleanBeforeMesh = true;

	StlExportParams p{};
	p.mesh = LoadMeshParamsFromIni(_T("StlExport"), meshDefaults);
	p.binary = ReadIniBool(_T("StlExport"), _T("Binary"), true);
	return p;
}

bool ApplyMeshToShape(TopoDS_Shape& shape, const MeshParams& p)
{
	try
	{
		if (shape.IsNull())
			return false;

		if (p.cleanBeforeMesh)
			BRepTools::Clean(shape);

		IMeshTools_Parameters params;
		params.Deflection = p.deflection;
		params.Angle = p.angleRad;
		params.Relative = p.relative;
		params.InParallel = p.parallel;
		params.MinSize = p.minSize;

		BRepMesh_IncrementalMesh mesher(shape, params);
		(void)mesher;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool ExportStlWithIniParams(const TopoDS_Shape& shape, const CString& outPath, const StlExportParams& p)
{
	try
	{
		if (shape.IsNull())
			return false;

		TopoDS_Shape copyShape = BRepBuilderAPI_Copy(shape);
		if (copyShape.IsNull())
			return false;

		if (!ApplyMeshToShape(copyShape, p.mesh))
			return false;

		StlAPI_Writer writer;
		writer.ASCIIMode() = p.binary ? Standard_False : Standard_True;
		return writer.Write(copyShape, CT2A(outPath));
	}
	catch (...)
	{
		return false;
	}
}
