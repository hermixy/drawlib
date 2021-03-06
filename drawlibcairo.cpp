#include "drawlibcairo.h"
#include <stdlib.h>
#include <pango/pangocairo.h>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "cairotwisted.h"
using namespace std;

DrawLibCairo::DrawLibCairo(cairo_surface_t *surface): LocalStore(),
	surface(surface)
{
	this->cr = cairo_create(surface);
	this->maskSurface = NULL;
}

DrawLibCairo::~DrawLibCairo()
{
	cairo_destroy(this->cr);
	if(this->maskSurface != NULL)
		cairo_surface_destroy(maskSurface);

	for(std::map<std::string, cairo_surface_t *>::iterator it = this->imageResources.begin();
		it != this->imageResources.end();
		it++)
		cairo_surface_destroy(it->second);
	this->imageResources.clear();
}

void DrawLibCairo::Draw()
{
	for(size_t i=0;i < cmds.size(); i++) {
		class BaseCmd *baseCmd = cmds[i];
		switch(baseCmd->type)
		{
		case CMD_POLYGONS:
			this->DrawCmdPolygons(*(class DrawPolygonsCmd *)baseCmd);
			break;
		case CMD_LINES:
			this->DrawCmdLines(*(class DrawLinesCmd *)baseCmd);
			break;
		case CMD_TEXT:
			this->DrawCmdText(*(class DrawTextCmd *)baseCmd);
			break;
		case CMD_TWISTED_TEXT:
			this->DrawCmdTwistedText(*(class DrawTwistedTextCmd *)baseCmd);
			break;
		case CMD_LOAD_RESOURCES:
			this->LoadResources(*(class LoadImageResourcesCmd *)baseCmd);
			break;
		case CMD_UNLOAD_RESOURCES:
			this->UnloadResources(*(class UnloadImageResourcesCmd *)baseCmd);
			break;
		}

	}
}

void DrawLibCairo::CreateMaskSurface(double width, double height)
{
	if(this->maskSurface != NULL) 
		cairo_surface_destroy(maskSurface);

	//Create mask surface
	this->maskSurface = cairo_image_surface_create (CAIRO_FORMAT_A8,
		                round(width),
		                round(height));
	if(cairo_surface_status(this->maskSurface)!=CAIRO_STATUS_SUCCESS)
	{
		cairo_restore(this->cr);
		throw runtime_error("Creating cairo surface failed");
	}
}

void DrawLibCairo::SetPolySource(const class ShapeProperties &properties)
{
	if(properties.imageId.size() == 0)
		cairo_set_source_rgba(cr, properties.r, properties.g, properties.b, properties.a);
	else
	{
		std::map<std::string, cairo_surface_t *>::iterator it = this->imageResources.find(properties.imageId);
		if(it != this->imageResources.end() && cairo_surface_status(it->second)==CAIRO_STATUS_SUCCESS)
		{
			cairo_pattern_t *pattern = cairo_pattern_create_for_surface (it->second);
			cairo_pattern_set_extend (pattern,
                  CAIRO_EXTEND_REPEAT);

			if(properties.texx != 0.0 || properties.texy != 0.0)
			{
				cairo_matrix_t mat;
				cairo_matrix_init_translate (&mat, properties.texx, properties.texy);
				cairo_pattern_set_matrix(pattern, &mat);
			}

			cairo_set_source (cr,
                      pattern);
			cairo_pattern_destroy (pattern);
		}
		else
			cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, properties.a);
	}
}

