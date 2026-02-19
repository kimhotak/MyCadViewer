#include "pch.h"
#include "MeasurementService.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Ax1.hxx>
#include <cmath>
#pragma warning(pop)

bool BuildMeasureSelectionFromDetected(const Handle(AIS_InteractiveContext)& context,
	MeasureSelection& outSelection, CString& outTypeName)
{
	if (context.IsNull() || !context->HasDetected())
		return false;

	TopoDS_Shape detectedShape = context->DetectedShape();
	if (detectedShape.IsNull())
		return false;

	outSelection = MeasureSelection{};
	outSelection.sourceShape = detectedShape;
	outSelection.distanceShape = detectedShape;

	if (detectedShape.ShapeType() == TopAbs_VERTEX)
	{
		outSelection.type = MeasureElementType::Point;
		outSelection.refPoint = BRep_Tool::Pnt(TopoDS::Vertex(detectedShape));
		outTypeName = _T("점");
		return true;
	}

	if (detectedShape.ShapeType() == TopAbs_EDGE)
	{
		outSelection.type = MeasureElementType::Edge;
		outTypeName = _T("에지");

		BRepAdaptor_Curve curve(TopoDS::Edge(detectedShape));
		const Standard_Real first = curve.FirstParameter();
		const Standard_Real last = curve.LastParameter();
		const Standard_Real mid = 0.5 * (first + last);
		gp_Pnt p;
		gp_Vec v;
		curve.D1(mid, p, v);
		outSelection.refPoint = p;
		if (v.Magnitude() > gp::Resolution())
		{
			outSelection.refDirection = gp_Dir(v);
			outSelection.hasDirection = true;
		}
		return true;
	}

	if (detectedShape.ShapeType() == TopAbs_FACE)
	{
		TopoDS_Face face = TopoDS::Face(detectedShape);
		BRepAdaptor_Surface surf(face, Standard_False);
		if (surf.GetType() == GeomAbs_Cylinder)
		{
			const gp_Ax1 axis = surf.Cylinder().Axis();
			outSelection.type = MeasureElementType::Axis;
			outSelection.refPoint = axis.Location();
			outSelection.refDirection = axis.Direction();
			outSelection.hasDirection = true;
			outTypeName = _T("축");
			return true;
		}

		if (surf.GetType() == GeomAbs_Plane)
		{
			const gp_Pln plane = surf.Plane();
			outSelection.type = MeasureElementType::Plane;
			outSelection.refPoint = plane.Location();
			outSelection.refDirection = plane.Axis().Direction();
			outSelection.hasDirection = true;
			outTypeName = _T("평면");
			return true;
		}

		outSelection.type = MeasureElementType::Face;
		outTypeName = _T("면");
		return true;
	}

	return false;
}

bool TryComputeAngleDegrees(const MeasureSelection& first, const MeasureSelection& second,
	Standard_Real& outAngleDegrees)
{
	outAngleDegrees = 0.0;
	if (!first.hasDirection || !second.hasDirection)
		return false;

	Standard_Real dot = first.refDirection.Dot(second.refDirection);
	if (dot < 0.0)
		dot = -dot;
	if (dot > 1.0)
		dot = 1.0;

	const Standard_Real kRadToDeg = 57.29577951308232;
	const Standard_Real baseDeg = std::acos(dot) * kRadToDeg;

	const bool firstAxis = first.type == MeasureElementType::Axis;
	const bool secondAxis = second.type == MeasureElementType::Axis;
	const bool firstPlane = first.type == MeasureElementType::Plane;
	const bool secondPlane = second.type == MeasureElementType::Plane;

	if ((firstAxis && secondPlane) || (firstPlane && secondAxis))
	{
		outAngleDegrees = 90.0 - baseDeg;
	}
	else
	{
		outAngleDegrees = baseDeg;
	}

	if (outAngleDegrees < 0.0)
		outAngleDegrees = -outAngleDegrees;

	return true;
}

CString FormatMeasureReport(const MeasureSelection& first, const MeasureSelection& second,
	Standard_Real minDistance, const gp_Pnt& pointOnFirst, const gp_Pnt& pointOnSecond,
	bool hasAngle, Standard_Real angleDegrees)
{
	const Standard_Real dx = pointOnSecond.X() - pointOnFirst.X();
	const Standard_Real dy = pointOnSecond.Y() - pointOnFirst.Y();
	const Standard_Real dz = pointOnSecond.Z() - pointOnFirst.Z();

	auto typeToText = [](MeasureElementType t) -> LPCTSTR
	{
		switch (t)
		{
		case MeasureElementType::Point: return _T("점");
		case MeasureElementType::Edge: return _T("에지");
		case MeasureElementType::Face: return _T("면");
		case MeasureElementType::Axis: return _T("축");
		case MeasureElementType::Plane: return _T("평면");
		default: return _T("알수없음");
		}
	};

	CString msg;
	msg.Format(_T("[Measure]\n첫 번째 요소: %s\n두 번째 요소: %s\n\nMinimum distance: %.3f mm\nΔX: %.3f mm\nΔY: %.3f mm\nΔZ: %.3f mm"),
		typeToText(first.type), typeToText(second.type), minDistance, dx, dy, dz);

	if (hasAngle)
	{
		CString angleLine;
		angleLine.Format(_T("\nAngle: %.3f deg"), angleDegrees);
		msg += angleLine;
	}

	return msg;
}
