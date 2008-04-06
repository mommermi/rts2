/* 
 * User set.
 * Copyright (C) 2008 Petr Kubanek <petr@kubanek.net>
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

#include "rts2userset.h"
#include "../utils/rts2app.h"

int
Rts2UserSet::load ()
{
	EXEC SQL BEGIN DECLARE SECTION;
		int db_id;
		VARCHAR db_login[25];
		VARCHAR db_email[200];
	EXEC SQL END DECLARE SECTION;

	EXEC SQL BEGIN TRANSACTION;

	EXEC SQL DECLARE user_cur CURSOR FOR
		SELECT
			usr_id,
			usr_login,
			usr_email
		FROM
			users
			ORDER BY
			usr_login asc;

	EXEC SQL OPEN user_cur;

	while (1)
	{
		EXEC SQL FETCH next FROM user_cur INTO
				:db_id,
				:db_login,
				:db_email;
		if (sqlca.sqlcode)
			break;
		push_back (Rts2User (db_id, std::string (db_login.arr), std::string (db_email.arr)));
	}
	if (sqlca.sqlcode != ECPG_NOT_FOUND)
	{
		logStream (MESSAGE_ERROR) << "Rts2UserSet::load cannot load user set " << sqlca.sqlerrm.sqlerrmc << sendLog;
		EXEC SQL ROLLBACK;
		return -1;
	}

	EXEC SQL COMMIT;

	// load types
	for (Rts2UserSet::iterator iter = begin (); iter != end (); iter++)
	{
		int ret = (*iter).loadTypes ();
		if (ret)
			return ret;
	}

	return 0;
}


Rts2UserSet::Rts2UserSet ()
{
	load ();
}


Rts2UserSet::~Rts2UserSet (void)
{
}


std::ostream & operator << (std::ostream & _os, Rts2UserSet & userSet)
{
	for (Rts2UserSet::iterator iter = userSet.begin (); iter != userSet.end (); iter++)
	{
		_os << (*iter);
	}
	return _os;
}
