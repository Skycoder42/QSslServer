#include "qsslserver.h"
#include <QFile>
#include <QCoreApplication>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslCipher>

QSslServer::QSslServer(QObject *parent) :
	QTcpServer(parent),
	_configuration(QSslConfiguration::defaultConfiguration())
{}

QSslSocket *QSslServer::nextPendingConnection()
{
	return qobject_cast<QSslSocket*>(QTcpServer::nextPendingConnection());
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

bool QSslServer::importPkcs12(const QString &path, const QByteArray &passPhrase, bool addCaCerts)
{
	auto ret = false;
	QFile file(path);
	if(file.open(QIODevice::ReadOnly)) {
		QSslKey key;
		QSslCertificate cert;
		QList<QSslCertificate> caCerts;
		ret = QSslCertificate::importPkcs12(&file, &key, &cert, &caCerts, passPhrase);
		file.close();

		if(ret) {
			if(!caCerts.isEmpty()) {
				if(addCaCerts)
					addCaCertificates(caCerts);
				else
					_configuration.setCaCertificates(caCerts);
			}
			if(!cert.isNull())
				_configuration.setLocalCertificate(cert);
			if(!key.isNull())
				_configuration.setPrivateKey(key);
		}
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

void QSslServer::incomingConnection(qintptr handle)
{
	//Create Socket
	auto socket = new QSslSocket();
	if (!socket->setSocketDescriptor(handle)) {
		emit acceptError(socket->error());
		delete socket;
		return;
	}

	//set ssl data
	socket->setSslConfiguration(_configuration);
	socket->startServerEncryption();
	addPendingConnection(socket);
}
