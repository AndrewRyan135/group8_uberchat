//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service),
      socket_(io_service)
  {
    do_connect(endpoint_iterator);
  }

  void write(const chat_message& msg)
  {
    io_service_.post(
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    io_service_.post([this]() { socket_.close(); });
  }

  //added methods starts here


private:
  void do_connect(tcp::resolver::iterator endpoint_iterator)
  {
    boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
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
            socket_.close();
          }
        });
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
    chat_client c(io_service, endpoint_iterator);

    std::thread t([&io_service](){ io_service.run(); });

    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      //added code for giving command
      std::string cmd = line;
      //request uuid
      if(cmd.compare("REQUUID") == 0){

      //set nickname
      }else if(cmd.substr(0,4).compare("NICK")==0
            && cmd.substr(4,1).compare(" ")==0){

      }else if(cmd.compare("REQCHATROOMS")==0){

      //rename chatroom
      }else if(cmd.substr(0,12).compare("NAMECHATROOM")==0
            && cmd.substr(12,1).compare(" ")==0){

      //send a text
      }else if(cmd.substr(0,8).compare("SENDTEXT")==0
            && cmd.substr(8,1).compare(" ")==0){

            chat_message msg;
            msg.body_length(std::strlen(line));
            std::memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            c.write(msg);
      //request tex
      }else if(cmd.compare("REQTEXT")==0){

      //request users in chatroom
      }else if(cmd.compare("REQUSERS")==0){

      }else if(cmd.compare("Help")==0){
            std::cout<<"Type:         Followed by:          To:"<<'\n';
            std::cout<<"REQUUID       (nothing)             request a user ID"<<'\n';
            std::cout<<"NICK          (desired nickname)    display as your nickname"<<'\n';
            std::cout<<"REQCHATROOMS  (nothing              request name of all available chatrooms"<<'\n';
            std::cout<<"NAMECHATROOM  (new chatroom name)   create and name a chatroom"<<'\n';
            std::cout<<"SENDTEXT      (message)             send message to all in chatroom"<<'\n';
            std::cout<<"REQTEXT       (nothing)             request text since last time REQTEXT was used"<<'\n';
            std::cout<<"REQUSERS      (nothing)             request all user's nickname in chatroom"<<'\n';
            std::cout<<"Help          (nothing)             show help menu"<<'\n';
      }
      else{
        std::cout<<"Error! Your entry does not fit the standard format."<<'\n';
        std::cout<<"Type 'Help' for a list of format and their functions"<<'\n';
      }
    }

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

