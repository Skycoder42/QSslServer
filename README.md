# QSslServer
An extension of QTcpServer to support ssl. The counterpart to QSslSocket.

## Features
- Works like a normal `QTcpServer`, but with `QSslSocket` instead of `QTcpSocket`
- Adds a variety of useful method to setup the server. Mainly loading certificates and keys.
- You can configure the server as deeply as you want by directly using `QSslConfiguration`

## Installation
The package is provided via qdep, as `Skycoder42/QSslServer`. To use it simply:

1. Install and enable qdep (See [qdep - Installing](https://github.com/Skycoder42/qdep#installation))
2. Add the following to your pro file:
```qmake
QDEP_DEPENDS += Skycoder42/QSslServer
!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")
```

## Usage
It works exactly like the TCP server. The only difference: you need to set a certificate before connecting:
```cpp
auto server = new QSslServer(this);

//emits newConnection just like the tcp variant
connect(server, &QSslServer::newConnection, this, [this](){
	while(server->hasPendingConnections()) {
		auto socket = server->nextPendingConnection();

		//work with the new socket
	}
});

//load a certificate and private key, then start listening
if(!server->importPkcs12(QStringLiteral("/path/to/cert.p12")))
	qCritical() << "Failed to import certificates";
else
	server->listen(QHostAddress::Any, 4711);
```

This code sample is basically it. For a more advanced example, check the project in the
Sample subfolder. It's a widgets application with an SSL based echo server. Certificates are included to emulate a real life situation:

- `Sample/certs/root.crt` is a PEM encoded CA certificate that is added as CA cert to the client
- `Sample/certs/server.p12` is a PKCS#12 container with the root CA certificate, the server certificate (signed by the CA cert) and the servers private key. It is loaded by the server.

Note: For this example the p12 file is unencrypted. Typically a password should be used to protect it.
