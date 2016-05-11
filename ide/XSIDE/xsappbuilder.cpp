#include <QDir>
#include<QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include "xsappbuilder.h"
#include "xs/container.h"
#include "xs/base.h"
#include "xs/pal.h"
#include "xs/coreclassid.h"
#include "xs/bon.h"
#include "xs/array.h"

XSAppBuilder::XSAppBuilder(const QString &projectPath, const QString &projectName)
{
    this->projectPath = projectPath;
    this->projectName = projectName;
    bon = static_cast<xsObject *>(xsObject::createInstance());;
    xsValue val;
    val.type = XS_VALUE_STRING;
    val.data.s = "bon";
    bon->setProperty("id", &val);
    isFirstObject = true;

    QFile transform(":xs_model/models/transform.json");
    if(transform.open(QIODevice::ReadOnly))
    {
        QString data = transform.readAll();
        transform.close();
        QJsonParseError jsonError;
        QJsonDocument transformer = QJsonDocument::fromJson(data.toUtf8(), &jsonError);
        if(jsonError.error == QJsonParseError::NoError)
        {
            root = transformer.object();
        }
        else
        {
            qDebug()<<jsonError.errorString();
        }
    }
}

XSAppBuilder::~XSAppBuilder()
{
    if(NULL != bon)
        bon->destroyInstance();
}

bool XSAppBuilder::packProject()
{
    if(projectPath.isEmpty())
    {
        return false;
    }

    bool ret = false;
    QDir dir(projectPath);
    QFileInfoList infoList = dir.entryInfoList();

    for(int i = 0; i < infoList.size(); i++)
    {
        if(infoList.at(i).isDir())
        {
            QDir jsDir = QDir(infoList.at(i).absoluteFilePath());
            QFileInfoList jsInforList = jsDir.entryInfoList();

            for(int i = 0; i < jsInforList.size(); i++)
            {
                if(jsInforList.at(i).suffix() == "js")
                {
                    QString prefix = jsDir.dirName() + "/";
                    ret = compileScript(jsInforList.at(i), prefix);
                    break;
                }
            }

            continue;
        }

        if(infoList.at(i).suffix() == "xpk")
        {
            continue;
        }
        else if(infoList.at(i).suffix() == "xml" || infoList.at(i).suffix() == "json")
        {
            ret = compileObject(infoList.at(i));
        }
        else if(infoList.at(i).suffix() == "js")
        {
            ret = compileScript(infoList.at(i), "");
        }
        else
        {
            ret = compileBinary(infoList.at(i));
        }

        if(ret == false)
        {
            return ret;
        }
    }

    ret = writeObject();

    return ret;
}

bool XSAppBuilder::compileObject(const QFileInfo &source)
{
    if(source.suffix() == "xml")
    {
        qDebug()<<"xml";
        QFile file(source.filePath());
        QXmlInputSource inputSource(&file);
        QXmlSimpleReader reader;
        reader.setContentHandler(this);
        reader.setErrorHandler(this);
        return reader.parse(inputSource);
    }
    else if(source.suffix() == "json")
    {
        QFile file(source.filePath());
        if(!file.open(QIODevice::ReadOnly))
        {
            return false;
        }
        QString data = file.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument jsonFile = QJsonDocument::fromJson(data.toUtf8(), &jsonError);
        if(jsonError.error == QJsonParseError::NoError)
        {
            if(jsonFile.isArray())
            {
                QJsonArray jsonArray = jsonFile.array();
                qDebug()<<"array: "<<jsonArray;
            }
            else if(jsonFile.isObject())
            {
                QJsonObject jsonObj = jsonFile.object();
                qDebug()<<"object: " << jsonObj;

            }
            return true;
        }
        else
        {
            qDebug()<<jsonError.errorString();
            return false;
        }
    }

    return false;
}

