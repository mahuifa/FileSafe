#include "fileencryption.h"

#include <QDir>
#include <QFileInfo>
#include <qcryptographichash.h>
#include <qdebug.h>
#include <qfile.h>
#include <qthread.h>

FileEncryption::FileEncryption(QObject *parent) : QObject(parent)
{
    connect(this, &FileEncryption::start, this, &FileEncryption::startEncryption);
    for(int i = 0; i < 16; i++)
    {
        m_iv.append((uchar)i);
    }
}

/**
 * @brief        设置原文件和加密文件路径
 * @param src    原文件
 * @param encod  加密文件
 */
void FileEncryption::setFile(const QString &src, const QString &encod)
{
    if(!src.isEmpty() && !encod.isEmpty())
    {
        this->m_strSrc = src;
        this->m_strEncod = encod;

        QFileInfo info(src);
        m_SrcName = info.completeBaseName();
        info.setFile(encod);
        m_encodName = info.completeBaseName();
    }
}

/**
 * @brief      设置使用的密钥
 * @param key
 */
void FileEncryption::setKey(const QByteArray &key)
{
    this->m_key = key;
}

/**
 * @brief        设置加密或者解密
 * @param flag   true：加密 false：解密
 */
void FileEncryption::setEncryption(bool flag)
{
    this->m_encryption = flag;
}

/**
 * @brief           设置AES加解密参数
 * @param aes
 * @param mode
 * @param padding
 */
void FileEncryption::setAESParameter(QAESEncryption::Aes aes, QAESEncryption::Mode mode, QAESEncryption::Padding padding)
{
    this->m_aes = aes;
    this->m_mode = mode;
    this->m_padding = padding;

    switch (aes)
    {
    case QAESEncryption::AES_128:
        m_len = 16 * 20;
        break;
    case QAESEncryption::AES_192:
        m_len = 24 * 20;
        break;
    case QAESEncryption::AES_256:
        m_len = 32 * 20;
        break;
    default:
        break;
    }
}

void FileEncryption::stop()
{
    this->m_run = false;
}

/**
 * @brief 加解密操作
 */
void FileEncryption::startEncryption()
{
    m_dataIn.clear();
    m_dataPut.clear();
    m_run = true;
    if(m_encryption)
    {
        encryption();
    }
    else
    {
        decrypt();
    }
}

/**
 * @brief 加密
 */
void FileEncryption::encryption()
{
    emit showLog("开始读取原文件！");
    if(ReadFile(m_strSrc))
    {
        emit showLog("原文件读取完成！");
        emit showLog("开始计算md5！");
        m_key = QCryptographicHash::hash(m_dataIn, QCryptographicHash::Md5).toHex();
        emit showLog("md5计算完成！");
        if(WriteFile(QString("./key/%1.key").arg(m_SrcName), m_key))
        {
            emit showLog("生成密钥文件！");
        }
        else
        {
            emit showLog("密钥文件生成失败！");
            return;
        }

        QAESEncryption encryption(m_aes, m_mode, m_padding);

        emit showLog("开始加密！");
        qint64 totalLen = m_dataIn.count();
        qint64 currentLen = 0;
        while (m_dataIn.count() && this->m_run)
        {
            QByteArray arr = m_dataIn.mid(0, m_len);
            currentLen += arr.count();
            if(m_mode == QAESEncryption::ECB)
            {
                m_dataPut.append(encryption.encode(arr, m_key));
            }
            else
            {
                 m_dataPut.append(encryption.encode(arr, m_key, m_iv));
            }
            m_dataIn.remove(0, m_len);
            emit complete(currentLen, totalLen);
        }

        emit showLog("加密完成，开始写入！");
        if(WriteFile(m_strEncod, m_dataPut))
        {
            emit showLog("加密成功！");
        }
    }
}

/**
 * @brief  解密
 */
void FileEncryption::decrypt()
{

    emit showLog("开始读取加密文件！");
    if(ReadFile(m_strEncod))
    {
        emit showLog("加密文件读取完成！");
        QAESEncryption encryption(m_aes, m_mode, m_padding);
        emit showLog("开始解密！");

        qint64 totalLen = m_dataIn.count();
        qint64 currentLen = 0;
        while (m_dataIn.count() && this->m_run)
        {
            QByteArray arr = m_dataIn.mid(0, m_len);
            currentLen += arr.count();
            if(m_mode == QAESEncryption::ECB)
            {
                m_dataPut.append(encryption.decode(arr, m_key));
            }
            else
            {
                 m_dataPut.append(encryption.decode(arr, m_key, m_iv));
            }
            m_dataIn.remove(0, m_len);
            emit complete(currentLen, totalLen);
        }
        QByteArray data = encryption.removePadding(m_dataPut);        // 移除填充数据

        emit showLog("解密完成，开始写入！");
        if(WriteFile(m_strSrc, data))
        {
            emit showLog("解密成功！");
        }

        QByteArray key = QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
        if(m_key == key)
        {
            emit showLog("文件未损毁！");
        }
        else
        {
            emit showLog("解密后文件存在不同，可能已损毁！");
        }
    }
}

/**
 * @brief           读取文件内容
 * @param fileName
 */
bool FileEncryption::ReadFile(const QString &fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        m_dataIn = file.readAll();
        file.close();
        return true;
    }
    else
    {
        emit showLog(QString("%1打开失败！").arg(fileName));
        return false;
    }
}

/**
 * @brief           写文件内容
 * @param fileName
 * @param data
 */
bool FileEncryption::WriteFile(const QString &fileName, const QByteArray &data)
{
    QFileInfo info(fileName);
    QString filePath = info.absolutePath();
    QDir dir;
    if(!dir.exists(filePath))
    {
        dir.mkpath(filePath);
    }

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();
        return true;
    }
    else
    {
        emit showLog(QString("%1打开失败！").arg(fileName));
        return false;
    }
}
