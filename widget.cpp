#include "widget.h"
#include "ui_widget.h"

#include <QMouseEvent>
#include <qdebug.h>
#include <QFileDialog>
#include <QMessageBox>
#include <qcryptographichash.h>

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
    m_fileEncryption = new FileEncryption();
    m_thread = new QThread();
    m_fileEncryption->moveToThread(m_thread);
    m_thread->start();

    ui->comboBox_Padding->setCurrentIndex(2);
    connect(m_fileEncryption, &FileEncryption::showLog, this, &Widget::on_showLog);
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


void Widget::on_but_in_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择输入文件", "./", "输入文件(*)");
    if(!fileName.isEmpty())
    {
        ui->lineEdit_in->setText(fileName);
        QFileInfo info(fileName);
        QString name = info.fileName();
        QString path = info.canonicalPath();
        ui->lineEdit_out->setText(path + "/out_" + name);
    }
}

void Widget::on_but_out_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "选择输出文件路径", ui->lineEdit_out->text(), "输出文件(*)");
    if(!fileName.isEmpty())
    {
        ui->lineEdit_out->setText(fileName);
    }
}

void Widget::on_but_start_clicked()
{
    m_fileEncryption->setFile(ui->lineEdit_in->text(), ui->lineEdit_out->text());
    m_fileEncryption->setEncryption(!ui->radio_de->isChecked());
    QByteArray arrkey(ui->lineEdit_key->text().trimmed().toUtf8());
    if(arrkey.isEmpty())
    {
        QMessageBox::about(this, "注意！", "未输入密钥。");
        return;
    }
    arrkey = QCryptographicHash::hash(arrkey, QCryptographicHash::Md5).toHex();         // 使用md5适配所有长度的密码，否则如果密码过短会导致解密失败
    m_fileEncryption->setKey(arrkey);
    m_fileEncryption->setAESParameter((QAESEncryption::Aes)ui->comboBox_len->currentIndex(),
                                      (QAESEncryption::Mode)ui->comboBox_mode->currentIndex(),
                                      (QAESEncryption::Padding)ui->comboBox_Padding->currentIndex());
    emit m_fileEncryption->start();
}

void Widget::on_showLog(QString log)
{
    ui->textEdit_log->append(log);
}

