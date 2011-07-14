#ifndef UNITY2DDECLARATIVEVIEW_H
#define UNITY2DDECLARATIVEVIEW_H

#include <QDeclarativeView>

class Unity2DDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool useOpenGL READ useOpenGL WRITE setUseOpenGL NOTIFY useOpenGLChanged)
    Q_PROPERTY(bool transparentBackground READ transparentBackground WRITE setTransparentBackground NOTIFY transparentBackgroundChanged)

public:
    Unity2DDeclarativeView(QWidget *parent = 0);

    // getters
    bool useOpenGL() const;
    bool transparentBackground() const;

    // setters
    void setUseOpenGL(bool);
    void setTransparentBackground(bool);

Q_SIGNALS:
    void useOpenGLChanged(bool);
    void transparentBackgroundChanged(bool);

protected:
    void setupViewport();

private:
    bool m_useOpenGL;
    bool m_transparentBackground;
};

Q_DECLARE_METATYPE(Unity2DDeclarativeView*)

#endif // UNITY2DDECLARATIVEVIEW_H
