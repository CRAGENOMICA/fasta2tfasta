/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CDataSequenceIndex.h
 * Author: jjene
 *
 * Created on 21 de noviembre de 2016, 11:53
 */

#ifndef CDATASEQUENCEINDEX_H
#define CDATASEQUENCEINDEX_H

#include <cstdint>

struct CDataSequenceIndex {
 private:
  int64_t first_base_position_;
  int64_t last_base_position_;
  int64_t last_index_;

 public:
  CDataSequenceIndex() { 
    first_base_position_ = 0;
    last_base_position_  = 0;
    last_index_          = 0;
  }
  
  CDataSequenceIndex(int64_t first_base_position,
                     int64_t last_base_position,
                     int64_t last_index) {
    first_base_position_ = first_base_position;
    last_base_position_  = last_base_position;
    last_index_          = last_index;
  }
  
  CDataSequenceIndex(const CDataSequenceIndex & other) {
    first_base_position_ = other.first_base_position();
    last_base_position_  = other.last_base_position();
    last_index_          = other.last_index();
  }
  
  inline int64_t first_base_position(void) const {
    return first_base_position_;
  }
  inline void set_first_base_position(const int64_t & first_base_position) {
    first_base_position_ = first_base_position;
  }
  
  inline int64_t last_base_position(void) const {
    return last_base_position_;
  }
  inline void set_last_base_position(const int64_t & last_base_position) {
    last_base_position_ = last_base_position;
  }
  
  inline int64_t last_index(void) const {
    return last_index_;
  }
  inline void set_last_index(const int64_t & last_index) {
    last_index_ = last_index;
  }
  inline void inc_last_index(void) {
    last_index_++;
  }
};


#endif /* CDATASEQUENCEINDEX_H */

