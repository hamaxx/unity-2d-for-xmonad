#ifndef UNITY2DDECLARATIVEVIEW_H
#define UNITY2DDECLARATIVEVIEW_H

#include <QDeclarativeView>

class Unity2DDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool useOpenGL READ useOpenGL WRITE setUseOpenGL NOTIFY useOpenGLChanged)

public:
    Unity2DDeclarativeView(QWidget *parent = 0);

    bool useOpenGL() const;
    void setUseOpenGL(bool);

Q_SIGNALS:
    void useOpenGLChanged(bool);

private:
    bool m_useOpenGL;
};

Q_DECLARE_METATYPE(Unity2DDeclarativeView*)

#endif // UNITY2DDECLARATIVEVIEW_H
