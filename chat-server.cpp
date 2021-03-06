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
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <string.h>
#include "chat_message.hpp"

using boost::asio::ip::tcp;




//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class users
{
public:
  users(int u_uuid, std::string u_nick) : uuid(u_uuid), nickname(u_nick) {}
  int get_uuid()
  {
    return uuid;
  }
  std::string get_nick()
  {
    return nickname;
  }
  void set_nick(std::string name)
  {
    nickname = name;
  }
  void set_uuid(int num)
  {
    uuid = num;
  }
  void mute_user(std::string name)
  {
    muted_list.push_back(name);
  }

private:
  int uuid;
  std::string nickname;
  std::vector<std::string> muted_list;
};


std::vector<std::string> chatrooms_list;
std::vector<int> uuid_vector;
std::vector<users> user_list;
//------------------------------------------------------------------

class command_handling
{
public:
  std::string nickname_handle(int uuid)
  {
    for (int i = 0; i < user_list.size(); i++)
    {
      if (user_list[i].get_uuid() == uuid)
      {
        return user_list[i].get_nick();
      }
    }
  }
  void change_nick(int uuid, std::string name)
  {
    for (int i = 0; i < user_list.size(); i++)
    {
      if (user_list[i].get_uuid() == uuid)
      {
        user_list[i].set_nick(name);
      }
    }
  }
  int requuid_handle()
  {
    bool val = false;
    int num = 0000;
    while (!val)
    {
      srand(time(NULL));
      num = rand()%1000000000;
      if (std::find(uuid_vector.begin(), uuid_vector.end(), num) != uuid_vector.end())
      {
        
      }
      else
      {
        val = true;
      }
    }
    return num; 
  }
  std::vector<std::string> chatroom_handle() //Will provide the vector containing the names of the chat rooms
  {
    return chatrooms_list;
  }
  void create_chatroom(std::string name)
  {

  }

  std::vector<std::string> request_users()
  {
    int i = 0;
    std::string return_value;
    std::vector<std::string> tmp;
    while (i < user_list.size())
    {
      int uuid = user_uuid(i);
      std::string nick = user_nick(i);
      return_value = std::to_string(uuid) + ',' + nick;
      tmp.push_back(return_value);
    }
    return tmp;
  }

  int user_uuid(int i)                                                        //user_uuid()
  {
    return user_list[i].get_uuid();
  }

  std::string user_nick(int i)                                                //user_nick()
  {
    return user_list[i].get_nick();
  }

private:
};

//----------------------------------------------------------------------------------------------------------

class chatrooms                             // Will get moved to class chat_room
{
public:
  chatrooms(bool in_private_id, std::string in_name) : private_id(in_private_id), name(in_name) {
    chatrooms_list.push_back(in_name);
  }
  void message_backlog(std::string message) //read from the vector from the last element to the first element
  {
    if (messages.size() < 101)
    {
      messages.push_back(message);
    }
    else
    {
      messages.erase(messages.begin()+0);
    }
    std::string get_name()
    {
      return name;
    }
    bool is_private()
    {
      return private_id;
    }
  }


private:
  bool private_id;
  std::string name;
  std::vector<users> chatroom_users;
  std::vector<std::string> messages;
};

//-------------------------------------------------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
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
            room_.leave(shared_from_this());
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
            room_.deliver(read_msg_);
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
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
            room_.leave(shared_from_this());
          }
        });
  }

  tcp::socket socket_;
  chat_room& room_;
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
            std::make_shared<chat_session>(std::move(socket_), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
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
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
