//
// Created by pdmitrano on 4/1/15.
//

#ifndef _PROJECT_RMS_QUEUE_MANAGER_H_
#define _PROJECT_RMS_QUEUE_MANAGER_H_

#include "ros/ros.h"
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/Int32.h"
#include <deque>

class RMS_Queue_Manager
{
public:
  /**
  * \brief The number of seconds per loop. This is how often we publish the queue
  */
  static const int LOOP_RATE = 10;
  /**
  * \brief this number of counts/loops each user gets before getting kicked out
  * total time per user is LOOP_RATE * COUNTS_PER_TRIAL
  */
  static const int COUNTS_PER_TRIAL = 3;


  /**
  * \brief Constructor
  *	This starts the server, waits for param server and create clickable parking spots
  */
  RMS_Queue_Manager();

private:

  /**
  * the queue to hold the user_ids in order of the queue
  */
  std::deque<int> queue_;

  /**
  * keeps track of the time left for the active user
  */
  int countdown_;

  /**
  * \brief Remove a user to the deque
  */
  void on_dequeue(const std_msgs::Int32::ConstPtr &msg);

  /**
  * \brief Add a user to the deque
  */
  void on_enqueue(const std_msgs::Int32::ConstPtr &msg);

};

#endif //_PROJECT_RMS_QUEUE_MANAGER_H_
