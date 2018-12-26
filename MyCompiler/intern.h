#include <set>
#include <string>

#pragma once

using std::string;

struct symbol {
    std::set<string>::iterator sym_p;

    explicit symbol(const string& n);

    const string& get_string() const { return *sym_p;}

    bool operator==(const symbol& rhs) const { return sym_p == rhs.sym_p; }

    bool operator<(const symbol& rhs) const { return *sym_p < *rhs.sym_p; }
};

