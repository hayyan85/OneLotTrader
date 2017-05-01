#ifndef SYMBOLPROP_H
#define SYMBOLPROP_H

#include <QDialog>
#include "ui_SymbolProp.h"
#include "RTypes.pb.h"

class SymbolProp : public QDialog
{
	Q_OBJECT

public:
	SymbolProp(QWidget *parent = 0);
	~SymbolProp();
	Serializable::CustROrder_ser corder;

private:
	Ui::SymbolProp ui;
	void InitializeForm();
	void FindStrikeCallPut(QString last , QString& Strike,QString& Callput);
public slots:
		void AddSymbol();
		void cancel();
		void TextChanged( const QString & txt);
};

#endif // SYMBOLPROP_H
