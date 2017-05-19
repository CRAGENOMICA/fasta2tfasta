/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CTFastaInfo.cpp
 * Author: jjene
 * 
 * Created on 21 de noviembre de 2016, 11:46
 */

#include "CTFastaInfo.h"

bool CTFastaInfo::GetFileInformation(const std::string & file_name) {
  bool result = false;
  
  if (OpenForRead(file_name)) {
    // =========================================================================
    // Loop for getting the T-Fasta information
    // =========================================================================
   
    const char *end_fasta           = GetLastDataPosition();
    bool on_header_row = true;

    columns_ = 1;
    for (const char *fasta = GetFirstDataPosition();
         fasta < end_fasta;
         fasta++) {
      
      if (on_header_row) {
        if (*fasta == '\n') {
          on_header_row = false;
        }
        else {
          if ((*fasta == ' ') && (*(fasta+1) == '>')) {
            columns_++;
          }
        }
      }
      else {
        if (*fasta == '\n') {
          rows_++;
        }        
      }
    }
    
    result = true;
  }
    
  return result;
}