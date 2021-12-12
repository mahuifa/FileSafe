#include "fileencryption.h"

#include <QDir>
#include <QFileInfo>
#include <QImage>
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
 * @brief         设置输入输出文件路径
 * @param strIn   输入文件路径
 * @param strOut  输出文件路径
 */
void FileEncryption::setFile(const QString& strIn, const QString& strOut)
{
    if(!strIn.isEmpty() && !strOut.isEmpty())
    {
        this->m_strIn = strIn;
        this->m_strOut = strOut;

        QFileInfo info(strIn);
        m_fileSuffix = info.suffix();
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
}

/**
 * @brief 加解密操作
 */
void FileEncryption::startEncryption()
{
    clear();
    emit showLog("开始输入原文件！");
    if(readFile(m_strIn))
    {
        emit showLog("文件读取完成！");
        if(m_encryption)
        {
            encryption();
        }
        else
        {
            decrypt();
        }
        if(writeFile(m_strOut))
        {
            emit showLog("数据写入成功！");
        }
        clear();
    }
    else
    {
        emit showLog("输入文件读取失败！");
    }
}

/**
 * @brief 加密
 */
void FileEncryption::encryption()
{
    QAESEncryption encryption(m_aes, m_mode, m_padding);
    emit showLog("开始加密！");

    if(m_mode == QAESEncryption::ECB)
    {
        m_dataOut.append(encryption.encode(m_dataIn, m_key));
    }
    else
    {
         m_dataOut.append(encryption.encode(m_dataIn, m_key, m_iv));
    }
    m_dataOut.insert(0, m_head);

    m_dataOut.append(m_md5);
    emit showLog("加密完成，开始写入！");

}

/**
 * @brief  解密
 */
void FileEncryption::decrypt()
{
    QAESEncryption encryption(m_aes, m_mode, m_padding);
    emit showLog("开始解密！");

    if(m_mode == QAESEncryption::ECB)
    {
        m_dataOut.append(encryption.decode(m_dataIn, m_key));
    }
    else
    {
         m_dataOut.append(encryption.decode(m_dataIn, m_key, m_iv));
    }
    m_dataOut = encryption.removePadding(m_dataOut);        // 移除填充数据
    m_dataOut.insert(0, m_head);

    emit showLog("解密完成，开始写入！");
    check();
}

/**
 * @brief           读取文件内容
 * @param fileName
 */
bool FileEncryption::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        m_dataIn = file.readAll();
        file.close();
        dataOperation();
        return true;
    }
    else
    {
        emit showLog(QString("%1打开失败！").arg(fileName));
        return false;
    }
}

/**
 * @brief 操作数据内容
 */
void FileEncryption::dataOperation()
{
    // 获取md5值
    if(m_encryption)
    {
        m_md5 = QCryptographicHash::hash(m_dataIn, QCryptographicHash::Md5).toHex();
    }
    else                  // 解密时读取md5值
    {
        m_md5 = m_dataIn.mid(m_dataIn.count() - 32, 32);
        m_dataIn.remove(m_dataIn.count() - 32, 32);
    }

    // 读取bmo文件头信息
    if(m_fileSuffix.compare("bmp", Qt::CaseInsensitive) == 0)
    {
        m_head = m_dataIn.mid(0, 54);        // bmp文件头：共14字节；位图信息头：共40字节；
        m_dataIn.remove(0, 54);
    }
}

/**
 * @brief  使用md5校验文件是否损毁
 */
void FileEncryption::check()
{
    QByteArray arr = QCryptographicHash::hash(m_dataOut, QCryptographicHash::Md5).toHex();
    if(arr == m_md5)
    {
        emit showLog("文件未损毁！");
    }
    else
    {
        emit showLog("文件存在不同，可能已损毁！");
    }
}


/**
 * @brief           写文件内容
 * @param fileName
 * @param data
 */
bool FileEncryption::writeFile(const QString &fileName)
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
        file.write(m_dataOut);
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
 * @brief 清空数据
 */
void FileEncryption::clear()
{
    m_dataIn.clear();
    m_dataOut.clear();
    m_head.clear();
    m_md5.clear();
}
