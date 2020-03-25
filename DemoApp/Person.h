#pragma once

#ifndef PERSON_H
#define PERSON_H

#include<dbapi.h>
#include<helpful.h>
#include"SimpleOperations.h"

class Person {
public:
	Person();
	Person(std::string name, double currentHealth, double damage, int personId);
	double damageTaken(double value);
	double damageDealt(double value);

	void setCurrentHealth(double newValue) { update("CurrentHealth", _name, newValue); };
	double getCurrentHealth() const { return select("CurrentHealth", _name); };
	double getBaseDamage() const { return select("Damage", _name); };
	bool isDead() const;
	std::string getName() const { return _name; };

private:
	std::string _name;
};
#endif // !PERSON_H

