#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <stdarg.h>
#include "drawlib.h"
using namespace std;

ShapeProperties::ShapeProperties() 
{r=1.0; g=1.0; b=1.0; a=1.0; texx=0.0; texy=0.0;}

ShapeProperties::ShapeProperties(double r, double g, double b): r(r), g(g), b(b), a(1.0), texx(0.0), texy(0.0)
{}

ShapeProperties::ShapeProperties(const ShapeProperties &arg) 
{r=arg.r; g=arg.g; b=arg.b; a=arg.a; imageId=arg.imageId; texx=arg.texx; texy=arg.texy;}

ShapeProperties::~ShapeProperties() 
{}

bool ShapeProperties::operator <(const ShapeProperties& rhs) const
{
	if(r < rhs.r) return true;
	if(r > rhs.r) return false;
	if(g < rhs.g) return true;
	if(g > rhs.g) return false;
	if(b < rhs.b) return true;
	if(b > rhs.b) return false;
	if(a < rhs.a) return true;
	if(a > rhs.a) return false;
	if(imageId < rhs.imageId) return true;
	if(imageId > rhs.imageId) return false;
	if(texx < rhs.texx) return true;
	if(texx > rhs.texx) return false;
	if(texy < rhs.texy) return true;
	if(texy > rhs.texy) return false;
	return false;
}

// *********************************************

LineProperties::LineProperties() 
{r=1.0; g=1.0; b=1.0; a=1.0; lineWidth=1.0; closedLoop = false;}

LineProperties::LineProperties(double r, double g, double b, double lineWidth): r(r), g(g), b(b), a(1.0),
	lineWidth(lineWidth), closedLoop(false),
	lineJoin("miter"), lineCap("butt")
{}

LineProperties::LineProperties(const LineProperties &arg) : r(arg.r), g(arg.g), b(arg.b), a(arg.a),
	lineWidth(arg.lineWidth), closedLoop(arg.closedLoop),
	lineJoin(arg.lineJoin), lineCap(arg.lineCap)
{}

LineProperties::~LineProperties()
{}

bool LineProperties::operator <(const LineProperties& rhs) const
{
	if(r < rhs.r) return true;
	if(r > rhs.r) return false;
	if(g < rhs.g) return true;
	if(g > rhs.g) return false;
	if(b < rhs.b) return true;
	if(b > rhs.b) return false;
	if(a < rhs.a) return true;
	if(a > rhs.a) return false;
	if(lineWidth < rhs.lineWidth) return true;
	if(lineWidth > rhs.lineWidth) return false;
	if(closedLoop < rhs.closedLoop) return true;
	if(closedLoop > rhs.closedLoop) return false;
	if(lineJoin < rhs.lineJoin) return true;
	if(lineJoin > rhs.lineJoin) return false;
	if(lineCap < rhs.lineCap) return true;
	if(lineCap > rhs.lineCap) return false;
	return false;
}

// *********************************************

TextProperties::TextProperties()
{
	fr=1.0; fg=1.0; fb=1.0; fa=1.0; lr=1.0; lg=1.0; lb=1.0; la=1.0; 
	fontSize=10.0; font="Sans"; outline = false; fill=true; lineWidth = 1.0;
	valign = 0.0; halign = 0.0;
}

TextProperties::TextProperties(double r, double g, double b): fr(r), fg(g), fb(b), fa(1.0),
	lr(r), lg(g), lb(b), la(1.0),
	fontSize(10.0), font("Sans"), outline(false), fill(true), lineWidth(1.0),
	valign(0.0), halign(0.0)
{}

TextProperties::TextProperties(const TextProperties &arg)
{
	fr=arg.fr; fg=arg.fg; fb=arg.fb; fa=arg.fa;
	lr=arg.lr; lg=arg.lg; lb=arg.lb; la=arg.la;  
	fontSize=arg.fontSize; font=arg.font; 
	outline=arg.outline; fill=arg.fill; lineWidth=arg.lineWidth;
	valign=arg.valign; halign=arg.halign;
}

TextProperties::~TextProperties()
{}

