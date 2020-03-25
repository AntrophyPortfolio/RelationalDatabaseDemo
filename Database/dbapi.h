#pragma once

#ifdef DLL_SPEC_EXPORT
#define DLL_SPEC __declspec(dllexport)

#else
#define DLL_SPEC __declspec(dllimport)
#endif

#include<functional>
#include<string>
#include"exceptions.h"
#include<ostream>
#include<vector>
// Typ datoveho pole
enum struct FieldType {
	Integer,
	Double,
	String,
	Field
};

// Polymorfni datovy objekt (reprezentuje jednu datovou hodnotu v tabulce)
// Rozhrani vyhovuje zakladnim typum int, double, string; pro typ "field" je rozhrani rozsireno
class DLL_SPEC Object {
public:
	Object() {};
	virtual ~Object() {};

	// Gettery a settery podle typu
	// Jejich funkce je definovana jen v pripade, ze aktualni objekt je odpovidajiciho typu
	// Automaticke konverze v zakladnim API nejsou vyzadovany

	virtual std::string getString() const { throw EXCEPTIONS_H::NotImplementedException(); };
	virtual void setString(std::string value) { throw EXCEPTIONS_H::NotImplementedException(); };

	virtual int getInt() const {throw EXCEPTIONS_H::NotImplementedException();};
	virtual void setInt(int value) { throw EXCEPTIONS_H::NotImplementedException(); };

	virtual double getDouble() const { throw EXCEPTIONS_H::NotImplementedException(); };
	virtual void setDouble(double value) { throw EXCEPTIONS_H::NotImplementedException(); };

	// Vraci true, pokud aktualni objekt predstavuje dany typ
	virtual bool isType(FieldType type) const { throw EXCEPTIONS_H::NotImplementedException(); };
};

// Objekt popisujici sloupecek "field"
class DLL_SPEC FieldObject : public Object {
public:
	~FieldObject();
	FieldObject() {}
	FieldObject(std::string name, FieldType type) :_name(name), _type(type) {}

	virtual bool isType(FieldType type) const override;

	// Nazev sloupecku
	std::string getName() const { return _name; }
	// Typ sloupecku
	FieldType getType() const { return _type; }

private:
	std::string _name;
	FieldType _type;
};
std::ostream& operator<<(std::ostream& os, const FieldType& dt); // TODO Kam to dát? Nebo to mám nechat mimo třídy?
// --------------------------------------------------------
// Rozhrani definujici podminku - pouze pro bonusove metody
class DLL_SPEC Condition {
	virtual ~Condition() { }
	virtual bool matches(int fieldCount, FieldObject** fields, Object** row) const = 0;
};

// Rozhrani iteratoru (select)
class DLL_SPEC Iterator {
public:
	virtual ~Iterator() {};
	// Posun na dalsi radek (vraci true, pokud je iterator platny; logika podle Java Iterator)
	virtual bool moveNext() = 0;
	// Vraci pole Object* obsahujici data radku
	virtual Object** getRow() = 0;
	// Vraci interni rowId aktualniho radku
	virtual int getRowId() = 0;
	// Uzavre iterator (dealokuje pametove prostredky)
	virtual void close() = 0;
};

// --------------------------------------------------------
// Tabulka
class DLL_SPEC Table {
public:

	~Table();
	// Vlozeni noveho radku do tabulky (pole Object* (pro jednotlive hodnoty sloupecku))
	void insert(Object** row);
	// Smazani vybraneho radku z tabulky
	void remove(int rowid);

	// Select - vytvori iterator k prochazeni tabulky
	Iterator* select();

	// Commit - prenese zmeny z pameti do datovych souboru
	void commit();

	//// Uzavre tabulku (dealokuje pametove prostredky)
	void close();
	// Vraci pocet radku v tabulce
	int getRowCount() const { return _rowCount; };
	void setRowCount(int numberOfRows) { _rowCount = numberOfRows; };

	// Vraci pole FieldObject* popisujici sloupecky tabulky
	FieldObject** getFields() const;

	void setFields(FieldObject** fields) { _fields = fields; }

	// Vraci pocet sloupecku
	int getFieldCount() const { return _fieldCount; };
	void setFieldCount(int fieldCount) { _fieldCount = fieldCount; }

	void setName(std::string name) { _nameOfTable = name; };

	Object*** getDataValues() const { return _dataValues; }
	void setDataValues(Object*** dataValues) { _dataValues = dataValues; };

	class TableIterator : public Iterator {

	public:
		TableIterator(Table* currentTable);
		// Posun na dalsi radek (vraci true, pokud je iterator platny; logika podle Java Iterator)
		bool moveNext() override;
		// Vraci pole Object* obsahujici data radku
		Object** getRow() override;
		// Vraci interni rowId aktualniho radku
		int getRowId() override;
		// Uzavre iterator (dealokuje pametove prostredky)
		void close() override;
	private:
		int _currentIndex;
		Table* _currentTable;
	};

	// ============== Bonusove metody: ================
		// Select s podminkou
	Iterator* select(Condition* condition) { throw 0; }
	// Nalezeni rowId s podminkou
	int findRowId(Condition* condition) { throw 0; }
	// Update - aktualizuje radky vyhovujici podmince, aktualizaci provadi funkce "callback"
	// callback na vstupu obdrzi data radku a vraci data
	void update(Condition* condition, std::function<void(Object**)> callback) { throw 0; }
private:
	std::string _nameOfTable;
	int _fieldCount;
	int _rowCount;
	FieldObject** _fields;
	// rows with data
	Object*** _dataValues;
	Iterator* _iterator;
};

// Databaze
class DLL_SPEC Db {
public:
	// Otevre databazi
	static Db* open(std::string database);
	// Uzavre databazi (dealokuje pametove prostredky)
	void close();
	// Vytvori novou tabulku
	Table* createTable(std::string name, int fieldsCount, FieldObject** fields);
	// Otevre existujici tabulku
	Table* openTable(std::string name);
	// Otevre tabulku (pokud neexistuje, vytvori automaticky novou)
	Table* openOrCreateTable(std::string name, int fieldsCount, FieldObject** fields);

	// Alokuje objekt "int"
	static Object* Int(int value);
	// Alokuje objekt "double"
	static Object* Double(double value);
	// Alokuje objekt "string"
	static Object* String(std::string value);
	// Alokuje objekt "field"
	static FieldObject* Field(std::string name, FieldType type);
private:
	std::string _nameOfDatabase;
	std::vector<Table*> _tables;
 };

class DLL_SPEC IntObject : public Object {
public:
	~IntObject();
	IntObject() : _value(0) {}
	IntObject(int v) : _value(v) {}
	void setInt(int value) override { _value = value; };
	int getInt() const override { return _value; };
	bool isType(FieldType type) const override;
private:
	int _value;
};

class DLL_SPEC DoubleObject : public Object {
public:
	~DoubleObject();
	DoubleObject() : _value(0.0) {}
	DoubleObject(double v) : _value(v) {}
	void setDouble(double value) override { _value = value; };
	double getDouble() const override { return _value; };
	bool isType(FieldType type) const override;

private:
	double _value;
};
class DLL_SPEC StringObject : public Object {
public:
	~StringObject();
	StringObject() : _value("") {}
	StringObject(std::string v) : _value(v) {}
	void setString(std::string value) override { _value = value; };
	std::string getString() const override { return _value; };
	bool isType(FieldType type) const override;

private:
	std::string _value;
};


