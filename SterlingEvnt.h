#ifndef Sterling_Events_Header
#define Sterling_Events_Header

#include <iostream>
#include <string>
#include "RTypes.pb.h"
#include "UpdateTrade.h"
#import "C:\\Windows\SysWOW64\Sti\Sterling.tlb" no_namespace    /*Location of Sterling Lib*/


using namespace Serializable;

namespace Framework
{
	namespace OneLot
	{
		class SterlingEventSink : public _ISTIEventsEvents
		{
			private:

			ULONG m_ulRefCount;
			LPTYPEINFO m_ptinfoEnglish;  // English type information for application interface
			LPTYPEINFO m_ptinfoGerman;   // German type information for application interface
	
			//boost::mutex mutex_MsgRetrieval; 

			//----------------- Implementation of IUnknown Methods:-------------------------------
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
			virtual ULONG STDMETHODCALLTYPE AddRef();
			virtual ULONG STDMETHODCALLTYPE Release();		
			//--------------------------------------------------------------------------------
     
			 public:
			 SterlingEventSink() : m_ulRefCount(0) {}
			 ~SterlingEventSink() {}

			 //IDispatch Methods implementation-------------------------------------------------------
			 STDMETHODIMP GetTypeInfoCount(UINT *pctinfo);
			 STDMETHODIMP GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo );
			 STDMETHODIMP GetIDsOfNames(const IID &riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId );
			 STDMETHODIMP Invoke( DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, UINT * puArgErr);
			 //---------------------------------------------------------------------------------------

			 //These are our defined events----------------------------------------------------------
			 void Convert__OrderUpdate(std::string& res_xml);  //Receives the Event from Invoke Parses the XML and then sends it to OrderUpdate
			 void Convert__ConfimEvent(std::string& res_xml);
			 void Convert__TradeUpdate(std::string& res_xml);
			 
			 void OrderConfirm(Serializable::CustROrder_ser& obj);
			 void OrderRejected(Serializable::CustROrder_ser& obj);
			 void OrderUpdate(Serializable::CustROrder_ser& obj);
			 void TradeUpdate(Framework::OneLot::TradeUpdate& tu);
		};
	}//end namespace OneLot
}//end namespace Framework
#endif 