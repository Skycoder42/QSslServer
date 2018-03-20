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

	QSslSocket *nextPendingConnection();

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

	//p12
	bool importPkcs12(const QString &path,
					  const QByteArray &passPhrase = QByteArray(),
					  bool addCaCerts = true);

	//Ciphers and protocols
	QList<QSslCipher> ciphers() const;
	void setCiphers(const QList<QSslCipher> &ciphers);
	void setCiphers(const QString &ciphers);
	QSsl::SslProtocol protocol() const;
	void setProtocol(QSsl::SslProtocol protocol);

	//Configuration
	void setSslConfiguration(const QSslConfiguration &configuration);
	QSslConfiguration sslConfiguration() const;

protected:
	void incomingConnection(qintptr handle);

private:
	QSslConfiguration _configuration;
};

#endif // QSSLSERVER_H
