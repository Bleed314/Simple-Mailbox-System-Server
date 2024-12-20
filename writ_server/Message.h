#pragma once
#include <QString>
#include <QByteArray>
#include <QVector>


class  Message
{
public:
	enum MsgType
	{
		MsgTypeNull = 0x00,					//Ĭ��ֵ
		EditWord,							//��ͬ�༭
		UserLogin,							//�û���¼
		ReceiveWrit,						//��������
		SendWrit,							//��������
		ReceiveSpecial,						//��������
		SendSpecial,						//��������
		HeartBeat,							//����
		GroupTrainStart,					//��ѵ����
		ResetOnlineEdit,				    //ֹͣ��������Эͬ�༭

		SimReStart,                         //��������
		RtiReConnect,						//̬������rti
		RtiDisconnect,						//̬�ƶϿ�rti

		UpdateArmy							//���±��

	};

	Message();

	
public:
	bool Message::toByteArray(QByteArray& byte_array);

	static  QVector<Message *> fromByteArray(const QByteArray & byte_array);

	Message::MsgType getType() const;
	void setType(Message::MsgType type);

	void setSenderAndSendTime(const QStringList& _list);

	QString getSender() const;

	QString getSendTime() const;
		
	QByteArray getBody() const;
	void setBody(const QByteArray & body);

	QStringList getReceiver() const;
	void setReceiver(const QStringList & receiver);

private:
	void setSender(const QString & sender);

	void setSendTime(const QString & send_time);

	//��ȡ����IP
	QString getIPAddress();
private:
	unsigned char head_[2];         //ͷ��
	Message::MsgType type_;         //����
	QString sender_;                //������
	QString send_time_;             //����ʱ��
	QByteArray body_;                  //����
	QStringList receiver_;			//����ip��
	QString IPAddress_;				//����IP
};

