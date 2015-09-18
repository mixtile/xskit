#ifndef _XS_PAL_H_
#define _XS_PAL_H_

#include "xsosdep.h"
#include <xs/systypes.h>
#include <xs/sysevent.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error code
 */
enum _xsSysErrorCode
{
	XS_EC_OK			= 0,
	XS_EC_ERROR			= -1,

	XS_EC_NOT_SUPPORT	= -5,

	XS_EC_NOMEM			= -10,
	XS_EC_NOT_EXISTS	= -11,
	XS_EC_OVERFLOW		= -12,

	XS_EC_IN_PROGRESS	= -20,

	XS_EC_END			= -1024,
	XS_EC_BEGIN			= -65536
};


/**
 * @addtogroup pal_dbg Debug Utilities
 * @ingroup pal
 */
/*@{*/

/**
 * Command line type. Indicates command string purpose.
 */
enum _xsCommandLineType
{
	XS_CL_UNKNOWN,
	XS_CL_TEST,
	XS_CL_DEBUG
};

/**
 * Add time header and print trace message.
 * time header format as "MM-DD HH:mm:ss"
 * @param format
 */
XS_INTERFACE void xsTrace(const char *format, ...);

XS_INTERFACE void xsAssert(const char *expression, const char *file, int line);

/**
 * Output debug information to host.
 * @param
 */
XS_INTERFACE void xsOutputDebugInfo(const char *msg);

#if defined(DEBUG) || defined(_DEBUG)
#define XS_DEBUG
#endif

#ifdef XS_DEBUG
#define XS_ASSERT(e)	((e) ? (void)0 : xsAssert(#e, __FILE__, __LINE__))
#else
#define XS_ASSERT(e)
#endif

#if ((__STDC_VERSION__ >= 199901L) || defined(_MSC_VER) && _MSC_VER >= 1400)

	#ifdef XS_DEBUG
	#define XS_TRACE(format, ...)	xsTrace(" T ["__FUNCTION__"] "format, ##__VA_ARGS__)
	#else
	#define XS_TRACE(format, ...)
	#endif

	#define XS_TRACE_NONE(format, ...)
	#define XS_ERROR(format, ...)	xsTrace(" E ["__FUNCTION__"] "format, ##__VA_ARGS__)
	#define XS_WARN(format, ...)	xsTrace(" W ["__FUNCTION__"] "format, ##__VA_ARGS__)
	#define XS_INFO(format, ...)	xsTrace(" I ["__FUNCTION__"] "format, ##__VA_ARGS__)

#else

	#ifdef XS_DEBUG
	#define XS_TRACE	xsTrace
	#else
	#define XS_TRACE	1 ? (void)0 : xsTrace
	#endif

	#define XS_TRACE_NONE	1 ? (void)0 : xsTrace
	#define XS_ERROR	xsTrace
	#define XS_WARN		xsTrace
	#define XS_INFO		xsTrace

#endif


/*@}*/


/**
 * @addtogroup pal_mem Memory Management
 * @ingroup pal
 */
/*@{*/
/**
 * Set buffers to a specified character.
 * @param
 */
XS_INTERFACE void *xsMemSet(void *dest, int c, size_t count);

/**
 * Compare two block of memory.
 */
XS_INTERFACE int xsMemCmp(const void *pointer1, const void *pointer2, size_t size);

/**
 * Copy characters.
 * @param
 */
XS_INTERFACE void *xsMemCpy(void *dest, const void *src, size_t count);

/**
 * Move memory block.
 * @param
 */
XS_INTERFACE void *xsMemMove(void *dest, const void *src, size_t count);

/**
 * Allocate memory.
 * @param
 */
XS_INTERFACE void *xsMallocNative(size_t size);

/**
 * Allocate memory and clear.
 * @param
 */
XS_INTERFACE void *xsCallocNative(size_t size);

/**
 * Free memory.
 * @param
 */
XS_INTERFACE void xsFreeNative(void *pointer);

/**
 * Re-alloc memory.
 * @param
 */
XS_INTERFACE void *xsReAllocNative(void *pointer, size_t size);

/*@}*/


/**
 * @addtogroup pal_str String
 * @ingroup pal
 */
/*@{*/

/**
 * \name ANSI String
 */
