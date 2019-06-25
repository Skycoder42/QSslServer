#include <QApplication>
#include <QThread>
#include "mainwindow.h"
#include "server.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QThread serverThread;
	QObject::connect(qApp, &QApplication::aboutToQuit, &serverThread, [&](){
		serverThread.quit();
		serverThread.wait();
	});

	auto svr = new Server{};
	svr->moveToThread(&serverThread);
	QObject::connect(&serverThread, &QThread::finished,
					 svr, &Server::stop);
	QObject::connect(&serverThread, &QThread::started,
					 svr, &Server::start);

	MainWindow w;
	QObject::connect(svr, &Server::connected,
					 &w, &MainWindow::connectTo,
					 Qt::QueuedConnection);

	serverThread.start();
	w.show();
	return a.exec();
}
