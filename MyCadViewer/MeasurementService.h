#pragma once

#pragma warning(push)
#pragma warning(disable: 4996)
#include <Standard_Handle.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_InteractiveContext.hxx>
#pragma warning(pop)

enum class MeasureElementType { None, Point, Edge, Face, Axis, Plane };

struct MeasureSelection
{
	MeasureElementType type = MeasureElementType::None;
	TopoDS_Shape sourceShape;
	TopoDS_Shape distanceShape;
	gp_Pnt refPoint;
	gp_Dir refDirection;
	bool hasDirection = false;
};

bool BuildMeasureSelectionFromDetected(const Handle(AIS_InteractiveContext)& context,
	MeasureSelection& outSelection, CString& outTypeName);

bool TryComputeAngleDegrees(const MeasureSelection& first, const MeasureSelection& second,
	Standard_Real& outAngleDegrees);

CString FormatMeasureReport(const MeasureSelection& first, const MeasureSelection& second,
	Standard_Real minDistance, const gp_Pnt& pointOnFirst, const gp_Pnt& pointOnSecond,
	bool hasAngle, Standard_Real angleDegrees);