/*@{*/

/**
 * Get string length.
 * @param string
 * @return
 */
XS_INTERFACE size_t xsStrLen(const char *string);

/**
 * Copy string.
 * @param
 * @return
 */
XS_INTERFACE char *xsStrCpy(char *strTo, const char *strFrom);

/**
 * Copy specified number of characters.
 * @param
 * @return
 */
XS_INTERFACE char *xsStrCpyN(char *strTo, const char *strFrom, size_t max);

/**
 * Compare strings.
 * @param
 * @return
 */
XS_INTERFACE int xsStrCmp(const char *string1, const char *string2);

/**
 * Compare strings not case-sensitive.
 * @param
 * @return
 */
XS_INTERFACE int xsStrCaseCmp(const char *string1, const char *string2);

/**
 * Compare specified number of characters.
 * @param
 * @return
 */
XS_INTERFACE int xsStrCmpN(const char *string1, const char *string2, int max);

/**
 * Compare specified characters not case-sensitive.
 * @param
 * @return
 */
XS_INTERFACE int xsStrCaseCmpN(const char *string1, const char *string2, int max);

/**
 * Append a string.
 * @param
 * @return
 */
XS_INTERFACE char *xsStrCat(char *strTo, const char *strFrom);

/**
 * Append specified numbers of characters to a string.
 * @param
 * @return
 */
XS_INTERFACE char *xsStrCatN(char *strTo, const char *strFrom, int max);

/**
 * Find a character in a string.
 * @param
 * @return
 */
XS_INTERFACE const char *xsStrChr(const char *string, int c);

/**
 * Find a substring.
 * @param
 * @return
 */
XS_INTERFACE const char *xsStrStr(const char *string, const char *substring);

/**
 * Converts a string to an integer in decimal value.
 * @param
 * @return
 */
XS_INTERFACE int xsStrToInt(const char *string);

/**
 * Convert a integer to decimal number string.
 * @param buffer	Fill with the result. Should at least 11 bytes.
 */
XS_INTERFACE char *xsIntToStr(int value, char *buffer, int radix);

/**
 * Convert an ANSI string to lower-case.
 * @param
 * @return
 */
XS_INTERFACE char *xsStrLwr(char *string);

/**
 * Convert an ANSI string to upper-case.
 * @param
 * @return
 */
XS_INTERFACE char *xsStrUpr(char *string);

/**
 * Write formatted characters to a string.
 * @param
 * @return
 */
XS_INTERFACE int xsSnprintf(char *buffer, size_t size, const char *format, ...);


/*@}*/

/**
 * \name Wide-char String
 */
/*@{*/

/**
 * Get string length.
 * @param string
 * @return
 */
XS_INTERFACE size_t xsWcsLen(const xsWChar *string);

/**
 * Copy string.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsCpy(xsWChar *strTo, const xsWChar *strFrom);

/**
 * Copy specified number of characters.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsCpyN(xsWChar *strTo, const xsWChar *strFrom, size_t max);

/**
 * Compare strings.
 * @param
 * @return
 */
XS_INTERFACE int xsWcsCmp(const xsWChar *string1, const xsWChar *string2);

/**
 * Compare strings not case-sensitive.
 * @param
 * @return
 */
XS_INTERFACE int xsWcsCaseCmp(const xsWChar *string1, const xsWChar *string2);

/**
 * Compare specified number of characters.
 * @param
 * @return
 */
XS_INTERFACE int xsWcsCmpN(const xsWChar *string1, const xsWChar *string2, int max);

/**
 * Compare specified characters not case-sensitive.
 * @param
 * @return
 */
XS_INTERFACE int xsWcsCaseCmpN(const xsWChar *string1, const xsWChar *string2, int max);

/**
 * Append a string.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsCat(xsWChar *strTo, const xsWChar *strFrom);

/**
 * Append specified numbers of characters to a string.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsCatN(xsWChar *strTo, const xsWChar *strFrom, int max);

/**
 * Find a character in a string.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsChr(const xsWChar *string, int c);

/**
 * Find a substring.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsStr(const xsWChar *string, const xsWChar *substring);

/**
 * Converts a string to an integer in decimal value.
 * @param
 * @return
 */
XS_INTERFACE int xsWcsToInt(const xsWChar *string);

