#pragma once
#ifndef __HELPFUL_H
#define __HELPFUL_H

#include "dbapi.h"

template<typename A>
Object** combineToRow(A a) {
	return new Object * [1]{ a };
}

template<typename A, typename B>
Object** combineToRow(A a, B b) {
	return new Object * [2]{ a, b };
}

template<typename A, typename B, typename C>
Object** combineToRow(A a, B b, C c) {
	return new Object * [3]{ a, b, c };
}

template<typename A, typename B, typename C, typename D>
Object** combineToRow(A a, B b, C c, D d) {
	return new Object * [4]{ a, b, c, d };
}

template<typename A, typename B, typename C, typename D, typename E>
Object** combineToRow(A a, B b, C c, D d, E e) {
	return new Object * [5]{ a, b, c, d, e };
}

template<typename A>
FieldObject** combineToDefinition(A a) {
	return new FieldObject * [1]{ a };
}

template<typename A, typename B>
FieldObject** combineToDefinition(A a, B b) {
	return new FieldObject * [2]{ a, b };
}

template<typename A, typename B, typename C>
FieldObject** combineToDefinition(A a, B b, C c) {
	return new FieldObject * [3]{ a, b, c };
}

template<typename A, typename B, typename C, typename D>
FieldObject** combineToDefinition(A a, B b, C c, D d) {
	return new FieldObject * [4]{ a, b, c, d };
}

template<typename A, typename B, typename C, typename D, typename E>
FieldObject** combineToDefinition(A a, B b, C c, D d, E e) {
	return new FieldObject * [5]{ a, b, c, d, e };
}

#endif
