/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
 * ����HTTP GET����ͨ��callback�����첽�¼�
 * @param url GET�����Ŀ�ĵ�ַ
 * @param singleBuffer �Ƿ񽫷������ݷ��뵥һ������Ϊ����GET���ص����ݹ�������ڴ�Ĺ��ռ�ã���ѡ���ΪXS_FALSE��
 * @param cb HTTP�¼��Ļص�����ص�����Ϊ(xsEvent *)
 * @return HTTP����ľ��
 */
xsHandle xsHttpGet(const char *url, xsBool singleBuffer, xsCallbackFunc cb);

/**
 * ʹ��HTTP GET����������Դ�����浽ָ�����ļ���ͨ��callback�������ؽ�Ⱥ�״̬
 * @param url GET�����Ŀ�ĵ�ַ
 * @param savePath ������ļ�·��
 * @param cb HTTP�¼��Ļص�����ص�����Ϊ(xsEvent *)
 * @return HTTP����ľ��
 */
xsHandle xsHttpDownload(const char *url, const xsTChar *savePath, xsCallbackFunc cb);

/**
 * ȡ�����ڽ����е�HTTP GET�����Ѿ���ɻ�ʧ�ܵ�HTTP GET������Զ��ͷ�ռ�õ���Դ���������xsHttpCancel��
 * @param ��������ʱ���صľ��
 * @return �ɹ�����XS_EC_OK, ʧ�ܷ��ش���ֵ
 */
int xsHttpCancel(xsHandle *handle);

#ifdef __cplusplus
}
#endif

#endif /* _XS_NETWORK_H_ */
