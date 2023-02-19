#include "online.h"
#include "ui_online.h"

online::online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::online)
{
    ui->setupUi(this);
}

online::~online()
{
    delete ui;
}

void online::showUser(PDU *pdu)
{
    if(NULL==pdu){
        return ;
    }
    int uiSize = pdu->uiMsgLen/32;
    char caTemp[32];
    ui->Online_listWidget->clear();
    for(int i=0;i< uiSize;i++){
        memcpy(caTemp,(char*)pdu->caMsg+i*32,32);
        ui->Online_listWidget->addItem(caTemp);
    }
}
