#include "unity2ddeclarativeview.h"
#include <QGLWidget>

Unity2DDeclarativeView::Unity2DDeclarativeView(QWidget *parent) :
    QDeclarativeView(parent), m_useOpenGL(false)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setTransparentBackground(false);
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

bool Unity2DDeclarativeView::transparentBackground() const
{
    return m_transparentBackground;
}

void Unity2DDeclarativeView::setTransparentBackground(bool transparentBackground)
{
    setAttribute(Qt::WA_TranslucentBackground, transparentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground, transparentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, !transparentBackground);

    m_transparentBackground = transparentBackground;
    Q_EMIT transparentBackgroundChanged(transparentBackground);
}

#include <unity2ddeclarativeview.moc>
