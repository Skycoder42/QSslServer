# QSslServer
An extension of QTcpServer to support ssl. The counterpart to QSslSocket.

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

## Features
- Works like a normal `QTcpServer`, but with `QSslSocket` instead of `QTcpSocket`
- Adds a variety of useful method to setup the server. Mainly loading certificates and keys.
- You can configure the server as deeply as you want by directly using `QSslConfiguration`

## Installation
The package is providet as qpm  package, [`de.skycoder42.qsslserver`](https://www.qpm.io/packages/de.skycoder42.qsslserver/index.html). You can install it either via qpmx (preferred) or directly via qpm.

### Via qpmx
[qpmx](https://github.com/Skycoder42/qpmx) is a frontend for qpm (and other tools) with additional features, and is the preferred way to install packages. To use it:

1. Install qpmx (See [GitHub - Installation](https://github.com/Skycoder42/qpmx#installation))
2. Install qpm (See [GitHub - Installing](https://github.com/Cutehacks/qpm/blob/master/README.md#installing), for **windows** see below)
3. In your projects root directory, run `qpmx install de.skycoder42.qsslserver`

### Via qpm

1. Install qpm (See [GitHub - Installing](https://github.com/Cutehacks/qpm/blob/master/README.md#installing), for **windows** see below)
2. In your projects root directory, run `qpm install de.skycoder42.qsslserver`
3. Include qpm to your project by adding `include(vendor/vendor.pri)` to your `.pro` file

Check their [GitHub - Usage for App Developers](https://github.com/Cutehacks/qpm/blob/master/README.md#usage-for-app-developers) to learn more about qpm.

**Important for Windows users:** QPM Version *0.10.0* (the one you can download on the website) is currently broken on windows! It's already fixed in master, but not released yet. Until a newer versions gets released, you can download the latest dev build from here:
- https://storage.googleapis.com/www.qpm.io/download/latest/windows_amd64/qpm.exe
- https://storage.googleapis.com/www.qpm.io/download/latest/windows_386/qpm.exe

## Usage
The code sample at the beginning of the page is basically it. For a more advanced example, check the project in the
Sample subfolder. It's a widgets application with an SSL based echo server. Certificates are included to emulate a real life situation:

- Sample/certs/root.crt is a PEM encoded CA certificate that is added as CA cert to the client
- Sample/certs/server.p12 is a PKCS#12 container with the root CA certificate, the server certificate (signed by the CA cert) and the servers private key. It is loaded by the server.

(Note: For this example the p12 file is unencrypted. Typically a password should be used to protect it)