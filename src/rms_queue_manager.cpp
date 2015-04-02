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
  //add user to queue when someone new visits the website
  ros::Subscriber enqueue_sub = n.subscribe("rms_enqueue", 1000, &RMS_Queue_Manager::on_enqueue, this);
  //remove user from queue if they're out of time or if they leave
  ros::Subscriber dequeue_sub = n.subscribe("rms_dequeue", 1000, &RMS_Queue_Manager::on_dequeue, this);

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
        queue_.pop_front(); //bye bye!

        
      }
      
      //stop counting if the queue is now empty
      if (queue_.empty()){
        countdown_ = RMS_Queue_Manager::COUNTS_PER_TRIAL;
        run_countdown = false; 
      }

      countdown_--;
    }

    ros::spinOnce();
    r.sleep();
  }
}

void RMS_Queue_Manager::on_dequeue(const std_msgs::Int32::ConstPtr &msg)
{
  //get the user Id from the message
  int user_id = msg->data;

  //iterate over the queue, and erase the user that mathces the user_id in the message
  std::deque<int>::iterator it = queue_.begin();
  while (it != queue_.end())
  {
    if (user_id == *(it))
    {
      ROS_INFO("removing user %i", user_id);
      queue_.erase(it);
      //rest time for the next user! (probably not smart because race condition, but it shouldn't matter)
      countdown_ = RMS_Queue_Manager::COUNTS_PER_TRIAL;
      return;
    }
    it++;
  }

}

void RMS_Queue_Manager::on_enqueue(const std_msgs::Int32::ConstPtr &msg)
{
  //get the user Id from the message
  int user_id = msg->data;

  //return without adding if the user id already exists
  std::deque<int>::iterator it = queue_.begin();
  while (it != queue_.end())
  {
    if (user_id == *it)
    {
      return;
    }
    it++;
  }

  //add that id to the back of the deque
  ROS_INFO("adding user %i", user_id);
  queue_.push_back(user_id);

  //start countin down
  run_countdown = true;
}