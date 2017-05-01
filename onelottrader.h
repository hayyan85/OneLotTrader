#ifndef ONELOTTRADER_H
#define ONELOTTRADER_H

#include <QtGui/QMainWindow>
#include "ui_onelottrader.h"
#include "RTypes.pb.h"

Q_DECLARE_METATYPE(Serializable::CustROrder_ser);
Q_DECLARE_METATYPE(std::string);

class OneLotTrader : public QMainWindow
{
	Q_OBJECT

public:
	OneLotTrader(QWidget *parent = 0, Qt::WFlags flags = 0);
	~OneLotTrader();

private:
	Ui::OneLotTraderClass ui;
};

#endif // ONELOTTRADER_H
