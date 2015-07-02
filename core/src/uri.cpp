#include <xs/pal.h>
#include <xs/uri.h>
#include <xs/memory.h>
#include <xs/string.h>

xsUri *xsUriParse(const char *uri)
{
	xsUri *u;
	const char *head, *tail;
	size_t len;
	char split = '/';


	head = uri;

	// Schema
	tail = xsStrStr(head, "://");
	if (tail == NULL)
		return NULL; // not a valid URI

	u = (xsUri *)xsCalloc(sizeof(xsUri));
	if (u == NULL)
		return NULL;

	u->schema = XS_URI_UNKNOWN;
	len = tail - head;
        
	if (xsStrCmpN(head, "file", len) == 0)
		u->schema = XS_URI_FILE;
	else if (xsStrCmpN(head, "http", len) == 0)
		u->schema = XS_URI_HTTP;
	else if (xsStrCmpN(head, "local", len) == 0)
		u->schema = XS_URI_LOCAL;
	else if (xsStrCmpN(head, "res", len) == 0)
		u->schema = XS_URI_RES;
	else if (xsStrCmpN(head, "roi", len) == 0)
		u->schema = XS_URI_ROI;

	head = tail + 3;
	tail = head;

	// schema://base/path/to?query#fragment
	while (*head)
	{
		if(*tail == split || *tail == '?' || *tail == '#' || *tail == 0)
		{
			len = tail - head;

			switch (*head)
			{
			case '/':
				u->path = len > 1 ? xsStrDupN(head + 1, len - 1) : NULL;
				break;
			case '?':
				u->query = len > 1 ? xsStrDupN(head + 1, len - 1) : NULL;
				break;
			case '#':
				u->fragment = len > 1 ? xsStrDupN(head + 1, len - 1) : NULL;
				break;
			default:
				u->base = len > 0 ? xsStrDupN(head, len) : NULL;
				break;
			}

			head = tail;
			if(*head == '/')
				split = '\0'; // skip path's / spliter
		}
		tail++;
	}

	return u;
}

void xsUriDestroy(xsUri *uri)
{
	if (uri == NULL)
		return;

	if (uri->base != NULL)
		xsFree(uri->base);

	if (uri->path != NULL)
		xsFree(uri->path);

	if (uri->query != NULL)
		xsFree(uri->query);

	if (uri->fragment != NULL)
		xsFree(uri->fragment);

	xsFree(uri);
}
