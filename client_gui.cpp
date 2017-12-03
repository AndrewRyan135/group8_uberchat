// This example will demonstrate the integration of
// the boost asio chatserver with a simple FLTK program
//
//
//
#include <assert.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "io.hpp"
#include "chat_server.h"
#include "chat_message.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Toggle_Button.H>

typedef std::deque<chat_message> chat_message_queue;
char line[chat_message::max_body_length + 1];




Fl_Window chat   (1000, 600, "UberChat");
Fl_Output r_name (450, 25, 100, 25, "Chatroom Name");
Fl_Output u_name (700, 25, 100, 25, "Username");
Fl_Input input1 (250, 400, 600, 25, "Message: ");
Fl_Button create (50, 20, 150, 50, "Create Chatroom");
Fl_Button join (50, 90, 150, 50, "Join Chatroom");
Fl_Multiline_Output users (50, 160, 100, 200, "Users");
Fl_Button nick (50, 450, 100, 50, "Change Nick");
Fl_Button quit  (50, 520, 50,20,"Quit");
Fl_Button clear (50, 560, 50,20,"Clear");
Fl_Text_Buffer *buff = new Fl_Text_Buffer ();
Fl_Text_Display *disp = new Fl_Text_Display (250,100,600,250,"chat");

Fl_Window login (250, 150, "Uberchat Login");
Fl_Input input2 (75, 20, 100, 25, "Nick");
Fl_Button new_u (75, 80, 150, 25, "LOGIN");

Fl_Window c_room (300, 100, "Create Room");
Fl_Input input3 (100, 20, 100, 25, "Room Name");
Fl_Button make (100, 50, 50, 25, "Submit");

Fl_Window j_room (500, 100, "Join Room");
Fl_Input input4 (300, 20, 100, 25, "Room Name");
Fl_Multiline_Output rooms(50, 20, 200, 70, "Rooms");
Fl_Button enter (300, 50, 50, 25, "Submit");

Fl_Window c_nick (250, 100, "Change Nick");
Fl_Input input5 (100, 20, 100, 25, "New Nick");
Fl_Button Change (100, 50, 50, 25, "Submit");


chat_client *c = NULL;
std::thread *t = NULL;
std::string lastsent;

void requ()
{
std::string str = "REQUSERS";
  int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	strcpy(line, "\0");
	str = "REQTEXT";
	cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	Fl::repeat_timeout(.1, (Fl_Timeout_Handler)requ);
}




static void cb_cnick()
{
	u_name.value(NULL);
	c_nick.hide();
	std::string n(input5.value());
	std::string str = "NICK " + n;
        int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	u_name.value(n.c_str());
	strcpy(line, "\0");
	chat.show();
	input5.value(NULL);
	
}

static void cb_recv ( std::string S )
{
  // Note, this is an async callback from the perspective
  // of Fltk..
  //
  // high chance of a lock needed here if certain fltk calls
  // are made.  (like show() .... )
  std::string T = S + '\n' + '\0';

if(S.substr(0,5).compare("NICK ")==0)
{
}
            
if(S.substr(0,9).compare("SENDTEXT,")==0)
{
  T = S.substr(14, S.length()-14) + '\n' + '\0';
  lastsent = T;
  if (buff)
  {
    buff->append ( T.c_str() );
  }
  if (disp)
  {
    disp->show ();
  }
  input1.value(NULL);
}  
if(S.substr(0,12).compare("REQCHATROOM ")==0)
{
  r_name.value(NULL);
  r_name.value(S.substr(12, S.length()-12).c_str());
}
if(S.substr(0,13).compare("REQCHATROOMS ")==0)
{
  int pos = S.rfind("REQCHATROOMS");
  rooms.value(NULL);
  rooms.value(S.substr(pos+13, S.length()-pos+13).c_str());
  
}
if(S.substr(0,13).compare("NAMECHATROOM ")==0)
{
}
if(S.substr(0,15).compare("CHANGECHATROOM ")==0)
{
}
if(S.substr(0,9).compare("REQUSERS ")==0)
{
  int p = S.rfind("NICK");
  users.value(S.substr(p+5,S.length()-p+5).c_str());
}
if(S.substr(0,8).compare("REQTEXT ")==0)
{
  std::string m;
  int n = S.rfind("NICK ");
  if(S.length()>=13)
  {
    m = T.substr(n+5, T.length()-n+5);
  }
  else
  {
    m = T.substr(8, T.length()-8);
  }
  if((m.length()>2)&&(m.compare(lastsent)!=0))
  {
    printf("here\n");
    lastsent = m;
    if (buff)
    {
      buff->append ( m.c_str() );
    }
    if (disp)
    {
      disp->show ();
    }
  }
}

}

