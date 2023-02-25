#include "book.h"
#include"tcpclient.h"
#include<QInputDialog>
#include<QMessageBox>

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

    connect(m_PBCreateDirPB,SIGNAL(clicked(bool)),
            this,SLOT(createDir()));
    connect(m_PBFlushFilePB,SIGNAL(clicked(bool)),
            this,SLOT(flushFiles()));
    connect(m_PBDelDirPB,SIGNAL(clicked(bool)),
            this,SLOT(delDir()));
    connect(m_PBRenamePB,SIGNAL(clicked(bool)),
            this,SLOT(renameFile()));
}

void Book::updateFileList(const PDU *pdu)
{
    if(NULL==pdu){
        return ;
    }
    m_pBookListW->clear();
//    QListWidgetItem *pItemTmp = NULL;
//    int row = m_pBookListW->count();
//    while(m_pBookListW->count()>0){
//        pItemTmp = m_pBookListW->item(row-1);
//        m_pBookListW->removeItemWidget(pItemTmp);
//        row--;
//    }
//    delete pItemTmp;
    FileInfo *pFileInfo = NULL;
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    for(int i =0 ;i<iCount;i++){
        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        qDebug() << pFileInfo->caName
                 << pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(0==pFileInfo->iFileType){
            pItem->setIcon(QIcon(QPixmap(":/dirIcon.png")));
        }else if(1==pFileInfo->iFileType){
            pItem->setIcon(QIcon(QPixmap(":/fileIcon.png")));
        }
        pItem->setText(pFileInfo->caName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","文件夹名称");
    if(!strNewDir.isEmpty()){
        if(strNewDir.size()>32){
            QMessageBox::warning(this,"新建文件夹","文件夹名称过长");
        }else{
            QString strName = TcpClient::getInstance().loginName();
            QString strCurPath = TcpClient::getInstance().curPath();
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }
    }else{
        QMessageBox::warning(this,"新建文件夹","名称不能为空");
    }
}

void Book::flushFiles()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size());
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    return ;
}

void Book::delDir()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"删除文件夹","请选择要删除的文件夹");
    }
    else{
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy((char*)pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }

}

void Book::renameFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"重命名文件","未选中文件");
    }
    else{
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"重命名文件","请输入新的文件夹名称");
        if(!strNewName.isEmpty()){
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }else{
            QMessageBox::warning(this,"重命名文件","新的名称不能为空");
        }
    }


}

