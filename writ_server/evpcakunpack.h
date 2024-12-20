#pragma once

#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <string>


/// �ṹ�����л�ģ�庯��
/// @param data �ṹ��
/// @param bLittleEndian ��С��
/// @return �ַ���
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

/// �ṹ�巴���л�ģ�庯��
/// @param varMsg �ַ���
/// @param data �ṹ��
/// @param bLittleEndian ��С��
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


/// �ṹ�����л�ģ�庯��
/// @param data �ṹ��
/// @param bLittleEndian ��С��
/// @return �ַ���
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

/// �ṹ�巴���л�ģ�庯��
/// @param varMsg �ַ���
/// @param data �ṹ��
/// @param bLittleEndian ��С��
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
