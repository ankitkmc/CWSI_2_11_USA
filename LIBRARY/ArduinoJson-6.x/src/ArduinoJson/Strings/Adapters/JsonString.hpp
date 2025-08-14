// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2023, Benoit BLANCHON
// MIT License

#pragma once

#include "../../Strings/Adapters/RamString.hpp"
#include "../../Strings/JsonString.hpp"
#include "../../Strings/StringAdapter.hpp"

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

class JsonStringAdapter : public SizedRamString {
 public:
  JsonStringAdapter(const JsonString& s)
      : SizedRamString(s.c_str(), s.size()), _linked(s.isLinked()) {}

  StringStoragePolicy::LinkOrCopy storagePolicy() const {
    StringStoragePolicy::LinkOrCopy policy = {_linked};
    return policy;
  }

 private:
  bool _linked;
};

template <>
struct StringAdapter<JsonString> {
  typedef JsonStringAdapter AdaptedString;

  static AdaptedString adapt(const JsonString& s) {
    return AdaptedString(s);
  }
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
