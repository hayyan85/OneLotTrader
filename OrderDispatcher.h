#ifndef ORDERDISPATCHER_H
#define ORDERDISPATCHER_H

#include "RTypes.pb.h"
#include "QTimer"
#include "TimedOrder.h"
#include "QObject"

namespace Framework
{
	namespace OneLot
	{
		class OrderDispatcher : public QObject
		{
			Q_OBJECT
		private:
			Serializable::CustROrder_ser ser_ord;
			QTimer tmr_seconds;
			std::string row_uuid;
			int Delay;
			int remaining_seconds;
			TimedOrder* frmPointer;
		    enum EnumTimerStatus {Running,Stopped,Paused};
			EnumTimerStatus Tstatus;
		
		public:
			OrderDispatcher(TimedOrder* frmptr,std::string row_uid,QObject *parent = 0);
			void StartOrderTimer(int timerDelay,Serializable::CustROrder_ser ser);
			void StopOrderTimer();
			void PauseOrderTime();
		
		signals:
			void UpdateTimerOnForm(std::string row_uuid,int remaining_time);
			void DispatchOrder(Serializable::CustROrder_ser ser_ord,std::string row_uuid);
		public slots:
			void SecondTimeOut();
		};

	}//End Namepsace		
}//End Namespace
#endif // 