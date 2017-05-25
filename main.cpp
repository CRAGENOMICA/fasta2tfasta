/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:    main.cpp
 * Author:  Joan Jené
 * Version: 1.2
 * Created: 21 de noviembre de 2016, 11:42
 * History: - 21/11/2016 Created.
 *          - 24/
 */

/*
    Properties > Build > C++ Compiler > Additional Options
        -std=c++11
    Properties > Build > Linker > Additional Options
        -lboost_iostreams
 */
using namespace std;

#include <cstdlib>
#include <string>
#include <list>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <unistd.h> //<----- Para hacer un truncate del memory mapped file de escritura

#include "CProgress.h"
#include "CFastaInfo.h"
#include "CTFastaInfo.h"
#include "CMemoryMappedFile.h"
#include "CDataSequenceIndex.h"

std::string GetFirstColumnIDString(const std::string & chrom, int64_t num, int digits) {
  std::string ret = std::to_string((long long int)num);
  
  while (ret.length() < digits) {
    ret = "0" + ret;
  }

  ret = chrom + ":" + ret;
  
  return ret;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << APP_INFO << std::endl
              << "syntax:" << std::endl
              << "  fasta2tfasta file.fas chrom_name" << std::endl;
  } else {
    CProgress progress;

    // The app uses the input file name for naming the output file name. (It 
    // adds ".tfa" to the input file name).
    std::string input_file_name = argv[1];
    std::string output_file_name = input_file_name + ".tfa";
    std::string chrom = argv[2];
  
    // ===========================================================================
    // Open the fasta file as a memory mapped file and get its information.
    // number of records, maximum number of bases per record, ...
    // It creates a "fai" file with the information.
    // ===========================================================================
    CFastaInfo fasta_info;
        
    if (fasta_info.GetFileInformation(input_file_name)) {
  
      // =========================================================================
      // Let's go to create the TFasta memory mapped file with the input file 
      // information.
      // =========================================================================

      CTFastaInfo tfa_info;
      if (tfa_info.OpenForWrite(output_file_name, &fasta_info, chrom)) {
        // Get a pointer to the start of the output tfasta file.
        char *tfasta = tfa_info.GetFirstReservedPosition();

             
        // Copy the application information string at the first row of the tfasta:
        memcpy(tfasta,
               tfa_info.app_info().c_str(),
               tfa_info.app_info().length());

        
        // Copy the descriptions string at the second row of the tfasta:
        memcpy(tfasta + tfa_info.app_info().length(),
               fasta_info.descriptions().c_str(),
               fasta_info.descriptions().length());

        // Then, copy the header title string to the third of the tfasta:
        memcpy(tfasta + tfa_info.app_info().length() + fasta_info.descriptions().length(),
               tfa_info.header_title().c_str(),
               tfa_info.header_title().length());

        // =======================================================================
        // Loop for creating transposing the fasta data to the t-fasta
        // The t-fasta will be accessed sequentially from the first position to
        // the last. And for every position we will find the base in the fasta
        // file.
        // =======================================================================
        
        
        // Here, we get the first line number as an string of a fixed number of digits
        int64_t current_data_line_number = 1;
        std::string data_line_number = GetFirstColumnIDString(chrom,
                                                       current_data_line_number,
                                                       tfa_info.id_digits());
                
       
        // Pointer to the input Fasta file
        std::list<CDataSequenceIndex *>::iterator it = 
          fasta_info.sequences_index()->begin();
        const char *fasta = NULL;
        
        
        // Get a pointer to the start position of the output T-Fasta file:
        char *start_position = tfa_info.GetFirstReservedPosition() + 
                               tfa_info.app_info().length() +
                               fasta_info.descriptions().length() +
                               tfa_info.header_title().length();
        
        // This variable has the number of output T-Fasta number of columns
        // For example:
        //   chrom1:000001\tTAGGCGA.............AGCCCTT
        // 
        int64_t num_columns = data_line_number.length() /*tfa_info.id_digits()*/ + 1 + fasta_info.GetNumSequences();
        
        // This variable has the current column number of the output T-Fasta file
        int64_t current_column_number = 0;

        
        
        // Update the progress bar information
        progress.set_task("Fasta to T-Fasta");
        progress.set_total(tfa_info.GetLastReservedPosition() - start_position);
        progress.Start();

        
        
        // Start the loop moving the tfasta pointer to the start of the data 
        // (after descriptions rows).
        // The T-Fasta file will be loop sequentially from the first byte to the last one:
        for(tfasta = start_position; tfasta < tfa_info.GetLastReservedPosition(); tfasta++) {

          if (current_column_number < data_line_number.length() /*tfa_info.id_digits()*/) {
            // ID
            // **
            *tfasta = data_line_number.at(current_column_number);
            current_column_number++;                         
          } else {
            if (current_column_number < data_line_number.length() /*tfa_info.id_digits()*/ + 1) {
              // TAB
              // ***
              *tfasta = '\t';
              current_column_number++;              
            } else {
              if (current_column_number < num_columns) {
                // SEQUENCE
                // ********

                // The current output tfasta position is not the last base of the
                // output row.

                // Get the base from the input fasta file.
                // last_index() is the internal pointer to the input fasta file. Every sequence (it) in the input file has its own pointer.
                if (((*it)->first_base_position() + (*it)->last_index()) <= 
                    ((*it)->last_base_position())) {
                  // The base can be gotten from the input file (because it exists)
                  fasta = fasta_info.GetChar((*it)->first_base_position() +
                                     (*it)->last_index());

                  // If the base is an EOL, let's pass through it and get the the
                  // valid base that has to be after the EOL (or more than one EOL).
                  while(*fasta == '\n') {
                    (*it)->inc_last_index();

                    fasta = fasta_info.GetChar((*it)->first_base_position() +
                                               (*it)->last_index());
                  }

                  // We have a valid base. So let's write it to the output file.
                  *tfasta = *fasta;

                  // Move the sequence index one char.
                  (*it)->inc_last_index();
                } else {
                  // If the base cannot be gotten (because it does not exist),
                  // a missing will be written to the output tfasta file.
                  *tfasta = 'N';
                }
                
                // A new base has been written to the output T-Fasta file, so it has a new column:
                current_column_number++;              

                // And let's get the next sequence index information.
                // (go to the next input Fasta file row)
                it++;                
              } else {  // (current_column_number == num_columns)
                
                // The end of an output row has been reached.
                // Let's write the EOL.
                *tfasta = '\n';

                // Restart the counter to the first column number.
                current_column_number = 0;

                // Get the position to the first sequence base.
                // (go to the first input Fasta file row)
                it = fasta_info.sequences_index()->begin();
                              
                // Here, we get the next line number as an string of a fixed number of digits
                current_data_line_number++;
                data_line_number = GetFirstColumnIDString(chrom,
                                                   current_data_line_number,
                                                   tfa_info.id_digits());
              }
            }
          }
          
          progress.MoveOn();
        }
        
        // Close the output tfasta file so that all data is stored on file.
        tfa_info.Close();
        
        progress.Done();
      }

      // Show the results.
      std::cout << fasta_info;
      std::cout << tfa_info;
    }
    else {
      std::cout << "File not found: '" << fasta_info.file_name() << "'..." << std::endl;

    }
  }
  
  return 0;
}
