#include "databaseoperatorsingleton.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

DataBaseOperatorSingleton *DataBaseOperatorSingleton::_instance = nullptr;
std::mutex DataBaseOperatorSingleton::_mutex;

DataBaseOperatorSingleton::DataBaseOperatorSingleton()
{
    _db=QSqlDatabase::addDatabase("QMYSQL","mysql");
    _db.setHostName("localhost");
    _db.setPort(3306);
    _db.setUserName("root");
    _db.setPassword("guoyao");
    _db.setDatabaseName("p1");
	if (!_db.open())
	{
		qDebug() << QString(QStringLiteral("打开数据库失败"));
		qDebug() << _db.lastError();
	}
}


void DataBaseOperatorSingleton::addWrit(WirtStruct writ)
{
	qDebug() << QString(QStringLiteral("添加文书条目...begin"));
    QSqlQuery query(_db);
    query.prepare("INSERT INTO writ (Id, SendId, ReceiveId, ReceiveTime, Name)"
                  "VALUES(:Id, :SendId, :ReceiveId, :ReceiveTime, :Name)");
    query.bindValue(0,writ.Id);
    query.bindValue(1,writ.SendId);
    query.bindValue(2,writ.ReceiveId);
    query.bindValue(3,writ.ReceiveTime);
    query.bindValue(4,writ.Name);
	query.exec();
	//qDebug() << query.lastError();
	qDebug() << QString(QStringLiteral("添加文书条目...end"));

}

void DataBaseOperatorSingleton::deleteWrit(int Id)
{
	qDebug() << QString(QStringLiteral("删除文书条目...begin"));
	QSqlQuery query(_db);
	query.prepare("DELETE FROM writ WHERE Id = ?");
	query.addBindValue(Id);
	query.exec();
	qDebug() << QString(QStringLiteral("删除文书条目...end"));
}

QString DataBaseOperatorSingleton::findNameById(int writId)
{
	qDebug() << QString(QStringLiteral("查找id为%1的文书条目...begin")).arg(writId);

	QString writName = "";
	QSqlQuery query(_db);
	query.prepare("SELECT * FROM writ WHERE Id = ?");
	query.addBindValue(writId);
	query.exec();
	if (query.first())
	{
		writName = query.value(4).toString();
	}
	if (writName=="")
	{
		qDebug() << QString(QStringLiteral("数据库中没有id为%1的文书")).arg(writId);
	}
	qDebug() << QString(QStringLiteral("id为%1的文书名称：")).arg(writId);
	qDebug() << writName;
	qDebug() << QString(QStringLiteral("查找id为%1的文书条目...end")).arg(writId);
	return writName;
}

QList<WirtStruct> DataBaseOperatorSingleton::findWrit(int userId)
{
	qDebug() << QString(QStringLiteral("查找用户%1的文书条目...begin")).arg(userId);
	QList<WirtStruct> writList;
	QSqlQuery query(_db);
	query.prepare("SELECT * FROM writ");
	query.exec();
	while (query.next())
	{
		QString receiveIds = query.value(2).toString();
		//qDebug() << "receiveIds:  " << receiveIds;
		bool isUserWrit = false;
		QStringList receiveIsList = receiveIds.split(",");
		for (QString id : receiveIsList)
		{
			if (QString::number(userId)==id)
			{
				isUserWrit = true;
			}
		}
		//该条记录是用户要接收的文书
		if (isUserWrit)
		{
			WirtStruct writ;
			writ.Id = query.value(0).toInt();
			writ.SendId = query.value(1).toInt();
			writ.ReceiveId = query.value(2).toString();
			writ.ReceiveTime = query.value(3).toString();
			writ.Name = query.value(4).toString();
			writList.push_back(writ);
		}
	}
	qDebug()<< QString(QStringLiteral("查找到用户%1的文书条目有：")).arg(userId);
	for (int j = 0; j < writList.size();j++)
	{
		qDebug() << writList.at(j).Name;
	}
	qDebug() << QString(QStringLiteral("查找用户%1的文书条目...end")).arg(userId);
	return writList;
}

