/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Didacticoop srls, http://www.maurobellone.com/                            |
*  | Didacticoop 1.0                                                           |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */



#include <QSplashScreen>
#include <QTimer>
#include <iostream>
#include <didacticoop.h>

//uncomment to hide the console when the app starts
#define HIDE_TERMINAL
#ifdef HIDE_TERMINAL
#if defined (_WIN64) || defined (_WIN32)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#else
// define it for a Unix machine
#endif
#endif




int main(int argc, char *argv[])
{

	try {

		QApplication a(argc, argv);
		
		QSplashScreen *s = new QSplashScreen();
		s->setPixmap(QPixmap(":/icons/galileoIcon.png"));
		s->show();
		QTimer::singleShot(1, s, SLOT(close()));

		//Sleep(500);

		didacticoop window;
		

#ifdef SOFTWARE_VERSION
		//window.setVersion(version);
#endif


		// get the screen resolution of the current screen
		// so we can resize the application in case of small screens
		QRect rec = QApplication::desktop()->screenGeometry();
		int screen_height = rec.height();
		int screen_width = rec.width();

		//window.showFullScreen();
		//window.showMaximized();
		//window.move(QPoint(50, 50));
		//window.show ();
		if (screen_width < 1400)
			window.showMaximized();
		//	QTimer::singleShot(5000, &window, SLOT(showMaximized()));
		else
			window.show();
		//	QTimer::singleShot(5000, &window, SLOT(show()));
		
		s->close();
		return a.exec ();

		//a.exec();
	}
	catch (std::exception &e) {
		std::cerr << " main ::: Unhandled Exception: " << e.what() << endl;
		// clean up here, e.g. save the session
		// and close all config files.
		std::cout << " Something really bad just happend, press ok to exit " << std::endl;
		std::cin.get();
		return 0; // exit the application
	}
}
