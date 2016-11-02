#include <ctime>
#include <iostream>
#include <thread>
#include "Poco/NamedMutex.h"
#include "utils.h"
#include "ffcTypes.h"
#include "ActionManager.h"

#define MAX_ORDER_COUNT	200



//---------- —екци€ общей пам€ти -----------------
#pragma data_seg ("shared")
FfcOrder master_orders[MAX_ORDER_COUNT] = { 0 };
int ordersCount		= 0;
int ordersTotal		= 0;
bool ordersValid	= false;
bool transmitterBusy = false;
int cocktails[20][20] = {0};
#pragma data_seg()
#pragma comment(linker, "/SECTION:shared,RWS")
//------------------------------------------------

Poco::NamedMutex mutex("ffc_mutex");

//---------- ѕеременные ресивера -----------------
int masterTickets[MAX_ORDER_COUNT];
bool recieverInit = false;
long id_login = 0;
int id_cocktails = 0;


//---------- ѕеременные трансмиттера -------------
FfcOrder client_orders[MAX_ORDER_COUNT] = { 0 };
int ordersRCount = 0;
bool transmitterInit = false;


namespace ffc {



	//---------- Transmitter part ----------------
	bool ffc_Init() {
		if (transmitterInit) return false;  //ѕовторна€ инициализаци€
		transmitterInit = true;
		ordersCount = 0;
		ordersValid = false;
		transmitterBusy = false;
		if (AllocConsole()) {
			freopen("CONOUT$", "w", stdout);
			freopen("conout$", "w", stderr);
			SetConsoleOutputCP(CP_UTF8);// GetACP());
			SetConsoleCP(CP_UTF8);
			std::cout << "DLL inited.\r\n";
		}
		return true;  //”дачна€ инициализаци€
	}

	void ffc_OrderUpdate(int OrderTicket, int magic, wchar_t* OrderSymbol, int orderType,
		double OrderLots, double OrderOpenPrice, __time64_t OrderOpenTime,
		double OrderTakeProfit, double OrderStopLoss, double  OrderClosePrice, __time64_t  OrderCloseTime,
		__time64_t OrderExpiration, double  OrderProfit, double  OrderCommission, double  OrderSwap, wchar_t* OrderComment) {

		transmitterBusy = true;

		master_orders[ordersCount] = { OrderTicket, getMagic(OrderComment) , L"default", orderType, OrderLots, OrderOpenPrice, OrderOpenTime, OrderTakeProfit, OrderStopLoss, OrderClosePrice, OrderCloseTime, OrderExpiration, OrderProfit, OrderCommission, OrderSwap, L"" };

		wcscpy_s(master_orders[ordersCount].symbol, SYMBOL_LENGTH, OrderSymbol);

		wchar_t s2[20];
		_itow(OrderTicket, s2, 10);
		wcscpy_s(master_orders[ordersCount].comment, COMMENT_LENGTH, L"ffc_");
		wcscat(master_orders[ordersCount].comment, s2);

		std::wcout << "order #" << OrderTicket << " magic=" << master_orders[ordersCount].magic << " comment = " << master_orders[ordersCount].comment << "\r\n";
		ordersCount++;

	}

	int ffc_OrdersTotal() {
		std::wcout << "ffc_OrdersTotal: " << ordersTotal << "\r\n";
		return ordersTotal;
	}

	int ffc_GetOrderInfo() {
		MqlOrder mqlGetOrder[1] = { master_orders[ordersCount].ticket, master_orders[ordersCount].magic };
		return 1;
	}

	void ffc_ordersCount(int num) {
		mutex.lock();
		transmitterBusy = true;
		ordersCount = 0;
		ordersTotal = num;
		std::cout << "ordersTotal - " << ordersTotal << "\r\n";
		mutex.unlock();
	}

	void ffc_validation(bool flag) {
		ordersValid = flag;
		transmitterBusy = false;
		std::cout << "Orders validation: " << ordersValid << "\r\n";
	}

	int ffc_GetTicket() { // выдаем тикет(ticket_id), о котором нам хотелось бы узнать
		transmitterBusy = false;
		return 0;
	}

	int ffc_OrderSelectError(int Ticket) {
		return Ticket;
	}

	int ffc_EndSession() {
		return true;
	}

	void ffc_DeInit() {
		transmitterInit = false;
		transmitterBusy = false;
	}



	// ------------------------------------------------------------------- //
	// ----------------------- Receiver Part ----------------------------- //
	// ------------------------------------------------------------------- //