bool TextProperties::operator <(const TextProperties& rhs) const
{
	if(fr < rhs.fr) return true;
	if(fr > rhs.fr) return false;
	if(fg < rhs.fg) return true;
	if(fg > rhs.fg) return false;
	if(fb < rhs.fb) return true;
	if(fb > rhs.fb) return false;
	if(fa < rhs.fa) return true;
	if(fa > rhs.fa) return false;
	if(lr < rhs.lr) return true;
	if(lr > rhs.lr) return false;
	if(lg < rhs.lg) return true;
	if(lg > rhs.lg) return false;
	if(lb < rhs.lb) return true;
	if(lb > rhs.lb) return false;
	if(la < rhs.la) return true;
	if(la > rhs.la) return false;
	if(font < rhs.font) return true;
	if(font > rhs.font) return false;
	if(fontSize < rhs.fontSize) return true;
	if(fontSize > rhs.fontSize) return false;
	if(outline < rhs.outline) return true;
	if(outline > rhs.outline) return false;
	if(fill < rhs.fill) return true;
	if(fill > rhs.fill) return false;
	if(lineWidth < rhs.lineWidth) return true;
	if(lineWidth > rhs.lineWidth) return false;
	if(valign < rhs.valign) return true;
	if(valign > rhs.valign) return false;
	if(halign < rhs.halign) return true;
	if(halign > rhs.halign) return false;
	return false;
}

// *************************************

TextLabel::TextLabel() : x(0.0), y(0.0), ang(0.0)
{}

TextLabel::TextLabel(std::string &text, double x, double y, double ang): text(text), x(x), y(y), ang(ang)
{}

TextLabel::TextLabel(const char *text, double x, double y, double ang): text(text), x(x), y(y), ang(ang)
{}

TextLabel::TextLabel(const TextLabel &arg):
	x(arg.x), y(arg.y), text(arg.text), ang(arg.ang)
{}

TextLabel::~TextLabel()
{}

void TextLabel::Translate(double tx, double ty)
{
	this->x += tx;
	this->y += ty;
}

// *************************************

TwistedTextLabel::TwistedTextLabel()
{}

TwistedTextLabel::TwistedTextLabel(std::string &text, const std::vector<TwistedCurveCmd> &path): text(text), path(path)
{}

TwistedTextLabel::TwistedTextLabel(const char *text, const std::vector<TwistedCurveCmd> &path): text(text), path(path)
{}

TwistedTextLabel::TwistedTextLabel(const TwistedTextLabel &arg):
	text(arg.text), path(arg.path)
{}

TwistedTextLabel::~TwistedTextLabel()
{}

void TwistedTextLabel::Translate(double tx, double ty)
{
	for(size_t i = 0;i < path.size(); i++)
	{
		TwistedCurveCmd &cmd = path[i];
		TwistedCurveCmdType cmdType = cmd.first;	
		std::vector<double> &params = cmd.second;

		switch(cmdType)
		{
		case MoveTo:
			for(size_t j=0;j+1 < params.size();j+=2)
			{
				params[j] += tx;
				params[j+1] += ty;
			}	
			break;
		case LineTo:
			for(size_t j=0;j+1 < params.size();j+=2)
			{
				params[j] += tx;
				params[j+1] += ty;
			}	
			break;
		case RelLineTo:
			break;
		case CurveTo:
			for(size_t j=0;j+1 < params.size();j+=2)
			{
				params[j] += tx;
				params[j+1] += ty;
			}	
			break;
		case RelCurveTo:
			break;
		}
	}
}

// *************************************

BaseCmd::BaseCmd(CmdTypes type): type(type)
{}

BaseCmd::BaseCmd(const BaseCmd &arg): type(arg.type)
{}

BaseCmd::~BaseCmd()
{}

BaseCmd *BaseCmd::Clone()
{return new class BaseCmd(*this);}

DrawPolygonsCmd::DrawPolygonsCmd(const std::vector<Polygon> &polygons, const class ShapeProperties &properties) : 
	BaseCmd(CMD_POLYGONS), polygons(polygons), properties(properties)
{}

DrawPolygonsCmd::DrawPolygonsCmd(const DrawPolygonsCmd &arg) : BaseCmd(CMD_POLYGONS), polygons(arg.polygons), properties(arg.properties) 
{}

DrawPolygonsCmd::~DrawPolygonsCmd() 
{}

BaseCmd *DrawPolygonsCmd::Clone()
{return new class DrawPolygonsCmd(*this);}

