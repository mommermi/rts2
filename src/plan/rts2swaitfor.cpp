/*
 * Wait for some event or for a number of seconds.
 * Copyright (C) 2007-2008 Petr Kubanek <petr@kubanek.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "rts2swaitfor.h"

void
Rts2SWaitFor::getDevice (char new_device[DEVICE_NAME_SIZE])
{
	strncpy (new_device, deviceName.c_str (), DEVICE_NAME_SIZE);
}


Rts2SWaitFor::Rts2SWaitFor (Rts2Script * in_script, const char *new_device,
char *in_valueName, double in_tarval,
double in_range):
Rts2ScriptElement (in_script)
{
	valueName = std::string (in_valueName);
	deviceName = std::string (new_device);
	// if value contain device..
	tarval = in_tarval;
	range = in_range;
	setIdleTimeout (1);
}


int
Rts2SWaitFor::defnextCommand (Rts2DevClient * client,
Rts2Command ** new_command,
char new_device[DEVICE_NAME_SIZE])
{
	return idle ();
}


int
Rts2SWaitFor::idle ()
{
	Rts2Value *val =
		script->getMaster ()->getValue (deviceName.c_str (), valueName.c_str ());
	if (!val)
	{
		Rts2Address *add =
			script->getMaster ()->findAddress (deviceName.c_str ());
		// we will get device..
		if (add != NULL)
			return NEXT_COMMAND_KEEP;
		logStream (MESSAGE_ERROR) << "Cannot get value " << deviceName << "." <<
			valueName << sendLog;
		return NEXT_COMMAND_NEXT;
	}
	if (fabs (val->getValueDouble () - tarval) <= range)
	{
		return NEXT_COMMAND_NEXT;
	}
	return Rts2ScriptElement::idle ();
}


Rts2SSleep::Rts2SSleep (Rts2Script * in_script, double in_sec):Rts2ScriptElement
(in_script)
{
	sec = in_sec;
}


int
Rts2SSleep::defnextCommand (Rts2DevClient * client,
Rts2Command ** new_command,
char new_device[DEVICE_NAME_SIZE])
{
	if (!isnan (sec))
	{
		// this caused idle to be called after sec..
		// Rts2ScriptElement keep care that it will not be called before sec expires
		setIdleTimeout (sec);
		return NEXT_COMMAND_KEEP;
	}
	return NEXT_COMMAND_NEXT;
}


int
Rts2SSleep::idle ()
{
	sec = nan ("f");
	return NEXT_COMMAND_NEXT;
}