static void cb_clear ()
{
   if (buff)
   {
     buff->remove (0,  buff->length () );
   }
   // may need to call show() ?
}



static void cb_new()
{
	u_name.value(NULL);
	login.hide();
	std::string n(input2.value());
	std::string str = "NICK "+ n;
        //get checksum of command only
        int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	u_name.value(n.c_str());
	strcpy(line,"\0");
	str = "REQCHATROOM";
	cksum = getChecksum(str);
	str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);	
	strcpy(line,"\0");
	str = "CHANGECHATROOM Main";
	cksum = getChecksum(str);
	str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	input2.value(NULL);
	chat.show();
	
}

static void cb_join()
{
	chat.hide();
	std::string str = "REQCHATROOMS";
        //get checksum of command only
        int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	strcpy(line, "\0");
	input4.value(NULL);
	j_room.show();
}

static void cb_make()
{
	chat.hide();
	c_room.show();
}

static void cb_nick()
{
	chat.hide();
	c_nick.show();
}

static void cb_made()
{
	c_room.hide();
	std::string n(input3.value());
	std::string str = "NAMECHATROOM,"+ n;
        //get checksum of command only
        int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	strcpy(line, "\0");
	input3.value(NULL);
	chat.show();
	
}


static void cb_joined()
{
	j_room.hide();
	std::string n(input4.value());
	std::string str = "CHANGECHATROOM,"+ n;
        //get checksum of command only
        int cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	strcpy(line, "\0");
	input4.value(NULL);
	str = "REQCHATROOM";
        //get checksum of command only
        cksum = getChecksum(str);
        //append time to front  
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	c->write(msg);
	strcpy(line, "\0");
	chat.show();
}


static void cb_quit ( )
{
  // this is where we exit to the operating system
  // any clean up needs to happen here
  //
  if (c)
  {
    c->close();
  }
  if (t)
  {
     t->join();
  }

  exit (0);
}
static void cb_input1() 
{
	
	std::string m(input1.value());
	m = m + "\0";
	std::string str = "SENDTEXT,";
	str = str + m;
        int cksum = getChecksum(str);
        //apend uuid to front 
        str = appendInt(str, getTime());
        //append checksum to front
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
       	chat_message msg;
      	msg.body_length(std::strlen(line));
      	std::memcpy(msg.body(), line, msg.body_length());
      	msg.encode_header();
	strcpy(line, "\0");
	c->write(msg);
}

int main ( int argc, char** argv ) 
{

  chat.begin ();
    chat.add (input1);
    input1.callback ((Fl_Callback*)cb_input1,( void *) input1.value());
    input1.when ( FL_WHEN_ENTER_KEY );
    quit.callback (( Fl_Callback*) cb_quit );
    clear.callback (( Fl_Callback*) cb_clear );
    create.callback((Fl_Callback*)cb_make);
    join.callback((Fl_Callback*)cb_join);
   nick.callback((Fl_Callback*)cb_nick);
    chat.add (quit);
    disp->buffer(buff);
  chat.end ();

  login.begin();
	login.add(input2);
	login.add(new_u);
	new_u.callback((Fl_Callback*) cb_new);
  login.end();

  c_room.begin();
	c_room.add(input3);
	c_room.add(make);
	make.callback((Fl_Callback*) cb_made);
  c_room.end();

  j_room.begin();
	j_room.add(input4);
	j_room.add(enter);
	j_room.add(rooms);
	enter.callback((Fl_Callback*) cb_joined);
  j_room.end();

  c_nick.begin();
	c_nick.add(input5);
	c_nick.add(Change);
	Change.callback((Fl_Callback*) cb_cnick);
  c_room.end();
  login.show ();

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
    c = new chat_client (io_service, endpoint_iterator, &cb_recv);

    t = new std::thread ([&io_service](){ io_service.run(); });

    // goes here, never to return.....
    Fl::add_timeout(1.0, (Fl_Timeout_Handler)requ);
    return Fl::run ();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  // never gets here
  return 0;
}
