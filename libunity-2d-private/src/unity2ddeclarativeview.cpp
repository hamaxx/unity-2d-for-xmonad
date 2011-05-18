#include "unity2ddeclarativeview.h"
#include <QGLWidget>

Unity2DDeclarativeView::Unity2DDeclarativeView(QWidget *parent) :
    QDeclarativeView(parent), m_useOpenGL(false)
{
}

bool Unity2DDeclarativeView::useOpenGL() const
{
    return m_useOpenGL;
}

void Unity2DDeclarativeView::setUseOpenGL(bool useOpenGL)
{
    if (useOpenGL == m_useOpenGL) {
        return;
    }

    if (useOpenGL) {
        QGLFormat format = QGLFormat::defaultFormat();
        format.setSampleBuffers(false);

        QGLWidget *glWidget = new QGLWidget(format);
        /* TODO: possibly faster, to be tested */
        //glWidget->setAutoFillBackground(false);

        setViewport(glWidget);
    } else {
        setViewport(0);
    }

    m_useOpenGL = useOpenGL;
    Q_EMIT useOpenGLChanged(useOpenGL);
}

#include <unity2ddeclarativeview.moc>
