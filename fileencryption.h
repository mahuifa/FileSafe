#ifndef FILEENCRYPTION_H
#define FILEENCRYPTION_H

#include <QObject>
#include "qaesencryption.h"

class FileEncryption : public QObject
{
    Q_OBJECT
public:
    explicit FileEncryption(QObject *parent = nullptr);
    void setFile(const QString& strIn, const QString& strOut);
    void setKey(const QByteArray& key);
    void setEncryption(bool flag);
    void setAESParameter(QAESEncryption::Aes aes, QAESEncryption::Mode mode, QAESEncryption::Padding padding);
    void stop();                        // 停止

signals:
    void start();                       // 开始
    void showLog(QString log);          // 日志

private:
    void startEncryption();
    void encryption();
    void decrypt();
    bool readFile(const QString& fileName);
    void dataOperation();
    void check();
    bool writeFile(const QString& fileName);
    void clear();

private:
    QString m_strIn;          // 输入文件路径
    QString m_strOut;         // 输出文件路径
    QString m_fileSuffix;     // 文件后缀
    QByteArray m_key;         // 密钥
    QByteArray m_iv;          // 偏移
    bool m_encryption = true;
    QAESEncryption::Aes m_aes;
    QAESEncryption::Mode m_mode;
    QAESEncryption::Padding m_padding;

    QByteArray m_dataIn;     // 输入内容
    QByteArray m_dataOut;    // 输出内容
    QByteArray m_md5;        // MD5值

    QByteArray m_head;       // bmp图片的文件信息
};

#endif // FILEENCRYPTION_H
