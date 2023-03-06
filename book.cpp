#include "book.h"
#include"tcpclient.h"
#include<QInputDialog>
#include<QMessageBox>
#include<QModelIndex>
#include<QFileDialog>


Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_strEnterDir.clear();

    m_pTimer = new QTimer;

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
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(enterDir(QModelIndex)));
    connect(m_PBReturnPB,SIGNAL(clicked(bool)),
            this,SLOT(returnPre()));
    connect(m_PBDelFilePB,SIGNAL(clicked(bool)),
            this,SLOT(delRegFile()));
    connect(m_PBUploadFilePB,SIGNAL(clicked(bool)),
            this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(clicked(bool)),
            this,SLOT(uploadFileData()));
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

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::getEnterDir()
{
    return m_strEnterDir;
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

void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();
    m_strEnterDir= strDirName;
    qDebug() << strDirName;
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Book::returnPre()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strRootPath = "./"+TcpClient::getInstance().loginName();
    if(strCurPath==strRootPath)
    {
        QMessageBox::warning(this,"返回","返回失败:已在用户根部目录下");
    }
    else
    {
        int index = strCurPath.lastIndexOf("/");
        strCurPath.remove(index,strCurPath.size()-index);   // 获取上级目录
        qDebug() << "return --->"  << strCurPath;
        TcpClient::getInstance().setCurPath(strCurPath);

        clearEnterDir();
        flushFiles();
    }


}

void Book::delRegFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件!");
    }else{
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Book::uploadFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    m_strUploadFilePath = QFileDialog::getOpenFileName();
    qDebug() << "上传路径--->" << m_strUploadFilePath;
    if(!m_strUploadFilePath.isEmpty()){
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        qDebug() << strFileName << "=====" ;
        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size(); // 获得文件大小

        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

        m_pTimer->start(1000);  // 1000ms后上传文件
    }else{
        QMessageBox::warning(this,"上传文件","上传文件名不能为空!");
    }
}

void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开文件失败!");
        return;
    }
    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = file.read(pBuffer,4096);
        if(ret>0&&ret<=4096){
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }else if(0==ret){
            break;
        }else{
            QMessageBox::warning(this,"上传文件","上传文件失败:读取文件失败");
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;
}


