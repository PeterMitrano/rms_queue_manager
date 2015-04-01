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
{

  ros::NodeHandle n;

  //sent out queue
  ros::Publisher queue_pub = n.advertise<std_msgs::Int32MultiArray>("rms_queue",1000);
  //add user to queue when someone new visits the website
  ros::Subscriber enqueue_sub = n.subscribe("rms_enqueue", 1000, &RMS_Queue_Manager::on_enqueue, this);
  //remove user from queue if they're out of time or if they leave
  ros::Subscriber dequeue_sub = n.subscribe("rms_dequeue", 1000, &RMS_Queue_Manager::on_dequeue, this);

  //loop at the rate of LOOP_RATE, and publish queue
  ros::Rate r(RMS_Queue_Manager::LOOP_RATE);

  while (ros::ok())
  {

    std_msgs::Int32MultiArray queue_message;

    //copy the queue into the queue message

    std::deque<int>::iterator it = queue_.begin();
    while (it != queue_.end())
    {
      int user_id = *(it++);
      ROS_INFO("user %i is in the queue", user_id);
      queue_message.data.push_back(user_id);
    }

    //publish the queue message
    queue_pub.publish(queue_message);

    countdown_--;

    //when you countdown has reached 0, reset it and remove the first/current user
    if (!countdown_)
    {
      countdown_ = RMS_Queue_Manager::COUNTS_PER_TRIAL;
      queue_.pop_front(); //bye bye!
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
  while (it != queue_.end()){

    if (user_id == *(it++)){
      ROS_INFO("removing user %i",user_id);
      queue_.erase(it);
      return;
    }
  }

}

void RMS_Queue_Manager::on_enqueue(const std_msgs::Int32::ConstPtr &msg)
{
  //get the user Id from the message
  int user_id = msg->data;
  //add that id to the back of the deque
  queue_.push_back(user_id);
}