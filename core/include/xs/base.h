#ifndef _XS_BASE_H_
#define _XS_BASE_H_

#ifdef __cplusplus

#include <xs/pal.h> // which defined XS_MEM_DEBUG

class xsBase
{
public:
#ifndef XS_MEM_DEBUG

	void *operator new(size_t size);
	void *operator new[](size_t size);

#else
	void *operator new(size_t size, const char *file, int line);
	void *operator new[](size_t size, const char *file, int line);
	void operator delete(void *p, const char *file, int line);
	void operator delete[](void *p, const char *file, int line);

#endif /* XS_MEM_DEBUG */

	void operator delete(void *p);
	void operator delete[](void *p);
};

#ifdef XS_MEM_DEBUG

#define XS_NEW_DEBUG				new(__FILE__, __LINE__)
#ifndef XS_BASE_SELF_INCLUDE
#define new							XS_NEW_DEBUG
#endif

#endif

#endif /* __cplusplus */

#endif /* _XS_BASE_H_ */
