#include "book.h"

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_pBookListW = new QListWidget;
    m_PBReturnPB =new QPushButton("返回");
    m_PBCreateDirPB = new QPushButton("创建文件夹");
    m_PBDelDirPB =new QPushButton("删除文件夹");
    m_PBRenamePB = new QPushButton("重命名文件");
    m_PBFlushFilePB = new QPushButton("刷新文件");
    m_PBUploadFilePB = new QPushButton("上传文件");
    m_PBDownloadPB = new QPushButton("下载文件");
    m_PBDelFilePB = new QPushButton("删除文件");
    m_PBShareFilePB = new QPushButton("分享文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_PBReturnPB);
    pDirVBL->addWidget(m_PBCreateDirPB);
    pDirVBL->addWidget(m_PBDelDirPB);
    pDirVBL->addWidget(m_PBRenamePB);
    pDirVBL->addWidget(m_PBFlushFilePB);

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_PBUploadFilePB);
    pFileVBL->addWidget(m_PBDownloadPB);
    pFileVBL->addWidget(m_PBDelFilePB);
    pFileVBL->addWidget(m_PBShareFilePB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);
}
