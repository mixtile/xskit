#ifndef _XS_NETWORK_H_
#define _XS_NETWORK_H_

#include <xs/event.h>
#include <xs/core.h>
#include <xs/buffer.h>

#define XS_MAX_URL_LENGTH		1024

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xsProxy
{
	int type;

	char *hostname;
	xsU16 port;
} xsProxy;

enum _xsProxyType
{
	XS_PROXY_NONE		= 0,
	XS_PROXY_HTTP
};

enum _xsConnResultCode
{
	XS_CONN_RES_OK,
	XS_CONN_RES_CLOSED,
	XS_CONN_RES_TIMEOUT
};

int xsStrToAddr(const char *ipStr, xsSockAddr *addr);
xsU16 xsHtons(xsU16 hostshort);

/**
 * 发起HTTP GET请求，通过callback接收异步事件
 * @param url GET请求的目的地址
 * @param singleBuffer 是否将返回内容放入单一缓冲区。为避免GET返回的内容过大而造成内存的过度占用，该选项建议为XS_FALSE。
 * @param cb HTTP事件的回调函数，回调参数为(xsEvent *)
 * @return HTTP请求的句柄
 */
xsHandle xsHttpGet(const char *url, xsBool singleBuffer, xsCallbackFunc cb);

/**
 * 使用HTTP GET请求下载资源并保存到指定的文件。通过callback接收下载进度和状态
 * @param url GET请求的目的地址
 * @param savePath 保存的文件路径
 * @param cb HTTP事件的回调函数，回调参数为(xsEvent *)
 * @return HTTP请求的句柄
 */
xsHandle xsHttpDownload(const char *url, const xsTChar *savePath, xsCallbackFunc cb);

/**
 * 取消正在进行中的HTTP GET请求。已经完成或失败的HTTP GET请求会自动释放占用的资源，无需调用xsHttpCancel。
 * @param 创建请求时返回的句柄
 * @return 成功返回XS_EC_OK, 失败返回错误值
 */
int xsHttpCancel(xsHandle *handle);

#ifdef __cplusplus
}
#endif

#endif /* _XS_NETWORK_H_ */
