#include <QApplication>
#include <QThread>
#include "mainwindow.h"
#include "server.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QThread mThread;
	QObject::connect(qApp, &QApplication::aboutToQuit, &mThread, [&](){
		mThread.quit();
		mThread.wait();
	});

	auto svr = new Server();
	svr->moveToThread(&mThread);
	QObject::connect(&mThread, &QThread::finished,
					 svr, &Server::stop);
	QObject::connect(&mThread, &QThread::started,
					 svr, &Server::start);

	MainWindow w;
	QObject::connect(svr, &Server::connected,
					 &w, &MainWindow::connectTo);

	mThread.start();
	w.show();
	return a.exec();
}
