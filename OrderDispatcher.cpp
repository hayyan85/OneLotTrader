#include "OrderDispatcher.h"
#include "RTypes.pb.h"
#include "TimedOrder.h"


#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

namespace Framework
{
	namespace OneLot
	{
		
		OrderDispatcher::OrderDispatcher(TimedOrder* frmptr,std::string row_uid,QObject *parent /* = 0 */)
		{
			this->row_uuid = row_uid;
			this->frmPointer = frmptr;
			QObject::connect(&tmr_seconds, SIGNAL(timeout()), this, SLOT(SecondTimeOut()));
			QObject::connect(this,SIGNAL(UpdateTimerOnForm(std::string,int)),frmPointer,SLOT(UpdateTimer(std::string,int)));
			QObject::connect(this,SIGNAL(DispatchOrder(Serializable::CustROrder_ser,std::string)),frmPointer,SLOT(SendOrderToSterling(Serializable::CustROrder_ser,std::string)));
			Tstatus = EnumTimerStatus::Stopped;
		}

		void OrderDispatcher::StartOrderTimer(int timerDelay,Serializable::CustROrder_ser ser)
		{
			if(Tstatus == EnumTimerStatus::Paused)
			{
				tmr_seconds.start(1000);
				Tstatus = EnumTimerStatus::Running;
			}

			else if(Tstatus != EnumTimerStatus::Running)
			{
				ser_ord = ser;
				Delay = timerDelay;
				remaining_seconds = timerDelay;
				//Start the timer
				tmr_seconds.start(1000);
				Tstatus = EnumTimerStatus::Running;
			}
		}

		void OrderDispatcher::StopOrderTimer()
		{
			tmr_seconds.stop();
			remaining_seconds = Delay;
			UpdateTimerOnForm(row_uuid,remaining_seconds);
			Tstatus = EnumTimerStatus::Stopped;
		}

		void OrderDispatcher::PauseOrderTime()
		{
			tmr_seconds.stop();
			Tstatus = EnumTimerStatus::Paused;
		}

		void OrderDispatcher::SecondTimeOut()
		{
			remaining_seconds--;
			if(remaining_seconds == 0)
			{
				remaining_seconds = Delay;
				DispatchOrder(ser_ord,row_uuid);
			}
			else
			{
				UpdateTimerOnForm(row_uuid,remaining_seconds);
			}
		}
		
	}//end namespace
}//End namespace