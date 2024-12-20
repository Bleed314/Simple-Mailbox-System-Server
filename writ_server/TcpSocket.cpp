#include "TcpSocket.h"

#include <windows.h>

#include <QDataStream>
#include <QFile>
#include <QDir>
#include <QHostAddress>
#include <QThread>
#include <QTextStream>
#include <QDateTime>
#include <QProcess>
#include "Message.h"

#include "evpcakunpack.h"
#include "MessageStruct.h"

using namespace std;

static int Index = 0;

TcpSocket::~TcpSocket()
{
	if (_heartBeatTimer)
	{
		if (_heartBeatTimer->isActive())
		{
			_heartBeatTimer->stop();
		}
	}
}

TcpSocket::TcpSocket(qintptr socketDescriptor, QTcpSocket *parent) : QTcpSocket(parent)
{
	_index = Index++;
	_socketDescriptor = socketDescriptor;
	this->setSocketDescriptor(socketDescriptor);
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(SocketErr(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(readyRead()), this, SLOT(slotReadPendingDatagrams()));

	QHostAddress address = this->peerAddress();
	int port = this->peerPort();
	connect(this, &QTcpSocket::disconnected, this, [=]()
	{
		qDebug() << QString(QStringLiteral("%1:%2�Ͽ�����")).arg(address.toString()).arg(port);
		qDebug() << QString(QStringLiteral("�û�%1��������.......end")).arg(_clientUserId);
		Qt::HANDLE id = QThread::currentThreadId();
	});


	connect(this, SIGNAL(signalUpdateArmyBySelf(QString)), this, SLOT(slotUpdateArmyMark(QString)), Qt::DirectConnection);
}

void TcpSocket::startHearBeat()
{
	_heartBeatTimer = new QTimer();
	connect(_heartBeatTimer, &QTimer::timeout, [=]
	{
		// ���
		QString str = QString("heart beat");
		QVariant var = PackProStructNew(str);
		Message _message;
		_message.setType(Message::HeartBeat);
		_message.setBody(var.toByteArray());
		QByteArray byte_array;
		if (_message.toByteArray(byte_array))
		{
			qint64  size = 1;
			if (this)
			{
				size = this->write(byte_array);
			}
			if (size <= 0)
			{
				_heartBeatFailCount++;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
				qDebug() << QStringLiteral("���û�%1��������ʧ�ܵ�%2�Σ�").arg(_clientUserId).arg(QString::number(_heartBeatFailCount));
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN| FOREGROUND_BLUE);
				if (_heartBeatFailCount == 3)//����ʧ��������ʼ
				{
					emit over();
					this->deleteLater();
				}
			}
			else
			{
				_heartBeatFailCount = 0;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
				qDebug() << QStringLiteral("���û�%1�������ͳɹ���").arg(_clientUserId);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
		}
	});
	_heartBeatTimer->start(5000);
}

void TcpSocket::stopHearBeat()
{
	if (_heartBeatTimer->isActive())
	{
		_heartBeatTimer->stop();
	}
}

void TcpSocket::SocketErr(QAbstractSocket::SocketError socketError)
{
	TcpSocket *socket = (TcpSocket*)sender();
	QString str = QString("socket[%1] ip[%2] port[%3] err[%4]").arg(socket->socketDescriptor())
		.arg(socket->peerAddress().toString().toLocal8Bit().data()).arg(socket->peerPort()).arg(socket->errorString().toLocal8Bit().data());
	qDebug() << str;
}

void TcpSocket::slotUpdateArmyMark(QString sceName)
{

	QVariant var = PackProStructNew(sceName);
	Message _message;
	_message.setBody(var.toByteArray());
	_message.setType(Message::UpdateArmy);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		flush();

	}
}

void TcpSocket::notifyClientNewWrit()
{
	Message _message;
	_message.setType(Message::ReceiveWrit);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		flush();

	}
}

void TcpSocket::notifyClientNewSpecial()
{
	Message _message;
	_message.setType(Message::ReceiveSpecial);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		flush();

	}
}

void TcpSocket::notifyClientSendedWrit()
{
	Message _message;
	_message.setType(Message::SendWrit);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		flush();

	}
}

void TcpSocket::notifyStartGroupMission()
{
	Message _message;
	_message.setType(Message::GroupTrainStart);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		flush();

	}
}

void TcpSocket::notifyUpdateArmy(QString sceName)
{

	emit signalUpdateArmyBySelf(sceName);


}

void TcpSocket::notifyRtiDisConnect()
{
	// ���
	Message _message;
	_message.setType(Message::RtiDisconnect);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		this->flush();

	}
}

void TcpSocket::notifyRtiReConnect()
{
	// ���
	Message _message;
	_message.setType(Message::RtiReConnect);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		this->flush();

	}
}

void TcpSocket::notifyClientSendedSpecial()
{
	Message _message;
	_message.setType(Message::SendSpecial);
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		flush();

	}
}

void TcpSocket::updateWordContent(const QString& id,const QString& user, const QString& tag, const QString& content)
{
	// ���
	WordMsg msg;
	msg.wordId = id;
	msg.mUser = user;
	msg.mBk = tag;
	msg.mContent = content;
	QVariant var = PackProStructNew(msg);

	Message _message;
	_message.setType(Message::EditWord);
	_message.setBody(var.toByteArray());
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		this->flush();
	}
}