/**
 * Convert an ANSI string to lower-case.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsLwr(xsWChar *string);

/**
 * Convert an ANSI string to upper-case.
 * @param
 * @return
 */
XS_INTERFACE xsWChar *xsWcsUpr(xsWChar *string);

/*@}*/

#ifdef DOXYGEN
/**
 * \name Text String
 */
/*@{*/

/**
 * Get string length.
 * @param string
 * @return
 */
size_t xsTcsLen(const xsTChar *string);

/**
 * Copy string.
 * @param
 * @return
 */
xsTChar *xsTcsCpy(xsTChar *strTo, const xsTChar *strFrom);

/**
 * Copy specified number of characters.
 * @param
 * @return
 */
xsTChar *xsTcsCpyN(const xsTChar *strTo, const xsTChar *strFrom, size_t max);

/**
 * Compare strings.
 * @param
 * @return
 */
int xsTcsCmp(const xsTChar *string1, const xsTChar *string2);

/**
 * Compare strings not case-sensitive.
 * @param
 * @return
 */
int xsTcsCaseCmp(const xsTChar *string1, const xsTChar *string2);

/**
 * Compare specified number of characters.
 * @param
 * @return
 */
int xsTcsCmpN(const xsTChar *string1, const xsTChar *string2, int max);

/**
 * Compare specified characters not case-sensitive.
 * @param
 * @return
 */
int xsTcsCaseCmpN(const xsTChar *string1, const xsTChar *string2, int max);

/**
 * Append a string.
 * @param
 * @return
 */
xsTChar *xsTcsCat(xsTChar *strTo, const xsTChar *strFrom);

/**
 * Append specified numbers of characters to a string.
 * @param
 * @return
 */
xsTChar *xsTcsCatN(xsTChar *strTo, const xsTChar *strFrom, int max);

/**
 * Find a character in a string.
 * @param
 * @return
 */
xsTChar *xsTcsChr(const xsTChar *string, int c);

/**
 * Find a substring.
 * @param
 * @return
 */
xsTChar *xsTcsStr(const xsTChar *string, const xsTChar *substring);

/**
 * Converts a string to an integer in decimal value.
 * @param
 * @return
 */
int xsTcsToInt(const xsTChar *string);

/**
 * Convert an ANSI string to lower-case.
 * @param
 * @return
 */
xsTChar *xsTcsLwr(xsTChar *string);

/**
 * Convert an ANSI string to upper-case.
 * @param
 * @return
 */
xsTChar *xsTcsUpr(xsTChar *string);

/*@}*/
#endif

/**
 * Convert locale multi-bytes string to wide-char string.
 * @param dest Pointer to an array of xsWChar elements long enough to store a wide string 'count' characters long.
 * @param src multibyte character string to be interpreted.
 * @param count Maximum number of xsWChar characters to be written to 'dest'.
 * @return The number of characters translated, not including the ending null-character.
 * If an invalid multibyte character is encountered, a -1 value is returned.
 */
XS_INTERFACE size_t xsMbsToWcs(xsWChar *dest, const char *src, size_t count);

/**
 * Convert wide-char string to locale multi-bytes string.
 * @param dest pointer to an array of char elements at least 'count' bytes long.
 * @param src multibyte character string to be interpreted.
 * @param count maximum number of bytes to be written to 'dest'.
 * @return The number of bytes (not characters) translated and written to 'dest', not including the ending null-character.
 * If an invalid multibyte character is encountered, a -1 value is returned.
 */
XS_INTERFACE size_t xsWcsToMbs(char *dest, const xsWChar *src, size_t count);

#ifdef XS_UNICODE

#define xsTcsLen	xsWcsLen
#define xsTcsCpy	xsWcsCpy
#define xsTcsCpyN	xsWcsCpyN
#define xsTcsCmp	xsWcsCmp
#define xsTcsCaseCmp	xsWcsCaseCmp
#define xsTcsCaseCmpN	xsWcsCaseCmpN
#define xsTcsCmpN	xsWcsCmpN
#define xsTcsCat	xsWcsCat
#define xsTcsCatN	xsWcsCatN
#define xsTcsChr	xsWcsChr
#define xsTcsStr	xsWcsStr
#define xsTcsToInt	xsWcsToInt
#define xsTcsLwr	xsWcsLwr
#define xsTcsUpr	xsWcsUpr

