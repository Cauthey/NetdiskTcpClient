#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include<QListWidget>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include"protocol.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);

signals:

public slots:
    void createDir();
    void flushFiles();
    void delDir();
    void renameFile();

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
};

#endif // BOOK_H
