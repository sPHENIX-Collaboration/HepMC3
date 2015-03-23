// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

#include "HepMC/GenWeights.h"

namespace HepMC {



  namespace {
    std::string _default_name(size_t n) {
      std::ostringstream name;
      name << n;
      return name.str();
    }
  }


  GenWeights::GenWeights(const std::vector<double>& wgts )
    : m_weights(wgts), m_names()
  {
    for (size_t i = 0; i < m_weights.size(); ++i)
      m_names.push_back(_default_name(i));
  }


  size_t GenWeights::index(const std::string& key) const {
    if (!has_key(key)) return size(); //< Invalid, cf. end()
    return std::distance(m_names.begin(), std::find(m_names.begin(), m_names.end(), key));
  }


  void GenWeights::push_back(const std::string& key, double wgt) {
    m_names.push_back(key.empty() ? _default_name(m_names.size()) : key);
    m_weights.push_back(wgt);
  }


  void GenWeights::set(size_t n, const std::string& key, double wgt) {
    // Pad the arrays to contain index n if necessary
    if (n > size()) {
      for (size_t i = 0; i < n+1-size(); ++i) push_back(0);
    }
    // Set the key and value for index n
    if (!key.empty()) m_names[n] = key;
    m_weights[n] = wgt;
  }


  double GenWeights::get(size_t n, double def) const {
    // if (n < 0 || n >= size()) return def;
    if (n >= size()) return def;
    return m_weights[n];
  }

  double GenWeights::get(const std::string& key, double def) const {
    if (!has_key(key)) return def;
    return get(index(key), def);
  }


  double& GenWeights::operator[](const std::string& key) {
    if (!has_key(key)) push_back(key, 0);
    return m_weights[index(key)];
  }

  const double& GenWeights::operator[]( const std::string& key) const {
    if (!has_key(key)) throw std::out_of_range("const GenWeights::operator[] ERROR: key '"+key+"' not found in GenWeights");
    return m_weights[index(key)];
  }


  bool GenWeights::operator == (const GenWeights& other) const {
    if (size() != other.size()) return false;
    if (m_names != other.m_names) return false;
    if (m_weights != other.m_weights) return false;
    return true;
  }


  void GenWeights::print(std::ostream& ostr) const {
    for (size_t i = 0; i < size(); ++i)
      ostr << "(" << key(i) << "," << get(i) << ") ";
    ostr << std::endl;
  }

  void GenWeights::write(std::ostream& ostr) const {
    for (size_t i = 0; i < size(); ++i)
      ostr << "Weight " << std::setw(4) << i << " with name " << std::setw(10) << key(i) << " is " << get(i) << std::endl;
  }


} // HepMC