void DrawLibCairo::DrawCmdPolygons(class DrawPolygonsCmd &polygonsCmd)
{
	cairo_save (this->cr);
	const class ShapeProperties &properties = polygonsCmd.properties;

	//Draw outer polygons
	const std::vector<Polygon> &polygons = polygonsCmd.polygons;
	for(size_t i=0;i < polygons.size();i++)
	{
		const Polygon &polygon = polygons[i];
		const Contour &outer = polygon.first;
		const Contours &inners = polygon.second;

		double x1=0.0, x2=0.0, y1=0.0, y2=0.0;

		if(inners.size() > 0)
		{
			this->GetDrawableExtents(x1, y1, x2, y2);
			double width = x2 - x1;
			double height = y2 - y1;

			if(maskSurface == NULL)
				this->CreateMaskSurface(width, height);
			else
			{
				//Check if mask surface can be reused
				int msw = cairo_image_surface_get_width (maskSurface);
				int msh = cairo_image_surface_get_height (maskSurface);
				if(round(width) != msw || round(height) != msh)
				{
					//Surface must be recreated
					this->CreateMaskSurface(width, height);
				}
			}

			cairo_t *maskCr = cairo_create (maskSurface);
			cairo_set_operator (maskCr, CAIRO_OPERATOR_SOURCE);

			//Clear mask surface content
			cairo_set_source_rgba(maskCr, 0.0, 0.0, 0.0, 0.0);
			cairo_move_to(maskCr, 0.0, 0.0);
			cairo_line_to(maskCr, width, 0.0);
			cairo_line_to(maskCr, width, height);
			cairo_line_to(maskCr, 0.0, height);
			cairo_fill (maskCr);

			//Draw outer polygon to mask surface
			cairo_set_source_rgba(maskCr, 1.0, 1.0, 1.0, 1.0);
			if(outer.size() > 0) {
				cairo_move_to(maskCr, outer[0].first-x1, outer[0].second-y1);
				for(size_t pt=1;pt < outer.size();pt++)
					cairo_line_to(maskCr, outer[pt].first-x1, outer[pt].second-y1);
				cairo_fill (maskCr);
			}

			//Draw inner polygons to mask surface
			cairo_set_source_rgba(maskCr, 0.0, 0.0, 0.0, 0.0);
			for(size_t j=0; j < inners.size(); j++)
			{
				const Contour &inner = inners[j];
				if(inner.size() > 0) {
					cairo_move_to(maskCr, inner[0].first-x1, inner[0].second-y1);
					for(size_t pt=1;pt < inner.size();pt++)
						cairo_line_to(maskCr, inner[pt].first-x1, inner[pt].second-y1);
					cairo_fill (maskCr);
				}
			}

			cairo_surface_flush(maskSurface);
			cairo_destroy(maskCr);

			//Fill using mask surface
			this->SetPolySource(properties);
			cairo_mask_surface(cr, maskSurface, x1, y1);
			cairo_fill (cr);
			
		}
		else
		{
			this->SetPolySource(properties);

			//Draw outer polygon
			if(outer.size() > 0) {
				cairo_move_to(cr, outer[0].first, outer[0].second);
				for(size_t pt=1;pt < outer.size();pt++)
					cairo_line_to(cr, outer[pt].first, outer[pt].second);
				cairo_fill (cr);
			}
		}

	}
	cairo_restore(this->cr);
}

void DrawLibCairo::DrawCmdLines(class DrawLinesCmd &linesCmd)
{
	cairo_save (this->cr);
	const class LineProperties &properties = linesCmd.properties;
	cairo_set_source_rgba(cr, properties.r, properties.g, properties.b, properties.a);
	cairo_set_line_width (cr, properties.lineWidth);

	if(properties.lineCap == "butt") //cairo default
		cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
	if(properties.lineCap == "sqaure")
		cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	if(properties.lineCap == "round")
		cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

	if(properties.lineJoin == "miter") //cairo default
		cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
	if(properties.lineJoin == "round") //cairo default
		cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	if(properties.lineJoin == "bevel") //cairo default
		cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);

	const Contours &lines = linesCmd.lines;
	for(size_t i=0;i < lines.size();i++)
	{
		const Contour &contour = lines[i];
		if(contour.size() > 0)
			cairo_move_to(cr, contour[0].first, contour[0].second);
		for(size_t pt=1;pt < contour.size();pt++)
		{
			cairo_line_to(cr, contour[pt].first, contour[pt].second);
		}
		if(properties.closedLoop)
			cairo_close_path (cr);

		cairo_stroke (cr);
	}
	cairo_restore(this->cr);
}

