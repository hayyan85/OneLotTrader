#include "onelottrader.h"
#include "RTypes.pb.h"

OneLotTrader::OneLotTrader(QWidget *parent, Qt::WFlags flags)	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	qRegisterMetaType<Serializable::CustROrder_ser>();
	qRegisterMetaType<std::string>();
}

OneLotTrader::~OneLotTrader()
{

}
