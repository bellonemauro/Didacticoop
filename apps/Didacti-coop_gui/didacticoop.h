/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Didacticoop srls, http://www.maurobellone.com/                            |
*  | Didacticoop 1.0                                                           |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef didacticoop_H_
#define didacticoop_H_


#include <QtWidgets>
#include <QtCore/qmath.h>
#include <QtCore/qstate.h>
#include <QMovie>
#include <QAction>
#include <QDateTime>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QGraphicsView>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/qvalueaxis.h>
#include <QPrinter>

#include <numeric>

#include "ui_didacticoop.h"
#include "didacticoop_hooke.h"
#include "tools.h"


class didacticoop_hooke;


class didacticoop : public  QMainWindow
{
	Q_OBJECT

		signals :
	void getData(const QByteArray &data);

public:
	explicit didacticoop( QMainWindow *parent = nullptr);

	~didacticoop();

private slots:

	void showToolDialog();

	void startStop(bool _enable);
	
	void simulationOnly();

	void about();

	void goToExp1();

	void goToExp2() { 
		ui->stackedWidget->setCurrentIndex(2);
		ui->actionHome->setEnabled(true);
	}

	void goToExp3() {
		ui->stackedWidget->setCurrentIndex(3); 
		ui->actionHome->setEnabled(true);
	}

	void goToExp4() { 
		ui->stackedWidget->setCurrentIndex(4); 
		ui->actionHome->setEnabled(true);
	}

	void backToIntro() { 
		ui->stackedWidget->setCurrentIndex(0); 
		ui->actionConnetti->setEnabled(false);
		ui->actionSimula->setEnabled(false);
		ui->actionHome->setEnabled(false);
		ui->actionEraser->setEnabled(false);
		ui->actionexportPDF->setEnabled(false);
		ui->actionswitchUnit->setEnabled(false);
	}

	void changePage();

	void hooke_page_1();

	void hooke_page_2() {
		if (ui->radioButton_trazione->isChecked())
			ui->stackedWidget_hooke->setCurrentIndex(2);
		if (ui->radioButton_compressione->isChecked())
			ui->stackedWidget_hooke->setCurrentIndex(3);
	}

	void hooke_page_3() {
		ui->stackedWidget_hooke->setCurrentIndex(4);
	}

	void hooke_page_4();

	void hooke_page_5();

	void hooke_page_6();
	
	void hooke_page_7() {
		ui->stackedWidget_hooke->setCurrentIndex(8);
		startStop(false);
	}

	void hooke_page_8() {
		ui->stackedWidget_hooke->setCurrentIndex(9);
	}

	void hooke_page_9() {
		ui->stackedWidget_hooke->setCurrentIndex(9);
	}

	void hooke_page_10() {
		ui->stackedWidget_hooke->setCurrentIndex(10);
	}

	void setFinalStiffness();

	void back() { 
		ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1); 
	}

	void forward() { 
		ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1); 
	}

	void goToCalibraHooke() { ui->stackedWidget->setCurrentIndex(5); } //TODO

	void simHooke();

	void changed_unit();

	void exit();
	void closeEvent(QCloseEvent *event);

	void handleError(QSerialPort::SerialPortError error);
	void handleTimeout();
	void handleReadyRead();
	void readData();
	bool disconnectCOM();
	bool connectCOM();
	void enumeratePorts();

	void showInitialValue();
	void showFinalValue();

	void onCheckBoxes_hooke_x(bool _state);
	void onCheckBoxes_hooke_F(bool _state);
	void onCheckBoxes_hooke_K(bool _state);
	void onDoubleSpinBox_hooke_x(double _value);
	void onDoubleSpinBox_hooke_F(double _value);
	void onDoubleSpinBox_hooke_K(double _value);
	void onHorizontalSlider_hooke_x(int _value);
	void onVerticalSlider_hooke_x(int _value);
	void onVerticalSlider_hooke_F(int _value);
	void onVerticalSlider_hooke_K(int _value);
	void exportToPdf();
	QString generateHTMLfile();
	QString generateColumnForTableHTML(int _column);

	void openSettingsFile();
	void saveSettingsFile();
	void help();
	void pulisci();

	void showHookeMsg(const QString &_message, int _idx, double _F, double _k, double _x );

private:

	void setGreenLed(bool _ON_OFF);
	void setRedLed(bool _ON_OFF);
	void setYellowLed(bool _ON_OFF);

	QGraphicsScene *m_scene;  
	QMovie *m_movie;
	QMovie *m_gif_pesi;
	QMovie *m_gif_pesi2;
	QMovie *m_gif_success;
	QMovie *m_movie_molla;

	bool m_simulation; //!< true in simulation mode

	QSerialPort *m_serial;

	didacticoop_hooke * m_hooke_simulator;
	QtCharts::QChartView *m_chartView;

	bool var;
	QTimer  m_timer;
	QTimer  m_disconnect_timer;
	QTimer  m_initial_value_timer;
	int m_max_wait_time;
	int m_wait_time;
	QTimer  m_final_value_timer;
	QByteArray  m_data;
	QString port_name;

	double m_offset;
	double m_calib;
	int m_weight_vector_size;
	int m_distance_vector_size;

	int m_initial_distance;
	int m_initial_weight;
	int m_final_distance;
	int m_num_of_points;
	int m_max_num_of_points;
	std::vector<int> m_dist_measures;
	std::vector<int> m_weight_measures;

	std::vector<double> m_volt_IR_d;
	std::vector<double> m_distance_mm_d;
	std::vector<double> m_volts_CC_d;
	std::vector<double> m_weight_g_d;

	double m_current_weight;
	double m_current_distance;

	//!< pointer to the tools dialog
	Didacticoop_tools * m_dialog_tools;    

	int m_led_size;
	// to visualize the led on the status bar, 
	// I create two different leds and switch between them to create the effect on/off
	QPixmap * led_green_on;
	QPixmap * led_red_on;
	QPixmap * led_yellow_on;
	QPixmap * led_green_off;
	QPixmap * led_red_off;
	QPixmap * led_yellow_off;
	QPixmap * led_grey;
	QPainter * painter_led_green_on;
	QPainter * painter_led_red_on;
	QPainter * painter_led_yellow_on;
	QPainter * painter_led_green_off;
	QPainter * painter_led_red_off;
	QPainter * painter_led_yellow_off;
	QPainter * painter_led_grey;

protected:
	Ui::didacticoop *ui;    //--> the user interface
};

#endif