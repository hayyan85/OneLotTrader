#include "CustomButtonDelegate.h"
#include <QPainter>
#include <QPushButton>
#include <QStylePainter>
#include <QDebug>
#include <QTableView>

ButtonColumnDelegate::ButtonColumnDelegate(TimedOrder* ptr,QObject *parent /* = 0 */) :QStyledItemDelegate(parent)
{
	if(QTableView *tableView = qobject_cast<QTableView *>(parent))
	{
		myWidget = tableView;
		btn = new QPushButton("...", myWidget);
		btn->hide();
		myWidget->setMouseTracking(true);
		connect(myWidget, SIGNAL(entered(QModelIndex)),this, SLOT(cellEntered(QModelIndex)));
		connect(this,SIGNAL(TriggerButton(int,int)),ptr,SLOT(TableViewButtonClicked(int,int)));
		CallBackInstance = ptr;
		isOneCellInEditMode = false;
	}
}

ButtonColumnDelegate::~ButtonColumnDelegate()
{

}


void ButtonColumnDelegate::ButtonClicked()
{
    TriggerButton(buttonrow,buttoncol);
}

//createEditor
QWidget * ButtonColumnDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
	if (index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt() == 1)
	{
		QPushButton * btn = new QPushButton(parent);
		btn->setText(index.data().toString());
		return btn;
	} else 
	{
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

//setEditorData
void ButtonColumnDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
	if (index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt() == 1) 
	{
		QPushButton * btn = qobject_cast<QPushButton *>(editor);
		//btn->setText(index.data().toString()); //added
		btn->setProperty("data_value", index.data());
		this->buttonrow = index.row();
		this->buttoncol = index.column();
		connect(btn,SIGNAL(clicked()),this,SLOT(ButtonClicked()));
	} 
	else 
	{
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

//setModelData
void ButtonColumnDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,	const QModelIndex &index) const
{
	if (index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt() == 1) 
	{
		QPushButton *btn = qobject_cast<QPushButton *>(editor);
	    //btn->setText(index.data().toString());//added
		model->setData(index, btn->property("data_value"));
	} 
	else 
	{
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}
//paint
void ButtonColumnDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if( index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt() == 1 )
	{
		btn->setGeometry(option.rect);
		btn->setText(index.data().toString());
		if (option.state == QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());
		QPixmap map = QPixmap::grabWidget(btn);
		painter->drawPixmap(option.rect.x(),option.rect.y(),map);
	} else 
	{
		QStyledItemDelegate::paint(painter,option, index);
	}
}
//updateGeometry
void ButtonColumnDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

//cellEntered
void ButtonColumnDelegate::cellEntered(const QModelIndex &index)
{
	if(index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole) == 1)
	{
		if(isOneCellInEditMode)
		{
			myWidget->closePersistentEditor(currentEditedCellIndex);
		}
		myWidget->openPersistentEditor(index);
		isOneCellInEditMode = true;
		currentEditedCellIndex = index;
	} else 
	{
		if(isOneCellInEditMode)
		{
			isOneCellInEditMode = false;
			myWidget->closePersistentEditor(currentEditedCellIndex);
		}
	}
}