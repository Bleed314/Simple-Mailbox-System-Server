#pragma once
#include <QByteArray>
#include "Message.h"
#include <QVector>

/*
ͷ����0xee 0xfe
      0    1

���ͣ�4���ֽ� 
     2 3 4 5

���� 4���ֽڣ�����ֵ����json���ĳ���
     6 7 8 9

json����
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

