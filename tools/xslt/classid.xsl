<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:redirect="http://xml.apache.org/xalan/redirect"
	xmlns:java="http://xml.apache.org/xalan/java"
	extension-element-prefixes="redirect"
	exclude-result-prefixes="java">

<xsl:output method="text" indent="no" encoding="utf-8"/>
<xsl:variable name="basedir" select="'../../gen/'"/>
<xsl:variable name="padding" select="'                           '"/>

<xsl:template match="/">
#ifndef _XS_CLASSID_H_
#define _XS_CLASSID_H_
<xsl:text>
</xsl:text>
<!-- Class Ids -->
	<xsl:for-each select="/classdef/class">
		<xsl:sort select="@name"/>
		<xsl:variable name="hash" select="java:org.xskit.bon.BONObject.hashStringHex(string(@xtag))"/>
		<xsl:variable name="clsName" select="string(@name)"/>
		<xsl:variable name="clsNameLen" select="string-length($clsName)"/>
		<xsl:variable name="line" select="concat('#define XS_CLS_', java:toUpperCase($clsName), substring($padding, $clsNameLen), '(xsU32)0x', $hash)"/>
<xsl:value-of select="$line"/>
<xsl:text>
</xsl:text>
	</xsl:for-each>
<xsl:text>
</xsl:text>
<!-- Property Ids -->
	<xsl:for-each select="/classdef/class">
	<xsl:sort select="@name"/>
	<xsl:variable name="clsName" select="string(@name)"/>
	<xsl:variable name="clsXtag" select="string(@xtag)"/>
	<xsl:variable name="clsId" select="java:org.xskit.bon.BONObject.hashStringHex(string(@xtag))"/>
	<!-- Generate class id define. -->
	<xsl:variable name="filename" select="concat($basedir, java:toLowerCase($clsName), '_def.cpp')"/>
	<xsl:variable name="headerName" select="concat(java:toLowerCase(string($clsName)), '.h')" />
	<xsl:variable name="baseCls" select="string(@inherit)"/>

	<redirect:write select="$filename">
#include &lt;xs/<xsl:value-of select="$headerName"/>&gt;

static const xsPropertyMap xs<xsl:value-of select="$clsName"/>Properties[] = {
	</redirect:write>
	<xsl:for-each select="property">
		<xsl:sort select="@name"/>
		<xsl:variable name="hash" select="java:org.xskit.bon.BONObject.hashStringHex(string(@name))"/>
		<xsl:variable name="fieldName" select="concat($clsName, '_', @name)"/>
		<xsl:variable name="fieldNameLen" select="java:length($fieldName)"/>
		<xsl:variable name="line" select="concat('#define XS_PROP_', java:toUpperCase($fieldName), substring($padding, $fieldNameLen), '(xsU32)0x', $hash)"/>
<xsl:value-of select="$line"/>
<xsl:text>
</xsl:text>
	<redirect:write select="$filename" append="true">
	{"<xsl:value-of select="@name"/>", <xsl:value-of select="concat(substring($padding, string-length(@name)), '(xsU32)0x', $hash)"/><xsl:text>},</xsl:text>
	</redirect:write>
	</xsl:for-each>
<!-- Event Ids, continue with properties. -->
	<xsl:variable name="propCount" select="count(property)"/>
	<xsl:for-each select="event">
		<xsl:sort select="@name"/>
		<xsl:variable name="hash" select="java:org.xskit.bon.BONObject.hashStringHex(string(@name))"/>
		<xsl:variable name="fieldName" select="concat($clsName, '_', @name)"/>
		<xsl:variable name="fieldNameLen" select="java:length($fieldName)"/>
		<xsl:variable name="line" select="concat('#define XS_PROP_', java:toUpperCase($fieldName), substring($padding, $fieldNameLen), '(xsU32)0x', $hash)"/>
<xsl:value-of select="$line"/>
<xsl:text>
</xsl:text>
	<redirect:write select="$filename" append="true">
	{"<xsl:value-of select="@name"/>", <xsl:value-of select="concat(substring($padding, string-length(@name)), '(xsU32)0x', $hash)"/><xsl:text>},</xsl:text>
	</redirect:write>
	</xsl:for-each>
<xsl:text>
</xsl:text>
	<redirect:write select="$filename" append="true">
	{NULL, 0}
};
	<xsl:choose>
			<xsl:when test="@inherit">
XS_OBJECT_IMPLEMENT(<xsl:value-of select="concat($clsName, ', XS_CLS_', java:toUpperCase($clsName), ', ', $baseCls)"/>)
			</xsl:when>
			<xsl:otherwise>
XS_OBJECT_IMPLEMENT_BASE(<xsl:value-of select="concat($clsName, ', XS_CLS_', java:toUpperCase($clsName))"/>)
			</xsl:otherwise>
	</xsl:choose>

	</redirect:write>
	</xsl:for-each>

#endif /* _XS_CLASSID_H_ */

</xsl:template>
</xsl:stylesheet>