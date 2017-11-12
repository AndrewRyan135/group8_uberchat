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
#include "chat_server.h"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class users_info
{
public:
  int get_uuid()
  {
    return uuid;
  }
  void set_uuid(int in_uuid)
  {
    uuid = in_uuid;
  }
  std::string get_nick()
  {
    return nick;
  }
  void set_nick(std::string name)
  {
    nick = name;
  }
private:
  std::string nick;
  int uuid;
};

users_info user;

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
            std::stringstream buffer;
            //std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
            buffer.write(read_msg_.body(), read_msg_.body_length());
            //std::cout << "\n";
            std::string text = buffer.str();
            buffer.str(std::string());
            text.erase(std::remove(text.begin(), text.end(), '\0'), text.end());
            std::string num;
            if (text.find("REQUUID") != std::string::npos)
            {
              num = text.substr(8,text.length()-8);
              user.set_uuid(std::stoi(num));
              text = text.substr(8,text.length()-8);
            }
            if (text.find("NICK") != std::string::npos)
            {
              std::string name = text.substr(14,text.length()-14);
              user.set_nick(name);
              text = text.substr(14,text.length()-14);
            }

            char line[chat_message::max_body_length+1];
            memset(line,0,sizeof(line));
            for (int i=0; i<=text.size();i++)
            {
              line[i] = text[i];
            }
            chat_message msg;
            msg.body_length(std::strlen(line));
            std::memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            std::cout.write(msg.body(), msg.body_length());
            std::cout << "\n";
            std::cout << "Finished reading" << std::endl;
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
     //temp checksum and time
      std::string str = line;

      if (str.compare("REQUUID")==0){
        int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
        str = appendInt(str, cksum);
      }else{      
        int id = user.get_uuid();
        //get checksum of command only
        int cksum = getChecksum(str);
        //apend uuid to front
        str = appendInt(str, id);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
        str = appendInt(str, cksum);
      }

      strcpy(line, str.c_str());
      //std::cout<<"char array: "<<line<<'\n';

      //original messaging without truncating "SENDTEXT"
      chat_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
       
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


