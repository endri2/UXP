#ifndef TUPLE_MATCHER_H
#define TUPLE_MATCHER_H

#include "TuplePattern.h"
#include "Tuple.h"
#include <mutex>
class TuplePattern;
class Tuple;
/*
 * klasa sprawdzajaca dopasowanie krotki do wzorca
 */
class TupleMatcher {
	//wzorzec, ktorego dopasowania szuka
	enum Type{
	STRING = Tuple::TupleElement::Type::STRING,
	INT =	 Tuple::TupleElement::Type::INT,
	FLOAT =	 Tuple::TupleElement::Type::FLOAT,
	UNKNOWN =Tuple::TupleElement::Type::UNKNOWN
	};
	
	TuplePattern *pattern;
	
	Tuple *tuple;
	//deskryptory FIFO/pipe do komunikacji z API
	int tupleSendFD; //odsylanie krotki
	int infoFD[2]; //odbior informacji od API (np. o timeoucie) (wewnetrzne, w timeoutOccured
	//jest deskryptor "wysylajacy", a w match "odbierajacy")

	//wewnetrzny deskryptor FIFO/pipe do odbioru wzorcow krotek (wewnetrzne, w putPattern
	//jest deskryptor "wysylajacy", a w match "odbierajacy")
	int patternFD [2];

	bool RelationResult(int CompareResult, int patternOperator);
	
	/*Mutex to protect critical section*/
	std::mutex mtx;

public:
	TupleMatcher(int tupleSendFD);

	/*
	 * Otrzymuje krotke, ktora nastepnie porownuje ze wzorcem jezeli istnieje
	 * (wczesniej wykonuje czasowe zatrzymanie sie na odbiorze wzorca jezeli go nie ma)
	 * a nastepnie probuje dopasowac wzorzec do otrzymanej w parametrze krotki.
	 * Sprawdzany jest tez deskryptor informacji, czy nie zaszedl timeout w API co powoduje
	 * usuniecie oczekiwania na okreslony wzorzec.
	 * Jezeli dopasowanie jest udane zwraca krotke poprzez deskryptor tupleSendFD i usuwa
	 * informacje o tym ze oczekuje na na jakis wzorzec
	 *
	 * binaryTuple - krotka w binarnej postaci
	 *
	 * Wartosc zwracana:
	 * -1 - brak dopasowania
	 * 0 - dopasowanie udane, operacja input
	 * 1 - dopasowanie udane, operacja read
	 */
	int match(unsigned char *binaryTuple);

	//Ustawia wzorzec do szukania na ten z parametru
	void putPattern(TuplePattern *pattern);

	//informuje o zajsciu timeoutu
	void timeoutOccured(void);

	/*tylko dla testow publiczne*/

	bool CheckString(unsigned char *binaryString, std::string stringPattern, int relOp);
	bool CheckInteger(unsigned char *binaryInt, int intPattern, int relOp);
	bool CheckFloat(unsigned char *binaryFloat, int floatPattern, int relOp);
	int CompareStrings(std::string newString, std::string pattern);
	int CompareIntegers(int newInteger, int pattern);
	int CompareFloats(float newInteger, float pattern);

};

#endif
