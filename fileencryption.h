#ifndef FILEENCRYPTION_H
#define FILEENCRYPTION_H

#include <QObject>
#include "qaesencryption.h"

class FileEncryption : public QObject
{
    Q_OBJECT
public:
    explicit FileEncryption(QObject *parent = nullptr);
    void setFile(const QString& src, const QString& encod);
    void setKey(const QByteArray& key);
    void setEncryption(bool flag);
    void setAESParameter(QAESEncryption::Aes aes, QAESEncryption::Mode mode, QAESEncryption::Padding padding);
    void stop();                        // 停止

signals:
    void start();                       // 开始
    void showLog(QString log);          // 日志
    /**
     * @brief            当前加密、解密进度
     * @param current    当前完成值
     * @param total      总值
     */
    void complete(qint64 current, qint64 total);

private:
    void startEncryption();
    void encryption();
    void decrypt();
    bool ReadFile(const QString& fileName);
    bool WriteFile(const QString& fileName, const QByteArray& data);

private:
    QString m_strSrc;         // 源文件路径
    QString m_strEncod;       // 加密文件路径
    QString m_SrcName;
    QString m_encodName;
    QByteArray m_key;         // 密钥
    QByteArray m_iv;          // 偏移
    bool m_encryption = true;
    bool m_run = true;
    int m_len = 320;           // 读取长度
    QAESEncryption::Aes m_aes;
    QAESEncryption::Mode m_mode;
    QAESEncryption::Padding m_padding;

    QByteArray m_dataIn;     // 输入内容
    QByteArray m_dataPut;    // 输出内容
};

#endif // FILEENCRYPTION_H
