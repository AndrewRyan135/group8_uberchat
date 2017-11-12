#include <vector>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/crc.hpp>  //for checksum
#include <boost/date_time/posix_time/posix_time.hpp> //for time



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
    users user(num, "");
    user_list.push_back(user);
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
      i++;
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
  //get everything up to first space
  std::string checksum = input.substr(0,input.find(' '));
  return checksum;
}

int parseTime(std::string input){
  //find first space and start a substring at the next index
  std::string temp = input.substr(input.find(' ')).erase(0,1); 
  //take everything up until 2nd space and convert to int in base 10
  int time = std::stoi(temp.substr(0,temp.find(' ')),nullptr,10);
  return time;   
}
int parseUUID(std::string input)
{
  std::string temp = input.substr(input.find(' ')).erase(0,1);
  temp = temp.substr(temp.find(' ')).erase(0,1);
  int UUID = std::stoi(temp.substr(0,temp.find(' ')),nullptr,10);
  return UUID;
}
std::string nouuid_parseCmd(std::string input){
/*  std::string cmd;
  std::vector<std::string> results;
  std::stringstream s(input);
  while(!s.eof())
  {
    std::string tmp;
    s >> tmp;
    results.push_back(tmp);
  }
*/
  std::string cmd = input.substr(input.find(' ')).erase(0,1); 
  //find 2nd space and start a substring at the next index
  cmd = cmd.substr(cmd.find(' ')).erase(0,1);
  std::cout<<cmd<<'\n';
  return cmd;
}

std::string parseCmd(std::string input){
/*  std::string cmd;
  std::vector<std::string> results;
  std::stringstream s(input);
  while(!s.eof())
  {
    std::string tmp;
    s >> tmp;
    results.push_back(tmp);
  }
*/
  std::string cmd = input.substr(input.find(' ')).erase(0,1); 
  //find 2nd space and start a substring at the next index
  cmd = cmd.substr(cmd.find(' ')).erase(0,1);
  cmd = cmd.substr(cmd.find(' ')).erase(0,1);
  std::cout<<cmd<<'\n';
  return cmd;

  //std::cout<<results[2]<<'\n';
  //return results[2];
}


//take command and execute the command
//if the command have arguements (such as NAMECHATROOM) it will also parse the optional command
std::string ExecCmd(std::string cmd){
  //request uuid
  if(cmd == "REQUUID"){
    //debugger print  
    int uuid = requuid_handle();
    //int uuid = 1234;
    std::string value = std::to_string(uuid);
    return value;
    //std::cout<<"REQUUID ran successfully"<<'\n';
  //set nickname
  }else if(cmd.substr(0,5).compare("NICK ")==0){                          //NICK
    
    //truncate "NICK " from string
    std::string cmdOption = cmd.substr(5,cmd.length()-5);

    //debugger print
    std::cout<<"NICK ran successfully"<<'\n';
    std::string uuid_str = cmdOption.substr(0,9);
    std::string name = cmdOption.substr(9,cmd.length()-9);
    int uuid = std::stoi(uuid_str);
    change_nick(uuid, name);
    return name;
            
    //debugger print for input after space
    //std::cout<<"Actual input: "<<input<<'\n';

    //check if new nickname is > 10
    //if(input.length()>10){
      //std::cout<<"Error! Nickname cannot exceed 10 characters"<<'\n';
    //}
    //else{
      //debugger print
      //std::cout<<input.length()<<'\n';
     // std::cout<<"NICK ran successfully"<<'\n';
    //}
      //request list of available chatroom
  }else if(cmd.compare("REQCHATROOMS")==0){
    //debugger print
    std::cout<<"REQUSERS ran successfully"<<'\n';

  //rename chatroom
  }else if(cmd.substr(0,13).compare("NAMECHATROOM ")==0){
    //truncate "NAMECHATROOM " from string
    std::string cmdOption = cmd.substr(13,cmd.length()-13);

    //debugger print
    std::cout<<"NAMECHATROOM ran successfully"<<'\n';

    //debugger print for input after space
    //std::cout<<"Actual input: "<<input<<'\n';

  //send a text
  }else if(cmd.substr(0,9).compare("SENDTEXT ")==0){                      //SENDTXT
    //debugger print
    std::cout<<"SENDTEXT ran successfully"<<'\n';
          
    //truncate "SENDTEXT " from string
    std::string cmdOption = cmd.substr(9,cmd.length()-9);
    return cmdOption;
        
    //debugger print for input after space
    std::cout<<"Actual input: "<<cmd<<'\n';

    //check for ; in text
    if(cmd.find(";") != std::string::npos){
      std::cout<<"Error! Message cannot contain ';' in it"<<'\n';   
      }
    //finally if conditions are all met send message
    else{ 
      //debugger print
      std::cout<<"SENDTEXT ran successfully"<<'\n';
    }

  //request text
  }else if(cmd.compare("REQTEXT")==0){                                    //REQTXT
    //debugger print
    std::cout<<"REQTEXT ran successfully"<<'\n';

    //request users in chatroom
  }else if(cmd.compare("REQUSERS")==0){                                   //REQUSERS
    std::vector<std::string> users = request_users();
    std::string ret_string;
    for (int i = 0; i < users.size(); i++){
      std::cout << "Yep" << std::endl;
      ret_string += users[i] + " \n";
    }
    std::cout<<"REQUSERS ran successfully"<<'\n';
    return ret_string;
    //debugger print
    std::cout<<"REQUSERS ran successfully"<<'\n';
  }else{
    std::cout<<"Error! Your entry does not fit the standard format."<<'\n';
    std::cout<<"Type 'Help' for a list of format and their functions"<<'\n';
  }

}

int getTime(){
  
  //get universal time in millisec
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
  int time = now.time_of_day().total_milliseconds();
  std::string time_str = std::to_string(time);
  std::string hour = time_str.substr(0,2);
  std::string min = time_str.substr(3,4);
  std::string sec = time_str.substr(5,6);
  std::string mil = time_str.substr(7,10);

  //this bottom code will convert into time of day hr:min:sec
  //boost::posix_time::time_duration time_formated = boost::posix_time::milliseconds(time);

  //debugger print
  //std::cout<<std::dec<<time<<'\n';
  std::cout << time << " ";

  return time;

}

int getChecksum(const std::string& str){
  //crc_32_type is typedef as crc_optimal(32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true) in boost
  boost::crc_32_type cksum;
  //turn string into an array of char of data's ascii and process it
  cksum.process_bytes(str.data(), str.length());
  //convert to an umber
  int result = cksum.checksum();

  //debugger code
  //std::cout<<"checksum: "<<std::hex<<result<<'\n';

  return result; 
}

//appends the int as a string to the front
std::string appendInt(std::string input, int num){
  std::string num_str = std::to_string(num);
  std::string result = num_str+" "+input;
  //debugger print
  //std::cout<<"current processed string: "<<result<<'\n';
  return result;
}

//return 0 for okay, 1 for corruption
int check_cksum(std::string cksum, std::string cmd){
  int newCksum = getChecksum(cmd);
  int expected = atoi(cksum.c_str());
  if(newCksum == expected){
    std::cout<<"Data integrity verified"<<'\n';
    return 0;
  }else{
    std::cout<<"Corrupted data"<<'\n';
    std::cout<<"Expected: "<<std::hex<<expected<<'\n';
    std::cout<<"Checksum: "<<std::hex<<newCksum<<'\n';
    return 1;
  }
}