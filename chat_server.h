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

<<<<<<< HEAD

std::vector<int> uuid_vector;

chat_message convert_to(std::string input)        //Converts the string input into a chat_message
=======
//std::vector<chatrooms> chatrooms_list;
std::vector<int> uuid_vector;
//std::vector<users> user_list;

chat_message convert_to(std::string input)
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
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

<<<<<<< HEAD
std::string convert_from(chat_message msg)      //Converts chat_message to a string
=======
std::string convert_from(chat_message msg)
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
{
  std::stringstream buffer;
  buffer.write(msg.body(), msg.body_length());
  std::string text = buffer.str();
  buffer.str(std::string());
  text.erase(std::remove(text.begin(), text.end(), '\0'), text.end());
  return text;
}


<<<<<<< HEAD
std::string parseChecksum(std::string input){                   //Parses the checksum from the input string
=======
std::string parseChecksum(std::string input){
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
  //get everything up to first space
  std::string checksum = input.substr(0,input.find(','));
  return checksum;
}

<<<<<<< HEAD
int parseTime(std::string input){                               //Parses the time from the input string
=======
int parseTime(std::string input){
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
  //find first space and start a substring at the next index
  std::string temp = input.substr(input.find(',')).erase(0,1); 
  //take everything up until 2nd space and convert to int in base 10
  int time = std::stoi(temp.substr(0,temp.find(',')),nullptr,10);
  return time;   
}
<<<<<<< HEAD
int parseUUID(std::string input)                                 //Parses the UUID from the input string
=======
int parseUUID(std::string input)
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
{
  std::string temp = input.substr(input.find(',')).erase(0,1);
  temp = temp.substr(temp.find(',')).erase(0,1);
  int UUID = std::stoi(temp.substr(0,temp.find(',')),nullptr,10);
  return UUID;
}
<<<<<<< HEAD
std::string nouuid_parseCmd(std::string input){                  //Parses the command where there is no uuid passes
=======
std::string nouuid_parseCmd(std::string input){
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862

  std::string cmd = input.substr(input.find(',')).erase(0,1); 
  //find 2nd space and start a substring at the next index
  cmd = cmd.substr(cmd.find(',')).erase(0,1);
  std::cout<<cmd<<'\n';
  return cmd;
}

<<<<<<< HEAD
std::string parseCmd(std::string input){                         //Parses the command when there is a uuid provided

  std::string cmd = input.substr(input.find(',')).erase(0,1); 
=======
std::string parseCmd(std::string input){

  std::string cmd = input.substr(input.find(' ')).erase(0,1); 
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
  //find 2nd space and start a substring at the next index
  cmd = cmd.substr(cmd.find(',')).erase(0,1);
  cmd = cmd.substr(cmd.find(',')).erase(0,1);
  std::cout<<cmd<<'\n';
  return cmd;

  //std::cout<<results[2]<<'\n';
  //return results[2];
}

<<<<<<< HEAD
int getTime(){                                                                          //Gets the current time
=======

int getTime(){
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
  
  //get universal time in millisec
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
  int time = now.time_of_day().total_milliseconds();
<<<<<<< HEAD
  std::string time_str = std::to_string(time);
  std::string hour = time_str.substr(0,2);
  std::string min = time_str.substr(3,4);
  std::string sec = time_str.substr(5,6);
  std::string mil = time_str.substr(7,10);

=======
  
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
  //this bottom code will convert into time of day hr:min:sec
  //boost::posix_time::time_duration time_formated = boost::posix_time::milliseconds(time);

  //debugger print
  //std::cout<<std::dec<<time<<'\n';
<<<<<<< HEAD
  std::cout << time << " ";
=======
  //std::cout << time << " ";
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862

  return time;

}

<<<<<<< HEAD
int getChecksum(const std::string& str){                                                                  //gets the checksum for the message
=======
int getChecksum(const std::string& str){
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
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
<<<<<<< HEAD
int check_cksum(std::string cksum, std::string cmd){                                        //checkes the check sum to see if it is correct
=======
int check_cksum(std::string cksum, std::string cmd){
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
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

<<<<<<< HEAD
std::string formatTime(int time_ms){
  int time_hr = (time_ms/(3600000)+6);
  //std::string result;
  int time_min = (time_ms%3600000)/60000;
  int time_sec = ((time_ms%3600000)%60000)/1000;
  //std::string universal_time = std::to_string(time_hr-6)+":"+min+"."+sec;
  std::string min, time, sec;

  if (time_min < 10){
    min = "0"+std::to_string(time_min);
  }else{
    min = std::to_string(time_min);
  }

  if (time_sec < 10){
    sec = "0"+std::to_string(time_sec);
  }else{
    sec = std::to_string(time_sec);
  }


  if (time_hr > 24){
    time_hr-= 24; 
  }

  if (time_hr == 24) {
    time_hr-=12;
    time = std::to_string(time_hr)+":"+min+"."+sec+"am"; 
  }else if (time_hr > 12){
    time_hr-=12;
    time = std::to_string(time_hr)+":"+min+"."+sec+"am"; 
  }else if(time_hr <= 12){ 
    time = std::to_string(time_hr)+":"+min+"."+sec+"pm"; 
  }
  
  //result = universal_time+'\n'+time;

  return time;
}

std::string timeStamp(){  
  
  return formatTime(getTime());
}

//replace space between command and optional command if there is any
std::string convert_OptionalCmd(std::string input){                                         //parses the optional arguments from the input
=======
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
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
  
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
<<<<<<< HEAD

=======
>>>>>>> 8f72ca0d7da98ed3183c92619bce17115f338862