DrawLinesCmd::DrawLinesCmd(const Contours &lines, const class LineProperties &properties) : BaseCmd(CMD_LINES), 
	lines(lines), properties(properties) 
{}

DrawLinesCmd::DrawLinesCmd(const DrawLinesCmd &arg) : BaseCmd(CMD_LINES), lines(arg.lines), properties(arg.properties)
{}

DrawLinesCmd::~DrawLinesCmd()
{}

BaseCmd *DrawLinesCmd::Clone()
{return new class DrawLinesCmd(*this);}

DrawTextCmd::DrawTextCmd(const std::vector<class TextLabel> &textStrs, const class TextProperties &properties) : BaseCmd(CMD_TEXT), 
	textStrs(textStrs), properties(properties) 
{}

DrawTextCmd::DrawTextCmd(const DrawTextCmd &arg) : BaseCmd(CMD_TEXT), textStrs(arg.textStrs), properties(arg.properties)
{}

DrawTextCmd::~DrawTextCmd()
{}

BaseCmd *DrawTextCmd::Clone()
{return new class DrawTextCmd(*this);}

DrawTwistedTextCmd::DrawTwistedTextCmd(const std::vector<class TwistedTextLabel> &textStrs, const class TextProperties &properties) : 
	BaseCmd(CMD_TWISTED_TEXT), textStrs(textStrs), properties(properties) 
{}

DrawTwistedTextCmd::DrawTwistedTextCmd(const DrawTwistedTextCmd &arg):
	BaseCmd(CMD_TWISTED_TEXT), textStrs(arg.textStrs), properties(arg.properties)
{}

DrawTwistedTextCmd::~DrawTwistedTextCmd()
{}

BaseCmd *DrawTwistedTextCmd::Clone()
{return new class DrawTwistedTextCmd(*this);}

LoadImageResourcesCmd::LoadImageResourcesCmd(const std::map<std::string, std::string> &loadIdToFilenameMapping):
	BaseCmd(CMD_LOAD_RESOURCES), loadIdToFilenameMapping(loadIdToFilenameMapping)
{}

LoadImageResourcesCmd::LoadImageResourcesCmd(const LoadImageResourcesCmd &arg):
	BaseCmd(CMD_LOAD_RESOURCES), loadIdToFilenameMapping(arg.loadIdToFilenameMapping)
{}

LoadImageResourcesCmd::~LoadImageResourcesCmd()
{}

BaseCmd *LoadImageResourcesCmd::Clone()
{return new class LoadImageResourcesCmd(*this);}

UnloadImageResourcesCmd::UnloadImageResourcesCmd(const std::vector<std::string> &unloadIds):
	BaseCmd(CMD_UNLOAD_RESOURCES), unloadIds(unloadIds)
{}

UnloadImageResourcesCmd::UnloadImageResourcesCmd(const UnloadImageResourcesCmd &arg):
	BaseCmd(CMD_UNLOAD_RESOURCES), unloadIds(arg.unloadIds)
{}

UnloadImageResourcesCmd::~UnloadImageResourcesCmd()
{}

BaseCmd *UnloadImageResourcesCmd::Clone()
{return new class UnloadImageResourcesCmd(*this);}

// *************************************

LocalStore::LocalStore() : IDrawLib()
{

}

LocalStore::~LocalStore()
{
	ClearDrawingCmds();
}

void LocalStore::ClearDrawingCmds()
{
	for(size_t i=0;i < cmds.size(); i++)
		delete cmds[i];
	cmds.clear();
}


void LocalStore::AddCmd(class BaseCmd *cmd)
{
	cmds.push_back(cmd->Clone());
}

void LocalStore::AddDrawPolygonsCmd(const std::vector<Polygon> &polygons, const class ShapeProperties &properties)
{
	class DrawPolygonsCmd cmd(polygons, properties);
	this->AddCmd(&cmd);
}

void LocalStore::AddDrawLinesCmd(const Contours &lines, const class LineProperties &properties)
{
	class DrawLinesCmd cmd(lines, properties);
	this->AddCmd(&cmd);
}

void LocalStore::AddDrawTextCmd(const std::vector<class TextLabel> &textStrs, const class TextProperties &properties)
{
	class DrawTextCmd cmd(textStrs, properties);
	this->AddCmd(&cmd);
}

