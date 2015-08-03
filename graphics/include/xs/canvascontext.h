#ifndef _XS_CONTEXT2D_H_
#define _XS_CONTEXT2D_H_

#include <xs/object.h>
#include <xs/line.h>
#include <xs/rectangle.h>
#include <xs/arc.h>
#include <xs/beziercurve.h>

typedef enum _xsTextAlign
{
	XS_TEXT_ALIGN_START = 0,
	XS_TEXT_ALIGN_END,
	XS_TEXT_ALIGN_LEFT,
	XS_TEXT_ALIGN_CENTER,
	XS_TEXT_ALIGN_RIGHT
}xsTextAlign;

typedef enum _xsTextBaseline
{
	XS_BASELINE_ALPHABETIC = 0,
	XS_BASELINE_TOP,
	XS_BASELINE_HANGING,
	XS_BASELINE_MIDDLE,
	XS_BASELINE_IDEOGRAPHIC,
	XS_BASELINE_BOTTOM
}xsTextBaseline;

typedef enum _xsStyleKind
{
	XS_COLOR = 0,
	XS_PATTERN,
	XS_GRADIENT,
}styleKind;

typedef struct _xsDrawStyle
{
	styleKind kind;
	union
	{
		xsColor styleColor;
		xsGradientPattern *stylePattern;
		xsGradientPattern *styleGradient;
	};
}drawStyle;

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

}xsCanvasAttribute;

typedef struct _xsGraphicsGroup
{
	xsArrayList lines;
	struct _xsGraphicsGroup *next;
}xsGraphicsGroup;

typedef struct _xsTextSize
{
	float width;
	float height;
}xsTextSize;

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
	xsCanvasContext();
	virtual ~xsCanvasContext();

	void clear();
	void createGraphics();
	void addLine(xsLine *line);
	void addRect(xsRectangle *rect);
	void addArc(xsArc *arc);
	void addCurve(xsBezierCurve *curve);
	void addStatus(xsCanvasAttribute* status);
	void paintStroke(xsGraphics *gc);
	void paintFill(xsGraphics *gc);
	void drawWithBaseline(const char* text,float x, float y, int maxWidth, int drawFlag);

public:

	void moveTo(float x, float y);
	void lineTo(float x, float y);

	void stroke();
	void fill();
	void rect(float x, float y, float width, float height);
	void fillRect(float x, float y, float width, float height);
	void strokeRect(float x, float y, float width, float height);
	void clearRect(float x, float y, float width, float height);
	void arc(float x, float y, float radius, float startAngle, float endAngle, xsBool anticlockwise = XS_FALSE);
	void arcTo(float x1, float y1, float x2, float y2, float radius);
	void beginPath();
	void closePath();
	void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
	void quadraticCurveTo(float qcpx, float qcpy, float qx, float qy);
	void translate(float x, float y);
	void scale(float scalewidth, float scaleheight);
	void rotate(float angle);
	void transform(float xx, float yx, float xy, float yy, float x0, float y0);
	bool isPointInPath(float x, float y);
//	createLinearGradient();
//	createPattern();
//	createRadialGradient()
//	setTransform();
	void drawImage(xsImage* image, float x, float y);
	void drawImage(xsImage* image,float x, float y, float width, float height);
	void drawImage(xsImage* image,float sx, float sy, float swidth, float sheight, float x, float y, float width, float height);
	void clip();
	void fillText(const char* text,float x, float y, xsU32 maxWidth = 1024);
	void strokeText(const char* text, float x, float y, xsU32 maxWidth = 1024);
	xsTextSize measureText(const char *text);//返回包含一个对象，该对象包含以像素计的指定字体尺寸。
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

	//当前画笔路径填充时只计算线段，其他图形不加入
	xsGraphicsGroup *graphicsGroups;//所有图形段
	xsGraphicsGroup *currentgraphics;//当前图形段
	xsArrayList rects;//矩形存储区
	xsArrayList arcs;//弧形存储区
	xsArrayList curves;//曲线存储区
	xsRectangle *currentRect;
	xsStack statusArchive;//canvas属性存档

	float lastX;
	float lastY;
	float beginX;
	float beginY;

	xsBool  beginFlag;//标识画线起点是否已存在
};

#endif /* _XS_CONTEXT2D_H_ */
