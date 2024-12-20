#pragma once

#include <QTcpSocket>
#include <QTimer>

class TcpSocket: public QTcpSocket
{
	Q_OBJECT
public:
	TcpSocket(qintptr socketDescriptor, QTcpSocket *parent = NULL);
	~TcpSocket();

signals:
	void over();//客户端断开连接时，线程退出，及时销毁线程
	void newWritToClintUsers(QList<int>); //通知其他客户端线程有新文书
	void newSpecialToClintUsers(QList<int>); //通知其他客户端线程有新特情
	void sendNewWrit(int);
	void sendNewSpecial(int);
	void updateWritContentSingal(int index,const QString& id,const QString& user, const QString& tag, const QString& content);//共同编辑中文书word内容更新
	void checkIsOnline(int index, int userID);//通知管理线程的server查询该用户是否已经建立连接
	void groupTrainStartSignal();//开户端开启组训任务
	void resetOlnineEditSignal();//重置是否开启协同编辑
	void signalRestartSimExe();
	void signalUpdateArmy(QString);
	void signalUpdateArmyBySelf(QString);


private slots:
	void slotReadPendingDatagrams();
	void SocketErr(QAbstractSocket::SocketError socketError);

	void slotUpdateArmyMark(QString);


public:
	int getClientUserId()
	{
		return _clientUserId;
	}
	int getIndex()
	{
		return _index;
	}
	void notifyClientNewWrit();//该线程的客户端用户有新文书时通知客户端
	void notifyClientNewSpecial();
	void notifyClientSendedWrit();//该线程的客户端用户发送了文书
	void notifyClientSendedSpecial();
	void updateWordContent(const QString& id,const QString& user, const QString& tag, const QString& content);//共同编辑:更新该线程客户端word
	void returnIsOnline(int isOnline);//0表示已在线，1表示不在线
	void notifyStartGroupMission();//通知服务端有新组训任务开启
	void notifyUpdateArmy(QString sceName);//通知态势

public slots:
	void notifyRtiDisConnect();
	void notifyRtiReConnect();

public:
	//心跳函数：防止客户端异常情况下退出（路由器故障，断电断网等），服务端无法检测，所以当连续发送心跳失败三次时，默认客户端已退出，服务端主动放弃该客户端
	void startHearBeat();
	void stopHearBeat();

private:
	int _clientUserId=-1;//每个客户端对应用户的id
	QMap<QString, QString> _wordMap;//文书word中tag以及其对应的content
	qintptr _socketDescriptor;
	int _index;
	int _heartBeatFailCount;
	QTimer* _heartBeatTimer=nullptr;
};

