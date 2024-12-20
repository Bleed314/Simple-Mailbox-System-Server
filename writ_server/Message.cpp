#include "Message.h"
#include <QStringList>
#include <QDateTime>
#include <QHostAddress>
#include <QNetworkInterface>
#include "MessageParser.h"

Message::Message()
{
	head_[0] = 0xee;
	head_[1] = 0xfe;
	type_ = MsgType::MsgTypeNull;
}

bool Message::toByteArray(QByteArray& byte_array)
{
	byte_array[0] = head_[0];
	byte_array[1] = head_[1];

	if (type_ == MsgType::MsgTypeNull /*|| body_.isEmpty()*/)
	{
		return false;
	}

	byte_array[2] = type_ >> 24 & 0xff;
	byte_array[3] = type_ >> 16 & 0xff;
	byte_array[4] = type_ >> 8 & 0xff;
	byte_array[5] = type_ & 0xff;
	QString receiverStr_ = receiver_.join(";");

	sender_ = getIPAddress();
	send_time_ = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QByteArray content = sender_.toUtf8() + "$" + send_time_.toUtf8() + "$" 
		+ body_ + "$" + receiverStr_.toUtf8();

	QByteArray byte_array_tmp;
	byte_array_tmp.append(content);
	int len = byte_array_tmp.size();

	byte_array[6] = len >> 24 & 0xff;
	byte_array[7] = len >> 16 & 0xff;
	byte_array[8] = len >> 8 & 0xff;
	byte_array[9] = len & 0xff;

	byte_array.append(content);
	return true;
}

QVector<Message *> Message::fromByteArray(const QByteArray & byte_array)
{
	QVector<Message *> msg_list;
	MessageParser::getInstance()->addBuffer(byte_array);
	while (true)
	{
		Message * msg = MessageParser::getInstance()->parseOneMessage();
		if (!msg)
		{
			break;
		}
		msg_list.push_back(msg);
	}
	return msg_list;
}

void Message::setType(Message::MsgType type)
{
	type_ = type;
}

void Message::setSenderAndSendTime(const QStringList& _list)
{
	if (_list.size() == 2)
	{
		setSender(_list.at(0));
		setSendTime(_list.at(1));
	}
}

void Message::setSender(const QString & sender)
{
	sender_ = sender;
}

void Message::setSendTime(const QString & send_time)
{
	send_time_ = send_time;
}

QString Message::getIPAddress()
{
	if (IPAddress_.isEmpty())
	{
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		for (QHostAddress _ipAddr : ipAddressesList)
		{
			if (_ipAddr.protocol() == QAbstractSocket::IPv4Protocol)
			{
				if (!_ipAddr.toString().startsWith("169"))//过滤掉网卡、路由器的IP
				{
					IPAddress_ = _ipAddr.toString();
					break;
				}
			}
		}
		if (IPAddress_.isEmpty())
		{
			IPAddress_ = QHostAddress(QHostAddress::LocalHost).toString();
		}
	}
	return IPAddress_;
}

void Message::setBody(const QByteArray & body)
{
	body_ = body;
}

QStringList Message::getReceiver() const
{
	return receiver_;
}

void Message::setReceiver(const QStringList & receiver)
{
	receiver_ = receiver;
}

Message::MsgType Message::getType() const
{
	return type_;
}

QString Message::getSender() const
{
	return sender_;
}

QString Message::getSendTime() const
{
	return send_time_;
}

QByteArray Message::getBody() const
{
	return body_;
}
