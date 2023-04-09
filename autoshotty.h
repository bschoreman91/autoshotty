#ifndef AUTOSHOTTY_H
#define AUTOSHOTTY_H

#include <QMainWindow>
#include <QPoint>
#include <QPainter>
#include <QPDFWriter>
#include <Windows.h>
#include <map>
#include <utility>

QT_BEGIN_NAMESPACE
namespace Ui { class AutoShotty; }
QT_END_NAMESPACE

class AutoShotty : public QMainWindow
{
    Q_OBJECT

public:
    AutoShotty(QWidget *parent = nullptr);
    ~AutoShotty();

protected:
    void mousePressEvent(QMouseEvent * event);

private:
    Ui::AutoShotty *ui;
    void expanders();
    void coordGetters();
    void keyboardMode();
    void startButton();
    void sendLeftClick(QPoint pnt);
    void sendKBInput();

    std::map<int, WORD> kbInputMap {
        { 0, VK_RIGHT }, // right arrow
        { 1, VK_LEFT }, // left arrow
        { 2, VK_RETURN }, // enter
        { 3, VK_NEXT }, // page down
        { 4, VK_PRIOR } // page up
    };

    void shootScreen();

    struct PointCap {
        bool capturing;
        QPoint point;
        bool isSet;
    };

    PointCap m_topLeftPoint;
    PointCap m_bottomRightPoint;
    PointCap m_nextPagePoint;
    std::array<PointCap*, 3> m_pointCaps;

    QPoint m_originalPos;

    int m_currentPage;

    QPdfWriter *m_writer;
    QPainter *m_painter;

};
#endif // AUTOSHOTTY_H
