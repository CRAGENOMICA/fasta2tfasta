/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CFastaInfo.h
 * Author: jjene
 *
 * Created on 21 de noviembre de 2016, 11:46
 */

#ifndef CFASTAINFO_H
#define CFASTAINFO_H

#include <cstdint>
#include <iostream>
#include <string>
#include <list>
#include <fstream>

#include "CMemoryMappedFile.h"
#include "CDataSequenceIndex.h"
#include "CProgress.h"

struct FaidxReg
{
    std::string name;      // Name of this reference sequence
    int64_t length;        // Total length of this reference sequence, in bases
    int64_t offset;        // Offset within the FASTA file of this sequence's first base
    int64_t line_bases;    // The number of bases on each line
    int64_t line_width;    // The number of bytes in each line, including the newline
};

class Faidx
{
public:
    std::list<FaidxReg *> info_;
    
    Faidx() {
        
    }
    
    ~Faidx() {
      for (std::list<FaidxReg *>::iterator it = info_.begin();
           it != info_.end(); it++) {
        delete (*it);
      }        
    }
    
    void Add(std::string name, int64_t length, int64_t offset, int64_t line_bases, int64_t line_width) {
        FaidxReg *reg = new FaidxReg();
        
        reg->name = name;
        reg->length = length;
        reg->offset = offset;
        reg->line_bases = line_bases;
        reg->line_width = line_width;
        
        info_.push_back(reg);
    }
    
    void Save(std::string file_name) {
        std::ofstream faidx_file;
        faidx_file.open (file_name + ".fai");
        for (std::list<FaidxReg *>::iterator it = info_.begin();
             it != info_.end(); it++) {
            faidx_file << (*it)->name << "\t"
                      << (*it)->length << "\t"
                      << (*it)->offset << "\t"
                      << (*it)->line_bases << "\t"
                      << (*it)->line_width << "\n";
        }
        faidx_file.close();
 
    }
    
    void Print(void) {
      for (std::list<FaidxReg *>::iterator it = info_.begin();
           it != info_.end(); it++) {
          std::cout << (*it)->name << "\t"
                    << (*it)->length << "\t"
                    << (*it)->offset << "\t"
                    << (*it)->line_bases << "\t"
                    << (*it)->line_width << "\n";
      }          
    }
};

class CFastaInfo : public CMemoryMappedFile {
 private:
  // std::string file_name_;          // Fasta file name with its path.
  int64_t   bases_;              // Maximum number of bases per record.
  std::string descriptions_;       // All descriptions. ">desc1 >desc2 >desc3 \n"
  bool        same_sizes_;         // True if all records have the same number
                                   // of bases.
  std::list<CDataSequenceIndex *> *sequences_index_;
  Faidx faidx_;
  
 public:  
  CFastaInfo() : CMemoryMappedFile() {
    bases_              = 0;
    descriptions_       = "#NAMES: ";
    same_sizes_         = true;
    sequences_index_    = new std::list<CDataSequenceIndex *>();
  }
    
  ~CFastaInfo() {
    // Free allocated memory:
    if (sequences_index_ != NULL) {
      for (std::list<CDataSequenceIndex *>::iterator it = sequences_index_->begin();
           it != sequences_index_->end(); it++) {
        delete (*it);
      }
      
      delete sequences_index_;
      sequences_index_ = NULL;
    }
  }

 public:
  bool GetFileInformation(const std::string & file_name);
   
  inline int64_t bases(void) const { return bases_; }
  inline void set_bases(const int64_t & bases) { 
    bases_ = bases; 
  }
   
  inline std::string descriptions(void) const { return descriptions_; }
  inline void set_descriptions(const std::string & descriptions) { 
    descriptions_ = descriptions; 
  }
  inline void AddDescriptionCharacter(const char & ch) { 
    descriptions_ += ch; 
  }
  inline void AddSeparatorToDescriptions(void) {
    descriptions_ += " "; 
  }
  inline void CloseDescriptions(void) {
    descriptions_ += "\n"; 
  }
   
  inline bool same_sizes(void) const { return same_sizes_; }
  inline void set_same_sizes(const bool & same_sizes) { 
    same_sizes_ = same_sizes; 
  }
  
  std::list<CDataSequenceIndex *> *sequences_index(void) {
    return sequences_index_;
  }
   
  inline bool SequencesIndexEmpty(void) const {
    return sequences_index_->empty();
  }
   
  inline int64_t GetNumSequences(void) const {
    return sequences_index_->size();
  }
   
  inline void SetLastBasePositionToLastSequence(int64_t position) {
    sequences_index_->back()->set_last_base_position(position);
  }

  inline void AddNewSequenceIndex(const int64_t & first_base_position) {
    sequences_index_->push_back(
      new CDataSequenceIndex(first_base_position, 0, 0));
  }
   
  inline void UpdateCounters(int64_t *current_record_bases) {
    // =======================================================================
    // Here we do the following:
    // - Count the maximum number of bases per record.
    // - Find out if all records have the same size.
    // =======================================================================
    if (*current_record_bases > bases_) {
      if (bases_ != 0) {
        same_sizes_ = false;
      }
      bases_ = *current_record_bases;
    }
    *current_record_bases = 0;
  }
  
  
  friend std::ostream& operator<<(std::ostream & stream, const CFastaInfo & fasta) {
    stream  << std::endl
            << "Fasta file name                  : "
            << fasta.file_name_
            << std::endl
            << "Sequences                        : "
            << fasta.GetNumSequences()
            << std::endl
            << "Max number of bases per sequence : "
            << fasta.bases_
            << std::endl
            << "All sequences have the same size : "
            << (fasta.same_sizes_?"true":"false")
            << std::endl
            << "Descriptions size in bytes       : "
            << fasta.descriptions_.length()
            << std::endl
            << std::endl;
    
    return stream;
  } 
};

#endif /* CFASTAINFO_H */

