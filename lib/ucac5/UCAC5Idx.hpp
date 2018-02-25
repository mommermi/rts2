#ifndef __UCAC5IDX__
#define __UCAC5IDX__

/**
 * Class for quick search in UCAC5 indices.
 *
 * @author Petr Kubanek <petr@rts2.org>
 */

#include "Vector.h"

#include <sys/types.h>

class UCAC5Idx
{
	public:
		UCAC5Idx ();
		virtual ~UCAC5Idx ();

		int openIdx (const char *idx);

		int select (size_t offset, size_t length);

		/**
		 * Returns index of the next matching star
		 */
		int nextMatched (Vector *fc, double minRad, double maxRad);
	
	private:
		int fd;
		Vector *data;
		size_t dataSize;
		Vector *current;
		Vector *currentEnd;
};

#endif // !__UCAC5IDX__
