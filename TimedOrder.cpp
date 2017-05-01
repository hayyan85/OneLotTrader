#include "TimedOrder.h"
#include <ObjBase.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "CustomButtonDelegate.h"
#include "SymbolProp.h"
#include "QMessageBox"
#include "OrderDispatcher.h"
#include "SterlingEvnt.h"
#include "boost/thread/thread.hpp"
#include <atlsafe.h>

#import "C:\\Windows\SysWOW64\Sti\Sterling.tlb" no_namespace    /*Location of Sterling Lib*/



using namespace Framework::OneLot;

TimedOrder::TimedOrder(QWidget *parent)	: QMainWindow(parent)
{
	ui.setupUi(this);
	InitiateDisplay();
	QObject::connect(ui.cmdAddOrder,SIGNAL(clicked()),this,SLOT(SelectSymbol()));
	QObject::connect(ui.cmdValidate,SIGNAL(clicked()),this,SLOT(ValidateSterlingAccount()));
}

TimedOrder::~TimedOrder()
{

}

void TimedOrder::InitiateDisplay()
{
	OrdersModel = new QStandardItemModel();
	ui.comboBoxPlatform->addItem("Sterling");
	
	QStringList labels;
	labels.push_back("RowID");
	labels.push_back("Symbol");
	labels.push_back("Side");
	labels.push_back("Quantity");
	labels.push_back("Price");
	labels.push_back("Exchange");
	labels.push_back("Dispatched");
	labels.push_back("Seconds");
	labels.push_back("Next Order"); //Time left in sending next order
	labels.push_back("Start");
	labels.push_back("Pause");
	labels.push_back("Stop");
	labels.push_back("Remove");
	OrdersModel->setHorizontalHeaderLabels(labels); //Labels for the columns
	OrdersModel->setHeaderData(OrderModelEnum::Start, Qt::Horizontal, 1, Qt::UserRole);
	OrdersModel->setHeaderData(OrderModelEnum::Pause, Qt::Horizontal, 1, Qt::UserRole);
	OrdersModel->setHeaderData(OrderModelEnum::Stop, Qt::Horizontal, 1, Qt::UserRole);
	OrdersModel->setHeaderData(OrderModelEnum::Remove, Qt::Horizontal, 1, Qt::UserRole);
	ui.TableViewOrders->setModel(OrdersModel);
	ButtonColumnDelegate *delegate = new ButtonColumnDelegate(this,ui.TableViewOrders);
	ui.TableViewOrders->setItemDelegate(delegate);
	ui.TableViewOrders->horizontalHeader()->setStretchLastSection(true);
	ui.cmdAddOrder->setEnabled(false);

}

void TimedOrder::SendOrderToSterling(Serializable::CustROrder_ser rorder,std::string row_uuid)
{
	//##########################################################################################
	//Before sending an order makes sure the row is still in the table
	//##########################################################################################

	QList<QStandardItem*> lst = OrdersModel->findItems(row_uuid.c_str(),Qt::MatchExactly,OrderModelEnum::RowID);

	if(lst.size()>0)
	{
			ISTIOrderPtr com_cust_ord(__uuidof(STIOrder));
			com_cust_ord->Account = _com_util::ConvertStringToBSTR(rorder.bstraccount().c_str());
			com_cust_ord->Side = _com_util::ConvertStringToBSTR(rorder.side().c_str());;
			com_cust_ord->Symbol = _com_util::ConvertStringToBSTR(rorder.bstrsymbol().c_str());
			com_cust_ord->Quantity = rorder.nquantity();
			STIPriceTypes pt = rorder.pricetype() == CustROrder_ser::EPriceType::CustROrder_ser_EPriceType_Lmt ? STIPriceTypes::ptSTILmt : STIPriceTypes::ptSTIMkt;
			com_cust_ord->PriceType = pt;
			com_cust_ord->Tif = _com_util::ConvertStringToBSTR(rorder.tif().c_str());
			com_cust_ord->Destination = _com_util::ConvertStringToBSTR(rorder.bstrdestination().c_str());
			com_cust_ord->ClOrderID = _com_util::ConvertStringToBSTR(boost::uuids::to_string(boost::uuids::random_generator()()).c_str());

			//-------------
			com_cust_ord->OpenClose =  _com_util::ConvertStringToBSTR(rorder.cust_ordercategory().c_str());
			com_cust_ord->Maturity =   _com_util::ConvertStringToBSTR(rorder.maturity().c_str());
			com_cust_ord->PutCall =    _com_util::ConvertStringToBSTR(rorder.putcall().c_str());
			com_cust_ord->Instrument = _com_util::ConvertStringToBSTR(rorder.instrument().c_str());
			com_cust_ord->StrikePrice = rorder.strikeprice();

			com_cust_ord->LmtPrice = QString::number(rorder.lmtprice(),'g',2).toDouble(); 

			//User field is in base 64
			com_cust_ord->User =      _com_util::ConvertStringToBSTR(rorder.bstruser().c_str());
			com_cust_ord->Underlying = _com_util::ConvertStringToBSTR(rorder.underlying().c_str());
			//-------------
			int a = com_cust_ord->SubmitOrder();
	
			if(a==0)
			{
				IncrementOrdercount(row_uuid);
			}
	}
}

