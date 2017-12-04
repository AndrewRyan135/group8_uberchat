//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <string.h>
#include "chat_server.h"
#include "chat_message.hpp"

using boost::asio::ip::tcp;




//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;
std::string ExecCmd(std::string cmd);
int requuid_handle();
void invalid_operation(std::string input);
class chatrooms;
class chat_participant ;

std::vector<std::string> chatroom_names;
std::vector<chatrooms> chatroom_list;
std::vector<std::string> user_list;
std::vector<int> uuid_list;
//----------------------------------------------------------------------

class chatrooms                                                               //Contains the chatrooms information and all the methods needed to proper keep the room uptodate and
{                                                                             //function properly.                                                                    
public:
  chatrooms(std::string in_name) : name(in_name) {}
  void message_backlog(std::string message) //read from the vector from the last element to the first element
  {
    if (messages.size() < 101)
    {
      messages.push_back(message);
    }
    else
    {
      messages.erase(messages.begin()+0);
      messages.push_back(message);
    }
  }
  void add_user(std::string name)
  {
      chatroom_users.push_back(name);
  }
  void remove_user(std::string name)
  {
    for (unsigned int i = 0; i < chatroom_users.size(); i++)
    {
      if (name.compare(chatroom_users[i])==0)
      {
        chatroom_users.erase(chatroom_users.begin()+(i));
      }
    }
  }
  int num_users()
  {
    return chatroom_users.size();
  }
  int msg_size()
  {
    return messages.size();
  }
  std::string get_user(int i)
  {
    return chatroom_users[i];
  }
    std::string get_name()
    {
      return name;
    }
  std::string get_msg(int index)
  {
    return messages[index];
  }
private:
  std::string name;
  std::vector<std::string> chatroom_users;
  std::vector<std::string> messages;
};

class chat_participant                                                       //Holds the information protaining to the user and also contains the fuction that will process all the commands
{                                                                            //since the information needed is stored here so it allows for less code to recieve this data.
public:
  chat_participant()
  {
    uuid = requuid_handle();
    room_name = "the lobby";
    //std::cout << "UUID " << uuid << " Chat room " << room_name << std::endl;
    message_ptr = 0;
  }
  ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
  void set_uuid(int num)
  {
    uuid = num;
  }
  int get_uuid()
  {
    return uuid;
  }
  void set_nick(std::string name)
  {
    nick = name;
    //user_list.push_back(name);
  }
  std::string get_nick()
  {
    return nick;
  }

