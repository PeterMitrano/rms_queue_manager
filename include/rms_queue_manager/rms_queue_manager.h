//
// Created by pdmitrano on 4/1/15.
//

#ifndef _PROJECT_RMS_QUEUE_MANAGER_H_
#define _PROJECT_RMS_QUEUE_MANAGER_H_

#include "ros/ros.h"
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/Int32.h"
#include <deque>

class RMS_Queue_Manager {
public:
  /**
  * \brief Constructor
  * This calls the constructors for the publishers and subscribers
  */
  static const int LOOP_RATE = 50;
  std::deque<int> queue_;

  /**
  * \brief Constructor
  *	This starts the server, waits for param server and create clickable parking spots
  */
  RMS_Queue_Manager();

private:

  int countdown_;

  void reset_countdown();

  void decrement_countdown();

  void on_dequeue(const std_msgs::Int32::ConstPtr& msg);

  void on_enqueue(const std_msgs::Int32::ConstPtr& msg);

};

#endif //_PROJECT_RMS_QUEUE_MANAGER_H_