#else

#define xsTcsLen	xsStrLen
#define xsTcsCpy	xsStrCpy
#define xsTcsCpyN	xsStrCpyN
#define xsTcsCmp	xsStrCmp
#define xsTcsCaseCmp	xsStrCaseCmp
#define xsTcsCaseCmpN	xsStrCaseCmpN
#define xsTcsCmpN	xsStrCmpN
#define xsTcsCat	xsStrCat
#define xsTcsCatN	xsStrCatN
#define xsTcsChr	xsStrChr
#define xsTcsStr	xsStrStr
#define xsTcsToInt	xsStrToInt
#define xsTcsLwr	xsStrLwr
#define xsTcsUpr	xsStrUpr

#endif

/*@}*/


/**
 * @addtogroup pal_fs File Systems
 * @ingroup pal
 */
/*@{*/

enum
{
	XS_OF_READONLY		= 0x00000001,
	XS_OF_WRITEONLY		= 0x00000002,
	XS_OF_READWRITE		= 0x00000004,
	XS_OF_CREATE		= 0x00000008,
	XS_OF_EXCLUSIVE		= 0x00000010	/// do not create new file if file exists
};

enum
{
	XS_FILE_BEGIN,
	XS_FILE_CURRENT,
	XS_FILE_END
};

#define XS_MAX_PATH			260

/**
 * Get external storage directory path.
 * @param buf Buffer to store directory, at least XS_MAX_PATH xsTChar length
 * @return Return 0 if successful or error code indicates failure.
 */
XS_INTERFACE xsTChar *xsGetExternalStoragePath(xsTChar *buf);

/**
 * Get internal public storage directory path, with endding path delimeter.
 * @param buf Buffer to store directory, at least XS_MAX_PATH xsTChar length
 * @return Return path if successful, or NULL if failed.
 */
XS_INTERFACE xsTChar *xsGetPublicStoragePath(xsTChar *buf);

/**
 * Get internal private storage directory path, with endding path delimeter.
 * @param buf Buffer to store directory, at least XS_MAX_PATH xsTChar length
 * @return Return path if successful, or NULL if failed.
 */
XS_INTERFACE xsTChar *xsGetPrivateStoragePath(xsTChar *buf);

/**
 * Get application local data directory path, with endding path delimeter.
 * @param buf Buffer to store directory, at least XS_MAX_PATH xsTChar length
 * @return Return path if successful, or NULL if failed.
 */
XS_INTERFACE xsTChar *xsGetLocalDataPath(xsTChar *buf);

/**
 * Opens or creates a file by file name, return a file handle.
 * @param filename File name
 * @param mode Read and write mode.
 * @return File handle or return invalid handle when open fail.
 */
XS_INTERFACE xsFile xsOpenFile(const xsTChar *filename, xsU32 style);

/**
 * Is file handle valid.
 * @param handle
 * @return XS_TRUE if handle is valid.
 */
XS_INTERFACE xsBool xsFileHandleValid(xsFile handle);

/**
 * Return an invalid handle.
 * @return An invalid handle.
 */
XS_INTERFACE xsFile xsFileInvalidHandle(void);

/**
 * Close an open file.
 * @param handle
 * @return 0 if closed successfully.
 */
XS_INTERFACE int xsCloseFile(xsFile handle);

/**
 * Read bytes to buffer from file.
 * @param
 * @param
 * @param
 * @return
 */
XS_INTERFACE size_t xsReadFile(xsFile handle, void *buffer, size_t size);

/**
 * Write bytes to file.
 * @return
 */
XS_INTERFACE size_t xsWriteFile(xsFile handle, const void *buffer, size_t size);

/**
 * Move the file cursor to offset specified location
 * @param handle Pointer to xsFile.
 * @param offset Bytes from origin.
 * @param origin Initial position.
 */
XS_INTERFACE long xsSetFilePointer(xsFile handle, long offset, int origin);

/**
 * Gets the current cursor position of a file handle.
 * @param handle
 * @return Current cursor position.
 */
XS_INTERFACE long xsGetFilePointer(xsFile handle);

