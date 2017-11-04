#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstdlib>




class users
{
private:
  int uuid;
  std::string nickname;
  std::vector<std::string> muted_list;
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
};

std::vector<std::string> chatrooms_list;
std::vector<int> uuid_vector;
std::vector<users> user_list;

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
  }
    std::string get_name()
    {
      return name;
    }
    bool is_private()
    {
      return private_id;
    }
  


private:
  bool private_id;
  std::string name;
  std::vector<users> chatroom_users;
  std::vector<std::string> messages;
};


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
      int uuid = user_list[i].get_uuid();
      std::string nick = user_list[i].get_nick();
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


std::string parseChecksum(std::string input){


  
}

std::string parse(std::string input){


  
}

std::string parse(std::string input){


  
}