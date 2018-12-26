//
// Created by pg999w on 18-12-10.
//

#include "intern.h"

std::set<string> symtab;

symbol::symbol(const string &n) {
    auto it = symtab.insert(n).first;
    this->sym_p = it;
}
