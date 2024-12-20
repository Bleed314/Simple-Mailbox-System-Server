#ifndef DATABASEOPERATORSINGLETON_H
#define DATABASEOPERATORSINGLETON_H

#include <mutex>
#include<QtSql/QSqlDatabase>
#include <QList>
#include <QDebug>
//#include <QSqlDatabase>

struct WirtStruct
{
  int Id;
  int SendId;
  QString ReceiveId;
  QString ReceiveTime;
  QString Name;//文书文件名称
   void operator <<(WirtStruct &writ)
  {
	  qDebug() << writ.Id << writ.SendId << writ.ReceiveId << ReceiveTime << Name;
  }
//  WritStruct()
//  {

//  }
};

class DataBaseOperatorSingleton
{
private:
    DataBaseOperatorSingleton();
public:
    static DataBaseOperatorSingleton* getInstance()
    {
        if(!_instance)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if(!_instance)
            {
                _instance= new DataBaseOperatorSingleton();
            }
        }
        return _instance;
    }

public:
    //文书的增删
    void addWrit(WirtStruct writ);
	void deleteWrit(int Id);
	//通过文书id查找文书名
	QString findNameById(int writId);
	//查找发送给用户的文书，数据库中记录的接收Id包括userId则是发送给该用户的文书
	QList<WirtStruct> findWrit(int userId);



private:
    static std::mutex _mutex;
    static DataBaseOperatorSingleton* _instance;
    QSqlDatabase _db;

};

#endif // DATABASEOPERATORSINGLETON_H
