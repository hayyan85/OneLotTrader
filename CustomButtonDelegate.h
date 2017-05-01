#ifndef BUTTONCOLUMNDELEGATE_H
#define BUTTONCOLUMNDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QPushButton>
#include <QTableView>
#include "TimedOrder.h"
#include "QStandardItemModel"

class ButtonColumnDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	explicit ButtonColumnDelegate(TimedOrder* ptr,QObject *parent = 0);
	~ButtonColumnDelegate();

	TimedOrder* CallBackInstance;

	QWidget * createEditor(QWidget *parent,
		const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor,
		const QModelIndex &index) const;

	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

	public slots:
		void cellEntered(const QModelIndex &index);
		void ButtonClicked();

 signals:
			void TriggerButton(int row,int col);
private:
	mutable int buttonrow;
	mutable int buttoncol;
	QTableView *myWidget;
	QPushButton *btn;
	bool isOneCellInEditMode;
	QPersistentModelIndex currentEditedCellIndex;
};

#endif // BUTTONCOLUMNDELEGATE_H