bool XSAppBuilder::compileScript(const QFileInfo &source, const QString &prefix)
{
    int ret = XS_FALSE;
    QFile js(source.filePath());
    if(!js.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray data = js.readAll();
//    QString name = prefix + source.fileName();

    xsValue val;
    val.type = XS_VALUE_STRING;
    val.data.s = (char *)xsCalloc(data.size() + 1);
    strncpy(val.data.s, data.data(), data.size() + 1);
    qDebug()<<source.fileName()<<data.size()+1;
    ret = bon->setProperty(source.fileName().toStdString().c_str(), &val);

    if(ret == XS_TRUE)
    {
        return true;
    }

    return false;
}

bool XSAppBuilder::compileBinary(const QFileInfo &source)
{
    int ret = XS_FALSE;
    QFile binary(source.filePath());
    if(!binary.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray data = binary.readAll();

    xsValue val;
    val.type = XS_VALUE_BINARY;
    val.data.ptr = (void *)xsCalloc(data.size() + sizeof(size_t));
    *((size_t *)val.data.ptr) = data.size();
    memcpy((char *)val.data.ptr + sizeof(size_t), data.data(), data.size());
    qDebug()<<source.fileName()<<data.size()+sizeof(size_t);
    ret = bon->setProperty(source.fileName().toStdString().c_str(), &val);

    if(ret == XS_TRUE)
    {
        return true;
    }

    return false;
}

void XSAppBuilder::compileObjectProperty(const QString &qName, const QXmlAttributes &atts, xsObject *object)
{
    if(root.contains(qName))
    {
        QJsonObject obj = root.value(qName).toObject();
        QJsonObject::const_iterator it = obj.constBegin();
        while(it != obj.constEnd())
        {
            if(it.key() == "prototype")
            {
                it++;
                continue;
            }

            if(it.key() == "@inherit")
            {
                compileObjectProperty(it.value().toString().toLower(), atts, object);
            }
           else if(it.value().isObject())
            {
                if(atts.value(it.key()).isEmpty())
                {
                    it++;
                    continue;
                }

                QJsonObject subObj = it.value().toObject();
                if(subObj.contains("type"))
                {
                    xsValue property;
                    transformValue(subObj, &property, atts.value(it.key()).remove(" "), it.key());
                    object->setProperty(it.key().toStdString().c_str(), &property);
                    if(it.key() == "position")
                    {
                        qDebug()<<it.key().toStdString().c_str();
                        qDebug()<<property.type;
                        qDebug()<<property.data.n;
                    }
                }
            }

            it ++;
        }
    }
}

void XSAppBuilder::createPropertyObject(const QString &proName, const QString &proData, xsObject *proObject)
{
    if(proObject == NULL)
    {
        return;
    }

    if(root.contains(proName))
    {
        QStringList list = proData.split(";", QString::SkipEmptyParts);
        QJsonObject obj = root.value(proName).toObject();
        QJsonObject::const_iterator it = obj.constBegin();

        while(it != obj.constEnd())
        {
            if(it.key() == "prototype" || it.key() == "@prototype")
            {
                it++;
                continue;
            }
            for(int i = 0; i < list.size(); i++)
            {
                if(list.at(i).contains(it.key()))
                {
                    if(it.key() == "@inherit")
                    {
                        createPropertyObject(it.value().toString(), proData, proObject);
                    }
                    else
                    {
                        QJsonObject subObj = it.value().toObject();
                        QStringList dataList = list.at(i).split(":",QString::SkipEmptyParts);
                        if(dataList.size() == 2)
                        {
                            xsValue property;
                            transformValue(subObj, &property, dataList.at(1), it.key());
                            proObject->setProperty(it.key().toStdString().c_str(), &property);
                        }
                    }
                }
            }

            it++;
        }
    }

    return;
}

void XSAppBuilder::transformValue(const QJsonObject &subObj, xsValue *property, const QString &data, const QString &name)
{
    if(subObj.value("type").toString() == "int")
    {
        property->type = XS_VALUE_INT32;
        property->data.n = data.toInt();
    }
    else if(subObj.value("type").toString() == "boolean")
    {
        property->type = XS_VALUE_BOOL;
        if(data == "true")
            property->data.n = true;
        else
            property->data.n = false;
    }
    else if(subObj.value("type").toString() == "hashcode")
    {
        property->type = XS_VALUE_UINT32;
        property->data.n = data.toInt();
    }
    else if(subObj.value("type").toString() == "string")
    {
        property->type = XS_VALUE_STRING;
        property->data.s = const_cast<char *>(data.toStdString().c_str());
    }
    else if(subObj.value("type").toString() == "text")
    {
        property->type = XS_VALUE_TEXT;
#ifdef XS_UNICODE
        property->data.t = const_cast<xsTChar *>(data.toStdWString().c_str());
#else
        property->data.s = const_cast<char *>(data.toStdString().c_str());
#endif
    }
    else if(subObj.value("type").toString() == "array")//功能可能有问题
    {
        QString regex = subObj.value(("regex")).toString();
        QString subStringType = subObj.value("stringType").toString();
        bool toText = false;
        if(!subStringType.isEmpty() && subStringType == "text")
        {
            toText = true;
        }
        if(regex.isEmpty())
        {
            if(toText)
            {
                property->type = XS_VALUE_TEXT;
#ifdef XS_UNICODE
                property->data.t = const_cast<xsTChar *>(data.toStdWString().c_str());
#else
                property->data.s = const_cast<char *>(data.toStdString().c_str());
#endif
            }
            else
            {
                property->type = XS_VALUE_STRING;
                property->data.s = const_cast<char *>(data.toStdString().c_str());
            }
            return;
        }

         xsArray *ar = xsArrayCreate(2);
        property->type = XS_VALUE_ARRAY;
        property->data.array = ar;
        if(regex.length() == 0)
        {
            xsValue str;
            str.type = XS_VALUE_STRING;
            str.data.s = const_cast<char *>(data.toStdString().c_str());
            xsArrayAdd(ar, &str);
        }
        else
        {
            QStringList strs = data.split(regex);
            for(int i = 0; i < strs.size(); i++)
            {
                if(toText)
                {
                    xsValue str;
                    str.type = XS_VALUE_TEXT;
#ifdef XS_UNICODE
                    str.data.t = const_cast<xsTChar *>(strs.at(i).toStdWString().c_str());
#else
                    str.data.s = const_cast<char *>(strs.at(i).toStdString().c_str());
#endif
                }
                else
                {
                    xsValue str;
                    str.type = XS_VALUE_STRING;
                    str.data.s = const_cast<char *>(strs.at(i).toStdString().c_str());
                    xsArrayAdd(ar, &str);
                }
            }
        }
    }
    else if(subObj.value("type").toString() == "enum")
    {
        property->type = XS_VALUE_INT32;
        QJsonObject enumObj = subObj.value("enum").toObject();
        property->data.n = enumObj.value(data).toInt();
    }
    else if(subObj.value("type").toString() == "combine")
    {
        property->type = XS_VALUE_UINT32;
        property->data.n = 0;
        QJsonObject enumObj = subObj.value("enum").toObject();
        QJsonObject::const_iterator enumIt = enumObj.constBegin();
        while(enumIt != enumObj.constEnd())
        {
            if(data.contains(enumIt.key()))
            {
                property->data.n |= enumIt.value().toInt();
            }

            enumIt++;
        }
    }
    else if(subObj.value("type").toString() == "object")
    {
        property->type = XS_VALUE_OBJECT;
        xsObject *proObject = static_cast<xsObject *>(xsObject::createInstance());
        property->data.obj = proObject;

        QString realProName = name;
        QStringList list = data.split(";", QString::SkipEmptyParts);
        for(int i = 0; i < list.size(); i++)
        {
            if(list.at(i).contains("prototype"))
            {
                QStringList dataList = list.at(i).split(":",QString::SkipEmptyParts);
                realProName = dataList.at(1);
            }
        }

        xsValue prototype;
        prototype.type = XS_VALUE_STRING;
        prototype.data.s = const_cast<char *>(realProName.toStdString().c_str());
        proObject->setProperty("prototype", &prototype);

        createPropertyObject(realProName, data, proObject);
    }
    else if(subObj.value("type").toString() == "any")
    {
        property->type = XS_VALUE_UNDEFINE;
        property->data.ptr = const_cast<char *>(data.toStdString().c_str());
    }
}

void XSAppBuilder::cmopileJsonObject(const QJsonObject &obj)
{
    QJsonObject::const_iterator it = obj.constBegin();
    while(it != obj.constEnd())
    {
        if(root.contains(it.key()))
        {
            //暂时不需要json
        }
        it++;
    }
}

bool XSAppBuilder::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);
 /*   if(qName == "application")
    {
        xsObject *app = static_cast<xsObject *>(xsObject::createInstance());
        if(!atts.value("id").isEmpty())
        {
            qDebug()<<"id: "<<atts.value("id");
            xsValue id;
            id.type = XS_VALUE_STRING;
            id.data.s =  const_cast<char *>(atts.value("id").toStdString().c_str());
            app->setProperty("id", &id);
        }
        if(!atts.value("uuid").isEmpty())
        {
            qDebug()<<"uuid: "<<atts.value("uuid");
            xsValue uuid;
            uuid.type = XS_VALUE_STRING;
            uuid.data.s =  const_cast<char *>(atts.value("uuid").toStdString().c_str());
            app->setProperty("uuid", &uuid);
        }
        if(!atts.value("name").isEmpty())
        {
            qDebug()<<"name: "<<atts.value("name");
            xsValue name;
            name.type = XS_VALUE_TEXT;
#ifdef XS_UNICODE
            name.data.t = const_cast<xsTChar *>(atts.value("name").toStdWString().c_str());
#else
            name.data.t = const_cast<xsTChar *>(atts.value("name").toStdString().c_str());
#endif
            app->setProperty("name", &name);
        }
        if(!atts.value("orient").isEmpty())
        {
            qDebug()<<"orient: "<<atts.value("orient");
            xsValue orient;
            orient.type = XS_VALUE_UINT32;
            if(atts.value("orient") == "default")
            {
                orient.data.n = 0;
            }
            else if(atts.value("orient") == "landscape")
            {
                orient.data.n = 1;
            }
            else if(atts.value("orient") == "portrait")
            {
                orient.data.n = 2;
            }
            app->setProperty("orient", &orient);
        }

        bon = app;
    }
    */
    if(root.contains(qName))
    {
        xsObject *app = static_cast<xsObject *>(xsObject::createInstance());
        QJsonObject obj = root.value(qName).toObject();
        if(obj.contains("@prototype"))
        {
            xsValue prototype;
            prototype.type = XS_VALUE_STRING;
            prototype.data.s = const_cast<char *>(obj.value("@prototype").toString().toStdString().c_str());
            app->setProperty("prototype", &prototype);
        }
        compileObjectProperty(qName, atts, app);

        if(isFirstObject)
        {
            if(qName == "manifest")
            {
                xsValue application;
                application.type = XS_VALUE_OBJECT;
                application.data.obj = app;
                bon->setProperty("app", &application);
            }
            isFirstObject = false;
        }
        objectMap.insert(qName, app);

        return true;
    }

    return true;
}