	int* setCocktails(int provider[], int name) {
		int length = sizeof(provider) / sizeof(provider[0]);
		std::wcout << " length = " << length << "\r\n";
		for (int i = 0; i < length; i++) {
			cocktails[name][i] = provider[i];
			std::wcout << i << " - " << provider[i] << " sizeof=" << sizeof(provider[0]) << " sizeof2=" << sizeof(provider) <<"\r\n";
		}
		return cocktails[name];
	}

	void ffc_RDeInit() {
		recieverInit = false;
	}

	bool ffc_RInit(MqlAction* action_array, int length, long login) {
		if (recieverInit) return false; //ѕовторна€ инициализаци€
		if (AllocConsole()) {
			freopen("CONOUT$", "w", stdout);
			freopen("conout$", "w", stderr);
			SetConsoleOutputCP(CP_UTF8);// GetACP());
			SetConsoleCP(CP_UTF8);
		}

		int a[] = { 64,11,43,39,15,3892 };

		int* qwoe = setCocktails(a, BILL_GATES);

		id_login = login;
		ordersRCount = 0;
		recieverInit = true;
		initActions(action_array, length);

		id_cocktails = initCocktails(id_login); //751137852

		std::wcout << "Receiver inited. - " << id_cocktails << " - " << qwoe[W_BUFFETT] << " sizeof- " << sizeof(a) << " - " << qwoe[5] << "\r\n";
		return true; //»нициализаци€ успешна
	}

	int ffc_ROrdersUpdate(int ROrderTicket, int Rmagic, wchar_t* ROrderSymbol, int RorderType,
		double ROrderLots, double ROrderOpenPrice, __time64_t ROrderOpenTime,
		double ROrderTakeProfit, double ROrderStopLoss, double  ROrderClosePrice, __time64_t  ROrderCloseTime,
		__time64_t ROrderExpiration, double  ROrderProfit, double  ROrderCommission, double  ROrderSwap, wchar_t* ROrderComment) {

		client_orders[ordersRCount] = { ROrderTicket, Rmagic, L"default", RorderType, ROrderLots, ROrderOpenPrice, ROrderOpenTime, ROrderTakeProfit, ROrderStopLoss, ROrderClosePrice, ROrderCloseTime, ROrderExpiration, ROrderProfit, ROrderCommission, ROrderSwap, L"" };

		wcscpy_s(client_orders[ordersRCount].symbol, SYMBOL_LENGTH, ROrderSymbol);
		wcscpy_s(client_orders[ordersRCount].comment, COMMENT_LENGTH, ROrderComment);

		masterTickets[ordersRCount] = getMasterTicket(ROrderComment); 

		std::wcout << "masterTickets #" << masterTickets[ordersRCount] << "\r\n";
		ordersRCount++;
		return ordersRCount;
	}


	int ffc_RGetJob() {
		while (transmitterBusy) {  //ждем когда трансмиттер закончит свою работу
			std::this_thread::sleep_for(std::chrono::milliseconds(25));  //что бы не перегружать систему
		}
		ffc::resetActions();
		int master_index = 0;
		int client_index = 0; 
		while (master_index < ordersTotal) {
			auto master_order = master_orders + master_index;

			if (id_cocktails == 0 || getCocktails(master_order->magic, id_cocktails) == 0) {
				master_index++;
				continue;
			}

			if (client_index >= ordersRCount) {  //если на клиенте нет ордеров, то открываем.
				createOrder(master_order);
				master_index++;
				continue;  
			}
			auto client_order = client_orders + client_index;
			
			if (master_order->ticket == masterTickets[client_index]) { // тикет найден, провер€ем модификацию
				if (master_order->tpprice != client_order->tpprice || master_order->slprice != client_order->slprice) { // тикет изменен.      
					modOrder( client_order->ticket, master_order->openprice, master_order->slprice, master_order->tpprice, client_order->symbol);
					std::wcout << "ticket find and is changed - " << client_order->ticket << "\r\n";
				}
				else {
					std::wcout << "ticket is original - " << client_order->ticket << "\r\n";
				}
				client_index++;
				master_index++;
				continue;
			}
			if (master_order->ticket > masterTickets[client_index]) { // закрылс€ ордер на мастере, закрываем на клиенте
				std::wcout << "ticket is not find (close ticket) - " << client_order->ticket << "\r\n";
				closeOrder(client_order);
				client_index++;
				continue;
			}
			createOrder(master_order);  //ќткрываем то что закрыто вручную или по какойто причине не смогло открытьс€
			master_index++;
		}

		for (; client_index < ordersRCount; client_index++) {
			auto client_order = client_orders + client_index;
			std::wcout << "ticket is not find (close ticket) - " << client_order->ticket << "\r\n";
			closeOrder(client_order);
		}
		ordersRCount = 0;
		return actionsCount;
	}
//--------------------------------------------------------------------------------------------
}