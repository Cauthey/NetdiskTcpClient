#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include<QPushButton>
#include<QHBoxLayout>
#include<QBoxLayout>
#include<QButtonGroup>
#include<QScrollArea>
#include<QCheckBox>
#include<QListWidget>


class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);

    static ShareFile &getInstance();

    void updateFriend(QListWidget *pFriendList);

signals:
public slots:
    void cancelSelect();
    void selectAll();

    void okShare();
    void cancelShare();

private:
    QPushButton *m_pSelsctAllPB;
    QPushButton *m_pCancelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;

    QWidget *m_pFriendW;
    QScrollArea *m_pSA;

    QVBoxLayout *m_pFriendWVBL;
    QButtonGroup *m_pButtonGroup;




};

#endif // SHAREFILE_H
