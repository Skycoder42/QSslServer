#include "qsslserver.h"
#include <QFile>
#include <QCoreApplication>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslCipher>

QSslServer::QSslServer(QObject *parent) :
	QTcpServer{parent},
	_configuration{QSslConfiguration::defaultConfiguration()}
{}

QSslSocket *QSslServer::nextPendingConnection()
{
	return qobject_cast<QSslSocket*>(QTcpServer::nextPendingConnection());
}

QSslConfiguration QSslServer::sslConfiguration() const
{
	return _configuration;
}

void QSslServer::setSslConfiguration(QSslConfiguration configuration)
{
	_configuration = std::move(configuration);
}

void QSslServer::setLocalCertificateChain(const QList<QSslCertificate> &localChain)
{
	_configuration.setLocalCertificateChain(localChain);
}

void QSslServer::setLocalCertificateChain(const QString &fileName, QSsl::EncodingFormat format, QRegExp::PatternSyntax syntax)
{
	_configuration.setLocalCertificateChain(QSslCertificate::fromPath(fileName, format, syntax));
}

QList<QSslCertificate> QSslServer::localCertificateChain() const
{
	return _configuration.localCertificateChain();
}

void QSslServer::setLocalCertificate(const QSslCertificate &certificate)
{
	_configuration.setLocalCertificate(certificate);
}

void QSslServer::setLocalCertificate(const QString &fileName, QSsl::EncodingFormat format)
{
	QFile file{fileName};
	if(file.open(QIODevice::ReadOnly)) {
		QSslCertificate certificate{&file, format};
		if(!certificate.isNull())
			_configuration.setLocalCertificate(certificate);
		file.close();
	}
}

QSslCertificate QSslServer::localCertificate() const
{
	return _configuration.localCertificate();
}

void QSslServer::setPrivateKey(const QSslKey &key)
{
	_configuration.setPrivateKey(key);
}

void QSslServer::setPrivateKey(const QString &fileName, QSsl::KeyAlgorithm algorithm, QSsl::EncodingFormat format, const QByteArray &passPhrase)
{
	QFile file{fileName};
	if(file.open(QIODevice::ReadOnly)) {
		QSslKey newKey{&file, algorithm, format, QSsl::PrivateKey, passPhrase};
		if(!newKey.isNull())
			_configuration.setPrivateKey(newKey);
		file.close();
	}
}

QSslKey QSslServer::privateKey() const
{
	return _configuration.privateKey();
}

bool QSslServer::addCaCertificates(const QString &path, QSsl::EncodingFormat format, QRegExp::PatternSyntax syntax)
{
	auto certs = QSslCertificate::fromPath(path, format, syntax);
	if (!certs.isEmpty()) {
		addCaCertificates(certs);
		return true;
	} else
		return false;
}

void QSslServer::addCaCertificate(const QSslCertificate &certificate)
{
	auto certs = _configuration.caCertificates();
	certs.append(certificate);
	_configuration.setCaCertificates(certs);
}

void QSslServer::addCaCertificates(const QList<QSslCertificate> &certificates)
{
	auto certs = _configuration.caCertificates();
	certs.append(certificates);
	_configuration.setCaCertificates(certs);
}

bool QSslServer::importPkcs12(const QString &path, const QByteArray &passPhrase, bool replaceCaCerts)
{
	QFile file{path};
	if(file.open(QIODevice::ReadOnly)) {
		QSslKey key;
		QSslCertificate cert;
		QList<QSslCertificate> caCerts;
		if (QSslCertificate::importPkcs12(&file, &key, &cert, &caCerts, passPhrase)) {
			file.close();
			if(!caCerts.isEmpty()) {
				if(replaceCaCerts)
					_configuration.setCaCertificates(caCerts);
				else
					addCaCertificates(caCerts);
			}
			if(!cert.isNull())
				_configuration.setLocalCertificate(cert);
			if(!key.isNull())
				_configuration.setPrivateKey(key);
			return true;
		} else
			file.close();
	}

	return false;
}

QSsl::SslProtocol QSslServer::protocol() const
{
	return _configuration.protocol();
}

void QSslServer::setProtocol(QSsl::SslProtocol protocol)
{
	_configuration.setProtocol(protocol);
}

void QSslServer::incomingConnection(qintptr handle)
{
	//Create Socket
	auto socket = new QSslSocket{};
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
