#include "TcpServer.h"

#include <windows.h>

#include <QHostAddress>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QDateTime>
#include <QThreadPool>
#include <QProcess>
#include <QSettings>
#include <QThread>

#include "Message.h"


TcpServer::TcpServer()
{
	_isCloseOnlineEdit = false;

	_timer = new QTimer(this);
	connect(_timer, &QTimer::timeout, this, [&]()
	{
		if (!isProcessRunning("SimEnginePlugin.exe"))
		{
			restartSimProcess();
		}
	});
	_timer->start(5000);
}


TcpServer::~TcpServer()
{
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
	//创建子线程
	TcpSocket* socket = new TcpSocket(socketDescriptor);

	connect(socket, SIGNAL(readyRead()), socket, SLOT(slotReadPendingDatagrams())); // 会移进线程里
	

	QThread *thread = new QThread(); // 以socket为父类，当socket释放删除后也会删除线程，或者将线程的quit信号关联槽deleteLater()也可以达到效果
	connect(socket, SIGNAL(disconnected()), thread, SLOT(quit()));
	connect(socket, &TcpSocket::disconnected, [=]
	{
	
		for (int i = 0; i < _writSocketList.size(); i++)
		{
			if (socket->getIndex() == _writSocketList.at(i)->getIndex())
			{
				_writSocketList.removeAt(i);
				break;
			}
		}
	});

	connect(socket, &TcpSocket::over, [=]
	{

		for (int i = 0; i < _writSocketList.size(); i++)
		{
			if (socket->getIndex() == _writSocketList.at(i)->getIndex())
			{
				_writSocketList.removeAt(i);
				break;
			}
		}
	});

	connect(socket, SIGNAL(newWritToClintUsers(QList<int>)), this, SLOT(newWritToClintUsersSlot(QList<int>)), Qt::DirectConnection);//文书分发
	connect(socket, SIGNAL(newSpecialToClintUsers(QList<int>)), this, SLOT(newSpecialToClintUsersSlot(QList<int>)), Qt::DirectConnection);//特情分发
	connect(socket, SIGNAL(sendNewWrit(int)), this, SLOT(sendNewWritSlot(int)), Qt::DirectConnection);//通知用户自己已发送文书
	connect(socket, SIGNAL(sendNewSpecial(int)), this, SLOT(sendNewSpecialSlot(int)), Qt::DirectConnection);//通知用户自己已发送特情
	connect(socket, SIGNAL(updateWritContentSingal(int, const QString&,const QString&, const QString&, const QString&)), this, SLOT(updateWritContentSlot(int, const QString&,const QString&, const QString&, const QString&)), Qt::DirectConnection);//共同编辑，word内容下发
	connect(socket, SIGNAL(checkIsOnline(int,int)), this, SLOT(checkIsOnlineSlot(int,int)),Qt::DirectConnection);//通知用户自己已发送文书
	connect(socket, SIGNAL(resetOlnineEditSignal()), this, SLOT(resetOlnineEditSlot()), Qt::DirectConnection);
	connect(socket, SIGNAL(groupTrainStartSignal()), this, SLOT(groupTrainStartSignalSlot()), Qt::DirectConnection);
	connect(socket, SIGNAL(signalUpdateArmy(QString)), this, SLOT(updateArmySlot(QString)), Qt::DirectConnection);
	connect(this, SIGNAL(signalRtiDisConnect()), socket, SLOT(notifyRtiDisConnect()), Qt::QueuedConnection);
	connect(this, SIGNAL(signalRtiReConnect()), socket, SLOT(notifyRtiReConnect()), Qt::QueuedConnection);
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

	_writSocketList.push_back(socket);
	socket->setParent(nullptr);
	socket->moveToThread(thread);
	thread->start();

}

void TcpServer::initial()
{
	if (listen(QHostAddress::Any, 8989))
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		qDebug() << QString(QStringLiteral("成功监听端口%1,开启服务...")).arg(8989);
		//for (int i = 0;i < 104;i++)
		//{
		//	qDebug() << QStringLiteral("红方指挥席位%1用户登录").arg(QString::number(i));
		//	qDebug() << QString(QStringLiteral("当前用户数量：%1").arg(QString::number(i)));
		//}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		qDebug() << QString(QStringLiteral("监听%1失败，请检查！")).arg(8989);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}
	//connect(_tcpTerver,&QTcpServer::newConnection,this,&WritServer::newConnectionSlot);
}

void TcpServer::newWritToClintUsersSlot(QList<int> ClintUsersIdList)
{
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		int clientUserID = _writSocketList.at(i)->getClientUserId();
		for (int id : ClintUsersIdList)
		{
			if (id == clientUserID)
			{
				_writSocketList.at(i)->notifyClientNewWrit();
				break;
			}
		}
	}
}

void TcpServer::newSpecialToClintUsersSlot(QList<int> ClintUsersIdList)
{
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		int clientUserID = _writSocketList.at(i)->getClientUserId();
		for (int id : ClintUsersIdList)
		{
			if (id == clientUserID)
			{
				_writSocketList.at(i)->notifyClientNewSpecial();
				break;
			}
		}
	}
}

