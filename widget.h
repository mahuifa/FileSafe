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

    void on_but_src_clicked();

    void on_but_put_clicked();

    void on_but_key_clicked();

    void on_but_start_clicked();

    void on_but_stop_clicked();

    void on_showLog(QString log);

    void on_complete(qint64 current, qint64 total);

private:
    Ui::Widget *ui;
    QPoint m_pointStart;
    FileEncryption* m_fileEncryption = nullptr;
    QThread* m_thread = nullptr;

};
#endif // WIDGET_H
