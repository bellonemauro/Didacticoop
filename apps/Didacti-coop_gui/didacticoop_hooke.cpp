/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Didacticoop srls, http://www.maurobellone.com/                            |
*  | Didacticoop 1.0                                                           |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "didacticoop_hooke.h"
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QScatterSeries>


didacticoop_hooke::didacticoop_hooke(QMainWindow *parent) :
	m_k_hooke(0.1),
	m_simulation_only(true),
	m_threadTerminationHandler(false)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< " Initializzazione Hooke " << endl;


	m_series_X = new QtCharts::QValueAxis(); //NOT USED
	m_series_Y = new QtCharts::QValueAxis(); //NOT USED
	m_series_force = new QtCharts::QLineSeries();
	m_series_force_measure = new QtCharts::QScatterSeries();

	m_series_force_measure->setMarkerSize(5.0);
	//m_series_force_measure->setMarkerShape(QtCharts::QScatterSeries::MarkerShape::MarkerShapeCircle);
	
	m_series_X->setRange(0, 200);
	m_series_X->setTickCount(5);
	m_series_X->setLabelFormat("%.0f");

	m_series_Y->setRange(0, 5);
	m_series_Y->setTickCount(6);
	m_series_Y->setLabelFormat("%.1f");

	*m_series_force << QPointF(0.0, 0.0) 
		<< QPointF(200.0, 500.0);
	*m_series_force_measure << QPointF(0.0, 0.0) 
		<< QPointF(200.0, 500.0);

	QPen series_pen;
	series_pen.setWidth(3);
	series_pen.setColor(Qt::blue);
	m_series_force->setPen(series_pen);
	m_series_force->setPointsVisible(true);
	
	QPen series_pen_measure;
	series_pen_measure.setWidth(3);
	series_pen_measure.setColor(Qt::red);
	m_series_force_measure->setPen(series_pen_measure);
	m_series_force_measure->setPointsVisible(true);

	QPen pen(0x3333FF);
	pen.setWidth(3);

	m_chart = new QtCharts::QChart();
	m_chart->legend()->hide();
	m_chart->setContentsMargins(0, 0, 0, 0);
	m_chart->addSeries(m_series_force);
	m_chart->addSeries(m_series_force_measure);
	

	QFont title_font;
	title_font.setPointSize(14);
	title_font.setFamily(QStringLiteral("Bradley Hand ITC"));
	//title_font.setBold(true); 
	//m_chart->setTitle("Simulazione legge di Hooke");
	//m_chart->setTitleFont(title_font);
	//m_chart->createDefaultAxes();
	m_chart->setAxisX(m_series_X);
	m_chart->setAxisY(m_series_Y);

	QPen axis_pen;
	axis_pen.setWidth(1);
	axis_pen.setColor(Qt::black);

	QFont axis_font;
	axis_font.setPointSize(12);
	axis_font.setFamily(QStringLiteral("Bradley Hand ITC"));
	//axis_font.setBold(true);
	//axis_font.setItalic(true);

	m_chart->axisX()->setTitleText("deformation [mm]");
	m_chart->axisX()->setTitleFont(title_font);
	m_chart->axisX()->setLabelsFont(axis_font);
	m_chart->axisX()->setLinePen(axis_pen);
	m_chart->axisX()->setGridLineVisible(false);
	
	m_chart->axisY()->setTitleText("force [N]");
	m_chart->axisY()->setTitleFont(title_font);
	m_chart->axisY()->setLabelsFont(axis_font);
	m_chart->axisY()->setLinePen(axis_pen);
	m_chart->axisY()->setGridLineVisible(false);
	m_chart->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA, 0x00)));


	m_chartView = new QtCharts::QChartView(m_chart);
	m_chartView->setContentsMargins(0, 0, 0, 0);
	m_chartView->setRenderHint(QPainter::Antialiasing);
	m_chartView->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA, 0x00)));

	pulisci();
}

void didacticoop_hooke::run() {
	QString result;
	m_threadTerminationHandler = false;
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< " Simulazione di Hooke iniziata, constante di hooke settata =  " << m_k_hooke << " kg/s^2 " << endl;

	try
	{
		m_series_force->clear();
		m_series_force_measure->clear();
		m_dist_sim.clear();
		m_weight_sim.clear();

		*m_series_force << QPointF(0.0, 0.0);
		m_chart->update();
		int cnt = 1;
		while (cnt < 10) {
			QString s = " Simulating for force "; //TODO: remove the message, it is not useful anymore
			s.append(QString::number(cnt));
			s.append(" N ");
			//emit emitMsg(s);
			s.clear();
			msleep(100);
			double estensione = double (10*cnt / m_k_hooke );
			s.append(" extension of the spring ");
			s.append(QString::number(estensione));
			s.append(" m ");
			msleep(100);
			double forza = 10 * cnt;
			m_weight_sim.push_back(forza);
			m_dist_sim.push_back(estensione);
			this->addPoint(forza, estensione);
			emit emitMsg(s, cnt, forza, m_k_hooke, estensione);
			cnt++;
		}

	}
	catch (exception &e) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " gui_hooke:: Unhandled Exception: " << e.what() << endl;
		return;
	}

}

void didacticoop_hooke::addPoint(double _f, double _x)
{
	*m_series_force << QPointF(_x, _f);
	m_chart->update();


}

void didacticoop_hooke::addPointToMeasure(double _f, double _x)
{
	*m_series_force_measure << QPointF(_x, _f);
	m_chart->update();


}


void didacticoop_hooke::pulisci()
{
	m_series_force->clear();
	m_series_force_measure->clear();
	m_chart->update();

}

void didacticoop_hooke::changeChartUnits(bool _unit)
{
	// N
	if (_unit == true)
	{
		m_chart->axisY()->setTitleText("force [N]");
		m_series_Y->setRange(0, 5);
		m_series_Y->setLabelFormat("%.1f");
		m_chart->update();

	}
	// kg
	else
	{
		m_chart->axisY()->setTitleText("force [g]");
		m_series_Y->setRange(0, 500);
		m_series_Y->setLabelFormat("%.0f");
		m_chart->update();

	}


}


didacticoop_hooke::~didacticoop_hooke()
{
	// if nothing is to be done here, is better to leave the compiler do it
}
