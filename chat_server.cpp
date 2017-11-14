
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
#include "chat_server.h"
#include "chat_message.hpp"

using boost::asio::ip::tcp;




//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;
std::string ExecCmd(std::string cmd);
int requuid_handle();
class chatrooms;

std::vector<std::string> chatroom_names;
std::vector<chatrooms> chatroom_list;
std::vector<std::string> user_list;
//----------------------------------------------------------------------

class chatrooms                             
{
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

class chat_participant
{
public:
  chat_participant()
  {
    uuid = requuid_handle();
    room_name = "Main";
    std::cout << "UUID " << uuid << " Chat room " << room_name << std::endl;
    index = 0;
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
      //chat_message ret_value = convert_to(std::to_string(uuid));

  }else if(cmd.substr(0,5).compare("NICK ")==0){                          //NICK
    
      std::string name = cmd.substr(5,cmd.length()-5);
      std::cout<<"NICK ran successfully"<<'\n';
      nick = name;
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      {
        if (chatroom_list[i].get_name().compare(room_name)==0)
        {
          chatroom_list[i].add_user(nick);
        }
      }
      user_list.push_back(name);
      chat_message msg = convert_to(nick);
      deliver(msg);

  }else if(cmd.compare("REQCHATROOM")==0){
      chat_message ret_value = convert_to(room_name);
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
        chat_message msg = convert_to(ret_value);
        deliver(msg);
        
      }
  }else if(cmd.substr(0,13).compare("NAMECHATROOM ")==0){
      std::string cmdOption = cmd.substr(13,cmd.length()-13);
      chatrooms room(cmdOption);
      chatroom_list.push_back(room);
      chatroom_names.push_back(cmdOption);
      std::cout<<"NAMECHATROOM ran successfully"<<'\n';
      std::string ret_value = "Created " + cmdOption;
      chat_message msg = convert_to(ret_value);
      deliver(msg);

  }else if(cmd.substr(0,14).compare("CHANGECHATROOM")==0){
      std::string name = cmd.substr(15,cmd.length()-15);
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      {
        std::cout << chatroom_list[i].get_name() << std::endl;
        if (chatroom_list[i].get_name().compare(room_name)==0)
        {
          for (int j = 0; j < chatroom_list[i].num_users(); i++)
          {
            if (chatroom_list[i].get_user(j).compare(nick)==0)
            {
              std::cout << chatroom_list[i].get_name() << " " << nick << std::endl;
              chatroom_list[i].remove_user(nick);
            }
          }
        }
      }
      room_name = name;
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      {
        if (room_name.compare(chatroom_list[i].get_name())==0)
        {
          std::cout << room_name << std::endl;
          chatroom_list[i].add_user(nick);
        }
      }
      std::string ret = "Changed to chat room " + name;
      chat_message msg = convert_to(ret);
      deliver(msg);
      std::cout << "Changed chat rooms" << std::endl;
  }else if(cmd.substr(0,9).compare("SENDTEXT ")==0){                      //SENDTXT

      std::cout<<"SENDTEXT ran successfully"<<'\n';
      std::string cmdOption = cmd.substr(9,cmd.length()-9);
      cmdOption = nick + ": " + cmdOption; 
      if(cmdOption.find(";") != std::string::npos){
        std::cout<<"Error! Message cannot contain ';' in it"<<'\n';   
      }
     else{ 
        std::cout<<"SENDTEXT ran successfully"<<'\n';
        for (unsigned int i = 0; i < chatroom_list.size(); i++)
        {
          if (chatroom_list[i].get_name().compare(room_name)==0)
          {
            chatroom_list[i].message_backlog(cmdOption);
          }
        }
        chat_message msg = convert_to(cmdOption);
        deliver(msg);
      }
  }else if(cmd.compare("REQTEXT")==0){   
      int num;
      int pos;
      std::string ret_value;
      for (unsigned int i = 0; i < chatroom_list.size(); i++)
      {
        if (chatroom_list[i].get_name().compare(room_name)==0)
        {
          pos = i;
          num = chatroom_list[i].msg_size();
        }
      }                               
      for (int i = index; i < num; i++)
      {
        ret_value = chatroom_list[pos].get_msg(i);
      }
      chat_message msg = convert_to(ret_value);
      deliver(msg);
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
      chat_message msg = convert_to(ret_value);
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
  int index;
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
            //std::cout << text << std::endl;
            std::string cksum = parseChecksum(text);
            std::string value;
            if (text.find("REQUUID") != std::string::npos)
            {
              value = nouuid_parseCmd(text);
            }
            else
            {
              value = nouuid_parseCmd(text);
            }
            //int test = check_cksum(cksum, value);
            self->ExecCmd(value);

            do_read_header();
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
    chatrooms room("Main");
    chatroom_list.push_back(room);
    chatroom_names.push_back("Main");
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
    //chat_participant user;
    //user_list.push_back(user);
    return num; 
  }

//--------------------------------------------------------------------------


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