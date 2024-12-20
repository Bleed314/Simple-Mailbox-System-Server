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
	void over();//�ͻ��˶Ͽ�����ʱ���߳��˳�����ʱ�����߳�
	void newWritToClintUsers(QList<int>); //֪ͨ�����ͻ����߳���������
	void newSpecialToClintUsers(QList<int>); //֪ͨ�����ͻ����߳���������
	void sendNewWrit(int);
	void sendNewSpecial(int);
	void updateWritContentSingal(int index,const QString& id,const QString& user, const QString& tag, const QString& content);//��ͬ�༭������word���ݸ���
	void checkIsOnline(int index, int userID);//֪ͨ�����̵߳�server��ѯ���û��Ƿ��Ѿ���������
	void groupTrainStartSignal();//�����˿�����ѵ����
	void resetOlnineEditSignal();//�����Ƿ���Эͬ�༭
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
	void notifyClientNewWrit();//���̵߳Ŀͻ����û���������ʱ֪ͨ�ͻ���
	void notifyClientNewSpecial();
	void notifyClientSendedWrit();//���̵߳Ŀͻ����û�����������
	void notifyClientSendedSpecial();
	void updateWordContent(const QString& id,const QString& user, const QString& tag, const QString& content);//��ͬ�༭:���¸��߳̿ͻ���word
	void returnIsOnline(int isOnline);//0��ʾ�����ߣ�1��ʾ������
	void notifyStartGroupMission();//֪ͨ�����������ѵ������
	void notifyUpdateArmy(QString sceName);//֪̬ͨ��

public slots:
	void notifyRtiDisConnect();
	void notifyRtiReConnect();

public:
	//������������ֹ�ͻ����쳣������˳���·�������ϣ��ϵ�����ȣ���������޷���⣬���Ե�������������ʧ������ʱ��Ĭ�Ͽͻ������˳�����������������ÿͻ���
	void startHearBeat();
	void stopHearBeat();

private:
	int _clientUserId=-1;//ÿ���ͻ��˶�Ӧ�û���id
	QMap<QString, QString> _wordMap;//����word��tag�Լ����Ӧ��content
	qintptr _socketDescriptor;
	int _index;
	int _heartBeatFailCount;
	QTimer* _heartBeatTimer=nullptr;
};