void LocalStore::AddDrawTwistedTextCmd(const std::vector<class TwistedTextLabel> &textStrs, const class TextProperties &properties)
{
	class DrawTwistedTextCmd cmd(textStrs, properties);
	this->AddCmd(&cmd);
}

void LocalStore::AddLoadImageResourcesCmd(const std::map<std::string, std::string> &loadIdToFilenameMapping)
{
	class LoadImageResourcesCmd cmd(loadIdToFilenameMapping);
	this->AddCmd(&cmd);
}

void LocalStore::AddUnloadImageResourcesCmd(const std::vector<std::string> &unloadIds)
{
	class UnloadImageResourcesCmd cmd(unloadIds);
	this->AddCmd(&cmd);
}

int LocalStore::GetTriangleBoundsText(const TextLabel &label, const class TextProperties &properties, 
		TwistedTriangles &trianglesOut)
{
	trianglesOut.clear();
	return -1;
}

int LocalStore::GetTriangleBoundsTwistedText(const TwistedTextLabel &label, 
	const class TextProperties &properties, 
	TwistedTriangles &trianglesOut,double &pathLenOut, double &textLenOut)
{
	pathLenOut = -1.0;
	textLenOut = -1.0;
	trianglesOut.clear();
	return -1;
}

int LocalStore::GetResourceDimensionsFromFilename(const std::string &filename, unsigned &widthOut, unsigned &heightOut)
{
	widthOut = 0;
	heightOut = 0;
	return -1;
}

// ****************************************

///Convenience factory to create a curve command
TwistedCurveCmd NewTwistedCurveCmd(TwistedCurveCmdType ty, int n_args, ...)
{

	switch(ty)
	{
	case CurveTo:
	case RelCurveTo:
		if(n_args != 6) throw std::invalid_argument("Incorrect number of arguments");
		break;
	default:
		if(n_args != 2) throw std::invalid_argument("Incorrect number of arguments");		
		break;
	}

	va_list ap;
	va_start(ap, n_args);
	std::vector<double> vals;
	for(int i=0;i < n_args;i++)
		vals.push_back(va_arg(ap, double));
	va_end(ap);
	return TwistedCurveCmd(ty, vals);
}

void FixBezierToPoints(const Contour &line, std::vector<TwistedCurveCmd> &bezierOut)
{
	bezierOut.clear();
	double cx = 0.0, cy = 0.0;
	double px = 0.0, py = 0.0; //Control point from previous curve sections

	//Control parameters
	double a = 0.2;
	double a2 = 1.0 - a;
	double b = 0.05;

	if(line.size() >= 1)
	{
		cx = line[0].first;
		cy = line[0].second;
		bezierOut.push_back(NewTwistedCurveCmd(MoveTo, 2, cx, line[0].second));
		px = cx; py = cy;
	}
	for(size_t i=1; i < line.size(); i++)
	{
		const Point &pt = line[i];
		double dx = pt.first - cx;
		double dy = pt.second - cy;
		
		//Control point at begining of straight
		double c1x = cx+dx*a;
		double c1y = cy+dy*a;
		double p1x = cx+dx*(a-b);
		double p1y = cy+dy*(a-b);
		double p2x = cx+dx*(a+b);
		double p2y = cy+dy*(a+b);

		//Control point at end of straight (and beginning of curve)
		double c2x = cx+dx*a2;
		double c2y = cy+dy*a2;
		double p3x = cx+dx*(a2-b);
		double p3y = cy+dy*(a2-b);

		bezierOut.push_back(NewTwistedCurveCmd(CurveTo, 6, px, py, p1x, p1y, c1x, c1y));
		bezierOut.push_back(NewTwistedCurveCmd(CurveTo, 6, p2x, p2y, p3x, p3y, c2x, c2y));

		px = cx+dx*(a2+b);
		py = cy+dy*(a2+b);
		cx = pt.first;
		cy = pt.second;
	}

	if(line.size() > 1)
	{
		const Point &penultimate = line[line.size()-2];
		const Point &end = line[line.size()-1];
		double dx = end.first - penultimate.first;
		double dy = end.second - penultimate.second;
		//End the path
		bezierOut.push_back(NewTwistedCurveCmd(CurveTo, 6, px, py, end.first - dx * a, end.second - dy * a, end.first, end.second));
	}
}

