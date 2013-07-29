//############################################################################
//##                                                                        ##
//##  EXAMMSS.C                                                             ##
//##                                                                        ##
//##  Example of using Bink with Miles and BinkBuffers on Win32             ##
//##                                                                        ##
//##  Author: Jeff Roberts                                                  ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Copyright (C) RAD Game Tools, Inc.                                    ##
//##                                                                        ##
//##  For technical support, contact RAD Game Tools at 425 - 893 - 4300.    ##
//##                                                                        ##
//############################################################################


#include <windows.h>
#include <mmsystem.h>

#include "bink.h"

#include "mss.h"

//
// Example globals
//

static HBINK Bink = 0;
static HBINKBUFFER Bink_buffer = 0;

// undefine and play with a movie that uses 4 sound tracks (FLR, C, S, BLR)
//   to use 5.1 audio

#define BINK51

//############################################################################
//##                                                                        ##
//## Clear_to_black - just fills a window with black pixels.                ##
//##                                                                        ##
//############################################################################

static void Clear_to_black( HWND window )
{
  PAINTSTRUCT ps;
  HDC dc;

  //
  // Get the repaint DC and then fill the window with black.
  //

  dc = BeginPaint( window, &ps );

  PatBlt( dc, 0, 0, 4096, 4096, BLACKNESS );

  EndPaint( window, &ps );
}


//############################################################################
//##                                                                        ##
//## WindowProc - the main window message procedure.                        ##
//##                                                                        ##
//############################################################################

LRESULT WINAPI WindowProc( HWND   window,
                           UINT   message,
                           WPARAM wparam,
                           LPARAM lparam )
{

  switch( message )
  {
    //
    // Just close the window if the user hits a key.
    //

    case WM_CHAR:
      DestroyWindow( window );
      break;

    //
    // Pause/resume the video when the focus changes.
    //

    case WM_KILLFOCUS:
      BinkPause( Bink, 1 );
      break;

    case WM_SETFOCUS:
      BinkPause( Bink, 0 );
      break;

    //
    // Handle the window paint messages.
    //

    case WM_PAINT:
      Clear_to_black( window );

      //
      // Redraw the frame (or the color mask for overlays).
      //

      if ( Bink_buffer )
        BinkBufferBlit( Bink_buffer, 0, 1 );

      return( 0 );

    case WM_ERASEBKGND:
      return( 1 );

    //
    // Handle the window being moved.
    //

    case WM_WINDOWPOSCHANGING:
      //
      // Is the window even being moved?
      //

      if ( ! ( ( ( WINDOWPOS* )lparam )->flags & SWP_NOMOVE ) )
      {

        if ( Bink_buffer )
        {
          S32 x,y;

          //
          // Yup, it's being moved - ask the BinkBuffer API to align the
          //   coordinates to a fast boundary.
          //

          x = ( ( WINDOWPOS* )lparam )->x;
          y = ( ( WINDOWPOS* )lparam )->y;
          BinkBufferCheckWinPos( Bink_buffer, &x, &y );
          ( ( WINDOWPOS* )lparam )->x = x;
          ( ( WINDOWPOS* )lparam )->y = y;
        }

      }
      break;

    case WM_WINDOWPOSCHANGED:
      //
      // Tell the BinkBuffer API when the window moves.
      //

      if ( Bink_buffer )
        BinkBufferSetOffset( Bink_buffer, 0, 0 );
      break;

    //
    // Post the quit message.
    //

    case WM_DESTROY:
      PostQuitMessage( 0 );
      return( 0 );
  }

  //
  // Call the OS default window procedure.
  //

  return( DefWindowProc( window, message, wparam, lparam ) );
}


//############################################################################
//##                                                                        ##
//## Build_window_handle - creates a window class and window handle.        ##
//##                                                                        ##
//############################################################################

