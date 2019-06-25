#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

public slots:
	void connectTo(quint16 port);

private slots:
	void on_sendButton_clicked();

	void readyRead();
	void error();
	void sslErrors(const QList<QSslError> &errors);

private:
	Ui::MainWindow *ui;
	QSslSocket *socket;
};

#endif // MAINWINDOW_H
