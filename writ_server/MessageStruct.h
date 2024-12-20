#pragma once

#include <QString>
#include <QDataStream>

struct WordMsg
{
	QString wordId;                     //ģ��id
	QString mUser;					// �û�
	QString mBk;					//��ǩ
	QString mContent;               //����
	QStringList srcs;				// ������Я������Դ�ļ��б�

	friend QDataStream& operator<<(QDataStream &os, const WordMsg &data)
	{
		os << data.wordId
			<< data.mUser
			<< data.mBk
			<< data.mContent << data.srcs;
		return os;
	}

	friend QDataStream& operator >> (QDataStream &is, WordMsg &data)
	{
		is >> data.wordId
			>> data.mUser
			>> data.mBk
			>> data.mContent
			>> data.srcs;

		return is;
	}

};
Q_DECLARE_METATYPE(WordMsg)

struct WritMsg
{
	int sendUser;					// ������
	QString receiveUsers;					//���Խ�����
	friend QDataStream& operator<<(QDataStream &os, const WritMsg &data)
	{
		os << data.sendUser
			<< data.receiveUsers;
		return os;
	}

	friend QDataStream& operator >> (QDataStream &is, WritMsg &data)
	{
		is >> data.sendUser
			>> data.receiveUsers;
		return is;
	}

};
Q_DECLARE_METATYPE(WritMsg)


struct MyFileInfo
{
	QString fileName;			// �ļ���
	QString md5;				// �ļ�md5 
	QString absFilePath;		// ����·��
	QString body;				// �ļ�����

	friend QDataStream& operator<<(QDataStream &os, const MyFileInfo &data)
	{
		os << data.fileName
			<< data.md5<<data.absFilePath<<data.body;
		return os;
	}

	friend QDataStream& operator >> (QDataStream &is, MyFileInfo &data)
	{
		is >> data.fileName >> data.md5 
			>> data.absFilePath >> data.body;
		return is;
	}

	bool operator == (const MyFileInfo &info)
	{
		return (this->absFilePath == info.absFilePath) &&
			(this->fileName == info.fileName) &&
			(this->md5 == info.md5);
	}


};

Q_DECLARE_METATYPE(MyFileInfo)