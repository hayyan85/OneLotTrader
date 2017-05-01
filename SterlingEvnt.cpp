#include "SterlingEvnt.h"
#include <iostream>
#include <intrin.h>  
#include "UpdateTrade.h"



#include <iomanip>


#include "RTypes.pb.h"



#include "QObject"
#include <QString>

/* XML Parser */
#include "tinyxml.h"
#ifdef TIXML_USE_STL
#include <iostream>
#include <sstream>
using namespace std;
#else
#include <stdio.h>
#endif

#if defined( WIN32 ) && defined( TUNE )
#include <crtdbg.h>
_CrtMemState startMemState;
_CrtMemState endMemState;
#endif


#import "C:\\Windows\SysWOW64\Sti\Sterling.tlb" no_namespace    /*Type Library for Sterling Events*/



namespace Framework
{
	namespace OneLot
	{
		//--------------------------------------------------------------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------------------------------------------------------------
		HRESULT STDMETHODCALLTYPE SterlingEventSink::QueryInterface(REFIID riid, void** ppvObject)
		{
			if (ppvObject == NULL)
				return E_POINTER;

			if (riid == __uuidof(_ISTIEventsEvents) || riid == __uuidof(IUnknown) || riid == __uuidof(IDispatch) )
			{
				*ppvObject = this;
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			static_cast<IUnknown*>(*ppvObject)->AddRef();
			return S_OK;
		}

		ULONG STDMETHODCALLTYPE SterlingEventSink::AddRef()
		{
			return ::InterlockedIncrement(&m_ulRefCount);
		}

		ULONG STDMETHODCALLTYPE SterlingEventSink::Release()
		{
			ULONG ulRefCount = ::InterlockedDecrement(&m_ulRefCount);

			if (ulRefCount == 0)
				delete this;

			return ulRefCount;
		}

	 //IDispatch Methods implementation-------------------------------------------------------
	 STDMETHODIMP SterlingEventSink::GetTypeInfoCount(UINT *pctinfo)
	 { 
		 return E_NOTIMPL;  
	 }


	 STDMETHODIMP SterlingEventSink::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
	 {
		 return E_NOTIMPL;  
	 }

	 STDMETHODIMP SterlingEventSink::GetIDsOfNames(const IID &riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
	 {	 
		 return E_NOTIMPL; 
	 }
	 

	 STDMETHODIMP SterlingEventSink::Invoke( DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, UINT * puArgErr)
	 { 
		 /*
		 DispIdMember    Type
		 13              OrderUpdate
		 15              OrderConfirm
		 12              TradeUpdate
		 */	
			 
			 BSTR ta = pdispparams->rgvarg->bstrVal;
			
			 std::string xml_str = _com_util::ConvertBSTRToString(ta);

			 if(dispidMember==11)
			 {
				
				std::string t = "DSds";
			 }
			 if(dispidMember==13)
			 {
				 Convert__OrderUpdate(xml_str);
			 }
			 else if (dispidMember==15)
			 {
				 Convert__ConfimEvent(xml_str);
			 }
			 else if (dispidMember==12)
			 {
				 Convert__TradeUpdate(xml_str);
			 }
			 return NULL;
		
	 }

	 //--------------------------------------------------------------------------------------------------------------------------------------------
	 // EVENT CONNECTION STUB
	 //--------------------------------------------------------------------------------------------------------------------------------------------
     void SterlingEventSink::Convert__ConfimEvent(std::string& res_xml)	
	 {
		 
		 //Update order
		 TiXmlDocument doc;

		 doc.Parse(res_xml.c_str(), 0, TIXML_ENCODING_UTF8);
		 TiXmlElement* root = doc.FirstChildElement();
		 TiXmlNode* root_node = 0;
		 TiXmlNode* node = 0;
		 std::string a;
		 std::string b;
		 Serializable::CustROrder_ser ou;

		 while( node = root->IterateChildren( node ) )
		 {
			 a = node->Value();
			 b = node->FirstChild()->Value();

			 if(a=="bstrAccount")
			 {	ou.set_bstraccount(b); }

			 else if (a=="bstrClOrderId")
			 { ou.set_bstrclorderid(b); }

		 }//end while

		OrderConfirm(ou);
	 }//End Method


	 void SterlingEventSink::Convert__OrderUpdate(std::string& res_xml)
	 {
		 //Update order
		 TiXmlDocument doc;

		 doc.Parse(res_xml.c_str(), 0, TIXML_ENCODING_UTF8);
		 TiXmlElement* root = doc.FirstChildElement();
		 TiXmlNode* root_node = 0;
		 TiXmlNode* node = 0;
		 std::string a;
		 std::string b;
		 Serializable::CustROrder_ser ou;

		 while( node = root->IterateChildren( node ) )
		 {
			 a = node->Value();
			 b = node->FirstChild()->Value();

			 if(a=="bstrAccount")
			 {	ou.set_bstraccount(b); }
			 
			 else if (a=="bstrDestination")
			 { ou.set_bstrdestination(b.c_str()); }

			 else if (a=="nOrderStatus")
			 { ou.set_norderstatus(atoi(b.c_str())); }

			 else if (a=="bstrSymbol")
			 { ou.set_bstrsymbol(b); }

			 else if (a=="bstrClOrderId")
			 { ou.set_bstrclorderid(b); }

			 else if (a=="bstrAction")
			 { ou.set_bstraction(b); }
			 
			 else if (a=="bstrSide")
			 { ou.set_side(b); }

			 else if (a=="nQuantity")
			 { ou.set_nquantity(atoi(b.c_str())); }

			 else if (a=="nLvsQuantity")
			 { ou.set_nlvsquantity(atoi(b.c_str())); }

			 else if (a=="bstrUser")
			 { ou.set_bstruser(b.c_str()); }
			 
			 else if (a=="bstrUpdateTime")
			 { ou.set_datetime(b.c_str()); }

			 else if (a=="bstrSymbol")
			 { ou.set_bstrsymbol(b); }

			 else if (a=="nOrderRecordId")
			 { ou.set_norderrecordid((b.c_str())); }

		 }//end while

		 /*
		 if(ou.norderstatus()==13)
		 {
			 //OrderUpdate(ou);
		 }*/
		 if(ou.norderstatus()==12) //Order Rejected
		 {
		 }
		 else
		 {
			 OrderUpdate(ou);
		 }
	 }//End method
	 


	 void SterlingEventSink::Convert__TradeUpdate(std::string& res_xml)
	 {
		 //Update order
		 TiXmlDocument doc;

		 doc.Parse(res_xml.c_str(), 0, TIXML_ENCODING_UTF8);
		 TiXmlElement* root = doc.FirstChildElement();
		 TiXmlNode* root_node = 0;
		 TiXmlNode* node = 0;
		 std::string a;
		 std::string b;
		 Framework::OneLot::TradeUpdate tu;

		 while( node = root->IterateChildren( node ) )
		 {
			 a = node->Value();
			 b = node->FirstChild()->Value();

			 if(a=="bstrSymbol")
			 {	tu.set_symbol(b); }


			 if(a=="bstrSide")
			 {	tu.set_side(b); }


			 if(a=="bstrTradeTime")
			 {	tu.set_time(b); }


			 if(a=="bstrClOrderId")
			 {	tu.set_client_id(b); }


			 if(a=="nQuantity")
			 {	tu.set_volume(atoi(b.c_str())); }


			 if(a=="fExecPrice")
			 {
				 tu.set_exec_price(atof(b.c_str()));
			 }
			 
		 }//end while

		 TradeUpdate(tu);
	 }//End method

	 //--------------------------------------------------------------------------------------------------------------------------------------------
	 //--------------------------------------------------------------------------------------------------------------------------------------------

	
	 
	 void SterlingEventSink::OrderConfirm(Serializable::CustROrder_ser& obj)
	 {
		 //This is the event that eventually informs us of the confirmation Event
		 
	 }

	 void SterlingEventSink::OrderRejected(Serializable::CustROrder_ser& obj)
	 {
		 //This is the event that eventually informs us of the Order Rejection Event
		 //LOG4CPLUS_INFO(ExecEngine::EngineLogger, "EVENT : ORDER-REJECTED OCCURED"); 
		
	 }

	 void SterlingEventSink::OrderUpdate(Serializable::CustROrder_ser& obj)
	 {
		 //This is the event that eventually informs us of the Order Rejection Event
		 //LOG4CPLUS_INFO(ExecEngine::EngineLogger, "EVENT : ORDER-UPDATE OCCURED"); 
		
	 }

	 void SterlingEventSink::TradeUpdate(Framework::OneLot::TradeUpdate& tu)
	 {
		  //This is the event that eventually informs us of the Trade Update Event
		// LOG4CPLUS_INFO(ExecEngine::EngineLogger, "EVENT : TRADE-UPDATE OCCURED"); 
		
	 }

	}//End namespace
}//End namespace
