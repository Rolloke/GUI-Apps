#include "qt2dview.h"
#include "qt2dviewplugin.h"

#include <QtPlugin>

Qt2dViewPlugin::Qt2dViewPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void Qt2dViewPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool Qt2dViewPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *Qt2dViewPlugin::createWidget(QWidget *parent)
{
    return new Qt2dView(parent);
}

QString Qt2dViewPlugin::name() const
{
    return QLatin1String("Qt2dView");
}

QString Qt2dViewPlugin::group() const
{
    return QLatin1String("Representation");
}

QIcon Qt2dViewPlugin::icon() const
{
    return QIcon();
}

QString Qt2dViewPlugin::toolTip() const
{
    return QLatin1String("");
}

QString Qt2dViewPlugin::whatsThis() const
{
    return QLatin1String("Display for 2D Diagramms");
}

bool Qt2dViewPlugin::isContainer() const
{
    return false;
}

QString Qt2dViewPlugin::domXml() const
{
    return QLatin1String("<widget class=\"Qt2dView\" name=\"qt2dView\">\n</widget>\n");
}

QString Qt2dViewPlugin::includeFile() const
{
    return QLatin1String("qt2dview.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qt2dviewplugin, Qt2dViewPlugin)
#endif // QT_VERSION < 0x050000
