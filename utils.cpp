#include "string.h"
#include "utils.h"



int ffc::getMagic(wchar_t* comment) {
	wchar_t* pwc;
	pwc = wcstok(comment, L"_");
	pwc = wcstok(NULL, L"_");
	pwc = wcstok(NULL, L"_");
	if (pwc == NULL) return 0;
	//std::wcout << "getMagic " << comment << "/" << _wtoi(pwc) << "\r\n";
	return _wtoi(pwc);
}

int ffc::getMasterTicket(wchar_t* comment) {
	wchar_t* pwc;
	pwc = wcstok(comment, L"_");
	pwc = wcstok(NULL, L"_");
	if (pwc == NULL) return 0;
	//std::wcout << "getMagic " << comment << "/" << _wtoi(pwc) << "\r\n";
	return _wtoi(pwc);
}

///Копируем строку с++ в MQLSting (на стороне MQL больше ничего делать не надо)
void ffc::writeMqlString(MqlString dest, wchar_t* source) {
	int len = min(wcslen(source), dest.size - 1);  //Определяем длину строки (небольше распределенного буфера)
	wcscpy_s(dest.buffer, len + 1, source);  //Копируем включая терминирующий ноль
	*(((int*)dest.buffer) - 1) = len;  // Записываем длину строки (хак, может измениться в будующих версиях терминала)
}


//cюда нужно получить инфу об уже имеющихся счетах и коктейлях
int ffc::initCocktails(long acc_number) {
	// запрос на сервер, к какому коктейлю привязан аккаунт, пока забил вручную
	//if (acc_number == 1732282) {
		return TIM_COOK;
	//}
	return false;
}


bool ffc::getCocktails(int provider, int name) {
	switch (name) {
		case TIM_COOK:
			if (provider == 1593142 || provider == 1627564 || provider == 1346753) return true;
			break;
		case BILL_GATES:
			if (provider == 500) return true;
			break;
		case V_PUTIN:
			if (provider == 1593142) return true;
			break;
	}
	return false;
}



bool ffc::showCocktails(int name) {

	return false;
}