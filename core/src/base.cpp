#include <xs/pal.h>
#include <xs/memory.h>
#define XS_BASE_SELF_INCLUDE
#include <xs/base.h>
#undef XS_BASE_SELF_INCLUDE

#ifndef XS_MEM_DEBUG
void *xsBase::operator new(size_t size)
{
#ifdef XS_MEM_DEBUG
	void *p = xsMallocNative(size);
	xsMemSet(p, 0xDE, size);
	return p;
#else
	return xsCallocNative(size);
#endif
}

void *xsBase::operator new[](size_t size)
{
#ifdef XS_MEM_DEBUG
	void *p = xsMallocNative(size);
	xsMemSet(p, 0xDE, size);
	return p;
#else
	return xsCallocNative(size);
#endif
}

void xsBase::operator delete(void *p)
{
	if (p != NULL)
		xsFreeNative(p);
}

void xsBase::operator delete[](void *p)
{
	if (p != NULL)
		xsFreeNative(p);
}

#else

void * __cdecl xsBase::operator new(size_t size, const char *file, int line)
{
	return xsCallocDebug(size, file, line);
}

void * __cdecl xsBase::operator new[](size_t size, const char *file, int line)
{
	return xsCallocDebug(size, file, line);
}

void xsBase::operator delete(void *p)
{
	if (p != NULL)
		xsFreeDebug(p, __FILE__, __LINE__);
}

void xsBase::operator delete[](void *p)
{
	if (p != NULL)
		xsFreeDebug(p, __FILE__, __LINE__);
}

void __cdecl xsBase::operator delete(void *p, const char *file, int line)
{
	if (p != NULL)
		xsFreeDebug(p, file, line);
}

void __cdecl xsBase::operator delete[](void *p, const char *file, int line)
{
	if (p != NULL)
		xsFreeDebug(p, file, line);
}

#endif /* XS_MEM_DEBUG */

extern "C" void __cxa_pure_virtual()
{
	XS_ERROR("Pure virtual function called.");
	XS_ASSERT(0);
	xsQuit();
}
