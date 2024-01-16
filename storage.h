/**
 * Mocked storage table for storing information
 * DON'T modify this file
 */

#pragma once

#include <string>
#include <vector>

class Tuple {
 public:
  Tuple(){};
  Tuple(int id_val, int value1_val, std::string value2_val)
      : id(id_val), val1(value1_val), val2(value2_val) {}
  int id;  // primary key
  int val1;
  std::string val2;
};

class Table {
 public:
  Table(){};

  std::vector<Tuple>::iterator Begin() { return data.begin(); }
  std::vector<Tuple>::iterator End() { return data.end(); }

  bool insert(Tuple tuple) {
    data.emplace_back(tuple);
    return true;
  }

  bool insert(int id, int val1, std::string val2) {
    data.emplace_back(Tuple(id, val1, val2));
    return true;
  }

 private:
  std::vector<Tuple> data;
  int tid;
};
