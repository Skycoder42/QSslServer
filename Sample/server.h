#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <qsslserver.h>

class Server : public QObject
{
	Q_OBJECT

public:
	explicit Server(QObject *parent = nullptr);

public slots:
	void start();
	void stop();

signals:
	void connected(quint16 port);

private slots:
	void newConnection();
	void error();

private:
	QSslServer *server;
};

#endif // SERVER_H
