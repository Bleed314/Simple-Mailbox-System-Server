#include "MessageParser.h"


MessageParser * MessageParser::m_staticParser = nullptr;
MessageParser * MessageParser::getInstance()
{
	if (!m_staticParser)
	{
		m_staticParser = new MessageParser;
	}
	return m_staticParser;
}

MessageParser::MessageParser()
{
	buffer.clear();
}


MessageParser::~MessageParser()
{
}

void MessageParser::addBuffer(const QByteArray & buf)
{
	buffer.append(buf);
}


Message * MessageParser::parseOneMessage()
{
	int size = buffer.size();
	if (size < 2)
	{
		return nullptr;
	}
	
	int head_index = -1;
	for (int i = 0; i < size-1;i++)
	{
		if (buffer[i]==(char )0xee && buffer[i+1]==(char)0xfe)
		{
			head_index = i;
			break;
		}
	}
	if (head_index ==-1)
	{
		return nullptr;
	}
	buffer = buffer.right(size-head_index);
	//现在开始，从索引0，代表头部
	size = buffer.size();  //重新计算缓冲区的大小
	if (size<6)   
	{
		return nullptr;
	}

	unsigned int type = ((unsigned int)(unsigned char)(buffer[5])) | (((unsigned int)(unsigned char)(buffer[4])) << 8) | ((unsigned int)((unsigned char)(buffer[3])) << 16) | ((unsigned int)((unsigned char)(buffer[2])) << 24);
	
	if (size<10)
	{
		return nullptr;
	}
	//这里的bodysize包含了所有  包括sender、sendtime、receiverStr_、body
	unsigned int body_size = ((unsigned int)(unsigned char)(buffer[9])) | (((unsigned int)(unsigned char)(buffer[8])) << 8) | ((unsigned int)((unsigned char)(buffer[7])) << 16) | ((unsigned int)((unsigned char)(buffer[6])) << 24);

	if (size<10+body_size)
	{
		return nullptr;
	}
	QByteArray contentByte = buffer.mid(10, body_size);

	QList<QByteArray> content_list = contentByte.split('$');
	int _size = content_list.size();
	if (_size != 4)
	{
		return new Message;
		QList<QByteArray> _tempList = content_list;
		content_list.clear();
		content_list.append(_tempList.at(0));
		content_list.append(_tempList.at(1));

		QByteArray _tempArray;
		for (int i = 2; i < _size - 1;++i)
		{
			_tempArray.append(_tempList.at(i));
			_tempArray.append('$');
		}
		_tempArray.chop(1);
		content_list.append(_tempArray);
		content_list.append(_tempList.at(_size - 1));
	}

	Message *msg = new Message;

	msg->setType((Message::MsgType)type);
	msg->setSenderAndSendTime(QStringList() << QString::fromUtf8(content_list.at(0)) << QString::fromUtf8(content_list.at(1)));
	msg->setBody(content_list.at(2));
	QStringList _ipList = QString::fromUtf8(content_list.at(3)).split(";");
	msg->setReceiver(_ipList);

	buffer = buffer.right(size - 10 - body_size);

	return msg;
}



