/*
 * Simplified RTS2 telescope pointing model.
 * Copyright (C) 2015 Petr Kubanek <petr@kubanek.net>
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

#ifndef __RTS2_RTS2MODEL__
#define __RTS2_RTS2MODEL__

#include "telmodel.h"
#include "teld.h"

namespace rts2telmodel
{

/**
 * Telescope pointing model. Based on the following article:
 *
 * ftp://ftp.lowell.edu/pub/buie/idl/pointing/pointing.pdf
 *
 * @author Petr Kubanek <petr@kubanek.net>
 */
class RTS2Model:public TelModel
{
	public:
		RTS2Model (rts2teld::Telescope * in_telescope, const char *in_modelFile);
		virtual ~ RTS2Model (void);

		/**
		 * Accepts RTS2_MODEL, as generated by model-fit.py script.
		 */
		virtual int load ();

		virtual int apply (struct ln_equ_posn *pos);
		virtual int applyVerbose (struct ln_equ_posn *pos);

		virtual int reverse (struct ln_equ_posn *pos);
		virtual int reverseVerbose (struct ln_equ_posn *pos);
		virtual int reverse (struct ln_equ_posn *pos, double sid);

		virtual std::istream & load (std::istream & is);
		virtual std::ostream & print (std::ostream & os);
	
	private:
		double params[9];
};

}
