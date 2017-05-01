#ifndef Update_Trade_Header
#define Update_Trade_Header

#include <iostream>

#define IMPLEMENT_SET_METHOD( type , name )	\
private:	type name;	\
public:		void set_##name(  type value ) {  name = value; }

#define IMPLEMENT_GET_METHOD( type , name )	\
public:		 type get_##name() 	{ return name; }

#define IMPLEMENT_SET_GET_METHOD( type , name ) \
	IMPLEMENT_SET_METHOD( type , name );	\
	IMPLEMENT_GET_METHOD( type , name );


namespace Framework
{
	namespace OneLot
	{
		class TradeUpdate
		{
			IMPLEMENT_SET_GET_METHOD(std::string,symbol);
			IMPLEMENT_SET_GET_METHOD(std::string,client_id);
			IMPLEMENT_SET_GET_METHOD(std::string,side);
			IMPLEMENT_SET_GET_METHOD(int,volume);
			IMPLEMENT_SET_GET_METHOD(float, exec_price);
			IMPLEMENT_SET_GET_METHOD(std::string,time);
		};

	}//end namespace ExecutionEngine
}//end namespace Framework
#endif 