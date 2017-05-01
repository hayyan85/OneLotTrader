#include "SymbolProp.h"
#include "RTypes.pb.h"

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

SymbolProp::SymbolProp(QWidget *parent)	: QDialog(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.cmdCancel,SIGNAL(clicked()),this,SLOT(cancel()));
	QObject::connect(ui.cmdContinue,SIGNAL(clicked()),this,SLOT(AddSymbol()));
	QObject::connect(ui.cmdContinue,SIGNAL(clicked()),this,SLOT(AddSymbol()));
	QObject::connect(ui.lineEditSymbol,SIGNAL(textChanged(QString)),this,SLOT(TextChanged(QString)));
	InitializeForm();
}

SymbolProp::~SymbolProp()
{

}


void SymbolProp::FindStrikeCallPut(QString last , QString& Strike,QString& Callput)
{
	//Find position of C or P
	int pos;
	pos = last.indexOf("C");
	if(pos==-1)
		pos = last.indexOf("P");
	
	int endpos = last.length() - (pos+1);
	Strike = last.right(endpos).left(2);
	Callput = last.right(endpos+1).left(1);
	std::string t = "sdds";
}

void SymbolProp::TextChanged(const QString & txt)
{
	//1=GOOG7 131004C730000
	//This method handles drags and drops
	if(txt.contains(".O~2="))
	{
		QString orig = txt;
		orig.replace("1=","");
		QStringList spliter = orig.split(".O~2=");
		std::string symbol = spliter[0].toStdString();
		std::string underline = spliter[1].replace("~","").toStdString();
		ui.lineEditSymbol->setText(symbol.c_str());
		ui.lineEditUnderline->setText(underline.c_str());

		//Now calculate theMAturity Date
		QString Mdate = QString(symbol.c_str()).split(" ")[1]; //contains 131004C730000
		QString Strike,Callput;
		FindStrikeCallPut(Mdate,Strike,Callput);
		ui.lineEditStrikePrice->setText(Strike);
		ui.comboBoxPutCall->setCurrentIndex(ui.comboBoxPutCall->findText(Callput));
		//Extract left most 6 characters
		Mdate = Mdate.left(6);
		for(int i=2;i<6;i += 2)
		{
			Mdate.insert(i, QString("-"));
			i++;
		}
		//Add the initial 20 for 2013
		Mdate.insert(0,"20");
		//Now we have 2013-10-4
		ui.lineEditMaturity->setText(Mdate);
		ui.lineEditQunatity->setText("1");
	}
}//End Method

void SymbolProp::AddSymbol()
{
 corder.set_lmtprice(ui.lineEditLimitPrice->text().toFloat());
 corder.set_nquantity(ui.lineEditQunatity->text().toInt());
 corder.set_tif(ui.comboBoxTif->currentText().toStdString());
 corder.set_bstrdestination(ui.comboBoxDestination->currentText().toStdString());
 
 corder.set_pricetype(Serializable::CustROrder_ser::EPriceType::CustROrder_ser_EPriceType_Lmt); //Fixed
 corder.set_side(ui.comboBoxSide->currentText().toStdString());
 corder.set_bstrsymbol(ui.lineEditSymbol->text().toStdString());
 corder.set_underlying(ui.lineEditUnderline->text().toStdString());
 corder.set_norderstatus(-99);
 //-------------------------------------------------------------------------------
 //---------------ADDITIONAL OPTION FIELD REQUIREMENTS----------------------------
 corder.set_cust_ordercategory("O");    //Fixed Open for now
 corder.set_instrument(ui.comboBoxInstrument->currentText().toStdString());
 corder.set_maturity(ui.lineEditMaturity->text().replace("-","").toStdString());
 corder.set_putcall(ui.comboBoxPutCall->currentText().toStdString());
 corder.set_strikeprice(ui.lineEditStrikePrice->text().toFloat());
 //-------------------------------------------------------------------------------
 //-------------------------------------------------------------------------------
 accept(); //Return control back to the main 
}

void SymbolProp::cancel()
{

}

void SymbolProp::InitializeForm()
{
	ui.comboBoxDestination->addItem("AMEX");
	ui.comboBoxDestination->addItem("CBOE");
	ui.comboBoxDestination->addItem("ISE");

	ui.comboBoxPriceType->addItem("Limit");
	ui.comboBoxInstrument->addItem("O");
	ui.comboBoxTif->addItem("D");
	ui.comboBoxPutCall->addItem("P");
	ui.comboBoxPutCall->addItem("C");
	ui.comboBoxSide->addItem("B");
	ui.comboBoxSide->addItem("S");
}