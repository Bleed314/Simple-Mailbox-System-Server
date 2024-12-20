#pragma once
#include <QString>
#include <QByteArray>
#include <QVector>


class  Message
{
public:
	enum MsgType
	{
		MsgTypeNull = 0x00,					//默认值
		EditWord,							//共同编辑
		UserLogin,							//用户登录
		ReceiveWrit,						//接收文书
		SendWrit,							//发送文书
		ReceiveSpecial,						//接收特情
		SendSpecial,						//发送特情
		HeartBeat,							//心跳
		GroupTrainStart,					//组训任务
		ResetOnlineEdit,				    //停止（开启）协同编辑

		SimReStart,                         //引擎重启
		RtiReConnect,						//态势重连rti
		RtiDisconnect,						//态势断开rti

		UpdateArmy							//更新标绘

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

	//获取本身IP
	QString getIPAddress();
private:
	unsigned char head_[2];         //头部
	Message::MsgType type_;         //类型
	QString sender_;                //发送人
	QString send_time_;             //发送时间
	QByteArray body_;                  //内容
	QStringList receiver_;			//接收ip号
	QString IPAddress_;				//本机IP
};