void DrawLibCairo::DrawCmdText(class DrawTextCmd &textCmd)
{
	cairo_save (this->cr);
	const class TextProperties &properties = textCmd.properties;
	cairo_set_font_size(cr, properties.fontSize);
	cairo_select_font_face(cr, properties.font.c_str(), CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	if(properties.outline)
		cairo_set_line_width (cr, properties.lineWidth);

	const std::vector<class TextLabel> &textStrs = textCmd.textStrs;
	for(size_t i=0;i < textStrs.size();i++)
	{
		cairo_text_extents_t extents;
		cairo_text_extents (cr,
                    textStrs[i].text.c_str(),
                    &extents);


		if(properties.outline)
		{
			cairo_move_to(cr, textStrs[i].x, textStrs[i].y + extents.height);
			cairo_save (this->cr);
			if(textStrs[i].ang!= 0.0)
				cairo_rotate (cr, textStrs[i].ang);

			cairo_set_source_rgba(cr, properties.lr, properties.lg, properties.lb, properties.la);
			cairo_text_path(cr, textStrs[i].text.c_str());
			cairo_stroke (cr);

			cairo_restore(this->cr);
		}

		if(properties.fill)
		{
			cairo_move_to(cr, textStrs[i].x, textStrs[i].y + extents.height);
			cairo_save (this->cr);
			if(textStrs[i].ang!= 0.0)
				cairo_rotate (cr, textStrs[i].ang);

			cairo_set_source_rgba(cr, properties.fr, properties.fg, properties.fb, properties.fa);
			cairo_show_text(cr, textStrs[i].text.c_str());

			cairo_restore(this->cr);
		}
		
	}
	cairo_restore(this->cr);
}

void DrawLibCairo::DrawCmdTwistedText(class DrawTwistedTextCmd &textCmd)
{
	throw std::runtime_error("Not implemented");
}

void DrawLibCairo::LoadResources(class LoadImageResourcesCmd &resourcesCmd)
{
	for(std::map<std::string, std::string>::const_iterator it = resourcesCmd.loadIdToFilenameMapping.begin();
		it != resourcesCmd.loadIdToFilenameMapping.end();
		it++)
	{
		cairo_surface_t *surf = NULL;
		#ifdef CAIRO_HAS_PNG_FUNCTIONS
		surf = cairo_image_surface_create_from_png(it->second.c_str());
		#endif //CAIRO_HAS_PNG_FUNCTIONS
		this->imageResources[it->first] = surf;
	}
}

void DrawLibCairo::UnloadResources(class UnloadImageResourcesCmd &resourcesCmd)
{
	for(size_t i=0; i< resourcesCmd.unloadIds.size(); i++)
	{
		const std::string &resId = resourcesCmd.unloadIds[i];
		std::map<std::string, cairo_surface_t *>::iterator it = this->imageResources.find(resId);
		if(it != this->imageResources.end())
		{
			cairo_surface_destroy(it->second);
			this->imageResources.erase(it);
		}
	}
}

int DrawLibCairo::GetTriangleBoundsText(const TextLabel &label, const class TextProperties &properties, 
		TwistedTriangles &trianglesOut)
{
	trianglesOut.clear();
	cairo_save (this->cr);
	cairo_set_font_size(cr, properties.fontSize);
	cairo_select_font_face(cr, properties.font.c_str(), CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);

	cairo_text_extents_t extents;
	cairo_text_extents (cr,
                label.text.c_str(),
                &extents);
	double width = extents.width;
	double height = extents.height;
	cairo_restore(this->cr);

	std::vector<Point> tri1;
	tri1.push_back(Point(label.x, label.y));
	tri1.push_back(Point(label.x+width, label.y));
	tri1.push_back(Point(label.x, label.y+height));
	trianglesOut.push_back(tri1);

	std::vector<Point> tri2;
	tri2.push_back(Point(label.x, label.y+height));
	tri2.push_back(Point(label.x+width, label.y));
	tri2.push_back(Point(label.x+width, label.y+height));
	trianglesOut.push_back(tri2);

	return 0;
}

int DrawLibCairo::GetDrawableExtents(double &x1,
	double &y1,
	double &x2,
	double &y2)
{
	cairo_clip_extents (this->cr, &x1, &y1, &x2, &y2);
	return 0; //Zero means ok, non-zero is not ok
}

int DrawLibCairo::GetResourceDimensionsFromFilename(const std::string &filename, unsigned &widthOut, unsigned &heightOut)
{
	int ret = -1;
	cairo_surface_t *surf = NULL;
	#ifdef CAIRO_HAS_PNG_FUNCTIONS
	surf = cairo_image_surface_create_from_png(filename.c_str());
	#endif //CAIRO_HAS_PNG_FUNCTIONS
	if(surf!=NULL)
	{
		widthOut = cairo_image_surface_get_width(surf);
		heightOut = cairo_image_surface_get_height(surf);
		cairo_surface_destroy(surf);
		ret = 0;
	}	
	return ret;
}

// *****************************************************

DrawLibCairoPango::DrawLibCairoPango(cairo_surface_t *surface) : DrawLibCairo(surface)
{
	
}

DrawLibCairoPango::~DrawLibCairoPango()
{

}

void DrawLibCairoPango::DrawCmdText(class DrawTextCmd &textCmd)
{
	cairo_save (this->cr);
	const class TextProperties &properties = textCmd.properties;

	PangoFontDescription *desc = pango_font_description_from_string (properties.font.c_str());
	pango_font_description_set_size (desc, round(properties.fontSize * PANGO_SCALE));

	const std::vector<class TextLabel> &textStrs = textCmd.textStrs;
	for(size_t i=0;i < textStrs.size();i++)
	{
		PangoLayout *layout = pango_cairo_create_layout (cr);

		pango_layout_set_text (layout, textStrs[i].text.c_str(), -1);
		pango_layout_set_font_description (layout, desc);

		PangoRectangle ink_rect;
		PangoRectangle logical_rect;
		pango_layout_get_pixel_extents (layout,
                                &ink_rect,
                                &logical_rect);

		if(properties.outline)
		{
			cairo_save (this->cr);

			cairo_translate (this->cr,
                  textStrs[i].x - logical_rect.x,
                  textStrs[i].y - logical_rect.y);
			if(textStrs[i].ang!= 0.0)
				cairo_rotate (cr, textStrs[i].ang);
			cairo_translate (this->cr,
                  - logical_rect.width * properties.halign,
                  - logical_rect.height * properties.valign);
			cairo_move_to(cr, 0.0, 
				0.0);

			cairo_set_source_rgba(cr, properties.lr, properties.lg, properties.lb, properties.la);
			cairo_set_line_width(cr, properties.lineWidth);
			pango_cairo_layout_path(cr, layout);
			cairo_stroke(cr);

			cairo_restore(this->cr);
		}

		if(properties.fill)
		{

			cairo_save (this->cr);

			cairo_translate (this->cr,
                  textStrs[i].x - logical_rect.x,
                  textStrs[i].y - logical_rect.y);
			if(textStrs[i].ang!= 0.0)
				cairo_rotate (cr, textStrs[i].ang);
			cairo_translate (this->cr,
                  - logical_rect.width * properties.halign,
                  - logical_rect.height * properties.valign);
			cairo_move_to(cr, 0.0, 
				0.0);

			cairo_set_source_rgba(cr, properties.fr, properties.fg, properties.fb, properties.fa);
			pango_cairo_show_layout (cr, layout);

			cairo_restore(this->cr);

		}

		g_object_unref (layout);
		
	}

	pango_font_description_free (desc);

	cairo_restore(this->cr);
}

void DrawLibCairoPango::DrawCmdTwistedText(class DrawTwistedTextCmd &textCmd)
{
	const class TextProperties &properties = textCmd.properties;
	for(size_t i=0; i< textCmd.textStrs.size(); i++)
	{
		const class TwistedTextLabel &tl = textCmd.textStrs[i];
		double pathLen = 0.0;
		double textLen = 0.0;
		draw_formatted_twisted_text (this->cr, tl.text, tl.path, properties, pathLen, textLen);
	}
}

int DrawLibCairoPango::GetTriangleBoundsText(const TextLabel &label, const class TextProperties &properties, 
		TwistedTriangles &trianglesOut)
{
	trianglesOut.clear();
	PangoFontDescription *desc = pango_font_description_from_string (properties.font.c_str());
	pango_font_description_set_size (desc, round(properties.fontSize * PANGO_SCALE));

	PangoLayout *layout = pango_cairo_create_layout (this->cr);

	pango_layout_set_text (layout, label.text.c_str(), -1);
	pango_layout_set_font_description (layout, desc);

	PangoRectangle ink_rect;
	PangoRectangle logical_rect;
	pango_layout_get_pixel_extents (layout,
                            &ink_rect,
                            &logical_rect);

	double width = logical_rect.width;
	double height = logical_rect.height;

	g_object_unref (layout);

	pango_font_description_free (desc);

	//Find rotated dimensions
	double vwx = width * cos(-label.ang);
	double vwy = - width * sin(-label.ang);
	double vhx = height * cos(-label.ang - M_PI / 2.0);
	double vhy = - height * sin(-label.ang - M_PI / 2.0);

	double valignx = - vhx * properties.valign;
	double valigny = - vhy * properties.valign;
	double halignx = - vwx * properties.halign;
	double haligny = - vwy * properties.halign;
	double alignx = valignx+halignx;
	double aligny = valigny+haligny;

	std::vector<Point> tri1;
	tri1.push_back(Point(label.x+alignx, label.y+aligny));
	tri1.push_back(Point(label.x+vwx+alignx, label.y+vwy+aligny));
	tri1.push_back(Point(label.x+vhx+alignx, label.y+vhy+aligny));
	trianglesOut.push_back(tri1);

	std::vector<Point> tri2;
	tri2.push_back(Point(label.x+vhx+alignx, label.y+vhy+aligny));
	tri2.push_back(Point(label.x+vwx+alignx, label.y+vwy+aligny));
	tri2.push_back(Point(label.x+vwx+vhx+alignx, label.y+vwy+vhy+aligny));
	trianglesOut.push_back(tri2);

	//cairo_set_source_rgba (this->cr, 0.5, 0.5, 0.5, 0.4);
	//fancy_cairo_draw_triangles(this->cr, trianglesOut);

	return 0;
}

int DrawLibCairoPango::GetTriangleBoundsTwistedText(const TwistedTextLabel &label, 
		const class TextProperties &properties, 
		TwistedTriangles &trianglesOut,
		double &pathLenOut, double &textLenOut)
{
	double pathLen = 0.0;
	double textLen = 0.0;
	get_bounding_triangles_twisted_text (this->cr, label.text, label.path,
		properties, trianglesOut, pathLenOut, textLenOut);
	//cairo_set_source_rgba (this->cr, 0.5, 0.5, 0.5, 0.4);
	//fancy_cairo_draw_triangles(this->cr, trianglesOut);
	return 0;
}

