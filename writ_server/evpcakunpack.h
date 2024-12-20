#pragma once

#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <string>


/// 结构体序列化模板函数
/// @param data 结构体
/// @param bLittleEndian 大小端
/// @return 字符串
template<class T>
std::string PackProStruct(const T& data, const bool bLittleEndian = true)
{
	QByteArray serial_array;
	QBuffer serial_buffer(&serial_array);
	serial_buffer.open(QIODevice::WriteOnly);
	QDataStream out_stream(&serial_buffer);
	out_stream.setByteOrder(bLittleEndian ? QDataStream::LittleEndian : QDataStream::BigEndian);

	out_stream << data;
	serial_buffer.close();
	std::string serial_std_string(serial_array.data(), serial_array.size());
	return serial_std_string;
}

/// 结构体反序列化模板函数
/// @param varMsg 字符串
/// @param data 结构体
/// @param bLittleEndian 大小端
template<class T>
void UnPackProStruct(const std::string& varMsg, T& data, const bool bLittleEndian = true)
{
	if (varMsg.empty()) return;

	QByteArray deserial_array(varMsg.c_str(), varMsg.size());
	QBuffer deserial_buffer(&deserial_array);
	deserial_buffer.open(QIODevice::ReadOnly);
	QDataStream in_stream(&deserial_buffer);
	in_stream.setByteOrder(bLittleEndian ? QDataStream::LittleEndian : QDataStream::BigEndian);

	in_stream >> data;
	deserial_buffer.close();
}


/// 结构体序列化模板函数
/// @param data 结构体
/// @param bLittleEndian 大小端
/// @return 字符串
template<class T>
QVariant PackProStructNew(const T& data, const bool bLittleEndian = true)
{
	QByteArray serial_array;
	QBuffer serial_buffer(&serial_array);
	serial_buffer.open(QIODevice::WriteOnly);
	QDataStream out_stream(&serial_buffer);
	out_stream.setByteOrder(bLittleEndian ? QDataStream::LittleEndian : QDataStream::BigEndian);

	out_stream << data;
	serial_buffer.close();

	return QVariant(serial_array);
}

/// 结构体反序列化模板函数
/// @param varMsg 字符串
/// @param data 结构体
/// @param bLittleEndian 大小端
template<class T>
void UnPackProStructNew(const QVariant &varMsg, T& data, const bool bLittleEndian = true)
{
	if (!varMsg.isValid() || varMsg.isNull()) return;

	QByteArray deserial_array = varMsg.toByteArray();
	QBuffer deserial_buffer(&deserial_array);
	deserial_buffer.open(QIODevice::ReadOnly);
	QDataStream in_stream(&deserial_buffer);
	in_stream.setByteOrder(bLittleEndian ? QDataStream::LittleEndian : QDataStream::BigEndian);

	in_stream >> data;
	deserial_buffer.close();
}
