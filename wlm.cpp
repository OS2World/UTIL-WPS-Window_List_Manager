///////////////////////////////////////////////////////////////////////////
//
//	WLM.CPP
//	Window List Manager
//	
//	Written by Scott Glenn, 1996
//
//
//
//	$Revision: 1.2 $
//
///////////////////////////////////////////////////////////////////////////


#define INCL_WINSWITCHLIST
#define INCL_DOSERRORS
#include <OS2.H>

#include <iostream.h>
#include <stdlib.h>

const char Version[] = "v2.0";

// Action names
const char RemoveAll[] 	= "/RA";
const char ListInvis[]	= "/INV";
const char Help[]			= "/?";

int main( int argc, char* argv[] )
{

	// Print Program Header
	cout << endl << "Window List Manager " << Version << endl;
	cout << "Copyright 1996 Scott Glenn" << endl << endl;

	HAB      hab        = NULLHANDLE;
	HWND     hwndFrame  = NULLHANDLE;
	ULONG    cbItems    = 0,            /* Number of items in list */
				ulBufSize  = 0;            /* Size of buffer for information */
	PSWBLOCK pswblk     = NULL;         /* Pointer to information returned */


	SWCNTRL swcntrl;	// Holds one entry in the list

	// Available actions to be performed
	enum Action { NONE,
					  LIST_INV,
					  LIST_VIS,
					  REMOVE_ONE,
					  REMOVE_ALL,
					  MAKE_VIS,
					  INVALID_PARAM,
					  SHOW_USAGE
					};

	// Return code for API calls
	APIRET rc;

	// Variables passed to the appropriate action
	Action action;
	HSWITCH HSwitch;

	/////////////////////////////////////////////////////////////////////////
	// Determine the action desired by checking the command line arguments

	if ( argc == 2 )
	{
		// Check for HSWITCH number
		if ( argv[ 1 ] [ 0 ] != '/' )
		{
			// It is either name or number

			// Right now number only
			action = REMOVE_ONE;

			HSwitch = atol( argv[ 1 ] );

		}
		else
		{
			// It is a slashed action
			if ( strcmp( strupr( argv[ 1 ] ), RemoveAll ) == 0 )
			{
				// Remove all
				action = REMOVE_ALL;
			}
			else if ( strcmp( strupr( argv[ 1 ] ), ListInvis ) == 0 )
			{
				// List invisible entries
				action = LIST_INV;
			}
			else if ( strcmp( strupr( argv[ 1 ] ), Help ) == 0 )
			{
				// List usage
				action = SHOW_USAGE;
			}
			else
			{
				// Invalid parameter
				action = INVALID_PARAM;
			}
		}

	}
	else if ( argc == 1 )
	{
		// Just want a list of visibles
		action = LIST_VIS;
	}
	else
	{
		// Invalid command line params
		action = INVALID_PARAM;
	}



	/////////////////////////////////////////////////////////////////////////
	// Go ahead and get the window list entries

	// Get number of items in task list
   cbItems = WinQuerySwitchList(hab, NULL, 0);

	// Compute the amount of memory needed
	ulBufSize = (cbItems * sizeof(SWENTRY)) + sizeof(HSWITCH);

	// Allocate the buffer to hold the info
	pswblk = (PSWBLOCK) new char[ ulBufSize ];

	// Call query again but this time to fill our block with info
	cbItems = WinQuerySwitchList(hab, pswblk, ulBufSize);



	////////////////////////////////////////////////////////////////////////
	//
	//	Perform the appropriate action
	//
	switch( action )
	{



		case LIST_VIS:
		{
	
			// Wants a list of the visible names and switch numbers
		
			// Print a little header
			cout << "HSWITCH : Program Title in Task List (Visible)" << endl;
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
		break;

		////////////////////////////////////////////////////////////////
		// List all entries including invisible ones
		//
		case LIST_INV:
		{
	
			// Wants a list of the invisible names and switch numbers
		
			// Print a little header
			cout << "HSWITCH : Program Title in Task List (Invisible)" << endl;
			cout << "-------- ----------------------------------------------------" << endl;
		
			// Count the loop in reverse
			// since that is the way the task list shows them
			for ( ULONG count = cbItems; count != 0; count-- )
			{
		
				// Only print VISIBLE entries in task list
				if ( pswblk->aswentry[ count].swctl.uchVisibility == SWL_INVISIBLE )
				{

					// Don't print the 0 hswitch entry, it isn't a valid
					// program
					if ( pswblk->aswentry[ count ].hswitch != 0 )
					{
						// Print the PID followed by the name
						cout.width( 7 );
						cout << pswblk->aswentry[ count ].hswitch << " : ";
			
						// Print the names of the tasks
						cout << pswblk->aswentry[ count ].swctl.szSwtitle << endl;
					}
				}
		
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////
		// Wants to remove one entry
		// (make it invisible)
		//
		case REMOVE_ONE:
		{
			// Wants to remove an entry

			// Command Line arg is passed in HSwitch

			APIRET rc;
	
			// Get the appropriate entry
			rc = WinQuerySwitchEntry( HSwitch, &swcntrl );
	
			if ( rc == NO_ERROR )
			{
	
				// Just make the entry invisible, instead of removing it

				// Toggle it's state

				if ( swcntrl.uchVisibility == SWL_VISIBLE )
				{
					// Make it invisible
					swcntrl.uchVisibility = SWL_INVISIBLE;
				}
				else
				{
					// Make it invisible
					swcntrl.uchVisibility = SWL_VISIBLE;
				}

				// Now save back the changes
				rc = WinChangeSwitchEntry( HSwitch, &swcntrl );

				if ( ( rc == NO_ERROR ) && ( swcntrl.uchVisibility == SWL_INVISIBLE ) )
				{
					cout << "HSWITCH " << HSwitch << " removed from list." << endl;
				}
				else if ( ( rc == NO_ERROR ) && ( swcntrl.uchVisibility == SWL_VISIBLE ) )
				{
					cout << "HSWITCH " << HSwitch << " placed on list." << endl;
				}
				else
				{
					// Error returned from API call
					cout << "Error changing HSWITCH " << HSwitch << " unable to change."
						  << endl;
				}


	
			}
			else
			{
				// Error occurred
				cout << "Invalid HSWITCH number, nothing removed from list." << endl;
			}
	
		}
		break;

		//////////////////////////////////////////////////////////////////////
		// Makes an invisible entry visible
		//
		case MAKE_VIS:
		{
			// Wants to make an invisible entry visible
	
			// Convert command line to a number
			HSWITCH HSwitch = atof( argv[ 1 ] );
	
			APIRET rc;
	
			// Get the appropriate entry
			rc = WinQuerySwitchEntry( HSwitch, &swcntrl );
	
			if ( rc == NO_ERROR )
			{
	
				// Just make the entry invisible, instead of removing it
	
				// Make it invisible
				swcntrl.uchVisibility = SWL_INVISIBLE;
		
				// Now save back the changes
				rc = WinChangeSwitchEntry( HSwitch, &swcntrl );
	
				cout << "HSWITCH " << HSwitch << " removed from list." << endl;
	
			}
			else
			{
				// Error occurred
				cout << "Invalid HSWITCH number, nothing removed from list." << endl;
			}
	
		}
		break;



		///////////////////////////////////////////////////////////////////
		// Remove all entries in the list
		// Actually make them all invisible
		case REMOVE_ALL:
		{
	
			// Traverse the entries
			// Skip the first one
			// since it is 0 and not valid

			cout << "Removing All Entries in List." << endl;

			for ( ULONG count = 0; count < cbItems; count++ )
			{
		
				// Don't erase the 0 hswitch entry, it isn't a valid
				// program
				if ( ( pswblk->aswentry[ count ].hswitch != 0 ) &&
					( pswblk->aswentry[ count ].swctl.uchVisibility == SWL_VISIBLE ) )
				{
					// Make the entry invisible
					pswblk->aswentry[ count ].swctl.uchVisibility = SWL_INVISIBLE;

					// Save the changes
					// Now save back the changes

					rc = WinChangeSwitchEntry( pswblk->aswentry[ count ].hswitch, &( pswblk->aswentry[ count ].swctl ) );

					if ( rc == NO_ERROR )
					{

						// Worked
						cout << "Removed " << pswblk->aswentry[ count ].swctl.szSwtitle << endl;

					}
					else
					{
						// Failed
						cout << "Unable to remove " << pswblk->aswentry[ count ].swctl.szSwtitle << endl;
					}

				}
				
			}

			cout << "Done." << endl << endl;

		}
		break;


		///////////////////////////////////////////////////////////////////////
		// Invalid command line argument
		//
		case INVALID_PARAM:
		{

			// Invalid parameters
			cout << "Invalid Command Line Parameters." << endl;

			// Drop on through and show correct usage

		}

		/////////////////////////////////////////////////////////////////////
		// Show command line usage
		//
		case SHOW_USAGE:
		{

			// Display the available modes
			cout << "Available WLM Modes:" << endl;
			cout << "	To Retrieve list of visible list entries:" << endl
				  << "		WLM" << endl;

			cout << "	To Retrieve list of visible and invisible entries (Show All):" << endl
				  << "		WLM /inv" << endl;

			cout << "	To Remove a visible entry by HSWITCH number:" << endl
				  << "		WLM <HSWITCH Number>" << endl;

			cout << "	To Put an invisible entry back on the list:" << endl
				  << "		WLM <HSWITCH Number>" << endl;

			// cout << "	To Remove an entry by full or partial name:" << endl
			//	  << "		WLM <Full or Partial Name>" << endl;

			cout << "	To Remove all visible entries in the list (Remove All): " << endl
				  << "		WLM /ra" << endl;

			cout << endl;
		}


	}	// End of switch( action )

	// Free up the memory
	delete pswblk;

	return 0;

}



