#ifndef TIMEDORDER_H
#define TIMEDORDER_H

#include <QMainWindow>
#include "ui_TimedOrder.h"
#include "QStandardItemModel"
#include "SymbolProp.h"


//Forward Declare it
namespace Framework
{
	namespace OneLot
	{
		class OrderDispatcher;
	}
}

class TimedOrder : public QMainWindow
{
	Q_OBJECT

public:
	TimedOrder(QWidget *parent = 0);
	~TimedOrder();
	QStandardItemModel* OrdersModel;

private:
	std::map<std::string,SymbolProp*> RowIdOrderTrack; //rowno - OrderPtr
	std::map<std::string,Framework::OneLot::OrderDispatcher*> OrderDispatcherTrack; 

	enum PlatformEnum {Sterling};
	
	PlatformEnum SelectedPlatform;
	Ui::TimedOrder ui;
	
	void InitiateDisplay();
    enum OrderModelEnum {RowID,Symbol,Side,Quantity,Price,Exchange,Dispatched,Seconds,Next_Order,Start,Pause,Stop,Remove};
	void RetrieveSterlingAccount();
	void AddNewOrder(SymbolProp* ord);
	void StartOrder(int row);
	void StopOrder(int row);
	void PauseOrder(int row);
	void StartStelingEventHandler();
	void IncrementOrdercount(std::string row_uid);

	
public slots:
	void SelectSymbol();
	void TableViewButtonClicked(int row,int col);
	void UpdateTimer(std::string row_uuid,int remaining_time);
	void ValidateSterlingAccount();
	void SendOrderToSterling(Serializable::CustROrder_ser rorder,std::string row_uuid);
};

#endif // TIMEDORDER_H
