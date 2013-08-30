#define INCL_WINSWITCHLIST  /* Or INCL_WIN, INCL_PM */
#include <OS2.H>

#include <iostream.h>
#include <stdlib.h>



int main( int argc, char* argv[] )
{

	// Print Program Header
	cout << endl << "Window List Manager v1.0" << endl;
	cout << "Copyright 1996 Scott Glenn" << endl << endl;

	HAB      hab        = NULLHANDLE;
	HWND     hwndFrame  = NULLHANDLE;
	ULONG    cbItems    = 0,            /* Number of items in list */
				ulBufSize  = 0;            /* Size of buffer for information */
	PSWBLOCK pswblk     = NULL;         /* Pointer to information returned */

	// Get number of items in task list
	cbItems = WinQuerySwitchList(hab, NULL, 0);

	// Compute the amount of memory needed
	ulBufSize = (cbItems * sizeof(SWENTRY)) + sizeof(HSWITCH);

	// Allocate the buffer to hold the info
	pswblk = (PSWBLOCK) new char[ ulBufSize ];

	// Call query again but this time to fill our block with info
	cbItems = WinQuerySwitchList(hab, pswblk, ulBufSize);

	///////////////////////////////////////////////////////////////////////
	// Check to see if want list
	// or want to remove an entry


	// Wants a list of the names and PID's
	if ( argc == 1 )
	{

		// Print a little header
		cout << "HSWITCH : Program Title in Task List" << endl;
		cout << "-------- ----------------------------------------------------" << endl;
	
		// Count the loop in reverse
		// since that is the way the task list shows them
		for ( ULONG count = cbItems; count != 0; count-- )
		{
	
			// Only print VISIBLE entries in task list
			if ( pswblk->aswentry[ count].swctl.uchVisibility == SWL_VISIBLE )
			{
				// Print the PID followed by the name
				cout.width( 7 );
				cout << pswblk->aswentry[ count ].hswitch << " : ";
	
				// Print the names of the tasks
				cout << pswblk->aswentry[ count ].swctl.szSwtitle << endl;
			}
	
		}

	}
	else if ( argc == 2 )
	{
		// Wants to remove an entry

		// Convert command line to a number
		int HSwitch = atof( argv[ 1 ] );

		APIRET rc;

		// Try to remove the entry
		rc = WinRemoveSwitchEntry( HSwitch );

		if ( rc == 0 )
		{
			// No problem
			cout << "HSWITCH " << HSwitch << " removed from list." << endl;
		}
		else
		{
			// Error occurred
			cout << "Invalid HSWITCH number, nothing removed from list." << endl;
		}

	}
	else
	{
		// Invalid parameters
		cout << "Invalid Number of Parameters." << endl;
	}

	// Free up the memory
	delete pswblk;

	return 0;

}





