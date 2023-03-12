#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include<QListWidget>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include"protocol.h"
#include<QTimer>


class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearFileList();
    void clearEnterDir();
    QString getEnterDir();
    void setDownloadFile(bool status);
    bool getDownloadStatus();
    QString getStrSaveFilePath();
    QString getShareFileName();

    qint64 m_iTotal;
    qint64 m_iRecved;


signals:

public slots:
    void createDir();
    void flushFiles();
    void delDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void returnPre();
    void delRegFile();
    void uploadFile();

    void uploadFileData();
    void downloadFile();

    void shareFile();
    void moveFile();
    void selectDestDir();



private:
    QListWidget *m_pBookListW;
    QPushButton *m_PBReturnPB;
    QPushButton *m_PBCreateDirPB;
    QPushButton *m_PBDelDirPB;
    QPushButton *m_PBRenamePB;
    QPushButton *m_PBFlushFilePB;
    QPushButton *m_PBUploadFilePB;
    QPushButton *m_PBDownloadPB;
    QPushButton *m_PBDelFilePB;
    QPushButton *m_PBShareFilePB;
    QPushButton *m_PBMoveFilePB;
    QPushButton *m_PBSelectDirPB;

    QString m_strEnterDir;
    QString m_strUploadFilePath;
    QTimer *m_pTimer;

    QString m_strSaveFilePath;
    bool m_bDownload;

    QString m_strShareFileName;

    QString m_strMoveFile;
    QString m_strMovePath;
    QString m_strDestDir;
};

#endif // BOOK_H