/**
 * Flush the buffer to disk.
 * @param handle
 * @return 0 if the buffer is successfully flushed.
 */
XS_INTERFACE int xsFlushFile(xsFile handle);

/**
 * Get the file size in bytes.
 * @param handle
 * @return File size or 0 if error occurred.
 */
XS_INTERFACE size_t xsGetFileSize(xsFile handle);

/**
 * Tests whether a specified file exists.
 * @param path file or directory's path
 * @return XS_TRUE if specified file exists or return XS_FALSE
 */
XS_INTERFACE xsBool xsFileExists(const xsTChar *path);

/**
 * Creates a new directory.
 * @param path directory path
 * @return return XS_EC_OK if a new directory was successfully created or error code.
 */
XS_INTERFACE int xsCreateDir(const xsTChar *path);

/**
 * Remove a directory.
 * @param path directory path
 * @return return XS_EC_OK if directory was successfully deleted or error code.
 */
XS_INTERFACE int xsRemoveDir(const xsTChar *path);

/*@}*/

/**
 * @addtogroup pal_res Resource
 * @ingroup pal
 */
/*@{*/

/**
 * Open resource by name, return resource handle.
 * @param name Resource name.
 * @return Resource handle. Use xsResHandleValid() to determine validation.
 */
XS_INTERFACE xsRes xsOpenRes(const xsTChar *name);

/**
 * Determine handle's validation.
 * @param handle Resource handle.
 * @return Return XS_TRUE if valid, otherwise return invalid.
 */
XS_INTERFACE xsBool xsResHandleValid(xsRes handle);

/**
 * Return invalid resource handle.
 * @return
 */
XS_INTERFACE xsRes xsResInvalidHandle(void);

/**
 * Close opened resource handle.
 * @param handle Resource handle.
 */
XS_INTERFACE void xsCloseRes(xsRes handle);

/**
 * Read data from resource to buffer.
 * @param handle Resource handle.
 * @param buffer
 * @param begin
 * @param count
 * @return
 */
XS_INTERFACE size_t xsReadRes(xsRes handle, void *buffer, size_t begin, size_t size);

/*@}*/


/**
 * @addtogroup pal_gui Graphical User Interface
 * @ingroup pal
 */
/*@{*/

typedef struct _xsColor
{
	xsU8 alpha;
	xsU8 red;
	xsU8 green;
	xsU8 blue;
} xsColor;

typedef struct _xsFontType
{
	xsU8 style;
	xsU8 face;
	float size;
} xsFontType;

typedef struct _xsStrokeStyle
{
	float width;
	xsU8 cap;
	xsU8 join;
} xsStrokeStyle;

enum _xsStrokeCap
{
    XS_STROKE_CAP_BUTT,
    XS_STROKE_CAP_ROUND,
    XS_STROKE_CAP_SQUARE
};

typedef struct _xsPoint
{
    float x;
    float y;
} xsPoint;

typedef struct _xsRect
{
	float left;
	float top;
	float right;
	float bottom;
} xsRect;

#define XS_RECT_WIDTH(rp)	(rp->right - rp->left + 1)
#define XS_RECT_HEIGHT(rp)	(rp->bottom - rp->top + 1)

typedef struct _xsGraphicsContext
{
	float xoffset;
	float yoffset;

//	xsRect clip;
	void *surface;	// system awared bitmap resource to draw on
	void *device;	// system awared graphic device, used for draw on surface
	void *fb;		// framebuffer device, direct operate a area of surface
	int pixelDepth; // current framebuffer bits-per-pixel
} xsGraphics;

typedef struct _xsImageParam
{
	int width;
	int height;
	xsS8 loaded;	// 1:loaded, 0:not load, -1:load failed
} xsImageParam;

typedef xsAFD xsImage;

enum _xsImageType
{
	XS_IMGTYPE_UNKNOWN	= 0,
	XS_IMGTYPE_BMP		= 1,
	XS_IMGTYPE_JPEG		= 2,
	XS_IMGTYPE_PNG		= 3,
	XS_IMGTYPE_GIF		= 4,

	XS_IMGTYPE_OTHER	= 255
};

enum _xsFontStyle
{
	XS_FONT_NORMAL		= 0,
	XS_FONT_BOLD		= 0x00000001,
	XS_FONT_ITALIC		= 0x00000002,
	XS_FONT_UNDERLINE	= 0x00000004
};