void TimedOrder::IncrementOrdercount(std::string row_uid)
{
	QList<QStandardItem*> lst = OrdersModel->findItems(row_uid.c_str(),Qt::MatchExactly,OrderModelEnum::RowID);
	
	if(lst.size()>0)
	{
		int row = lst[0]->row();
		int order =  OrdersModel->item(row,OrderModelEnum::Dispatched)->text().toInt();
		order++;
		OrdersModel->item(row,OrderModelEnum::Dispatched)->setText(QString::number(order));
	}
}

void TimedOrder::ValidateSterlingAccount()
{
	SAFEARRAY** q = new SAFEARRAY*();
	try
	{
		ISTIAcctMaintPtr acct(__uuidof(STIAcctMaint));
		acct->GetAccountList(q);
	}
	catch(_com_error&  e)
	{
		QMessageBox::warning(this,"Sterling not running","Cannot connect to sterling. Make sure it is running",QMessageBox::Ok);
		return;
	}

	CComSafeArray<BSTR> sa(*q);

	// For each string in the array
	for (ULONG i = 0; i < sa.GetCount(); i++)
	{
		BSTR d = sa.GetAt(0);
		std::string t = _com_util::ConvertBSTRToString(d);
		ui.comboBoxAsociatedAcount->addItem(t.c_str());
	}
    ui.cmdAddOrder->setEnabled(true);
	boost::thread t(&TimedOrder::StartStelingEventHandler,this);
}


