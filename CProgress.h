/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CProgress.h
 * Author: jjene
 *
 * Created on 24 de noviembre de 2016, 9:26
 */

#ifndef CPROGRESS_H
#define CPROGRESS_H

#include <string>

enum tProgressType {
    INFO,
    PERCENT,
    BAR_PERCENT,   
    BAR_X,
    STAR
};

class CProgress {
public:
    CProgress();
    CProgress(const CProgress& orig);
    virtual ~CProgress();
    
private:
    double total_;
    double current_;
    int percent_;
    std::string task_;
    tProgressType type_;
    int bar_length_;
    

public:
    void set_task(const std::string & task) { task_ = task; }
    std::string task(void) const { return task_; }
    
    void set_total(const double & total) { total_ = total; }
    double total(void) const { return total_; }

    void set_percent(const int & percent) { percent_ = percent; }
    int percent(void) const { return percent_; }
    
    void set_bar_length(const int & bar_length) { bar_length_ = bar_length; }
    int bar_length(void) const { return bar_length_; }
    
    void set_type(const tProgressType & type) { type_ = type; }
    tProgressType type(void) const { return type_; }
    
private:
    void set_current(const double & current) { current_ = current; }
    double current(void) const { return current_; }

    void Show(void);
    
public:
    void MoveOn(void);
    void Start(void);
    void Done(void);   
};

#endif /* CPROGRESS_H */

