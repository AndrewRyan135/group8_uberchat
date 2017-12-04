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




Fl_Window chat   (1000, 700, "UberChat");
Fl_Output r_name (450, 25, 100, 25, "Chatroom Name");
Fl_Output u_name (700, 25, 100, 25, "Username");
Fl_Input input1 (250, 400, 600, 25, "Message: ");
Fl_Button create (50, 20, 150, 50, "Create Chatroom");
Fl_Button join (50, 90, 150, 50, "Join Chatroom");
Fl_Multiline_Output users (50, 160, 100, 200, "Users");
Fl_Button u (50, 510, 50, 20, "UUID");
Fl_Button nick (50, 450, 100, 50, "Change Nick");
Fl_Button quit  (50, 570, 50,20,"Quit");
Fl_Button clear (50, 630, 50,20,"Clear");
Fl_Text_Buffer *buff = new Fl_Text_Buffer ();
Fl_Text_Display *disp = new Fl_Text_Display (250,100,600,250,"chat");

Fl_Window login (250, 150, "Uberchat Login");
Fl_Input input2 (75, 20, 100, 25, "Nick");
Fl_Button new_u (75, 80, 150, 25, "LOGIN");

Fl_Window c_room (300, 100, "Create Room");
Fl_Input input3 (100, 20, 100, 25, "Room Name");
Fl_Button make (100, 50, 50, 25, "Submit");

Fl_Window j_room (500, 500, "Join Room");
Fl_Input input4 (300, 20, 100, 25, "Room Name");
Fl_Multiline_Output rooms(50, 20, 200, 450, "Rooms");
Fl_Button enter (300, 50, 50, 25, "Submit");

Fl_Window c_nick (250, 100, "Change Nick");
Fl_Input input5 (100, 20, 100, 25, "New Nick");
Fl_Button Change (100, 50, 50, 25, "Submit");

Fl_Window uuid (250, 150, "UUID");
Fl_Output UUID(50, 38, 100, 25, "UUID");
Fl_Button ret(50, 70, 50, 25, "Return");



chat_client *c = NULL;
std::thread *t = NULL;
std::string lastsent;

static void s_uuid()
{
	std::string str = "REQUUID";
	int cksum = getChecksum(str);
	str = appendInt(str, getTime());
	str = appendInt(str, cksum);
	strcpy(line, str.c_str());
	chat_message msg;
	msg.body_length(std::strlen(line));
	std::memcpy(msg.body(), line, msg.body_length());
	msg.encode_header();
	c->write(msg);
	strcpy(line, "\0");
	uuid.show();
}

static void r_uuid()
{
  	uuid.hide();
}


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
  Fl::repeat_timeout(1, (Fl_Timeout_Handler)requ);
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
if(S.find("REQUUID,")!=std::string::npos)
{
  int pos = S.rfind("REQUUID,");
  UUID.value(S.substr(pos+8, S.length()-(pos+8)).c_str());
}
if(S.find("NICK") != std::string::npos)
{
}
            
if(S.substr(0,14).compare("SENDTEXT,")==0)
{
  T = S.substr(10, S.length()-10) + '\n' + '\0';
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
if(S.find("REQCHATROOM,") != std::string::npos)
{
  r_name.value(NULL);
  std::string m = S.substr(S.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);

  r_name.value(m.c_str());
}
if(S.find("REQCHATROOMS,") != std::string::npos)
{
 
  rooms.value(NULL);
  std::string m = S.substr(S.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  std::cout << "The name of the rooms is " << m << std::endl;
  rooms.value(m.c_str());
  
}
if(S.find("NAMECHATROOM,") != std::string::npos)
{
}
if(S.find("CHANGECHATROOM,") != std::string::npos)
{
}
if(S.find("REQUSERS,") != std::string::npos)
{
 /* while(S.find("NICK ") != std::string::npos)
  {
    int i = S.find("NICK ");
    S.erase(i, 5);
  }*/
  std::string m = S.substr(S.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  users.value(m.c_str());
}
if(S.find("REQTEXT") != std::string::npos)
{
  std::string m;
  //int n = S.rfind("NICK ");
  //if(S.length()>=13)
  //{
  //  m = T.substr(9, T.length()-9);
    //m = T;
  //}
  //else
  //{
  //  m = T.substr(8, T.length()-8);
  //}
  m = S.substr(S.find(',')).erase(0,1);
  //std::string time = m.substr(0,m.find(','));
  //m = m.substr(m.find(',')).erase(0,1);
  m = m.substr(m.find(',')).erase(0,1);
  //m = time + m;
  //m = T;
  if((m.length()>2)&&(m.compare(lastsent)!=0))
  {
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
  std::cout << "here\n";
  u_name.value(NULL);
  login.hide();
  std::string n(input2.value());
  std::string str = "NICK,"+ n;
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
  input2.value(NULL);
  chat.show();
  
}

static void cb_join()
{
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
  c_room.show();
}

static void cb_nick()
{
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
    chat.add(u);
    u.callback((Fl_Callback*) s_uuid);
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

  uuid.begin();
    uuid.add(UUID);
    uuid.add(ret);
    ret.callback((Fl_Callback*) r_uuid);
  uuid.end();

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
    Fl::add_timeout(0.8, (Fl_Timeout_Handler)requ);
    return Fl::run ();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  // never gets here
  return 0;
}