static HWND Build_window_handle( HINSTANCE instance,
                                 HINSTANCE previous_instance )
{
  //
  // Create the window class if this is the first instance.
  //

  if ( !previous_instance )
  {
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon( instance, MAKEINTRESOURCE( 101 ) );
    wc.hCursor = LoadCursor( 0, IDC_ARROW );;
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = "BinkExam";

    //
    // Try to register the class.
    //

    if ( !RegisterClass( &wc ) )
    {
      return( 0 );
    }
  }

  //
  // Return the new window with a tiny initial default size (it is resized
  //   later on with the help of the BinkBuffer API).
  //

  return( CreateWindow( "BinkExam",
                        "Bink Example Player",
                        WS_CAPTION|WS_POPUP|WS_CLIPCHILDREN|
                        WS_SYSMENU|WS_MINIMIZEBOX,
                        64, 64,
                        64, 64,
                        0, 0, instance,0 ) );
}


//############################################################################
//##                                                                        ##
//## Show_next_frame - advances to the next Bink frame.                     ##
//##                                                                        ##
//############################################################################

static void Show_next_frame( HBINK bink,
                             HBINKBUFFER bink_buffer,
                             HWND window )
{
  //
  // Decompress the Bink frame.
  //

  BinkDoFrame( bink );

  //
  // Lock the BinkBuffer so that we can copy the decompressed frame into it.
  //

  if ( BinkBufferLock( bink_buffer ) )
  {
    //
    // Copy the decompressed frame into the BinkBuffer (this might be on-screen).
    //

    BinkCopyToBuffer( bink,
                      bink_buffer->Buffer,
                      bink_buffer->BufferPitch,
                      bink_buffer->Height,
                      0,0,
                      bink_buffer->SurfaceType);

    //
    // Unlock the BinkBuffer.
    //

    BinkBufferUnlock( bink_buffer );
  }

  //
  // Tell the BinkBuffer to blit the pixels onto the screen (if the
  //   BinkBuffer is using an off-screen blitting style).
  //

  BinkBufferBlit( bink_buffer,
                  bink->FrameRects,
                  BinkGetRects( bink, bink_buffer->SurfaceType ) );

  //
  // Are we at the end of the movie?
  //

  if ( bink->FrameNum == bink->Frames )
  {
    //
    // Yup, close the window.
    //

    DestroyWindow( window );
  }
  else
  {
    //
    // Nope, advance to the next frame.
    //

    BinkNextFrame( bink );
  }
}


//############################################################################
//##                                                                        ##
//## WinMain - the primary function entry point                             ##
//##                                                                        ##
//############################################################################

