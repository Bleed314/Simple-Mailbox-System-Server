#include <QCoreApplication>

#include <QMap>
#include <QFile>
#include <QDir>

#include "TcpServer.h"

#include "evpcakunpack.h"
#include "Message.h"

#include "MessageStruct.h"
#include "http/JQHttpServer.h"

QMap<QString,MyFileInfo> g_mapInfo;

QStringList m_files;
void copyAndSaveFile(const MyFileInfo & info)
{
	
	if (g_mapInfo.contains(info.absFilePath))
	{
		// do nothing 
	}
	else
	{

		MyFileInfo temp = info;
		QString a = temp.absFilePath.split("\\").at(temp.absFilePath.split("\\").size() - 1);
		QString path = temp.absFilePath.remove(QRegExp(a));
		QDir dir(path);
		if (!dir.exists())
		{
			dir.mkpath("./");
		}

		QByteArray hex = QByteArray::fromHex(info.body.toUtf8());
		QFile file(info.absFilePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			return;
		}


		file.write(hex);
		file.close();

		g_mapInfo.insert(info.absFilePath, info);
	}
}

void onHttpAccepted(const QPointer<JQHttpServer::Session> &session)
{
	QString method  = session->requestMethod().toUpper();
	QString requestUrl = session->requestUrl();
	if (method == "POST")
	{
		// 上传文件
		if (requestUrl.contains("/file/fileupload"))
		{
			QByteArray body = session->requestBody();		

			MyFileInfo fileInfo;
			UnPackProStructNew(QVariant(body), fileInfo);

			copyAndSaveFile(fileInfo);

			session->replyText("OK");
		}

		// 上传文件列表 ,上传文件列表是干嘛的？
		if (requestUrl.contains("/file/filelist"))
		{
			QByteArray body = session->requestBody();

			QStringList fileList;
			UnPackProStructNew(QVariant(body), fileList);
			
			// 检测 是否存在

			QStringList nothasFiles;

			for each (QString var in fileList)
			{
				if (!m_files.contains(var))
				{
					nothasFiles.append(var);
				}
			}

			QVariant vr = PackProStructNew(nothasFiles);

			session->replyBytes(vr.toByteArray());
		}

		// 获取文件
		if (requestUrl.contains("/file/download"))
		{
			// body 是文件名称，实际
			QByteArray body = session->requestBody();

			if (QFile::exists(body))
			{
				// 解析文件路径
				session->replyFile(body);
			}
		}
	}
}

void initializeHttpServer()
{
	static JQHttpServer::TcpServerManage tcpServerManager(2);

	tcpServerManager.setHttpAcceptedCallback(std::bind(onHttpAccepted, std::placeholders::_1));

	const auto listenSucceed = tcpServerManager.listen(QHostAddress::Any, 23412);

}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	// 初始化 tcp 服务
	TcpServer server;
	server.initial();

	// 初始化 http 服务
	initializeHttpServer();


    return a.exec();
}
