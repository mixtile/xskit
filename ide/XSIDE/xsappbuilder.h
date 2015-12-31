#ifndef XSAPPBUILDER_H
#define XSAPPBUILDER_H
#include <QString>
#include <QFileInfo>
#include <QXmlSimpleReader>
#include <QJsonDocument>
#include <QJsonObject>
#include "xs/object.h"

class XSAppBuilder : public QXmlDefaultHandler
{
public:
    XSAppBuilder(const QString &projectPath, const QString &projectName);
    ~XSAppBuilder();

    bool packProject();

protected:
    bool compileObject(const QFileInfo &source);
    bool compileScript(const QFileInfo &source);
    void compileObjectProperty(const QString &qName, const QXmlAttributes &atts, xsObject *object);
    void createPropertyObject(const QString &proName, const QString &proData, xsObject *proObject);
    void transformValue(const QJsonObject &subObj, xsValue *property, const QString &data, const QString &name);
    bool writeObject();
    void cmopileJsonObject(const QJsonObject &obj);
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);//还未增加对对象数组(例如<forms>)标签的判断和解析
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters(const QString &ch);
    bool fatalError(const QXmlParseException &exception);
    bool startDocument();
    bool endDocument();

private:
    QString projectPath;
    QString projectName;
    QJsonObject root;
    xsObject *bon;
    bool isFirstObject;

    QMap<QString, xsObject *> objectMap;
};

#endif // XSAPPBUILDER_H