bool XSAppBuilder::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    objectMap.insert(qName, NULL);
    return true;
}

bool XSAppBuilder::characters(const QString &ch)
{

}

bool XSAppBuilder::fatalError(const QXmlParseException &exception)
{
    qWarning() << "Fatal error on line" << exception.lineNumber()
                   << ", column" << exception.columnNumber() << ":"
                   << exception.message();

    return false;
}

bool XSAppBuilder::startDocument()
{
    isFirstObject = true;
    objectMap.clear();
    return true;
}

bool XSAppBuilder::endDocument()
{
    QStringList preObjectNames;
    QMap<QString, xsObject *>::const_iterator it = objectMap.constBegin();
    for(; it != objectMap.constEnd(); it++)
    {
        if(preObjectNames.isEmpty())
        {
            preObjectNames.append(it.key());
            continue;
        }

        if(it.value() == NULL)
        {
            preObjectNames.removeLast();
            continue;
        }

        objectMap.value(preObjectNames.last())->add(it.value());
        preObjectNames.append(it.key());
    }

    return true;
}

/*
void XSAppBuilder::compileXML(const QDomElement &root, xsObject *rootObject)
{
    QDomElement child = root.firstChildElement();
    while(!child.isNull())
    {
        xsObject *childObject = compileObjectAttributes(child);
        compileXML(child, childObject);
        xsValue  childValue;
        childValue.type = XS_VALUE_OBJECT;
        childValue.data.obj = childObject;
        rootObject->setProperty(child.tagName().toStdString().c_str(),&childValue);
        QDomElement child = child.nextSiblingElement();
    }
}

xsObject *XSAppBuilder::compileObjectAttributes(const QDomElement &object)
{
    xsObject *obj = static_cast<xsObject *>(xsObject::createInstance());
    QDomNamedNodeMap attributes = object.attributes();
    for(int i = 0; i < attributes.size(); i++)
    {
        QDomNode attribute = attributes.item(i);
        if(attribute.nodeName() == "id")
        {
            qDebug()<<"id";
            xsValue id;
            id.type = XS_VALUE_STRING;
            id.data.s = const_cast<char *>(attribute.nodeValue().toStdString().c_str());
            obj->setProperty(attribute.nodeName().toStdString().c_str(), &id);
        }
        else if(attribute.nodeName() == "name")
        {
            xsValue name;
            name.type = XS_VALUE_TEXT;
#ifdef XS_UNICODE
            name.data.t = const_cast<xsTChar *>(attribute.nodeValue().toStdWString().c_str());
#else
            name.data.t = const_cast<xsTChar *>(attribute.nodeValue().toStdString().c_str());
#endif
            obj->setProperty(attribute.nodeName().toStdString().c_str(), &name);
        }
        else if(attribute.nodeName() == "uuid")
        {
            xsValue uuid;
            uuid.type = XS_VALUE_STRING;
            uuid.data.s = const_cast<char *>(attribute.nodeValue().toStdString().c_str());
            obj->setProperty(attribute.nodeName().toStdString().c_str(), &uuid);
        }
        else if(attribute.nodeName() == "orient")
        {
            xsValue orient;
            orient.type = XS_VALUE_UINT32;
            if(attribute.nodeValue() == "default")
            {
                orient.data.n = 0;
            }
            else if(attribute.nodeValue() == "landscape")
            {
                orient.data.n = 1;
            }
            else if(attribute.nodeValue() == "portrait")
            {
                orient.data.n = 2;
            }
            obj->setProperty(attribute.nodeName().toStdString().c_str(), &orient);
        }
    }

    return obj;
}
*/
bool XSAppBuilder::writeObject()
{
//    QString xpkFileName = projectPath + "/" + projectName + ".xpk";
    QString xpkFileName = projectPath + "/"  + "app.xpk";
#ifdef XS_UNICODE
    const xsTChar *output = xpkFileName.toStdWString().c_str();
#else
    const xsTChar *output = xpkFileName.toStdString().c_str();
#endif
    xsValue xpk;
    xpk.type = XS_VALUE_OBJECT;
    xpk.data.obj = bon;
    if(XS_EC_OK == xsBonSaveToFile(output, &xpk, true))
    {
        return true;
    }

    return false;
}

