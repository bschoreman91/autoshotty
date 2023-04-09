#include "autoshotty.h"
#include "./ui_autoshotty.h"

#include <QRect>
#include <QScreen>
#include <QTimer>
#include <QDebug> // delete
#include <cmath>

AutoShotty::AutoShotty(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AutoShotty)
    , m_topLeftPoint({false, QPoint(0,0), false})
    , m_bottomRightPoint({false, QPoint(0,0), false})
    , m_nextPagePoint({false, QPoint(0,0), false})
    , m_pointCaps({ &m_topLeftPoint, &m_bottomRightPoint, &m_nextPagePoint})
    , m_originalPos(this->pos())
    , m_currentPage(0)
{
    QCoreApplication::instance()->installEventFilter(this);
    setMouseTracking(true);
    ui->setupUi(this);
    expanders(); // bind expand and compress buttons
    coordGetters(); // bind coordinate get buttons
    keyboardMode(); // bind keyboard mode things
    startButton();
}

AutoShotty::~AutoShotty()
{
    delete ui;
}

void AutoShotty::expanders() {
    auto resizeWin = [=](int width, int height) {
        QSize windowSize;
        windowSize.setHeight(height);
        windowSize.setWidth(width);
        this->resize(windowSize);
    };

    connect(this->ui->compressButton,
    &QPushButton::clicked, this,
    [=] { resizeWin(this->minimumWidth(), this->minimumHeight()); } );

    connect(this->ui->expandButton,
    &QPushButton::clicked, this,
    [=] { resizeWin(this->baseSize().width(), this->baseSize().height()); } );
    return;
}

void AutoShotty::coordGetters() {
    QScreen *screen = QGuiApplication::primaryScreen();
    auto geo = screen->availableGeometry();
    auto startCapture = [=]() {
        QSize windowSize;
        windowSize.setHeight(geo.height());
        windowSize.setWidth(geo.width());
        m_originalPos = this->pos();
        this->move(0,0);
        this->resize(windowSize);
        this->setWindowOpacity(0.01); // we set opacity very low so that the program becomes an "overlay"
        return true;
    };

    connect(this->ui->setTopLeftButton,
    &QPushButton::clicked, this,
    [=] { m_topLeftPoint.capturing = startCapture(); } );

    connect(this->ui->setBottomRightButton,
    &QPushButton::clicked, this,
    [=] { m_bottomRightPoint.capturing = startCapture(); } );

    connect(this->ui->setNextPageButton,
    &QPushButton::clicked, this,
    [=] { m_nextPagePoint.capturing = startCapture(); } );

    return;
}

void AutoShotty::mousePressEvent(QMouseEvent *event)
{
    auto posSetter = [&](auto &caps) {
        POINT p;
        GetCursorPos(&p);
        caps->point.setX(p.x);
        caps->point.setY(p.y);
        caps->isSet = true;
        this->ui->expandButton->click();
        this->move(m_originalPos);
        this->setWindowOpacity(1);
        caps->capturing = false;
    };
    \
    bool allSet = true;
    for(auto & c : m_pointCaps) {
        if(c->capturing) {
            posSetter(c);
        }
        allSet &= c->isSet;
    }
    this->ui->startButton->setEnabled(allSet);
    return;
}

void AutoShotty::keyboardMode() {
    connect(this->ui->keyboardModeCheckbox,
    &QCheckBox::stateChanged, this,
    [=] {
       if(this->ui->keyboardModeCheckbox->isChecked()) {
           this->ui->setNextPageButton->setText("set target");
           this->ui->keyboardComboBox->setEnabled(true);
       }
       else {
           this->ui->setNextPageButton->setText("set next page");
           this->ui->keyboardComboBox->setEnabled(false);
       }
    } );
}

void AutoShotty::shootScreen() {

    QRect capArea(m_topLeftPoint.point, m_bottomRightPoint.point);
    QScreen *screen = QGuiApplication::primaryScreen();

    QPixmap pixmap = screen->grabWindow(0, capArea.x(), capArea.y(), capArea.width(), capArea.height());
    QImage image = pixmap.toImage();

    if(this->ui->filetypeComboBox->currentText() == "jpgs") {
        image.save(QString(this->ui->filenameLineEdit->text())+"_"+QString::number(m_currentPage)+".jpg", "jpg", this->ui->qualitySlider->value());
    }


    if(this->ui->filetypeComboBox->currentText() == "pngs") {
        image.save(QString(this->ui->filenameLineEdit->text())+"_"+QString::number(m_currentPage)+".png", "png", this->ui->qualitySlider->value());
    }

    if(this->ui->filetypeComboBox->currentText() == "PDF") {

        static QPdfWriter writer(this->ui->filenameLineEdit->text()+".pdf");
        writer.newPage();
        writer.setPageSize(QPageSize::A4Small);
        double res = 300*(static_cast<double>(this->ui->qualitySlider->value())/100);
        writer.setResolution(static_cast<int>(res));
        writer.width();
        writer.height();
        auto image2 = image.scaled(writer.width(),writer.height(),Qt::KeepAspectRatio);
        static QPainter painter(&writer);
        QRect imageFrame = image2.rect();
        painter.drawImage(imageFrame, image2);

    }

}

void AutoShotty::startButton() {

    connect(this->ui->startButton,
    &QPushButton::clicked, this,
    [=] {
        m_currentPage = 0;
        this->ui->startButton->setEnabled(false);
        this->ui->compressButton->click();
        const int msDelay = (this->ui->delaySpinBox->value())*1000;
        QTimer *timer = new QTimer(this);
        timer->start(msDelay);

        QObject::connect(timer, &QTimer::timeout, [this, timer](){
            shootScreen();

            if(!this->ui->keyboardModeCheckbox->isChecked()) {
                sendLeftClick(m_nextPagePoint.point);

            }
            else {
                sendLeftClick(m_nextPagePoint.point);
                sendKBInput();
            }

            m_currentPage++;

            if(m_currentPage >= this->ui->pagesSpinBox->value()) {
                timer->stop();
                this->ui->startButton->setEnabled(true);
                this->ui->expandButton->click();
            }


        });
    } );
}

void AutoShotty::sendLeftClick(QPoint pnt) {
    POINT p;
    GetCursorPos(&p);

    INPUT input;
    input.type=INPUT_MOUSE;
    input.mi.dx=pnt.x()-p.x; // we must subtract current x
    input.mi.dy=pnt.y()-p.y; // we must subtract current y
    input.mi.dwFlags=(MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP);
    input.mi.mouseData=0;
    input.mi.dwExtraInfo=NULL;
    input.mi.time=0;
    SendInput(1,&input,sizeof(INPUT));
}

void AutoShotty::sendKBInput() {

    if(kbInputMap.find(this->ui->keyboardComboBox->currentIndex()) != kbInputMap.end()) {
        INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = 0;
        input.ki.time = 0;
        input.ki.dwExtraInfo = 0;
        input.ki.wVk = kbInputMap[this->ui->keyboardComboBox->currentIndex()];
        input.ki.dwFlags = 0; // 0 for key press
        SendInput(1, &input, sizeof(INPUT));
        input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
        SendInput(1, &input, sizeof(INPUT));
    }


}
