//+------------------------------------------------------------------+
//|                                                        Order.mq4 |
//|                                    Copyright 2016, FairForex.org |
//|                                             http://fairforex.org |
//+------------------------------------------------------------------+
#property library
#property copyright "Copyright 2016, FairForex.org"
#property link      "http://fairforex.org"
#property version   "1.00"
#property strict

bool open;

   void CreateOrder(OrderAction& action)
	{
      open = false; // проверяем цену
   	if (action.type==OP_BUY) {
   	   if (normPrice(mpo[action.type], action.symbol) <= action.openprice) open = true;
   	} else if (action.type==OP_SELL) {
   	   if (normPrice(mpo[action.type], action.symbol) >= action.openprice) open = true;
   	}
		if (!checkSymbol(action.symbol) || !open) return ;

		int ticket = OrderSend(
			action.symbol,
			action.type, 
			action.lots, 
			normPrice(mpo[action.type], action.symbol),
			0,
			action.slprice, 
			action.tpprice,
			StringConcatenate(action.comment),
			action.magic,0, clrRed
		);

		if (ticket < 0) {
			int err = GetLastError();
		}
	}

   void ModifyOrder(OrderAction& action)
	{
	   if (!checkSymbol(action.symbol)) return ;
		OrderAction a;
		if (!OrderModify(action.ticket, action.openprice, action.slprice, action.tpprice, 0, clrBlue)) {
			int err = GetLastError();
			if (err == 1) //Убираем часто встречающуюся ошибку - "Нет ошибки" =)
				return;

		}
	}

   void DeleteOrder(OrderAction& action)
	{
		if (!OrderDelete(action.ticket)) {
			int err = GetLastError();
		}
	}

   void CloseOrder(OrderAction& action)
	{
	   if (checkSymbol(action.symbol))
		if (!OrderClose(action.ticket, action.lots, normPrice(mpc[action.type], action.symbol), 0, clrGreen)) {
			Alert(GetLastError());
		}
	}
	