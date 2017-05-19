/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CProgress.cpp
 * Author: jjene
 * 
 * Created on 24 de noviembre de 2016, 9:26
 */

#include "CProgress.h"

#include <iostream>
#include <iomanip> 

#undef MOVE_SLOW

#ifdef MOVE_SLOW
  // SLEEP functions:
  #include <thread>         // std::thread
  #include <chrono>         // std::chrono::seconds
  #include <mutex>          // std::mutex
#endif


CProgress::CProgress() {
  set_total(0);
  set_current(0); 
  set_percent(0);
  set_task("");
  set_type(tProgressType::PERCENT);
  set_bar_length(30);
}

CProgress::CProgress(const CProgress& orig) {
  set_total(orig.total());
  set_current(orig.current());
  set_percent(orig.percent());
  set_task(orig.task());
  set_type(orig.type());
  set_bar_length(orig.bar_length());
}

CProgress::~CProgress() {
  set_total(0);
  set_current(0); 
  set_percent(0);
  set_task("");
  set_type(tProgressType::PERCENT);
  set_bar_length(0);
}

void CProgress::Show(void) {
  int aux = 0;
  std::string bar;
  std::string bar2;
  int bar_size = bar_length();
  char box = 'X';
  static int star_pos = 0;
  star_pos++;
  
  switch(type()) {
    case tProgressType::PERCENT:
      std::cout << std::setprecision(3)
                << "\r"
                << task()
                << ": "
                << percent()
                << "%"
                << std::flush;
      break;
    case tProgressType::INFO:
      std::cout << "\r"
                << task()
                << ": "
                << current()
                << " / "
                << total()
                << std::flush;
      break;      
    case tProgressType::STAR:
      std::cout << std::setprecision(3)
                << "\r"
                << task()
                << ": ";
      if (star_pos == 0) { std::cout << "-"; }
      if (star_pos == 1) { std::cout << "\\"; }
      if (star_pos == 2) { std::cout << "|"; }
      if (star_pos == 3) { std::cout << "/"; star_pos = -1;  }

      std::cout << std::flush;
      break;
    case tProgressType::BAR_PERCENT:
      aux = percent();
      aux *= bar_size;
      aux /= 100;
      
      for (int i=0; i<aux; i++) {
         bar+=".";
      }
      aux = bar_size + 2 - bar.length() - std::to_string((long long int)percent()).length(); // +2 pq empieza por un digito y acaba en 3
      if (aux < 0) aux = 0;
      for (int i=0; i<aux; i++) {
         bar2+=" ";
      }

      std::cout << std::setprecision(3)
                << "\r"
                << task()
                << ": ["
                << bar
                << percent()
                << "%"
                << bar2
                << "]"
                << std::flush;
      
      break;
    case tProgressType::BAR_X:
      for (int i=0; i<bar_size; i++) {
        aux = percent();
        aux *= bar_size;
        aux /= 100;
        
        if (i <= aux) {
          bar+=box;
        }
        else {
          bar+=" ";
        }
      }
      std::cout << std::setprecision(3)
                << "\r"
                << task()
                << ": "
                << "["
                << bar
                << "]"
                << std::flush;  
      break;
  }


}

void CProgress::MoveOn(void) {
  set_current(current() + 1);
  
  double p = current();
  p *= 100;
  p /= total();

  int previous_percent = percent();  
  set_percent((int)p);
  
  if (previous_percent != percent()) {
    Show();
  }

#ifdef MOVE_SLOW
  std::this_thread::sleep_for (std::chrono::milliseconds(1));
#endif  
}

void CProgress::Start(void) {
  set_current(0);
  
  std::cout << std::endl;
  
  Show();
}

void CProgress::Done(void) {
  std::cout << "\r" << task() << ": done.                                                                           " << std::endl;
}

    