enum _xsFontDefaultSize
{
	XS_FONT_XX_SMALL	= -1,
	XS_FONT_X_SMALL		= -2,
	XS_FONT_SMALL		= -3,
	XS_FONT_MEDIUM		= -4,
	XS_FONT_LARGE		= -5,
	XS_FONT_X_LARGE		= -6,
	XS_FONT_XX_LARGE	= -7,

	XS_FONT_SIZE_COUNT	= 7
};

enum _xsInputType
{
	XS_INPUT_NORMAL		= 0,
	XS_INPUT_NUMBER,
	XS_INPUT_PASSWORD
};

/**
 * \name Basic Screen operation
 */
/*@{*/

/**
 * Set screen orientation.
 * @param orient can be XS_APP_ORIENT_DEFAULT, XS_APP_ORIENT_LANDSCAPE and
 *  XS_APP_ORIENT_PORTRAIT
 */
XS_INTERFACE xsBool xsSetScreenOrient(int orient);

/**
 * Lock the screen for drawing access. The screen will not be flushed until unlock.
 */
XS_INTERFACE void xsLockScreen(void);

/**
 * Unlock the screen for flush.
 */
XS_INTERFACE void xsUnlockScreen(void);

/**
 * Flush specified clip buffer to physical screen.
 * @param left
 * @param top
 * @param right
 * @param bottom
 */
XS_INTERFACE void xsFlushScreen(int left, int top, int right, int bottom);

/**
 * Get system default graphics context.
 */
XS_INTERFACE xsGraphics *xsGetSystemGc(void);

/**
 * Lock graphics context pixel buffer to direct draw.
 */
XS_INTERFACE void *xsLockPixelBuffer(xsGraphics *gc, int pixelDepth);

/**
 * Unlock pixel buffer.
 */
XS_INTERFACE void xsUnlockPixelBuffer(xsGraphics *gc);

/**
 * Translate coordinate.
 *
 */
XS_INTERFACE void xsGcTranslate(xsGraphics *gc, float xoffset, float yoffset);

/**
 * Rotate coordinate.
 *
 */
XS_INTERFACE void xsGcRotate(xsGraphics *gc, float angle);

/**
 * Scale coordinate.
 *
 */
XS_INTERFACE void xsGcScale(xsGraphics *gc, float scalewidth, float scaleheight);

/**
 * Transform coordinate.
 *
 */
XS_INTERFACE void xsGcTransform(xsGraphics *gc, float xx, float yx, float xy, float yy, float x0, float y0);

/**
 * Get screen dimension in pixels.
 * @param
 */
XS_INTERFACE xsBool xsGetScreenDimension(int *width, int *height);

/**
 * Get client area rectangle.
 * @param
 */
XS_INTERFACE xsBool xsGetClientRect(xsRect *rect);

/**
 * Get current drawing area.
 * @param
 */
XS_INTERFACE void xsGetClipRect(xsGraphics *gc, xsRect *rect);

/**
 * Set drawing area.
 * @param
 */
XS_INTERFACE void xsSetClipRect(xsGraphics *gc, xsRect *rect);

/**
 * Reset drawing area.
 * @param
 */
XS_INTERFACE void xsResetClipRect(xsGraphics *gc);

/**
 * Set drawing color
 */
void xsSetColor(xsGraphics *gc, xsColor color);

/**
 * Set stroke line style
 */
void xsSetStrokeStyle(xsGraphics *gc, xsStrokeStyle *style);

/**
 * Draw a line.
 * @param
 */
XS_INTERFACE void xsDrawLine(xsGraphics *gc, float x1, float y1, float x2, float y2);

/**
 * Draw a rectangle.
 * @param
 */
XS_INTERFACE void xsDrawRectangle(xsGraphics *gc, float x, float y, float width, float height);

/**
 * Fill a rectangle.
 * @param
 */
XS_INTERFACE void xsFillRectangle(xsGraphics *gc, float x, float y, float width, float height);

/**
 * Draw a polygon.
 * @param
 */
XS_INTERFACE void xsDrawPolygon(xsGraphics *gc, xsPoint pt[], xsU32 count);

