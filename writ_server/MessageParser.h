#pragma once
#include <QByteArray>
#include "Message.h"
#include <QVector>

/*
头部：0xee 0xfe
      0    1

类型：4个字节 
     2 3 4 5

长度 4个字节，它的值代表json串的长度
     6 7 8 9

json串：
     10 11 12 ...

*/


class MessageParser
{
public:
	static MessageParser * getInstance();
	MessageParser();
	~MessageParser();

	void addBuffer(const QByteArray & buf);

	Message * parseOneMessage();

private:
	QByteArray buffer;

	static MessageParser * m_staticParser;
};

