#ifndef WIDGET_H
#define WIDGET_H

#include <QThread>
#include <QWidget>
#include "fileencryption.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_but_min_clicked();
    void on_but_close_clicked();

    void on_but_in_clicked();

    void on_but_out_clicked();

    void on_but_start_clicked();

    void on_showLog(QString log);

private:
    Ui::Widget *ui;
    QPoint m_pointStart;                          // 移动窗口时鼠标初始坐标
    FileEncryption* m_fileEncryption = nullptr;   // 文件加解密对象
    QThread* m_thread = nullptr;                  // 文件加解密线程
};
#endif // WIDGET_H
