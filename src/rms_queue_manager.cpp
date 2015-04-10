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
    : countdown_(COUNTS_PER_TRIAL),
      run_countdown(false)
{

  ros::NodeHandle n;

  //sent out queue
  ros::Publisher queue_pub = n.advertise<rms_queue_manager::RMSQueue>("rms_queue", 1000);
  //kick people out
  ros::Publisher pop_front_pub = n.advertise<std_msgs::Int32>("rms_pop_front", 1000);
  //add user to queue when someone new visits the website
  ros::ServiceServer update_queue_serv = n.advertiseService("update_queue", &RMS_Queue_Manager::on_update_queue, this);

  //loop at the rate of LOOP_RATE, and publish queue
  ros::Rate r(RMS_Queue_Manager::LOOP_RATE);

  ROS_INFO("publishing queue...");

  while (ros::ok())
  {

    rms_queue_manager::RMSQueue rms_queue_message;

    //copy the queue into the queue message

    std::deque<int>::iterator it = queue_.begin();
    int position = 0;
    while (it != queue_.end())
    {
      int user_id = *(it++);

      rms_queue_manager::UserStatus user_status;
      user_status.user_id = user_id;

      //calculate wait time in seconds
      int t;
      if (position > 0)
      {
        t = LOOP_RATE * (COUNTS_PER_TRIAL * (position - 1) + countdown_);
      }
      else
      {
        t = 0;
      }


      ros::Duration wait_time(t);
      user_status.wait_time = wait_time;
      rms_queue_message.queue.push_back(user_status);

      position++;

      ROS_INFO("user %i, timer %i", user_status.user_id, t);
    }

    //publish the queue message
    queue_pub.publish(rms_queue_message);

    ROS_INFO("countdown: %i\n", countdown_);

    //only run the countdown when the queue isn't empty
    if (run_countdown)
    {
      //when you countdown has reached 0, reset it and remove the first/current user
      if (!countdown_)
      {
        countdown_ = RMS_Queue_Manager::COUNTS_PER_TRIAL;
        std_msgs::Int32 pop_front_msg;
        pop_front_msg.data = queue_.front();
        pop_front_pub.publish(pop_front_msg);
        queue_.pop_front(); //bye bye!

      }

      //stop counting if the queue is now empty
      if (queue_.empty())
      {
        countdown_ = RMS_Queue_Manager::COUNTS_PER_TRIAL;
        run_countdown = false;
      }

      countdown_--;
    }

    ros::spinOnce();
    r.sleep();
  }
}

bool RMS_Queue_Manager::on_update_queue(rms_queue_manager::UpdateQueue::Request &req,
    rms_queue_manager::UpdateQueue::Response &res)
{
  //get the user Id from the message
  int user_id = req.user_id;

  //iterate over the queue, and erase the user that mathces the user_id in the message
  std::deque<int>::iterator it = queue_.begin();
  while (it != queue_.end())
  {
    if (user_id == *(it))
    {
      if (!req.enqueue)
      {
        ROS_INFO("removing user %i", user_id);
        //when first user leaves rest time for the next user!
        if (it == queue_.begin())
        {
          countdown_ = RMS_Queue_Manager::COUNTS_PER_TRIAL;
        }
        queue_.erase(it);
      }
      return true;
    }
    it++;
  }

  if (req.enqueue){
    //add that id to the back of the deque
    ROS_INFO("adding user %i", user_id);
    queue_.push_back(user_id);

    //start counting down
    run_countdown = true;
  }
  return true;
}