/**
 * Fill a polygon.
 * @param
 */
XS_INTERFACE void xsFillPolygon(xsGraphics *gc, xsPoint pt[], xsU32 count);

/**
 * Draw a circle.
 * @param
 */
XS_INTERFACE void xsDrawCircle(xsGraphics *gc, int x, int y, int r);

/**
 * Draw a solid circle.
 * @param
 */
XS_INTERFACE void xsFillCircle(xsGraphics *gc, int x, int y, int r);

/**
 * Draw a  arc.
 * @param
 */
XS_INTERFACE void xsDrawArc(xsGraphics *gc, float x, float y, float r, float startAngle, float endAngle);

/**
 * Fill a solid arc.
 * @param
 */
XS_INTERFACE void xsFillArc(xsGraphics *gc, float x, float y, float r, float startAngle, float endAngle);

/**
 * Draw a cubic bezier curve.
 * @param
 */
void xsDrawCubicBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3);

/**
 *Fill a cubic bezier curve.
 * @param
 */
void xsFillCubicBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3);

/**
 * Draw a quadratic bezier curve.
 * @param
 */
void xsDrawQuadraticBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3);

/**
 *Fill a quadratic bezier curve.
 * @param
 */
void xsFillQuadraticBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3);

/**
 * Set the font style and size.
 * @param
 */
XS_INTERFACE void xsSetFont(xsGraphics *gc, xsFontType *font);

XS_INTERFACE float xsGetFontHeight(xsGraphics *gc, xsFontType *font);

/**
 * Draw text in specified coordinates.
 * @param count Specifies the number of characters you want to draw. \nIf count is -1, function will computes the character count automatically.
 */
XS_INTERFACE void xsDrawText(xsGraphics *gc, const xsTChar *text, int count, float x, float y);

/**
 * Draw text with border in specified coordinates.
 * @param
 */
XS_INTERFACE void xsDrawBorderText(xsGraphics *gc, const xsTChar *text, int count, float x, float y, float width, xsColor tc, xsColor bc, xsBool is_bordered);

/**
 * Measures the size of the string in the current font.
 * @param
 */
XS_INTERFACE void xsMeasureText(xsGraphics *gc, const xsTChar *text, int count, xsFontType *font, float *width, float *height);

/**
 * Determine the image type.
 * @param
 */
XS_INTERFACE int xsGetImageType(xsImage *img);

/**
 * Get image's dimension
 * @param
 */
XS_INTERFACE int xsGetImageDimension(xsImage *img, float *width, float *height);

/**
 * Free system awared image object.
 */
XS_INTERFACE void xsFreeImageObject(xsImage *img);

/**
 * Draw image in specified coordinates.
 * @param
 */
XS_INTERFACE void xsDrawImage(xsGraphics *gc, xsImage *img, float x, float y, float width, float height);

/*@}*/

/**
 * \name UI Component
 */
/*@{*/

/**
 * Show text input dialog and get user input.
 */
XS_INTERFACE void xsInputDialog(const xsTChar *prompt, xsTChar *buffer, size_t len,
	int type, xsCallbackFunc cb, void *userdata);

XS_INTERFACE void xsInputCtrlShow(int x, int y, int width, int height,
	const xsFontType *font, xsTChar *buffer, size_t len,
	int type, xsCallbackFunc cbUpdate, void *userdata);
XS_INTERFACE void xsInputCtrlHide(void);
XS_INTERFACE xsBool xsGetCaretPos(int *x, int *y);

/*@}*/

/*@}*/

/**
 * @addtogroup pal_dev Device Control
 * @ingroup pal
 */
/*@{*/

/**
 * Backlight control mode.
 */
enum _xsBacklightMode
{
	/// Turn backlight on
	XS_BACKLIGHT_ON		= 0,
	/// Hold backlight on
	XS_BACKLIGHT_LOCK,
	/// Unhold backlight
	XS_BACKLIGHT_UNLOCK
};

/**
 * Set back light of screen and keyboard (if exists).
 */
XS_INTERFACE void xsSetBacklight(int mode);

/*@}*/


/**
 * @addtogroup pal_sys System Interfaces
 * @ingroup pal
 */
/*@{*/

/**
 * Quit
 */
XS_INTERFACE void xsQuit(void);

