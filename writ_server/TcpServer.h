#pragma once

#include <QTcpServer>
#include <QThread>
#include <QTimer>
#include "TcpSocket.h"

class TcpServer : public QTcpServer
{
	Q_OBJECT
public:
	TcpServer();
	~TcpServer();

protected:
	void incomingConnection(qintptr  socketDescriptor);

public:
	void initial();

private slots:
	void newWritToClintUsersSlot(QList<int> ClintUsersId);
	void newSpecialToClintUsersSlot(QList<int> ClintUsersId);
	void sendNewWritSlot(int sendID);
	void sendNewSpecialSlot(int sendID);
	void updateWritContentSlot(int index,const QString& wordId ,const QString& user, const QString& tag, const QString& content);
	void checkIsOnlineSlot(int index, int userID);
	void groupTrainStartSignalSlot();
	void updateArmySlot(QString sceName);
	void resetOlnineEditSlot();
	bool killProcessByName(const QString &peocessNaME);
	void restartSimProcess();
	bool isProcessRunning(QString exeName);

	void slotRestartSimExe();

signals:
	void signalRtiDisConnect();
	void signalRtiReConnect();
	

private:
	QList<TcpSocket*> _writSocketList;//每个客户端对应一个线程
	bool _isCloseOnlineEdit;
	QTimer* _timer;
};