  void ExecCmd(std::string cmd){
  if(cmd == "REQUUID"){
      std::string num = std::to_string(uuid);
      num = "REQUUID," + num;
      std::string time = timeStamp();
      std::string check_sum = std::to_string(getChecksum(time+","+"REQUUID,"+num));
      chat_message msg = convert_to(check_sum + "," + time + "," + "REQUUID," +  num);
      deliver(msg);

  }else if(cmd.find("NICK,") != std::string::npos){                          //NICK
    
      std::string name = cmd.substr(5,cmd.length()-5);
      //std::cout<<"NICK ran successfully"<<'\n';
      int num = 0;
      chat_message msg;
      nick = name;
      for (unsigned int i = 0; i < user_list.size(); i++)
      {
        if (user_list[i].compare(name)==0)
        {
          srand(time(NULL));
          num = rand()%100;
          if (nick.compare("")==0)
          {
            user_list.push_back(name + std::to_string(num));
          }
          nick = name + std::to_string(num);
        }
      }
      if (nick.compare(name+std::to_string(num))!=0)
      {
        user_list.push_back(name);
        nick = name;
      }
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
        {
          if (chatroom_list[i].get_name().compare(room_name)==0)
          {
            chatroom_list[i].add_user(nick);
          }
        }
      std::string time = timeStamp();
      std::string check_sum = std::to_string(getChecksum(time+","+"NICK,"+nick));
      std::string output = "NICK," + nick;
      msg = convert_to(check_sum + "," + time + "," + output);
      deliver(msg);

  }else if(cmd.compare("REQCHATROOM")==0){
      
      std::string time = timeStamp();
      std::string check_sum = std::to_string(getChecksum(time+","+"REQCHATROOM,"+room_name));
      chat_message ret_value = convert_to(check_sum + "," + time + "," + "REQCHATROOM," + room_name);
      deliver(ret_value);

  }else if(cmd.compare("REQCHATROOMS")==0){
    std::string ret_value;
      for (unsigned int i = 0; i < chatroom_names.size(); i++)
      {
        if (i < 1)
        {
          ret_value = chatroom_names[i] + '\n';
        }
        else
        {
          ret_value = ret_value + "      " + chatroom_names[i] + "\n";
        }
        std::string time = timeStamp();
        std::string check_sum = std::to_string(getChecksum(time+","+"REQCHATROOMS,"+ret_value));
        chat_message msg = convert_to(check_sum + "," + time + "," + "REQCHATROOMS," + ret_value);
        deliver(msg);
        
      }
  }else if(cmd.substr(0,12).compare("NAMECHATROOM")==0){
    int flag = 0;
    std::string ret_value;
      std::string cmdOption = cmd.substr(13,cmd.length()-13);
      for (unsigned int i = 0; i < chatroom_names.size(); i++)
      {
        if (chatroom_names[i].compare(cmdOption)==0)
        {
          flag = 1;
        }
      }
      if (flag == 0)
      {
        std::cout << "This is the name of the chatroom " << cmdOption << std::endl;
        chatrooms room(cmdOption);
        chatroom_list.push_back(room);
        chatroom_names.push_back(cmdOption);
        ret_value = "NAMECHATROOM " + cmdOption;
      }
      else
      {
        ret_value = "NAMECHATROOM Name already exists, please enter a new name";
      }
      chat_message msg = convert_to(ret_value);
      deliver(msg);

  }else if(cmd.substr(0,14).compare("CHANGECHATROOM")==0){
      std::string name = cmd.substr(15,cmd.length()-15);
      int flag = 0;
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      {
        std::cout << "The value of flag is " << flag <<std::endl;
        if (chatroom_list[i].get_name().compare(name)==0)
        {
          flag = 1;
        }
      }
      if (flag == 1)
      {
        for (unsigned int i = 0; i < chatroom_list.size(); i++)
        {
          std::cout << chatroom_list[i].get_name() << std::endl;
          if (chatroom_list[i].get_name().compare(room_name)==0)
          {
            for (int j = 0; j < chatroom_list[i].num_users(); j++)
            {
              if (chatroom_list[i].get_user(j).compare(nick)==0)
              {
                std::cout << chatroom_list[i].get_name() << " " << nick << std::endl;
                chatroom_list[i].remove_user(nick);
                std::cout << "After removing the user" << std::endl;
              }
            }
          }
        }
        room_name = name;
        for (unsigned int i = 0; i < chatroom_list.size(); i++)
        {
          if (room_name.compare(chatroom_list[i].get_name())==0)
          {
            std::cout << "Added " << nick << "to " << chatroom_list[i].get_name() << std::endl;
            chatroom_list[i].add_user(nick);
          }
        }
        std::cout << "I update the chatroom in the server" << std::endl;
        std::string ret = "Changed to chat room " + name;
        std::string time = timeStamp();
        std::string check_sum = std::to_string(getChecksum(time+","+"CHANGECHATROOM,"+ret));
        chat_message msg = convert_to(check_sum + "," + time + "," + "CHANGECHATROOM," + ret);
        message_ptr = 0;
        deliver(msg);
      }
      std::cout << "Changed chat rooms" << std::endl;
  }else if(cmd.substr(0,8).compare("SENDTEXT")==0){                      //SENDTXT

      std::cout<<"SENDTEXT ran successfully"<<'\n';
      std::string cmdOption = cmd.substr(9,cmd.length()-9);
      cmdOption = nick + ": " + cmdOption; 
      cmdOption = " " + cmdOption;
      std::string time_1 = timeStamp() + ",";
      if(cmdOption.find(";") != std::string::npos){
        std::cout<<"Error! Message cannot contain ';' in it"<<'\n';   
      }
     else{ 
        std::cout<<"SENDTEXT ran successfully"<<'\n';
        for (unsigned int i = 0; i < chatroom_list.size(); i++)
        {
          if (chatroom_list[i].get_name().compare(room_name)==0)
          {
            chatroom_list[i].message_backlog(time_1 + cmdOption + "\n");
          }
        }
        std::string time = timeStamp();
        std::string check_sum = std::to_string(getChecksum(time+","+"SENDTEXT,"+cmdOption));
        chat_message msg = convert_to(check_sum + "," + time + "," + "SENDTEXT," + cmdOption);
        //std::cout << cmdOption << std::endl;
        deliver(msg);
      }
  }else if(cmd.compare("REQTEXT")==0){   
      int num = 0;
      int pos = 0;
      int flag = 0;
      std::string ret_value;
      std::cout << "First loop" << std::endl;
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      {
        if (chatroom_list[i].get_name().compare(room_name)==0)
        {
          pos = i;
          num = chatroom_list[i].msg_size();
        }
      }  
      std::cout << "Second loop" << "\n" << message_ptr << "\n" << num << std::endl;                            
      for (int i = message_ptr; i < num; i++)
      {
        ret_value += chatroom_list[pos].get_msg(i);
        if (message_ptr == 0)
        {
          flag = 1;
        }
        else
        {
        flag = i;
        }
      }
      
      message_ptr = chatroom_list[pos].msg_size();
      std::string time = timeStamp();
      std::string check_sum = std::to_string(getChecksum("REQTEXT,"+ret_value));
      chat_message msg = convert_to(check_sum + "," +"REQTEXT," + ret_value);
      if (flag >= 1)
      {
        deliver(msg);
      }
      std::cout<<"REQTEXT ran successfully"<<'\n';
  }else if(cmd.compare("REQUSERS")==0){  
      std::string ret_value;  
      int num;          
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      { 
        if (chatroom_list[i].get_name().compare(room_name)==0)
        {
          num = i;
          std::cout << i << std::endl;
        }
      }                  
      for (int i = 0; i < chatroom_list[num].num_users(); i++)
      {
        ret_value += chatroom_list[num].get_user(i) + '\n';
      }
      ret_value = "REQUSERS," + ret_value;
      std::string time = timeStamp();
      std::string check_sum = std::to_string(getChecksum("REQUSERS,"+ret_value));
      chat_message msg = convert_to(check_sum + "," + time + "," + "REQUSERS," + ret_value);
      deliver(msg);
      std::cout<<"REQUSERS ran successfully"<<'\n';
  }else{
      std::cout<<"Error! Your entry does not fit the standard format."<<'\n';
      std::cout<<"Type 'Help' for a list of format and their functions"<<'\n';
  }

}
private:
  std::string room_name;
  int uuid;
  std::string nick;
  int message_ptr;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;
//std::vector<chat_participant> user_list;


//----------------------------------------------------------------------------------------------------------


class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  void start()
  {
    //room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            //room_.leave(shared_from_this());

          }
        });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          
          if (!ec)
          {
            std::string text = convert_from(read_msg_);
            std::cout << text << std::endl;
            std::string cksum = parseChecksum(text);
            std::string value;

            //int test = check_cksum(cksum, value); //return 1 if checksum not verified

            if (text.find("REQUUID") != std::string::npos)
            {
              value = nouuid_parseCmd(text);
            }
            else
            {
              value = nouuid_parseCmd(text);
            }

            switch(check_cksum(cksum, value)){
              case 0:

                self->ExecCmd(value);
                do_read_header();
                break;

              case 1: 
                //sends nothing to client
                //server recieves error message 
                //and cksum mapping in check_cksum func
                break;
            }

          }
          else
          {
            //room_.leave(shared_from_this());
            std::cout << "Leaving" << std::endl;
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            //room_.leave(shared_from_this());
            std::cout << "Leaving" << std::endl;
          }
        });
  }

  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint)
    : acceptor_(io_service, endpoint),
      socket_(io_service)
  {
    chatrooms room("the lobby");
    chatroom_list.push_back(room);
    chatroom_names.push_back("the lobby");
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::make_shared<chat_session>(std::move(socket_))->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

//----------------------------------------------------------------------

  int requuid_handle()                                                                //Provides a uuid when requested
  {
    bool val = false;
    int num = 0000;
    while (!val)
    {
      srand(time(NULL));
      num = rand()%1000000000;
      if (std::find(uuid_list.begin(), uuid_list.end(), num) != uuid_list.end())
      {
        
      }
      else
      {
        val = true;
      }
    }
    uuid_list.push_back(num);
    return num; 
  }
//--------------------------------------------------------------------------


int main(int argc, char* argv[])
{
  std::string line;
  int i = 0;
  int x = 0;
  std::ifstream infile ("server_uuid.txt");
  if (infile.is_open())
  {
    while (getline(infile,line))
    {
      std::stringstream geek(line);
      geek >> x;
      uuid_list.push_back(x);
      i++;
    }
    infile.close();
  }

  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_service, endpoint);
      std::ofstream myfile;
       myfile.open("server_uuid.txt");
        for (unsigned int i = 0; i < uuid_list.size(); i++)
        {
         myfile << uuid_list[i] << std::endl;
       }
       myfile.close();
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