void TcpSocket::returnIsOnline(int isOnline)
{
	// ���
	int msg = isOnline;
	QVariant var = PackProStructNew(msg);

	Message _message;
	_message.setType(Message::UserLogin);
	_message.setBody(var.toByteArray());
	QByteArray byte_array;
	if (_message.toByteArray(byte_array))
	{
		QDataStream outStream(this);
		outStream.setVersion(QDataStream::Qt_5_7);
		outStream << byte_array;
		this->flush();

	}
}


void TcpSocket::slotReadPendingDatagrams()
{
	if (this != NULL)
	{
		qint64 size = this->bytesAvailable();
		if (size > 0)
		{
			QByteArray datagram;
			datagram.resize(size);
			datagram = this->read(size);
			if (!datagram.isEmpty())
			{
				QVector<Message* > vector = Message::fromByteArray(datagram);
				for each (Message*  mess in vector)
				{
					if (mess->getType()==Message::HeartBeat)
					{
						QString datas;
						UnPackProStructNew(QVariant(mess->getBody()), datas);
						//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY| FOREGROUND_BLUE);
						//qDebug() << datas;
						//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY );
					}
					if (mess->getType() == Message::EditWord)
					{
						qDebug() << QString(QStringLiteral("Эͬ�༭......"));
						WordMsg  datas;
						UnPackProStructNew(QVariant(mess->getBody()), datas);
						//ת���������ͻ���
						emit updateWritContentSingal(_index,datas.wordId,datas.mUser, datas.mBk, datas.mContent);
					}
					if (mess->getType() == Message::ResetOnlineEdit)
					{
						qDebug() << QString(QStringLiteral("����Эͬ�༭�Ƿ���......"));

						emit resetOlnineEditSignal();
					}
					if (mess->getType() == Message::UserLogin)
					{
						UnPackProStructNew(QVariant(mess->getBody()), _clientUserId);
						qDebug() << QString(QStringLiteral("%1�û�����......").arg(_clientUserId));
						//֪ͨ�����̵߳�server��ѯ���û��Ƿ��Ѿ���������
						emit checkIsOnline(_index,_clientUserId);
					}
					if (mess->getType() == Message::ReceiveWrit)
					{
						qDebug() << QString(QStringLiteral("�������ϴ�......"));
						WritMsg writ;
						UnPackProStructNew(QVariant(mess->getBody()), writ);
						int sendUserId = writ.sendUser;
						QString receiveUserId = writ.receiveUsers;
						//֪ͨ�����ͻ����߳��������飬�������߳��жϸ������Ƿ��Ƿ��͸�����
						QStringList receiveUserIdStrList = receiveUserId.split(",");
						QList<int> receiveUserIdList;
						for (QString str : receiveUserIdStrList)
						{
							receiveUserIdList.push_back(str.toInt());
						}
						emit newWritToClintUsers(receiveUserIdList);//֪ͨ�����ͻ����߳���������,���ո�����Ŀͻ�����Ҫˢ�½���
						emit sendNewWrit(sendUserId);//���ڷ�������Ŀͻ��˶˿ڿ��ܺ��������ͻ��˶˿ڲ�ͬ�����Է������Ժ�ҲҪ֪ͨ�����û����ڵ������߳�
					}
					if (mess->getType() == Message::ReceiveSpecial)
					{
						qDebug() << QString(QStringLiteral("�������ϴ�......"));
						WritMsg writ;
						UnPackProStructNew(QVariant(mess->getBody()), writ);
						int sendUserId = writ.sendUser;
						QString receiveUserId = writ.receiveUsers;
						//֪ͨ�����ͻ����߳��������飬�������߳��жϸ������Ƿ��Ƿ��͸�����
						QStringList receiveUserIdStrList = receiveUserId.split(",");
						QList<int> receiveUserIdList;
						for (QString str : receiveUserIdStrList)
						{
							receiveUserIdList.push_back(str.toInt());
						}
						emit newSpecialToClintUsers(receiveUserIdList);//֪ͨ�����ͻ����߳���������,���ո�����Ŀͻ�����Ҫˢ�½���
						emit sendNewSpecial(sendUserId);//���ڷ�������Ŀͻ��˶˿ڿ��ܺ��������ͻ��˶˿ڲ�ͬ�����Է������Ժ�ҲҪ֪ͨ�����û����ڵ������߳�
					}
					if (mess->getType() == Message::GroupTrainStart)
					{
						qDebug() << QString(QStringLiteral("������ѵ����......"));
						emit groupTrainStartSignal();
					}
					if (mess->getType() == Message::SimReStart)
					{
						qDebug() << QString(QStringLiteral("����ע��ʵ��ʧ�ܣ��������棬֪ͨ���пͻ�������rti......"));

						QDateTime date;
						UnPackProStructNew(QVariant(mess->getBody()), date);

						static QDateTime s_dateTime = date.addSecs(6);

						// kill SimEngimePlugin.exe 

						QProcess process;
						QString command = QString("taskkill /F /IM %1").arg("SimEnginePlugin.exe");
						process.start("cmd.exe", QStringList() << "/C" << command);
						process.waitForFinished();
						if (process.exitCode() == 0)
						{
							process.close();
						
						}
						else
						{
						
						}
						



						emit signalRestartSimExe();


						s_dateTime = date;

					}
					if (mess->getType() == Message::UpdateArmy)
					{
						QString sceName;
						UnPackProStructNew(QVariant(mess->getBody()), sceName);
						qDebug() << QString(QStringLiteral("���±��"));
						emit signalUpdateArmy(sceName);
					}
				}
			}
		}
	}
}
