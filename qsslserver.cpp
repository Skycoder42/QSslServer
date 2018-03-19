#include "qsslserver.h"
#include <QFile>
#include <QCoreApplication>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslCipher>

QSslServer::QSslServer(QObject *parent) :
	QTcpServer(parent),
	_configuration(QSslConfiguration::defaultConfiguration()),
	_lastError(QAbstractSocket::UnknownSocketError),
	_lastSslErrors()
{}

QSslSocket *QSslServer::nextPendingSslConnection()
{
	return qobject_cast<QSslSocket*>(nextPendingConnection());
}

void QSslServer::addCaCertificate(const QSslCertificate &certificate)
{
	auto certs = _configuration.caCertificates();
	certs.append(certificate);
	_configuration.setCaCertificates(certs);
}

bool QSslServer::addCaCertificate(const QString &path, QSsl::EncodingFormat format)
{
	auto ret = false;
	QFile file(path);
	if(file.open(QIODevice::ReadOnly)) {
		QSslCertificate cert(file.readAll(), format);
		if(!cert.isNull()) {
			addCaCertificate(cert);
			ret = true;
		}
		file.close();
	}

	return ret;
}

void QSslServer::addCaCertificates(const QList<QSslCertificate> &certificates)
{
	auto certs = _configuration.caCertificates();
	certs.append(certificates);
	_configuration.setCaCertificates(certs);
}

QList<QSslCertificate> QSslServer::caCertificates() const
{
	return _configuration.caCertificates();
}

void QSslServer::setCaCertificates(const QList<QSslCertificate> &certificates)
{
	_configuration.setCaCertificates(certificates);
}

QSslCertificate QSslServer::localCertificate() const
{
	return _configuration.localCertificate();
}

QList<QSslCertificate> QSslServer::localCertificateChain() const
{
	return _configuration.localCertificateChain();
}

void QSslServer::setLocalCertificate(const QSslCertificate &certificate)
{
	_configuration.setLocalCertificate(certificate);
}

bool QSslServer::setLocalCertificate(const QString &path, QSsl::EncodingFormat format)
{
	auto ret = false;
	QFile file(path);
	if(file.open(QIODevice::ReadOnly)) {
		QSslCertificate cert(file.readAll(), format);
		if(!cert.isNull()) 	{
			_configuration.setLocalCertificate(cert);
			ret = true;
		}
		file.close();
	}

	return ret;
}

void QSslServer::setLocalCertificateChain(const QList<QSslCertificate> &localChain)
{
	_configuration.setLocalCertificateChain(localChain);
}

QSslKey QSslServer::privateKey() const
{
	return _configuration.privateKey();
}

void QSslServer::setPrivateKey(const QSslKey &key)
{
	_configuration.setPrivateKey(key);
}

bool QSslServer::setPrivateKey(const QString &fileName, QSsl::KeyAlgorithm algorithm, QSsl::EncodingFormat format, const QByteArray &passPhrase)
{
	auto ret = false;
	QFile file(fileName);
	if(file.open(QIODevice::ReadOnly)) {
		QSslKey newKey(file.readAll(), algorithm, format, QSsl::PrivateKey, passPhrase);
		if(!newKey.isNull()) {
			_configuration.setPrivateKey(newKey);
			ret = true;
		}
		file.close();
	}

	return ret;
}

QList<QSslCipher> QSslServer::ciphers() const
{
	return _configuration.ciphers();
}

void QSslServer::setCiphers(const QList<QSslCipher> &ciphers)
{
	_configuration.setCiphers(ciphers);
}

void QSslServer::setCiphers(const QString &ciphers)
{
	auto cphl = ciphers.split(QLatin1Char(':'), QString::SkipEmptyParts);
	QList<QSslCipher> cphs;
	for(auto cph : cphl) {
		QSslCipher c(cph);
		if(!c.isNull())
			cphs.append(c);
	}
	_configuration.setCiphers(cphs);
}

QSsl::SslProtocol QSslServer::protocol() const
{
	return _configuration.protocol();
}

void QSslServer::setProtocol(QSsl::SslProtocol protocol)
{
	_configuration.setProtocol(protocol);
}

void QSslServer::setSslConfiguration(const QSslConfiguration &configuration)
{
	_configuration = configuration;
}

QSslConfiguration QSslServer::sslConfiguration() const
{
	return _configuration;
}

QAbstractSocket::SocketError QSslServer::clientError() const
{
	return _lastError;
}

QList<QSslError> QSslServer::clientSslErrors() const
{
	return _lastSslErrors;
}

void QSslServer::incomingConnection(qintptr handle)
{
	//Create Socket
	auto socket = new QSslSocket();
	if (!socket->setSocketDescriptor(handle)) {
		_lastError = socket->error();
		delete socket;
		emit clientError(_lastError);
		return;
	}

	//Connects
	connect(socket, &QSslSocket::encrypted,
			this, &QSslServer::socketReady);
	connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
			this, &QSslServer::socketErrors);
	connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
			this, &QSslServer::sslErrors);

	//set ssl data
	socket->setSslConfiguration(_configuration);
	socket->startServerEncryption();
}

void QSslServer::socketReady()
{
	auto socket = qobject_cast<QSslSocket*>(sender());
	if(socket) {
		socket->disconnect(this);
		addPendingConnection(socket);
		emit newConnection();
	}
}

void QSslServer::socketErrors(QAbstractSocket::SocketError error)
{
	_lastError = error;
	emit clientError(error);
	auto socket = qobject_cast<QSslSocket*>(sender());
	if(socket)
		socket->deleteLater();
}

void QSslServer::sslErrors(const QList<QSslError> &errors)
{
	_lastSslErrors = errors;
	emit clientSslErrors(errors);
	auto socket = qobject_cast<QSslSocket*>(sender());
	if(socket)
		socket->deleteLater();
}
