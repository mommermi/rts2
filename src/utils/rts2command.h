#ifndef __RTS2_COMMAND__
#define __RTS2_COMMAND__

#include "rts2block.h"

#define RTS2_COMMAND_REQUE	-5

class Rts2Command
{
protected:
  Rts2Block * owner;
  Rts2Conn *connection;
  char *text;
public:
    Rts2Command (Rts2Block * in_owner);
    Rts2Command (Rts2Block * in_owner, char *in_text);
    virtual ~ Rts2Command (void);
  int setCommand (char *in_text);
  void setConnection (Rts2Conn * conn)
  {
    connection = conn;
  }
  virtual int send ()
  {
    return connection->send (text);
  }
  int commandReturn (int status);
  char *getText ()
  {
    return text;
  }
  virtual int commandReturnFailed (int status);
  virtual int commandReturnOK ();
};

class Rts2CentraldCommand:public Rts2Command
{

public:
  Rts2CentraldCommand (Rts2Block * in_owner,
		       char *in_text):Rts2Command (in_owner, in_text)
  {
  }
};

class Rts2CommandSendKey:public Rts2Command
{
private:
  int key;
public:
    Rts2CommandSendKey (Rts2Block * in_master, int in_key);
  virtual int send ();

  virtual int commandReturnOK ()
  {
    connection->setConnState (CONN_CONNECTED);
    return -1;
  }
  virtual int commandReturnFailed ()
  {
    connection->setConnState (CONN_AUTH_FAILED);
    return -1;
  }
};

class Rts2CommandAuthorize:public Rts2Command
{
public:
  Rts2CommandAuthorize (Rts2Block * in_master, const char *device_name);
};

#endif /* !__RTS2_COMMAND__ */
