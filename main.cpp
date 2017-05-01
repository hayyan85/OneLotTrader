#include "onelottrader.h"
#include "TimedOrder.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	//OneLotTrader w;
	//w.show();
	
	TimedOrder w;
	w.show();
	return a.exec();
}
