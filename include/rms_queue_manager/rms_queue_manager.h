//
// Created by pdmitrano on 4/1/15.
//

#ifndef _PROJECT_RMS_QUEUE_MANAGER_H_
#define _PROJECT_RMS_QUEUE_MANAGER_H_

#include "ros/ros.h"
#include <rms_queue_manager/RMSQueue.h> //custom message file with array of UserStatus
#include <rms_queue_manager/UserStatus.h> //custom message file with user_id and wait_time
#include <rms_queue_manager/UpdateQueue.h>
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/Int32.h"
#include <deque>

class RMS_Queue_Manager
{
public:
  /**
  * \brief The frequency of the loop in (1/seconds). This is how often we publish the queue
  */
  static const int LOOP_RATE = 1;

  /**
  * \brief default length of trial is 600 seconds. This will always be overwritten if the enqueue
  */
  static const int DEFAULT_TRIAL = 600;

  /**
  * \brief Constructor
  *	This starts the server, waits for param server and create clickable parking spots
  */
  RMS_Queue_Manager();

private:

  /**
  * the queue to hold the user_ids and wait times in order.
  */
  std::deque<std::pair<int, int> > queue_;

  /**
  * keeps track of the time left for the active user
  */
  int countdown_;

  /**
  * run countdown if there is someone in the queue
  */
  bool run_countdown;

  /**
  * \brief Add or remove a user to the deque
  */
  bool on_update_queue(rms_queue_manager::UpdateQueue::Request &req,
                       rms_queue_manager::UpdateQueue::Response &res);

};

#endif //_PROJECT_RMS_QUEUE_MANAGER_H_
