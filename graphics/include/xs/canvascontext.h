#ifndef _XS_CONTEXT2D_H_
#define _XS_CONTEXT2D_H_

#include <xs/object.h>
#include <xs/line.h>
#include <xs/rectangle.h>
#include <xs/arc.h>
#include <xs/beziercurve.h>

const int MAXLEN = 1024;//maximum text length.

/**
 * text align type.
 */
typedef enum _xsTextAlign
{
	XS_TEXT_ALIGN_START = 0,//Text begins at the specified location.
	XS_TEXT_ALIGN_END,//End of text at the specified location.
	XS_TEXT_ALIGN_LEFT,//Left-aligned text.
	XS_TEXT_ALIGN_CENTER,//Center of the text is placed in the specified location.
	XS_TEXT_ALIGN_RIGHT//Right-aligned text
} xsTextAlign;

/**
 * text baseline type.
 */
typedef enum _xsTextBaseline
{
	XS_BASELINE_ALPHABETIC = 0,//Text baseline is the baseline of ordinary letters.
	XS_BASELINE_TOP,//Text baseline is the top of the em box.
	XS_BASELINE_HANGING,//Text baseline is the hanging baseline.
	XS_BASELINE_MIDDLE,//Text baseline is the middle of the em box.
	XS_BASELINE_IDEOGRAPHIC,//Text baseline is the ideographic baseline.
	XS_BASELINE_BOTTOM//Text baseline is the bottom of the em box.
} xsTextBaseline;

/**
 * fill or stroke style  type.
 * not used now.
 */
typedef enum _xsStyleKind
{
	XS_COLOR = 0,//use rgb or rgba color to draw
	XS_PATTERN,//use pattern to draw
	XS_GRADIENT,//use gradient color to draw
} xsStyleKind;

/**
 * structure to store fill or stroke data.
 * not used now.
 */
/*
typedef struct _xsDrawStyle
{
	xsStyleKind kind;
	xsColor *styleColor;
	xsGradientPattern *stylePattern;
	xsGradientPattern *styleGradient;
} xsDrawStyle;
*/

/**
 * The structure stores data of canvas properties..
 */
typedef struct _xsCanvasAttribute
{
	xsColor fillColor;
	xsColor strokeColor;
//	xsColor shadowColor;
//	float shadowBlur;
//	float shadowOffsetX;
//	float shadowOffsetY;
	float lineWidth;
////	int globalCompositeOperation;
//	float globalAlpha;
	xsFontType font;
	xsTextAlign textAlign;
	xsTextBaseline textBaseline;

} xsCanvasAttribute;

/**
 * This structure stores the drawing path(line only).
 */
typedef struct _xsGraphicsGroup
{
	xsArrayList lines;
	struct _xsGraphicsGroup *next;
} xsGraphicsGroup;

/**
 * Text width and height are stored here.
 */
typedef struct _xsTextSize
{
	float width;
	float height;
} xsTextSize;

class xsCanvasContext: public xsObject
{
XS_OBJECT_DECLARE(CanvasContext, XS_CLS_CANVASCONTEXT)

public:
	xsColor fillColor;
	xsColor strokeColor;
//	xsColor shadowColor;
//	float shadowBlur;
//	float shadowOffsetX;
//	float shadowOffsetY;
	float lineWidth;
//	int globalCompositeOperation;
//	float globalAlpha;

	xsFontType font;
	xsTextAlign textAlign;
	xsTextBaseline textBaseline;
//	int lineCap;
//	int lineJoin;
//	int miterLimit;

protected:

	void clear();//Clear all the graphics path data.
	void createGraphics();//create a new path.
	void addLine(xsLine *line);//add a line to current path.
	void addRect(xsRectangle *rect);//add a rectangle to the rectangle list.
	void addArc(xsArc *arc);//add a arc to the arc list.
	void addCurve(xsBezierCurve *curve);//add a curve to the curve list.
	void addStatus(xsCanvasAttribute* status);//add a new status to the status list.
	void paintStroke(xsGraphics *gc);//stroke all paths.
	void paintFill(xsGraphics *gc);//fill all paths.
	void drawWithBaseline(const xsTChar* text, int count, float x, float y, float maxWidth, int drawFlag);//draw text width baseline.

public:

	xsCanvasContext();
	virtual ~xsCanvasContext();

	void moveTo(float x, float y);
	void lineTo(float x, float y);

	void stroke();
	void fill();//Filling only the straight-line path.
	void rect(float x, float y, float width, float height);
	void fillRect(float x, float y, float width, float height);
	void strokeRect(float x, float y, float width, float height);
	void clearRect(float x, float y, float width, float height);
	void arc(float x, float y, float radius, float startAngle, float endAngle, xsBool anticlockwise = XS_FALSE);
	void arcTo(float x1, float y1, float x2, float y2, float radius);//unimplemented
	void beginPath();
	void closePath();
	void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
	void quadraticCurveTo(float qcpx, float qcpy, float qx, float qy);//unimplemented on linux,win32 and MTK.
	void translate(float x, float y);
	void scale(float scalewidth, float scaleheight);//Unimplemented on MTK.
	void rotate(float angle);//Unimplemented on MTK.
	void transform(float xx, float yx, float xy, float yy, float x0, float y0);//Unimplemented on MTK.
	bool isPointInPath(float x, float y);//unimplemented
//	createLinearGradient();
//	createRadialGradient();
//	createPattern();
//	setTransform();
	void drawImage(xsImage* image, float x, float y);
	void drawImage(xsImage* image,float x, float y, float width, float height);
	void drawImage(xsImage* image,float sx, float sy, float swidth, float sheight, float x, float y, float width, float height);
	void clip();//unimplemented
	void fillText(const xsTChar* text, float x, float y, xsU32 maxWidth = MAXLEN);
	void strokeText(const xsTChar* text, float x, float y, xsU32 maxWidth = MAXLEN);
	xsTextSize measureText(const xsTChar *text);
//	createImageData()
//	getImageData()
//	putImageData()
//toDataURL()
//createEvent()
	void save();
	void restore();

public:

//	virtual xsBool getProperty(int propId, xsValue *value);
//	virtual xsBool setProperty(int propId, xsValue *value);

private:

	xsGraphicsGroup *graphicsGroups;//All graphics paths
	xsGraphicsGroup *currentgraphics;//current graphics path
	xsArrayList rects;//rectangle list
	xsArrayList arcs;//arc list
	xsArrayList curves;//curve list
	xsStack statusArchive;//canvas status list

	float lastX;//X-coordinate of the final point of the path.
	float lastY;//Y-coordinate of the final point of the path.
	float beginX;//X-coordinate of the starting point of the path.
	float beginY;//Y-coordinate of the starting point of the path.

	xsBool  beginFlag;//Whether the starting point to identify the path already exists.
};

#endif /* _XS_CONTEXT2D_H_ */
