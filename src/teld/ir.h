#ifndef __RTS2_TELD_IR__
#define __RTS2_TELD_IR__

#include <errno.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <libnova/libnova.h>

#include "status.h"
#include "telescope.h"

#include <cstdio>
#include <cstdarg>
#include <opentpl/client.h>
#include <list>
#include <iostream>

using namespace OpenTPL;

class ErrorTime
{
  time_t etime;
  int error;
public:
    ErrorTime (int in_error);
  int clean (time_t now);
  int isError (int in_error);
};

class Rts2TelescopeIr:public Rts2DevTelescope
{
private:
  std::string * ir_ip;
  int ir_port;
  Client *tplc;
  double cover;
  enum
  { OPENED, OPENING, CLOSING, CLOSED } cover_state;

  void addError (int in_error);

  void checkErrors ();
  void checkCover ();
  void checkPower ();

  void initCoverState ();

    std::list < ErrorTime * >errorcodes;

protected:
    time_t timeout;

    template < typename T > int tpl_get (const char *name, T & val,
					 int *status);
    template < typename T > int tpl_set (const char *name, T val,
					 int *status);
    template < typename T > int tpl_setw (const char *name, T val,
					  int *status);
  virtual int processOption (int in_opt);

  virtual int coverClose ();
  virtual int coverOpen ();
public:
    Rts2TelescopeIr (int argc, char **argv);
    virtual ~ Rts2TelescopeIr (void);
  virtual int initIrDevice ();
  virtual int init ();
  virtual int initValues ();
  virtual int idle ();
  virtual int ready ();
  virtual int info ();
  virtual int startPark ();
  virtual int isParking ();
  virtual int endPark ();
  virtual int correct (double cor_ra, double cor_dec, double real_ra,
		       double real_dec);
  virtual int saveModel ();
  virtual int loadModel ();
  virtual int resetMount (resetStates reset_mount);

  virtual int getError (int in_error, std::string & desc);
};

#endif /* !__RTS2_TELD_IR__ */
