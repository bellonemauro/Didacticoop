/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Didacticoop srls, http://www.maurobellone.com/                            |
*  | Didacticoop 1.0                                                           |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Didacticoop_TOOLS_H_
#define Didacticoop_TOOLS_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_tools.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QSettings>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>


using namespace std;


class Didacticoop_tools : public  QMainWindow
{
	Q_OBJECT

/** Create signals to be passed to the main app
* 
*/
signals :
	void ok();  // generated when ok is pressed
	void apply(); // generated when apply is pressed
	void cancel(); // generated when cancel is pressed

public:

	explicit Didacticoop_tools(QWidget *parent = 0);

	~Didacticoop_tools();

	bool setLoadSettingsFileName(QString _filename) { 
		m_setting_file_name = _filename; 
		return loadSettings(m_setting_file_name);
	}

	bool setFileNameAndSaveSettings(QString _filename) {
		m_setting_file_name = _filename;
		return saveSettings(_filename);
	}

	QString getSerialPortName() { return m_serial->portName(); }

	QString getStudentName() {
		return ui_tools->lineEdit_userName->text();
	}

	void tryConnect();


private slots:

	void goToPage1();
	void goToPage2();
	void goToPage3();

	/** Show information for the serial port communication
	* \note
	*/
	void showPortInfo(int idx);


	/** emit ok signal, save the setting, send the current macro to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit cancel signal and close the window
	* \note
	*/
	void cancelPressed();

	/** emit apply signal, save the setting, and send the current macro to the main
	*   
	*/
	void applyPressed();


	/** Load an ini setting file
	* in the GUI initialization it takes a default value ./settings/setting.ini
	* \note
	*/
	bool loadSettings(QString _path = QString("./settings/setting.ini"));

	/** save the setting file
	*  
	* \note
	*/
	bool saveSettings(QString _file_name = QString("./settings/setting_save.ini"));

	/** Reset to default values
	*  
	* \note
	*/
	void resetToDefaultValues();

	void enumeratePorts();

	void disconnectCOM();

	void connectCOM();

	void handleError(QSerialPort::SerialPortError error);

	void handleTimeout();

	void handleReadyRead();

	void readData();

	void sendData();



private:

	QSettings *m_settings;

	QString m_setting_file_name;

	QSerialPort *m_serial;
	QByteArray  m_data;
	QTimer      m_timer;

	std::vector<double> * m_IR_measure;

protected:
	Ui::Didacticoop_tools *ui_tools;    //!<  the user interface
};


#endif /* Didacticoop_TOOLS_H_ */