void TcpServer::sendNewWritSlot(int sendID)
{
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		int clientUserID = _writSocketList.at(i)->getClientUserId();
		if (sendID == clientUserID)
		{
			_writSocketList.at(i)->notifyClientSendedWrit();
			break;
		}
	}
}

void TcpServer::sendNewSpecialSlot(int sendID)
{
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		int clientUserID = _writSocketList.at(i)->getClientUserId();
		if (sendID == clientUserID)
		{
			_writSocketList.at(i)->notifyClientSendedSpecial();
			break;
		}
	}
}

void TcpServer::updateWritContentSlot(int index, const QString& wordId, const QString& user, const QString& tag, const QString& content)
{
	if (_isCloseOnlineEdit)
	{
		return;
	}
	//客户端word内容更改，不要通知发起更改的客户端
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		if (_writSocketList.at(i)->getIndex() != index)
		{
			_writSocketList.at(i)->updateWordContent(wordId,user, tag, content);
		}
	}
}

void TcpServer::checkIsOnlineSlot(int index, int userID)
{
	int isOnline = 1;
	int count = 0;
	TcpSocket* socket = dynamic_cast<TcpSocket*>(sender());
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		if (_writSocketList.at(i)->getClientUserId() == userID)
		{
			count++;
		}
		if (count==2)
		{
			isOnline = 0;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			qDebug() << QStringLiteral("%1用户已经登录，登录失败").arg(userID);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			
			break;
		}
	}

	if (count == 1)
	{
		qDebug() << QStringLiteral("%1用户登录成功").arg(userID);
		for (int i = 0; i < _writSocketList.size(); i++)
		{
			if (_writSocketList.at(i)->getIndex() == index)
			{
				//开启心跳
				_writSocketList.at(i)->startHearBeat();
				break;
			}
		}

	}

	for (int i = 0; i < _writSocketList.size(); i++)
	{
		if (_writSocketList.at(i)->getIndex()==index)
		{
			_writSocketList.at(i)->returnIsOnline(isOnline);
			break;
		}
	}
}

void TcpServer::groupTrainStartSignalSlot()
{
	//开启一个组训任务时，默认开启协同编辑
	_isCloseOnlineEdit = false;
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		_writSocketList.at(i)->notifyStartGroupMission();
	}
}

void TcpServer::updateArmySlot(QString sceName)
{
	for (int i = 0; i < _writSocketList.size(); i++)
	{
		_writSocketList.at(i)->notifyUpdateArmy(sceName);
	}
}

void TcpServer::resetOlnineEditSlot()
{
	_isCloseOnlineEdit = !_isCloseOnlineEdit;
}

bool TcpServer::killProcessByName(const QString & processName)
{
	QProcess process;
	QString command = QString("taskkill /F /IM %1").arg(processName);
	process.start("cmd.exe",QStringList()<<"/C"<<command);
	process.waitForFinished();
	if (process.exitCode()==0)
	{
		process.close();
		return true;
	}
	else
	{
		return false;
	}
}

void TcpServer::restartSimProcess()
{
	emit signalRtiDisConnect();
	QThread::msleep(3000);

	//if (killProcessByName("rtinode.exe"))
	//{
		killProcessByName("rtinode.exe");
		QSettings setFile("./simPath.ini", QSettings::IniFormat);
		QString simExePath = setFile.value("path/simExePath").toString();
		QProcess process;
		QString program = "cmd.exe";
		QStringList arg;
		arg << "/c" << simExePath;
		process.start(program, arg);
		process.waitForFinished();
		QString output = process.readAllStandardOutput();
		process.close();
		qDebug() << output;

		emit signalRtiReConnect();
		//for (TcpSocket* socket:_writSocketList)
		//{
		//	socket->notifyRtiReConnect();
		//}
	//}
}

bool TcpServer::isProcessRunning(QString exeName)
{
	QProcess process;
	QStringList arg;
    arg<<"/c"<<QString("tasklist | findstr %1").arg(exeName);
	process.start("cmd",arg);
	process.waitForFinished();
	QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
	return output.contains(exeName);
}

void TcpServer::slotRestartSimExe()
{
	for (TcpSocket* socket : _writSocketList)
	{
		socket->notifyRtiDisConnect();
	}

	//QThread::msleep(3000);

	if (killProcessByName("SimEnginePlugin.exe"))
	{
		killProcessByName("rtinode.exe");
		QSettings setFile("./simPath.ini", QSettings::IniFormat);
		QString simExePath = setFile.value("path/simExePath").toString();
		QProcess *process = new QProcess();
		QString program = "cmd.exe";
		QStringList arg;
		arg << "/c" << simExePath;
		process->start(program, arg);
		process->waitForFinished();
		QString output = process->readAllStandardOutput();
		qDebug() << "start SimEnginePlugin:"<<output;
		delete process;


		for (TcpSocket* socket : _writSocketList)
		{
			socket->notifyRtiReConnect();
		}
	}
}
