/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CTFastaInfo.h
 * Author: jjene
 *
 * Created on 21 de noviembre de 2016, 11:46
 */

#ifndef CTFASTAINFO_H
#define CTFASTAINFO_H

#include <cstdint>
#include <string>

#include "CFastaInfo.h"
#include "CMemoryMappedFile.h"

#define APP_INFO     "#fasta2tfasta 24-NOV-2016\n"
#define HEADER_TITLE "#POSITION\tGENOTYPES\n"

class CTFastaInfo : public CMemoryMappedFile
{
 private:
  int64_t   descriptions_size_;  // The whole size of descriptions.
  int64_t   columns_;            // Number of individuals.
  int64_t   rows_;               // Number of bases (in vertical). It does not
                                   // take into account the first row with
                                   // descriptions.
  std::string app_info_;           // Header title with the application information
  std::string header_title_;       // Header title with columns titles
  int         id_digits_;          // Digits of the id. For example 1000 has 4 digits.
  
 public:
  CTFastaInfo(const std::string & file_name) : CMemoryMappedFile(file_name) {
    rows_               = 0;
    columns_            = 0;
    descriptions_size_  = 0;
    app_info_           = APP_INFO;
    header_title_       = HEADER_TITLE;
    id_digits_          = 0;
  } 
  
  CTFastaInfo() : CMemoryMappedFile() {
    file_name_          = "";
    rows_               = 0;
    columns_            = 0;
    descriptions_size_  = 0;
    app_info_           = APP_INFO;
    header_title_       = HEADER_TITLE;
    id_digits_          = 0;
  }
  
 
  bool OpenForWrite(const std::string & file_name, CFastaInfo *fasta_info) {
    rows_               = fasta_info->bases();
    columns_            = fasta_info->GetNumSequences();
    descriptions_size_  = fasta_info->descriptions().length();

    std::string max_row_id = std::to_string((long long int)rows()); 
    set_id_digits(max_row_id.length());
    
    boost::intmax_t new_file_size =
                  app_info_.length()
                  + descriptions_size()  // this size has the EOL char
                  + header_title_.length()
                  + ((columns() + 2) * rows()) // +2 because of the end of line (\n) and the tabulator (\t) separator of the two columns.
                  + (id_digits() * rows());  
    
    CMemoryMappedFile::OpenForWrite(file_name, new_file_size);
    
    return is_open_;
  }
 
 public:
  bool GetFileInformation(const std::string & file_name);
  
 public:  
  inline std::string app_info(void) const { return app_info_; }
  inline void set_app_info(const std::string & app_info) { 
    app_info_ = app_info; 
  }
  
  inline std::string header_title(void) const { return header_title_; }
  inline void set_header_title(const std::string & header_title) { 
    header_title_ = header_title; 
  }
 
  inline int id_digits(void) const { return id_digits_; }
  inline void set_id_digits(const int & id_digits) { 
    id_digits_ = id_digits; 
  }
  
  inline int64_t descriptions_size(void) const { return descriptions_size_; }
  inline void set_descriptions_size(const int64_t & descriptions_size) { 
    descriptions_size_ = descriptions_size; 
  }

  inline int64_t columns(void) const { return columns_; }
  inline void set_columns(const int64_t & columns) { 
    columns_ = columns; 
  }

  inline int64_t rows(void) const { return rows_; }
  inline void set_rows(const int64_t & rows) { 
    rows_ = rows; 
  }
  
  friend std::ostream& operator<<(std::ostream & stream, const CTFastaInfo & tfasta) {
    stream  << "T-Fasta file name                : "
            << tfasta.file_name_
            << std::endl
            << "Columns                          : "
            << tfasta.columns_
            << std::endl
            << "Rows                             : "
            << tfasta.rows_
            << std::endl
            << "Descriptions size in bytes       : "
            << tfasta.descriptions_size_
            << std::endl;
    
    return stream;
  } 
};


#endif /* CTFASTAINFO_H */

