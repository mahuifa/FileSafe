#include "widget.h"
#include "ui_widget.h"

#include <QMouseEvent>
#include <qdebug.h>
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle(ui->label_title->text());
    this->setWindowIcon(QIcon(":/new/prefix1/image/Safe.ico"));

    ui->widget_title->installEventFilter(this);
    m_pointStart = QPoint(0, 0);
    ui->comboBox_fill->setCurrentIndex(2);

    m_fileEncryption = new FileEncryption();
    m_thread = new QThread();
    m_fileEncryption->moveToThread(m_thread);
    m_thread->start();
    connect(m_fileEncryption, &FileEncryption::showLog, this, &Widget::on_showLog);
    connect(m_fileEncryption, &FileEncryption::complete, this, &Widget::on_complete);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_but_min_clicked()
{
    this->showMinimized();
}

void Widget::on_but_close_clicked()
{
    this->close();
}

/**
 * @brief             鼠标移动窗口
 * @param watched
 * @param event
 * @return
 */
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widget_title)
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        switch (e->type())
        {
        case QEvent::MouseButtonPress:
        {
            if(e->button() == Qt::LeftButton)
            {
                m_pointStart = e->globalPos() - this->geometry().topLeft();
                break;
            }
        }
        case QEvent::MouseMove:
        {
            if(m_pointStart != QPoint(0, 0))
            {
                this->move(e->globalPos() - m_pointStart);
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            m_pointStart = QPoint(0, 0);
            break;
        }
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}


void Widget::on_but_src_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择原文件", "./", "文件(*);");
    if(!fileName.isEmpty())
    {
        ui->lineEdit_src->setText(fileName);
        ui->lineEdit_put->setText(fileName + ".en");
    }
}

void Widget::on_but_put_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "选择加密文件路径", ui->lineEdit_put->text(), "加密文件(*.en);");
    if(!fileName.isEmpty())
    {
        ui->lineEdit_put->setText(fileName);
    }
}

void Widget::on_but_key_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择密钥文件", "./key", "密钥文件(*.key);");
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly))
        {
            QByteArray arr = file.readAll();
            file.close();
            ui->lineEdit_key->setText(QString(arr));
        }
    }
}


void Widget::on_but_start_clicked()
{
    m_fileEncryption->setFile(ui->lineEdit_src->text(), ui->lineEdit_put->text());

    bool flag = ui->radioButton_2->isChecked();
    m_fileEncryption->setEncryption(!flag);
    if(flag)
    {
        QByteArray key;
        QString strKey = ui->lineEdit_key->text();
        if(!strKey.isEmpty())
        {
            key.append(strKey);
        }
        else
        {
            QMessageBox::about(this, "注意！", "未输入密钥。");
            return;
        }
        m_fileEncryption->setKey(key);
    }
    m_fileEncryption->setAESParameter((QAESEncryption::Aes)ui->comboBox_len->currentIndex(),
                                      (QAESEncryption::Mode)ui->comboBox_mode->currentIndex(),
                                      (QAESEncryption::Padding)ui->comboBox_fill->currentIndex());
    emit m_fileEncryption->start();
}


void Widget::on_but_stop_clicked()
{
    m_fileEncryption->stop();
}

void Widget::on_showLog(QString log)
{
    ui->textEdit_log->append(log);
}

void Widget::on_complete(qint64 current, qint64 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(current);
}