void TimedOrder::StartStelingEventHandler()
{
	HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
	
	//Register Events
	ISTIAppPtr app(__uuidof(STIApp));
	ISTIEventsPtr evnt(__uuidof(STIEvents));

	_bstr_t nme = app->GetTraderName();
	app->SetModeXML(true);
	bool a = app->IsApiEnabled();
	evnt->SetOrderEventsAsStructs(true);

	IConnectionPointContainer *point_container = NULL;
	IConnectionPoint *conn_point = NULL;

	
	//See if this implements a sink type interface
	//retrieve from a pointer to an interface that the object implements
	hr = evnt->QueryInterface(IID_IConnectionPointContainer,(void**)&point_container);

	//Get Interface Pointer to the Events
	hr = point_container->FindConnectionPoint(__uuidof(_ISTIEventsEvents),&conn_point);

	DWORD cookie;
	Framework::OneLot::SterlingEventSink *snk = new Framework::OneLot::SterlingEventSink();

	//Pass custom Interface to the Events
	hr = conn_point->Advise(snk,&cookie);

	if(hr!=S_OK)
	{
		__debugbreak();
	}

	bool bRet;
	MSG msg;
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void TimedOrder::SelectSymbol()
{
		SymbolProp* sload = new SymbolProp(this);		
		sload->setWindowModality(Qt::WindowModal); //Block only this form instance
		if(sload->exec()==1) //Block until the user selects from a form
		{
			sload->corder.set_bstraccount(ui.comboBoxAsociatedAcount->currentText().toStdString().c_str());
			AddNewOrder(sload); //Add a new row to the Orders table
		}
	
}//End method

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//This method displays the timer
void TimedOrder::UpdateTimer(std::string row_uuid,int remaining_time)
{
	QList<QStandardItem*> lst = OrdersModel->findItems(row_uuid.c_str(),Qt::MatchExactly,OrderModelEnum::RowID);

	if(lst.size()>0)
	{
		int row = lst[0]->row();
	   OrdersModel->item(row,OrderModelEnum::Next_Order)->setText(QString::number(remaining_time));
	}
}

void TimedOrder::TableViewButtonClicked(int row,int col) 
{
	std::string uuid = OrdersModel->item(row,OrderModelEnum::RowID)->text().toStdString();

	if(col == OrderModelEnum::Start) //Start
	{
		bool convesrion;
		int sec = -1;
		sec = OrdersModel->item(row,OrderModelEnum::Seconds)->text().toInt(&convesrion);
		if(convesrion && sec!=0)
		{
			if(OrderDispatcherTrack.find(uuid) != OrderDispatcherTrack.end())
			{
				//Instance already running - Manipulate that
				OrderDispatcherTrack[uuid]->StartOrderTimer(OrdersModel->item(row,OrderModelEnum::Seconds)->text().toInt(),RowIdOrderTrack[uuid]->corder);
			}
			else
			{
				StartOrder(row);
			}
		}
		else
		{
			QMessageBox::information(this,"Invalid Timeout Value","Please insert a valid Time out value",QMessageBox::StandardButton::Ok);
		}
	}

	else if(col == OrderModelEnum::Pause) //Pause
	{
		//This check would always be true
		if(OrderDispatcherTrack.find(uuid) != OrderDispatcherTrack.end())
		{
			//Instance already running - Manipulate that
			OrderDispatcherTrack[uuid]->PauseOrderTime();
		}
	}
	else if(col== OrderModelEnum::Stop) //Stop
	{
		//This check would always be true
		if(OrderDispatcherTrack.find(uuid) != OrderDispatcherTrack.end())
		{
			//Instance already running - Manipulate that
			OrderDispatcherTrack[uuid]->StopOrderTimer();
		}
	}
	else if(col==OrderModelEnum::Remove)   //Remove
	{
		//Delete the order from the tracker
		delete RowIdOrderTrack[uuid];
		delete OrderDispatcherTrack[uuid];
		RowIdOrderTrack.erase(uuid);
		OrderDispatcherTrack.erase(uuid);
		OrdersModel->removeRow(row);
	}
}//End Method


void TimedOrder::StartOrder(int row)
{
  std::string uuid = OrdersModel->item(row,OrderModelEnum::RowID)->text().toStdString();
  Framework::OneLot::OrderDispatcher* od = new Framework::OneLot::OrderDispatcher(this,uuid);
  od->StartOrderTimer(OrdersModel->item(row,OrderModelEnum::Seconds)->text().toInt(),RowIdOrderTrack[uuid]->corder);
  OrderDispatcherTrack[uuid] = od;
}

void TimedOrder::StopOrder(int row)
{

}

void TimedOrder::PauseOrder(int row)
{

}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void TimedOrder::AddNewOrder(SymbolProp* ord)
{
	QList<QStandardItem*> lst ;
	std::string uuid;
	uuid = boost::uuids::to_string(boost::uuids::random_generator()()).c_str();
	QStandardItem* item_rowguid     = new QStandardItem(uuid.c_str());                                   item_rowguid->setFlags(item_rowguid->flags() &= ~Qt::ItemIsEditable);
	QStandardItem* item_symbol      = new QStandardItem(ord->corder.bstrsymbol().c_str());               item_symbol->setFlags(item_symbol->flags() &= ~Qt::ItemIsEditable);  
	QStandardItem* item_side        = new QStandardItem(ord->corder.side().c_str());                     item_side->setFlags(item_side->flags() &= ~Qt::ItemIsEditable);    
	QStandardItem* item_quantity    = new QStandardItem(QString::number(ord->corder.nquantity()));       item_quantity->setFlags(item_quantity->flags() &= ~Qt::ItemIsEditable);     
	QStandardItem* item_price       = new QStandardItem(QString::number(ord->corder.lmtprice()));        item_price->setFlags(item_price->flags() &= ~Qt::ItemIsEditable);     
	QStandardItem* item_exchange    = new QStandardItem(ord->corder.bstrdestination().c_str());          item_exchange->setFlags(item_exchange->flags() &= ~Qt::ItemIsEditable);     
	QStandardItem* item_counter     = new QStandardItem("0");                                            item_counter->setFlags(item_counter->flags() &= ~Qt::ItemIsEditable);     
	QStandardItem* item_seconds     = new QStandardItem("--");
	QStandardItem* item_next_order  = new QStandardItem("--");                                           item_next_order->setFlags(item_next_order->flags() &= ~Qt::ItemIsEditable);      
	QStandardItem* item_running     = new QStandardItem("Start");                                        item_running->setFlags(item_running->flags() &= ~Qt::ItemIsEditable);        
	QStandardItem* item_pause       = new QStandardItem("Pause");
	QStandardItem* item_stop        = new QStandardItem("Stop");
	QStandardItem* item_remove      = new QStandardItem("Remove");
	
	lst.push_back(item_rowguid);
	lst.push_back(item_symbol);
	lst.push_back(item_side);
	lst.push_back(item_quantity);
	lst.push_back(item_price);
	lst.push_back(item_exchange);
	lst.push_back(item_counter);
	lst.push_back(item_seconds);
	lst.push_back(item_next_order);
	lst.push_back(item_running);
	lst.push_back(item_pause);
	lst.push_back(item_stop);
	lst.push_back(item_remove);
	OrdersModel->appendRow(lst);

	RowIdOrderTrack[uuid] = ord;
}