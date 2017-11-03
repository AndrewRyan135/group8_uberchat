#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstdlib>


class users{
private:
  int uuid;
  std::string nickname;
  std::vector<std::string> muted_list;
public:
	users(int u_uuid, std::string u_nick);
	int get_uuid();
	std::string get_nick();
	void set_nick(std::string name);
	void set_uuid(int num);
	void mute_user(std::string name);

};
class command_handling{
private:

public:
	std::string nickname_handle(int uuid);
	void change_nick(int uuid, std::string name);
	int requuid_handle();
	std::vector<std::string> chatroom_handle();
	void create_chatroom(std::string name);
	std::vector<std::string> request_users();
	int user_uuid(int i);
	std::string user_nick(int i);
};