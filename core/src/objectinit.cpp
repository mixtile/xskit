#include <xs/coreapplication.h>
#include <xs/canvas.h>

///////////////////////////////////////////////////////////////////////////////


int xsObjInit(void)
{
//	int i = 0;

    XS_TRACE("[CORE]xsObjInit");

    // register inner object classes
    xsObjectRegister(XS_CLASS(xsObject));
    xsObjectRegister(XS_CLASS(xsCoreApplication));
    xsObjectRegister(XS_CLASS(xsManifest));
//	xsObjectRegister(XS_CLASS(xsDataSource));
//	xsObjectRegister(XS_CLASS(xsRemote));

    xsObjectRegister(XS_CLASS(xsCanvas));
    xsObjectRegister(XS_CLASS(xsCanvasContext));
    xsObjectRegister(XS_CLASS(xsShape));
    xsObjectRegister(XS_CLASS(xsLine));
    xsObjectRegister(XS_CLASS(xsRectangle));
    xsObjectRegister(XS_CLASS(xsArc));
    xsObjectRegister(XS_CLASS(xsBezierCurve));

    return XS_EC_OK;
}

void xsObjUninit(void)
{

    // release object class list

}
