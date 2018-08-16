/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Didacticoop srls, http://www.maurobellone.com/                            |
*  | Didacticoop 1.0                                                           |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef didacticoop_hooke_H_
#define didacticoop_hooke_H_

// standard libraries
#include <iostream>
#include <string>

#include "didacticoop.h"

// QT threads
#include <qthread.h>
#include <qmutex.h>
#include <QtCharts/QScatterSeries>

//#include "ui_didacticoop_hooke.h"

using namespace std;


class didacticoop_hooke : public  QThread
{
	Q_OBJECT

		signals :
	void getData(const QByteArray &data);

	void emitMsg(const QString &_s, int _idx, double _F, double _k, double _x);

public:
	explicit didacticoop_hooke(QMainWindow *parent = nullptr);

	~didacticoop_hooke();

	void run() Q_DECL_OVERRIDE;

	void setKhooke(double _k_hooke) { m_k_hooke = _k_hooke; };

	void killMacro(bool _kill) { m_threadTerminationHandler = !_kill; }

	void addPoint(double _f, double _x);
	void addPointToMeasure(double _f, double _x);

	QtCharts::QChartView* getChartView() {
		return m_chartView;
	}

	void pulisci();

	// unit == true => N
	// unit == false => kg
	void changeChartUnits(bool _unit);

	std::vector<int> getForceValuesFromSim() { return m_weight_sim;	}
	std::vector<int> getDistValuesFromSim() { return m_dist_sim; }


private:

	QtCharts::QValueAxis *m_series_X;
	QtCharts::QValueAxis *m_series_Y;
	QtCharts::QLineSeries *m_series_force;
	QtCharts::QScatterSeries *m_series_force_measure;
	//QtCharts::QXYSeries

	QtCharts::QChart *m_chart;
	QtCharts::QChartView *m_chartView;

	double m_k_hooke;
	bool m_simulation_only;
	bool m_threadTerminationHandler;

	std::vector<int> m_dist_sim;
	std::vector<int> m_weight_sim;

};

#endif