/**
 * Set random seed
 */
XS_INTERFACE void xsSeed(int seed);

/**
 * Get a pseudo random number
 */
XS_INTERFACE int xsRandom(void);

/**
 * Get seconds elapsed since January 1, 1970
 */
XS_INTERFACE xsU32 xsTime(void);

/**
 * Fill 'time' struct with current date time.
 */
XS_INTERFACE xsTimeType *xsGetLocalTime(xsTimeType *tmt);

/**
 * Start a timer with specified time-out value.
 */
XS_INTERFACE xsU32 xsStartTimer(xsU32 elapseInMs, xsCallbackFunc cb, void *userdata);

/**
 * Cancel a timer.
 */
XS_INTERFACE void xsStopTimer(xsU32 id);

/**
 * Get device unique information.
 * @return buffer contains information, size indicates information's length by
 * bytes. Caller must use xsFree() to release this buffer.
 */
XS_INTERFACE char *xsGetDeviceInfo(size_t *size);

/*@}*/


/**
 * @addtogroup pal_media Media
 * @ingroup pal
 */
/*@{*/

typedef xsAFD xsMedia;
/**
 * Media volume range, 0 - 10000
 */
#define XS_MAX_MEDIA_VOLUME		10000

/**
 * Media type.
 */
enum xsMediaType
{
	XS_MEDIA_TYPE_UNKNOWN	= 0,
	XS_MEDIA_TYPE_MIDI		= 1,
	XS_MEDIA_TYPE_WAV		= 2,
	XS_MEDIA_TYPE_MP3		= 3,
	XS_MEDIA_TYPE_AAC		= 4
};

/**
 * Open media abstract file and prepare for playing.
 * It is the caller's responsibility to close the abstract file descriptor.
 * @param media descriptor of media file
 * @return XS_EC_OK if successed
 */
XS_INTERFACE int xsMediaOpen(xsMedia *media);

/**
 * Close opened media handle.
 * @param media descriptor of media file
 */
XS_INTERFACE void xsMediaClose(xsMedia *media);

/**
 * Play media.
 * @param media descriptor of media file
 * @return XS_EC_OK if successed
 */
XS_INTERFACE int xsMediaPlay(xsMedia *media);

/**
 * Set loop playing.
 * @param media descriptor of media file
 */
XS_INTERFACE void xsMediaSetLooping(xsMedia *media, xsBool looping);

/**
 * Set playing volume.
 * @param media descriptor of media file
 * @param volume integer between 0 and XS_MAX_MEDIA_VOLUME
 */
XS_INTERFACE void xsMediaSetVolume(xsMedia *media, int volume);

/**
 * Pause playing media.
 * @param media descriptor of media file
 * @return XS_EC_OK if successed
 */
XS_INTERFACE int xsMediaPause(xsMedia *media);

/**
 * Stop play.
 * @param media descriptor of media file
 * @return XS_EC_OK if successed

 */
XS_INTERFACE int xsMediaStop(xsMedia *media);

/*@}*/

/**
 * @addtogroup pal_socket Socket
 * @ingroup pal
 */
/*@{*/

/** GPRS MTU should be 576 */

/**
 * Socket initialize.
 * @param
 */
XS_INTERFACE int xsSocketInitialize(void);

/**
 * Socket terminate.
 */
XS_INTERFACE int xsSocketUninitialize(void);

/**
 * Create a socket.
 */
XS_INTERFACE int xsSocketCreate(void);

/**
 * Close a socket.
 */
XS_INTERFACE int xsSocketClose(int socket);

/**
 * Establishes a connection.
 */
XS_INTERFACE int xsSocketConnect(int socket, const xsSockAddr *addr);

/**
 * Shutdown a connection.
 */
XS_INTERFACE int xsSocketShutdown(int socket, int how);

/**
 * Send bytes.
 */
XS_INTERFACE int xsSocketSend(int socket, const void *buffer, int length);

/**
 * Receive bytes.
 */
XS_INTERFACE int xsSocketRecv(int socket, void *buffer, int length);

/**
 * Resolve hostname to address
 */
XS_INTERFACE int xsGetAddrByName(const char *hostname, xsSockAddr *addr, int tag);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _XS_PAL_H_ */
