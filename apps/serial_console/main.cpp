/*  +---------------------------------------------------------------------------+
 *  |                                                                           |
 *  |                      http://www.maurobellone.com                          |
 *  |                                                                           |
 *  | Copyright (c) 2017, - All rights reserved.                                |
 *  | Authors: Mauro Bellone                                                    |
 *  | Released under ___ License.                                               |
 *  +---------------------------------------------------------------------------+ */

//uncomment to hide the console when the app starts
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "serial_console.h"

  
int main (int argc, char *argv[])
{	
  QApplication a (argc, argv);
  serial_console window;

  window.show ();
  return a.exec ();
}
