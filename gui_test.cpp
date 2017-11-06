// This example will demonstrate the integration of
// the boost asio chatserver with a simple FLTK program
//
//
//
#include <assert.h>

/*#include "io.hpp"*/

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Toggle_Button.H>


Fl_Window chat   (1000, 600, "UberChat");
Fl_Input input1 (250, 400, 600, 25, "Message: ");
Fl_Button create (50, 20, 150, 50, "Create Chatroom");
Fl_Button join (50, 90, 150, 50, "Join Chatroom");
Fl_Multiline_Output users (50, 160, 100, 200, "Users");
Fl_Button uuid (50, 380, 100, 50,"Request UUID");
Fl_Button nick (50, 450, 100, 50, "Change Nick");
Fl_Button quit  (50, 520, 50,20,"Quit");
Fl_Button clear (50, 560, 50,20,"Clear");
Fl_Text_Buffer *buff = new Fl_Text_Buffer ();
Fl_Text_Display *disp = new Fl_Text_Display (250,100,600,250,"chat");

Fl_Window login (250, 100, "Uberchat Login");
Fl_Input input2 (75, 20, 100, 25, "Nick");
Fl_Button submit (75, 50, 50, 25, "Submit");

Fl_Window c_room (300, 100, "Create Room");
Fl_Input input3 (100, 20, 100, 25, "Room Name");
Fl_Toggle_Button pri (210, 20, 50, 25, "private");
Fl_Button make (100, 50, 50, 25, "Submit");

Fl_Window j_room (500, 100, "Join Room");
Fl_Input input4 (300, 20, 100, 25, "Room Name");
Fl_Multiline_Output (50, 20, 200, 70, "Rooms");
Fl_Button enter (300, 50, 50, 25, "Submit");

Fl_Window c_nick (250, 100, "Change Nick");
Fl_Input input5 (100, 20, 100, 25, "New Nick");
Fl_Button Change (100, 50, 50, 25, "Submit");

chat_client *c = NULL;
std::thread *t = NULL;

/*static void cb_recv ( std::string S )
{
  // Note, this is an async callback from the perspective
  // of Fltk..
  //
  // high chance of a lock needed here if certain fltk calls
  // are made.  (like show() .... )
  std::string T = S + '\n' + '\0';
  if (buff)
  {
    buff->append ( T.c_str () );
  }
  if (disp)
  {
    disp->show ();
  }

  chat.show ();
}*/

static void cb_clear ()
{
   if (buff)
   {
     buff->remove (0,  buff->length () );
   }
   // may need to call show() ?
}

static void cb_login()
{
	login.hide();
	chat.show();
}

static void cb_join()
{
	chat.hide();
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
	chat.show();
}

static void cb_cnick()
{
	c_nick.hide();
	chat.show();
}

static void cb_joined()
{
	j_room.hide();
	chat.show();
}

static void cb_uuid()
{
	printf("UUID\n");
}

static void cb_quit ( )
{
  // this is where we exit to the operating system
  // any clean up needs to happen here
  //
  /*if (c)
  {
    c->close();
  }
  if (t)
  {
     t->join();
  }*/

  exit (0);
}
/*static void cb_input1 (Fl_Input*, void * userdata) 
{
  chat_message msg;
  msg.body_length(std::strlen( ( const char *) input1.value ()) + 1);
  // ensure it is null terminated
  std::memset (msg.body(), 0, msg.body_length());
  // copy over the payload
  std::memcpy (msg.body(), ( const char *) input1.value (), msg.body_length()-1);
  msg.encode_header();
  std::cout << "sent " << msg.body() << std::endl;
  c->write(msg);
}*/

int main ( int argc, char** argv ) 
{

  chat.begin ();
    chat.add (input1);
    input1.callback ((Fl_Callback*)cb_input1,( void *) "Enter next:");
    input1.when ( FL_WHEN_ENTER_KEY );
    quit.callback (( Fl_Callback*) cb_quit );
    clear.callback (( Fl_Callback*) cb_clear );
    create.callback((Fl_Callback*)cb_make);
    join.callback((Fl_Callback*)cb_join);
   nick.callback((Fl_Callback*)cb_nick);
    chat.add (quit);
   uuid.callback((Fl_Callback*)cb_uuid);
    disp->buffer(buff);
  chat.end ();

  login.begin();
	login.add(input2);
	login.add(submit);
	submit.callback((Fl_Callback*) cb_login);
  login.end();

  c_room.begin();
	c_room.add(input3);
	c_room.add(pri);
	c_room.add(make);
	make.callback((Fl_Callback*) cb_made);
  c_room.end();

  j_room.begin();
	j_room.add(input4);
	j_room.add(enter);
	enter.callback((Fl_Callback*) cb_joined);
  j_room.end();

  c_nick.begin();
	c_nick.add(input5);
	c_nick.add(Change);
	Change.callback((Fl_Callback*) cb_cnick);
  j_room.end();

  login.show ();

Fl::run ();

  /*try
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
    return Fl::run ();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  // never gets here
  return 0;*/
}
