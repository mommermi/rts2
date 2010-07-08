/* 
 * Driver for Hlohovec (Slovakia) 50cm telescope.
 * Copyright (C) 2009 Petr Kubanek <petr@kubanek.net>
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

#include "tgdrive.h"
#include "teld.h"

#define OPT_RA            OPT_LOCALHOST + 2201
#define OPT_DEC           OPT_LOCALHOST + 2202

using namespace rts2teld;

namespace rts2teld
{

/**
 * Class for Hlohovec (50cm) telescope.
 *
 * @author Petr Kubanek <petr@kubanek.net>
 */
class Hlohovec:public Telescope
{
	public:
		Hlohovec (int argc, char **argv);
		virtual ~Hlohovec ();
	protected:
		virtual void usage ();
		virtual int processOption (int opt);
		virtual int init ();
		virtual int info ();

		virtual int resetMount ();

		virtual int startResync ();
		virtual int stopMove ();
		virtual int endMove ();
		virtual int startPark ();
		virtual int endPark ();

		virtual int setValue (Rts2Value *old_value, Rts2Value *new_value);
	private:
		TGDrive *raDrive;
		TGDrive *decDrive;

		const char *devRA;
		const char *devDEC;

		Rts2ValueInteger *ra_rPos;
		Rts2ValueInteger *ra_dPos;
		Rts2ValueInteger *ra_aCur;
		Rts2ValueInteger *ra_appStatus;
		Rts2ValueInteger *ra_faults;

		Rts2ValueInteger *dec_rPos;
		Rts2ValueInteger *dec_dPos;
		Rts2ValueInteger *dec_aCur;
		Rts2ValueInteger *dec_appStatus;
		Rts2ValueInteger *dec_faults;
};

}

void Hlohovec::usage ()
{
	std::cout << "\t" << getAppName () << " --ra /dev/ttyS0 --dec /dev/ttyS1" << std::endl;
}

int Hlohovec::processOption (int opt)
{
	switch (opt)
	{
		case OPT_RA:
			devRA = optarg;
			break;
		case OPT_DEC:
			devDEC = optarg;
			break;
		default:
			return Telescope::processOption (opt);
	}
	return 0;
}

int Hlohovec::init ()
{
	int ret;
	ret = Telescope::init ();
	if (ret)
		return ret;

	if (devRA == NULL)
	{
		logStream (MESSAGE_ERROR) << "RA device file was not specified." << sendLog;
		return -1;
	}

	if (devDEC == NULL)
	{
		logStream (MESSAGE_ERROR) << "DEC device file was not specified." << sendLog;
	}

	raDrive = new TGDrive (devRA, this);
	raDrive->setDebug ();
	raDrive->setLogAsHex ();
	ret = raDrive->init ();
	if (ret)
		return ret;

	raDrive->write2b (TGA_MASTER_CMD, 2);
	raDrive->write2b (TGA_AFTER_RESET, TGA_AFTER_RESET_ENABLED);
	raDrive->write2b (TGA_MASTER_CMD, 5);
	raDrive->write4b (TGA_MODE, 0x4004);

	raDrive->write4b (TGA_ACCEL, 8947850);
	raDrive->write4b (TGA_DECEL, 8947850);
	raDrive->write4b (TGA_VMAX, 858993459);

	raDrive->write2b (TGA_DESCUR, 500);

	if (devDEC != NULL)
	{
		decDrive = new TGDrive (devDEC, this);
		decDrive->setDebug ();
		decDrive->setLogAsHex ();
		ret = decDrive->init ();
		if (ret)
			return ret;
		decDrive->write2b (TGA_MASTER_CMD, 0x02);
	}

	return 0;
}

int Hlohovec::info ()
{
	ra_dPos->setValueInteger (raDrive->read4b (TGA_TARPOS));
	ra_rPos->setValueInteger (raDrive->read4b (TGA_CURRPOS));
	ra_aCur->setValueInteger (raDrive->read2b (TGA_ACTCUR));
	ra_appStatus->setValueInteger (raDrive->read2b (TGA_STATUS));
	ra_faults->setValueInteger (raDrive->read2b (TGA_FAULTS));

	if (decDrive)
	{
		dec_dPos->setValueInteger (decDrive->read4b (TGA_TARPOS));
		dec_rPos->setValueInteger (decDrive->read4b (TGA_CURRPOS));
		dec_aCur->setValueInteger (decDrive->read2b (TGA_ACTCUR));
		ra_appStatus->setValueInteger (decDrive->read2b (TGA_STATUS));
		ra_faults->setValueInteger (decDrive->read2b (TGA_FAULTS));
	}

	return Telescope::info ();
}

int Hlohovec::resetMount ()
{
	try
	{
		raDrive->write2b (TGA_MASTER_CMD, 5);
		return Telescope::resetMount ();
	}
	catch (TGDriveError e)
	{
	  	logStream (MESSAGE_ERROR) << "error reseting mount" << sendLog;
		return -1;
	}
}

int Hlohovec::startResync ()
{
	return 0;
}

int Hlohovec::stopMove ()
{
	return 0;
}

int Hlohovec::endMove ()
{
	return 0;
}

int Hlohovec::startPark ()
{
	return 0;
}

int Hlohovec::endPark ()
{
	return 0;
}

int Hlohovec::setValue (Rts2Value *old_value, Rts2Value *new_value)
{
	if (old_value == ra_dPos)
	{
		try
		{
			raDrive->write4b (TGA_TARPOS, new_value->getValueInteger ());
			return 0;
		}
		catch (TGDriveError e)
		{
			return -2;
		}
	}
	return Telescope::setValue (old_value, new_value);
}

Hlohovec::Hlohovec (int argc, char **argv):Telescope (argc, argv)
{
	raDrive = NULL;
	decDrive = NULL;

	devRA = NULL;
	devDEC = NULL;

	createValue (ra_dPos, "AX_RA_T", "target RA position", true, RTS2_VALUE_WRITABLE);
	createValue (ra_rPos, "AX_RA_C", "current RA position", true);
	createValue (ra_aCur, "AX_RA_CU", "actual RA current", false);
	createValue (ra_appStatus, "AX_RA_S", "RA axis status", true, RTS2_DT_HEX);
	createValue (ra_faults, "AX_RA_F", "RA axis faults", true, RTS2_DT_HEX);

	createValue (dec_rPos, "AX_DEC_T", "target DEC position", true, RTS2_VALUE_WRITABLE);
	createValue (dec_dPos, "AX_DEC_C", "current DEC position", true);
	createValue (dec_aCur, "AX_DEC_CU", "actual DEC current", false);
	createValue (dec_appStatus, "AX_DEC_S", "DEC axis status", true, RTS2_DT_HEX);
	createValue (dec_faults, "AX_DEC_F", "DEC axis faults", true, RTS2_DT_HEX);

	addOption (OPT_RA, "ra", 1, "RA drive serial device");
	addOption (OPT_DEC, "dec", 1, "DEC drive serial device");
}

Hlohovec::~Hlohovec ()
{
	delete raDrive;
	delete decDrive;
}

int main (int argc, char **argv)
{
	Hlohovec device = Hlohovec (argc, argv);
	return device.run ();
}
