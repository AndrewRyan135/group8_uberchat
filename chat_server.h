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
#include "chat_message.hpp"

class chatrooms;

//std::vector<chatrooms> chatrooms_list;
std::vector<int> uuid_vector;
//std::vector<users> user_list;

chat_message convert_to(std::string input)
{
  char line[chat_message::max_body_length +1];
  memset(line,0,sizeof(line));
  for (unsigned int i=0; i<=input.size();i++)
  {
      line[i] = input[i];
  }
  chat_message msg;
  msg.body_length(std::strlen(line));
  std::memcpy(msg.body(), line, msg.body_length());
  msg.encode_header();
  return msg;
}

std::string convert_from(chat_message msg)
{
  std::stringstream buffer;
  buffer.write(msg.body(), msg.body_length());
  std::string text = buffer.str();
  buffer.str(std::string());
  text.erase(std::remove(text.begin(), text.end(), '\0'), text.end());
  return text;
}


std::string parseChecksum(std::string input){
  //get everything up to first space
  std::string checksum = input.substr(0,input.find(','));
  return checksum;
}

int parseTime(std::string input){
  //find first space and start a substring at the next index
  std::string temp = input.substr(input.find(',')).erase(0,1); 
  //take everything up until 2nd space and convert to int in base 10
  int time = std::stoi(temp.substr(0,temp.find(',')),nullptr,10);
  return time;   
}
int parseUUID(std::string input)
{
  std::string temp = input.substr(input.find(',')).erase(0,1);
  temp = temp.substr(temp.find(',')).erase(0,1);
  int UUID = std::stoi(temp.substr(0,temp.find(',')),nullptr,10);
  return UUID;
}
std::string nouuid_parseCmd(std::string input){

  std::string cmd = input.substr(input.find(',')).erase(0,1); 
  //find 2nd space and start a substring at the next index
  cmd = cmd.substr(cmd.find(',')).erase(0,1);
  std::cout<<cmd<<'\n';
  return cmd;
}

std::string parseCmd(std::string input){

  std::string cmd = input.substr(input.find(' ')).erase(0,1); 
  //find 2nd space and start a substring at the next index
  cmd = cmd.substr(cmd.find(',')).erase(0,1);
  cmd = cmd.substr(cmd.find(',')).erase(0,1);
  std::cout<<cmd<<'\n';
  return cmd;

  //std::cout<<results[2]<<'\n';
  //return results[2];
}


int getTime(){
  
  //get universal time in millisec
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
  int time = now.time_of_day().total_milliseconds();
  
  //this bottom code will convert into time of day hr:min:sec
  //boost::posix_time::time_duration time_formated = boost::posix_time::milliseconds(time);

  //debugger print
  //std::cout<<std::dec<<time<<'\n';
  //std::cout << time << " ";

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
  std::string result = num_str+","+input;
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

std::string timeStamp(){
  int time_ms = getTime();
  std::string hr = std::to_string(time_ms/(3600000));
  std::string min = std::to_string((time_ms%3600000)/60000);
  std::string sec = std::to_string(((time_ms%3600000)%60000)/1000);
  std::string ms = std::to_string((((time_ms%3600000)%60000)%1000));

  std::string result = "Universal Time: "+hr+"hr:"+min+"min:"+sec+"."+ms+"s";

  //debugger print 
  //std::cout<<result<<'\n';

  return result;
}

//replace space between command and optional command if there is any
std::string convert_OptionalCmd(std::string input){
  
  std::string output;

  if (input.substr(0,5).compare("NICK ")==0){
    output = input.replace(4,1,",");
  }else if(input.substr(0,13).compare("NAMECHATROOM ")==0){
    output = input.replace(12,1,",");
  }else if (input.substr(0,9).compare("SENDTEXT ")==0){
    output = input.replace(8,1,",");
  }else{
    output = input; //no optional command hence no changes
  }
  //debugger print
  //std::cout<<"convert_optionalCmd returned: "<<output<<'\n';
  return output;
}