int PASCAL WinMain( HINSTANCE instance,
                    HINSTANCE previous_instance,
                    LPSTR cmd_line,
                    int cmd_show )
{
  //
  // Win32 locals.
  //

  HWND window = 0;
  MSG msg;

  //
  // Miles locals
  //

  HDIGDRIVER digital=0;


  //
  // Try to create our window.
  //

  window = Build_window_handle( instance,
                                previous_instance );
  if ( !window )
  {
    MessageBox( 0,
                "Error creating window.",
                "Windows",
                MB_OK | MB_ICONSTOP );
    return( 1 );
  }

  //
  // Open the Miles Sound System.

  AIL_startup();

  #ifdef BINK51
  digital = AIL_open_digital_driver( 44100, 16, MSS_MC_51_DISCRETE, 0 );
  #else
  digital = AIL_open_digital_driver( 44100, 16, 2, 0 );
  #endif
  
  if ( !digital )
  {
    MessageBox( 0,
                AIL_last_error( ),
                "Error opening Miles",
                MB_OK | MB_ICONSTOP );
    return( 2 );
  }


  //
  //
  // Tell Bink to use the Miles Sound System (must be before BinkOpen)!
  //

  BinkSoundUseMiles( digital );

  //
  // Try to open the Bink file.
  //

  #ifdef BINK51
  {
    //
    // Tell Bink to play all of the 5.1 tracks
    //

    S32 volumes[ 6 ];
    U32 TrackIDsToPlay[ 4 ] = { 0, 1, 2, 3 };
  
    BinkSetSoundTrack( 4, TrackIDsToPlay );
  
    //
    // Try to open the Bink file.
    //
  
    Bink = BinkOpen( cmd_line, BINKSNDTRACK );
 
    //
    // Now route the sound tracks to the correct speaker
    //
  
    volumes[ 0 ] = 32768;  volumes[ 1 ] = 32768; volumes[ 2 ] = 0;  volumes[ 3 ] = 0; volumes[ 4 ] = 0;  volumes[ 5 ] = 0;
    BinkSetMixBinVolumes( Bink, 0, 0, volumes, 6 );
    volumes[ 0 ] = 0;  volumes[ 1 ] = 0; 

    volumes[ 2 ] = 32768;
    BinkSetMixBinVolumes( Bink, 1, 0, volumes, 6 );
    volumes[ 2 ] = 0;  

    volumes[ 3 ] = 32768;
    BinkSetMixBinVolumes( Bink, 2, 0, volumes, 6 );
    volumes[ 3 ] = 0;  

    volumes[ 4 ] = 32768; volumes[ 5 ] = 32768;
    BinkSetMixBinVolumes( Bink, 3, 0, volumes, 6 );
  }
  #else

    Bink = BinkOpen( cmd_line, 0 );

  #endif

  if ( !Bink )
  {
    MessageBox( 0,
                BinkGetError( ),
                "Bink Error",
                MB_OK | MB_ICONSTOP );

    DestroyWindow( window );
    return( 3 );
  }


  //
  // Try to open the Bink buffer.
  //

  Bink_buffer = BinkBufferOpen( window, Bink->Width, Bink->Height, 0 );
  if ( !Bink_buffer )
  {
    MessageBox( 0,
                BinkBufferGetError( ),
                "Bink Error",
                MB_OK | MB_ICONSTOP );

    DestroyWindow( window );
    BinkClose( Bink );

    return( 4 );
  }

  //
  // Size the window such that its client area exactly fits our Bink movie.
  //

  SetWindowPos( window, 0,
                0, 0,
                Bink_buffer->WindowWidth,
                Bink_buffer->WindowHeight,
                SWP_NOMOVE );

  //
  // Now display the window and start the message loop.
  //

  ShowWindow( window, cmd_show );

  for ( ; ; )
  {
    //
    // Are there any messages to handle?
    //

    if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
    {
      //
      // Yup, handle them.
      //

      if ( msg.message == WM_QUIT )
        break;

      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
    else
    {
      //
      // Is it time for a new Bink frame?
      //

      if ( !BinkWait( Bink ) )
      {
        //
        // Yup, draw the next frame.
        //

        Show_next_frame( Bink,
                         Bink_buffer,
                         window );
      }
      else
      {
        //
        // Nope, give the rest of the system a chance to run (1 ms).
        //

        Sleep( 1 );
      }

    }
  }

  //
  // Close the Bink file.
  //

  if ( Bink )
  {
    BinkClose( Bink );
    Bink = 0;
  }

  //
  // Close the Bink buffer.
  //

  if ( Bink_buffer )
  {
    BinkBufferClose( Bink_buffer );
    Bink_buffer = 0;
  }

  //
  // Close Miles.
  //

  if ( digital )
  {
    AIL_close_digital_driver( digital );
    digital = 0;
  }

  AIL_shutdown( );

  //
  // And exit.
  //

  return( 0 );
}

// some stuff for the RAD build utility
// @cdep pre $DefaultsWin3264EXE
// @cdep pre $set(INCs, $INCs -I$clipfilename($file)\..\..\..\..\mss\ship\sdk\win$platnum\include )
// @cdep pre $requiresbinary($clipfilename($file)\..\..\..\..\mss\ship\sdk\win$platnum\lib\mss$platnum.lib)
// @cdep pre $requiresbinary(winmm.lib)
// @cdep pre $requiresbinary(binkw$platnum.lib)
// @cdep post $BuildWin3264EXE

