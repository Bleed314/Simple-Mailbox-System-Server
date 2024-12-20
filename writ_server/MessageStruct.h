#pragma once

#include <QString>
#include <QDataStream>

struct WordMsg
{
	QString wordId;                     //模板id
	QString mUser;					// 用户
	QString mBk;					//书签
	QString mContent;               //内容
	QStringList srcs;				// 内容所携带的资源文件列表

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
	int sendUser;					// 发送者
	QString receiveUsers;					//所以接收者
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
	QString fileName;			// 文件名
	QString md5;				// 文件md5 
	QString absFilePath;		// 绝对路径
	QString body;				// 文件主体

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