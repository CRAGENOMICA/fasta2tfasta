/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CFastaInfo.cpp
 * Author: jjene
 * 
 * Created on 21 de noviembre de 2016, 11:46
 */

#include "CFastaInfo.h"

#include <string>

#include <sstream>
#include <iostream>
#include <vector>

bool CFastaInfo::GetFileInformation(const std::string & file_name) {
  bool result = false;
  
  std::vector<std::string> seq_names;
  std::string seq_name;
  
  if (OpenForRead(file_name)) {
    
    // =========================================================================
    // Loop for getting the fasta information
    // =========================================================================

    int64_t fasta_pos             = 0;
    int64_t current_record_bases  = 0;
    char      previous_ch           = '\n';
    const char *end_fasta           = GetLastDataPosition();

    CProgress progress;
    progress.set_task("Indexing Fasta Progress");
    progress.set_total(GetLastDataPosition() - GetFirstDataPosition());
    progress.Start();

        
    for (const char *fasta = GetFirstDataPosition();
         fasta < end_fasta;
         fasta++, fasta_pos++) {    

      if ((*fasta == '>') && (previous_ch == '\n')) {
        // Description found.

        if (!SequencesIndexEmpty()) {          
          // This start of a description is also the end of a previous sequence.
          // Store the last base position of the current sequence:
          SetLastBasePositionToLastSequence(fasta_pos - 2);

          // Update the maximum number of bases and the same_sizes:
          UpdateCounters(&current_record_bases);
        }

        // Get description chars in a loop:
        seq_name = "";
        while (*fasta != '\n') {
          AddDescriptionCharacter(*fasta);
          if (*fasta != '>') {
            seq_name += *fasta;
          }
          fasta++;
          progress.MoveOn();
          fasta_pos++;
        }

        seq_names.push_back(seq_name);
          
        // Descriptions must be separated by a separator (space at the end):

        AddSeparatorToDescriptions();

        // As the end of the description has been reached, this is also the 
        // start of a new sequence. Let's create a new sequence index and
        // store the position of its first base (+1 for passing through the EOL).
        AddNewSequenceIndex(fasta_pos + 1);
      }

      // Count the current sequence number of bases. The objective is to 
      // get the maximum number of bases per sequence.
      // EOLs can not be taken into account. They are all passed through.
      if (*fasta != '\n') {
        current_record_bases++;
      }

      previous_ch = *fasta;
      
      progress.MoveOn();
    }

    // This is the end of the last sequence.
    // Let's store the position of its last base.
    if (!SequencesIndexEmpty()) {
      SetLastBasePositionToLastSequence(fasta_pos - 2);
    }

    // Also, let's update the fasta counters with the information of the last
    // sequence.
    UpdateCounters(&current_record_bases);

    // And finally, add the end character to the descriptions string (an EOL).
    CloseDescriptions();

    result = true;
    progress.Done();
  }
  
  int64_t seq_len = 0;
  int size_of_eol = 1;

  int i = 0;
  for (std::list<CDataSequenceIndex *>::iterator it = sequences_index_->begin();
       it != sequences_index_->end(); it++, i++) {
    seq_len = ((*it)->last_base_position() - (*it)->first_base_position()) + 1;
    faidx_.Add(seq_names.at(i), seq_len, (*it)->first_base_position(), seq_len, seq_len + size_of_eol);
  }
  
  faidx_.Save(file_name);
    
  return result;
}
