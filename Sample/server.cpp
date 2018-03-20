#include "server.h"

Server::Server(QObject *parent) :
	QObject(parent),
	server(new QSslServer(this))
{
	if(!server->importPkcs12(QStringLiteral(":/certs/server.p12")))
		qCritical() << " >>> SERVER ERROR: Failed to import certificates";

	connect(server, &QSslServer::newConnection,
			this, &Server::newConnection);
	connect(server, &QSslServer::acceptError,
			this, &Server::error);
}

void Server::start()
{
	if(server->listen(QHostAddress::LocalHost))
		emit connected(server->serverPort());
	else
		error();
}

void Server::stop()
{
	server->close();
}

void Server::newConnection()
{
	while(server->hasPendingConnections()) {
		auto socket = server->nextPendingConnection();

		connect(socket, &QSslSocket::readyRead, socket, [socket]{
			socket->write(socket->readAll());
		});
		connect(socket, &QSslSocket::disconnected,
				socket, &QSslSocket::deleteLater);
		connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
				socket, [socket](){
			qCritical() << " >>> SERVER SOCKET ERROR:" << socket->errorString();
		});
		connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
				this, [socket](QList<QSslError> errors){
			for(auto error : errors)
				qCritical() << " >>> SERVER SOCKET SSL ERROR:" << error.errorString();
		});
	}
}

void Server::error()
{
	qCritical() << " >>> SERVER ERROR:" << server->errorString();
}
