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
	  if (!checkSymbol(action.symbol)) return; //если нет данных по символу - нечего делать дальше (короткая цепочка выполнения)

	  action.openprice = normPrice(action.openprice); //Нормализуем цену, так как точность котирования у клиента может отличаться от поставщика

	  if (action.type < 2 && ((action.type)? mpo[action.type] < action.openprice : action.openprice > mpo[action.type])) return;  //Если не отложка и цена плохая, ничего не делаем

	  /*
	  open = false; // проверяем цену
   	if (action.type==OP_BUY) {
   	   if (normPrice(mpo[action.type], action.symbol) <= action.openprice) open = true;   <---- цен mpo до вызова checkSymbol - нет!!!
   	} else if (action.type==OP_SELL) {
   	   if (normPrice(mpo[action.type], action.symbol) >= action.openprice) open = true;
   	}
		if (!checkSymbol(action.symbol) || !open) return ;
		*/
		int ticket = OrderSend(
			action.symbol,
			action.type, 
			action.lots, 
			mpo[action.type % 2],   //нет смысла нормализовывать цену полученную от терминала, она и так нормализована
			0,
			0,   //тейкпрофит и стоплосс сразу не можем выставить на eсn счетах, выдаст ошибку
			0,
			action.comment,    // а StringConcatenate зачем?
			action.magic,0, clrRed
		);

		if (ticket < 0) {
			int err = GetLastError();
		}
	}

   void ModifyOrder(OrderAction& action)
	{
	   if (!checkSymbol(action.symbol)) return ;
		//OrderAction a;  <--- а это зачем?
	   //Тут еще надо проверить на уровни Freeze и sl/tp
		if (!OrderModify(action.ticket, action.openprice, action.slprice, action.tpprice, 0, clrBlue)) {
			int err = GetLastError();
			if (err == 1) //Убираем часто встречающуюся ошибку - "Нет ошибки" =)
				return;

		}
	}

   void DeleteOrder(OrderAction& action)
	{
		//Тут еще надо проверить на уровни Freeze
		if (!OrderDelete(action.ticket)) {
			int err = GetLastError();
		}
	}

   void CloseOrder(OrderAction& action)
	{
		//Тут еще надо проверить на уровни Freeze
	   if (checkSymbol(action.symbol))
		if (!OrderClose(action.ticket, action.lots, normPrice(mpc[action.type], action.symbol), 0, clrGreen)) {
			Alert(GetLastError());
		}
	}

   bool IsPriceOk(OrderAction& action)
   {
	   return ((action.type) ? mpo[action.type] >= action.openprice : action.openprice <= mpo[action.type]);
   }
	