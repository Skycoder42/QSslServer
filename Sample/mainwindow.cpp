#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow{parent},
	ui{new Ui::MainWindow},
	socket{new QSslSocket{this}}
{
	ui->setupUi(this);

	socket->addCaCertificates(QSslCertificate::fromPath(QStringLiteral(":/certs/root.crt")));

	connect(socket, &QSslSocket::readyRead,
			this, &MainWindow::readyRead);
	connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
			this, &MainWindow::error);
	connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
			this, &MainWindow::sslErrors);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::connectTo(quint16 port)
{
	socket->connectToHostEncrypted(QStringLiteral("127.0.0.1"), port);
}

void MainWindow::on_sendButton_clicked()
{
	socket->write(ui->lineEdit->text().toUtf8());
	ui->lineEdit->clear();
}

void MainWindow::readyRead()
{
	ui->plainTextEdit->appendPlainText(QString::fromUtf8(socket->readAll()));
}

void MainWindow::error()
{
	ui->plainTextEdit->appendPlainText(QStringLiteral(" >>> ERROR: %1").arg(socket->errorString()));
}

void MainWindow::sslErrors(const QList<QSslError> &errors)
{
	for(const auto &error : errors)
		ui->plainTextEdit->appendPlainText(QStringLiteral(" >>> SSL-ERROR: %1").arg(error.errorString()));
}
