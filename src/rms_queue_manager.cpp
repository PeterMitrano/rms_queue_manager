//
// Created by pdmitrano on 4/1/15.
//

#include <rms_queue_manager/rms_queue_manager.h>

int main(int argc, char *argv[])
{
  ros::init(argc, argv, "rms_queue_manager");

  //construct a queue
  RMS_Queue_Manager rms_queue_manager;

  return EXIT_SUCCESS;
}

RMS_Queue_Manager::RMS_Queue_Manager()
    : countdown_(DEFAULT_TRIAL),
      run_countdown(false)
{

  ros::NodeHandle n;

  //sent out queue
  ros::Publisher queue_pub = n.advertise<rms_queue_manager::RMSQueue>("rms_queue", 1000);

  //add user to queue when someone new visits the website
  ros::ServiceServer update_queue_serv = n.advertiseService("update_queue", &RMS_Queue_Manager::on_update_queue, this);

  //loop at the rate of LOOP_RATE, and publish queue
  ros::Rate r(RMS_Queue_Manager::LOOP_RATE);

  ROS_INFO("publishing queue...");

  while (ros::ok())
  {

    ROS_INFO("countdown: %i\n", countdown_);

    //only run the countdown when the queue isn't empty
    if (run_countdown)
    {
      //when you countdown has reached 0, reset it and remove the first/current user
      if (!countdown_)
      {
        queue_.pop_front(); //bye bye!
        //reset count down to next users trial time
        countdown_ = queue_.begin()->second;
      }

      //stop counting if the queue is now empty
      if (queue_.empty())
      {
        run_countdown = false;
      }

      countdown_--;
    }

    rms_queue_manager::RMSQueue rms_queue_message;

    std::deque<std::pair<int, int> >::iterator it = queue_.begin();
    int position = 0;
    int wait_t;
    int t_left;

    //create queue to publish
    while (it != queue_.end())
    {
      std::pair<int, int> user = *(it++);

      rms_queue_manager::UserStatus user_status;
      user_status.user_id = user.first;

      //calculate wait time in seconds

      if (position > 0)
      {
        wait_t = countdown_ + (position - 1) * user.second;
      }
      else if (position == 0)
      {
        wait_t = -1;
        t_left = countdown_;
      }

      ros::Duration wait_time(wait_t);
      ros::Duration time_left(t_left);
      user_status.wait_time = wait_time;
      user_status.time_left = time_left;
      rms_queue_message.queue.push_back(user_status);

      position++;

      ROS_INFO("user %i", user_status.user_id);
    }

    //publish the queue message
    queue_pub.publish(rms_queue_message);

    ros::spinOnce();
    r.sleep();
  }
}

bool RMS_Queue_Manager::on_update_queue(rms_queue_manager::UpdateQueue::Request &req,
                                        rms_queue_manager::UpdateQueue::Response &res)
{
  //get the user Id from the message
  int user_id = req.user_id;
  int study_time = req.study_time;

  //iterate over the queue, and erase the user that matches the user_id in the message
  std::deque<std::pair<int, int> >::iterator it = queue_.begin();
  while (it != queue_.end())
  {
    if (user_id == (*it).first)
    {
      if (!req.enqueue)
      {
        //when first user leaves reset time for the next user!
        if (it == queue_.begin())
        {
          queue_.pop_front();
          int next_study_time = queue_.front().second;
          countdown_ = next_study_time ? next_study_time : RMS_Queue_Manager::DEFAULT_TRIAL;
        } else
        {
          queue_.erase(it);
        }
      }
      return true;
    }
    it++;
  }

  if (req.enqueue)
  {

    study_time = study_time ? study_time : RMS_Queue_Manager::DEFAULT_TRIAL;

    //add that id to the back of the deque
    queue_.push_back(std::pair<int, int>(user_id, study_time));
    countdown_ = study_time;

    //start counting down
    run_countdown = true;
  }
  return true;
}
