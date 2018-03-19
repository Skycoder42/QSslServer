#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslConfiguration>

class QSslServer : public QTcpServer
{
	Q_OBJECT
public:
	explicit QSslServer(QObject *parent = nullptr);

	QSslSocket *nextPendingSslConnection();

	//Ca-Certificates
	void addCaCertificate(const QSslCertificate &certificate);
	bool addCaCertificate(const QString &path, QSsl::EncodingFormat format = QSsl::Pem);
	void addCaCertificates(const QList<QSslCertificate> &certificates);
	QList<QSslCertificate> caCertificates() const;
	void setCaCertificates(const QList<QSslCertificate> &certificates);

	//Local Certificates
	QSslCertificate localCertificate() const;
	QList<QSslCertificate> localCertificateChain() const;
	void setLocalCertificate(const QSslCertificate &certificate);
	bool setLocalCertificate(const QString &path, QSsl::EncodingFormat format = QSsl::Pem);
	void setLocalCertificateChain(const QList<QSslCertificate> &localChain);

	//PrivKey
	QSslKey privateKey() const;
	void setPrivateKey(const QSslKey &key);
	bool setPrivateKey(const QString &fileName,
					   QSsl::KeyAlgorithm algorithm = QSsl::Rsa,
					   QSsl::EncodingFormat format = QSsl::Pem,
					   const QByteArray &passPhrase = QByteArray());

	//Ciphers and protocols
	QList<QSslCipher> ciphers() const;
	void setCiphers(const QList<QSslCipher> &ciphers);
	void setCiphers(const QString &ciphers);
	QSsl::SslProtocol protocol() const;
	void setProtocol(QSsl::SslProtocol protocol);

	//Configuration
	void setSslConfiguration(const QSslConfiguration &_configuration);
	QSslConfiguration sslConfiguration() const;

	//Errors
	QAbstractSocket::SocketError clientError() const;
	QList<QSslError> clientSslErrors() const;

signals:
	void newSslConnection();
	void clientError(QAbstractSocket::SocketError error);
	void clientSslErrors(const QList<QSslError> &errors);

protected:
	void incomingConnection(qintptr handle);

private slots:
	void socketReady();
	void socketErrors(QAbstractSocket::SocketError error);
	void sslErrors(const QList<QSslError> &errors);

private:
	QSslConfiguration _configuration;
	QAbstractSocket::SocketError _lastError;
	QList<QSslError> _lastSslErrors;
};

#endif // QSSLSERVER_H
