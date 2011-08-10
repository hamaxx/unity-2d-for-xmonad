#ifndef UNITY2DDECLARATIVEVIEW_H
#define UNITY2DDECLARATIVEVIEW_H

#include <QDeclarativeView>

class Unity2DDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool useOpenGL READ useOpenGL WRITE setUseOpenGL NOTIFY useOpenGLChanged)
    Q_PROPERTY(bool transparentBackground READ transparentBackground WRITE setTransparentBackground NOTIFY transparentBackgroundChanged)
    Q_PROPERTY(QPoint globalPosition READ globalPosition NOTIFY globalPositionChanged)

public:
    Unity2DDeclarativeView(QWidget *parent = 0);

    // getters
    bool useOpenGL() const;
    bool transparentBackground() const;
    QPoint globalPosition() const;

    // setters
    void setUseOpenGL(bool);
    void setTransparentBackground(bool);

Q_SIGNALS:
    void useOpenGLChanged(bool);
    void transparentBackgroundChanged(bool);
    void globalPositionChanged(QPoint);

protected:
    void setupViewport();
    virtual void moveEvent(QMoveEvent* event);

protected Q_SLOTS:
    void forceActivateWindow();
    void forceDeactivateWindow();

private:
    void saveActiveWindow();
    void forceActivateThisWindow(WId);

    bool m_useOpenGL;
    bool m_transparentBackground;
    WId m_last_focused_window;
};

Q_DECLARE_METATYPE(Unity2DDeclarativeView*)

#endif // UNITY2DDECLARATIVEVIEW_H
