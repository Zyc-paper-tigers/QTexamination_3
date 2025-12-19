#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");//添加QSQL LITE数据库驱动
    QString aFile = "D:/File/daily/QT/examination_3/lab3.db";
    database.setDatabaseName(aFile);//设置数据库名称

    if(!database.open()){//打开数据库
        qDebug() << "failed to open database";
    }else{
        qDebug() << "open database is ok"<< database.connectionName();
    }
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);//数据保存方式
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder);//排序
    if(!(patientTabModel->select()))//查询数据
        return false;
    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(), QModelIndex());//在末尾添加一个记录
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount()-1, 1);//创建最后一行的ModelIndex

    int CurRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(CurRecNo);//获取当前记录
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(CurRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    // 检查当前索引是否有效（是否有选中行）
    QModelIndex curIndex = thePatientSelection->currentIndex();
    if (!curIndex.isValid()) {
        return false; // 无选中行，直接返回
    }

    // 执行删除操作
    patientTabModel->removeRow(curIndex.row());
    bool result = patientTabModel->submitAll(); // 获取提交结果
    patientTabModel->select(); // 刷新数据
    return result; // 返回操作结果
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
//    return "LoginOK"
    QSqlQuery query;
    query.prepare("select username,password from user where username = :USER");
    query.bindValue(":USER",userName);
    query.exec();
    qDebug()<<query.lastQuery()<<query.first();

    if(query.first() && query.value("username").isValid()){
        QString passwd = query.value("password").toString();
        if(passwd == password){
            qDebug()<<"login ok";
            return "loginOk";
        }else{
            qDebug()<<"wrong password";
            return "wrongPassword";
        }
    }else{
        qDebug()<< "no such user";
        return "wrongUserName";
    }
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    ininDatabase();
}
