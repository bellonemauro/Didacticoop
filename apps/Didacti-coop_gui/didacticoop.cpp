/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Didacticoop srls, http://www.maurobellone.com/                            |
*  | Didacticoop 1.0                                                           |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include <didacticoop.h>

didacticoop::didacticoop( QMainWindow *parent) :
	QMainWindow(parent),
	ui(new Ui::didacticoop),
	m_serial(new QSerialPort),
	m_simulation(true),
	m_led_size(30),
	led_green_on(new QPixmap(QSize(m_led_size, m_led_size))),
	led_red_on(new QPixmap(QSize(m_led_size, m_led_size))),
	led_yellow_on(new QPixmap(QSize(m_led_size, m_led_size))),
	led_green_off(new QPixmap(QSize(m_led_size, m_led_size))),
	led_red_off(new QPixmap(QSize(m_led_size, m_led_size))),
	led_yellow_off(new QPixmap(QSize(m_led_size, m_led_size))),
	led_grey(new QPixmap(QSize(m_led_size, m_led_size))),
	m_timer(new QTimer), 
	m_disconnect_timer(new QTimer),
	m_initial_value_timer(new QTimer),
	m_final_value_timer (new QTimer),
	m_offset (-1.076), m_calib(1.66419533),
	m_weight_vector_size(30),
	m_distance_vector_size(10),
	m_current_weight(0),
	m_current_distance(0),
	m_max_wait_time(50),
	m_wait_time(0),
	m_num_of_points(0),
	m_max_num_of_points(4)
{  
	// allows to use path alias
	QDir::setSearchPaths("icons", QStringList(QDir::currentPath() + "/icons/"));
	ui->setupUi(this);

	m_dialog_tools = new Didacticoop_tools();

	m_movie = new QMovie(":/icons/gif4.gif");
	m_gif_pesi = new QMovie(":/icons/gif2.gif");
	m_gif_pesi2 = new QMovie(":/icons/gif1.gif");
	m_gif_success = new QMovie(":/icons/gif3.gif");

	ui->label_title->setStyleSheet(QLatin1String("border: 0px solid white;border-image: none;"));
	ui->label_gif1->setMovie(m_movie);
	ui->label_gif1->setScaledContents(true);
	ui->label_gif_2->setMovie(m_gif_pesi2);
	ui->label_gif_2->setScaledContents(true);
	ui->label_pesi_vaschetta->setMovie(m_gif_pesi);
	ui->label_pesi_vaschetta->setScaledContents(true);
	ui->label_gif_success->setMovie(m_gif_success);
	ui->label_gif_success->setScaledContents(true);

	m_movie->start(); 
	m_gif_pesi->start();
	m_gif_pesi2->start();
	m_gif_success->start();
	
	ui->label_gif1->show();
	ui->label_gif_2->show();
	ui->label_pesi_vaschetta->show();
	ui->label_gif_success->show();

	ui->stackedWidget->setCurrentIndex(0);
	ui->stackedWidget_hooke->setCurrentIndex(0);
	ui->actionHome->setEnabled(false); 
	ui->actionConnetti->setEnabled(false);
	ui->actionSimula->setEnabled(false);
	ui->actionEraser->setEnabled(false);
	ui->actionSave->setEnabled(false);
	ui->actionLoad->setEnabled(false);
	ui->actionexportPDF->setEnabled(false);
	ui->actionswitchUnit->setEnabled(false);

	m_hooke_simulator = new didacticoop_hooke(this);
	m_chartView = m_hooke_simulator->getChartView();
	m_chartView->setContentsMargins(0, 0, 0, 0);
	ui->gridLayout_chart->addWidget(m_chartView);
	
	// status bar to not connected
	led_green_on->fill(Qt::transparent);
	painter_led_green_on = new QPainter(led_green_on);
	QRadialGradient radialGradient_green_on(12, 12, 17);
	radialGradient_green_on.setColorAt(0.0, 0xF0F0F0);
	radialGradient_green_on.setColorAt(0.7, 0x30D030);
	radialGradient_green_on.setColorAt(1.0, Qt::transparent);
	painter_led_green_on->setBackground(Qt::blue);
	painter_led_green_on->setBrush(radialGradient_green_on);
	painter_led_green_on->setPen(Qt::gray);
	painter_led_green_on->drawEllipse(2, 2, 25, 25);

	led_green_off->fill(Qt::transparent);
	painter_led_green_off = new QPainter(led_green_off);
	QRadialGradient radialGradient_green_off(12, 12, 17);
	radialGradient_green_off.setColorAt(0.0, 0xa0a0a0);
	radialGradient_green_off.setColorAt(0.7, 0x308030);
	radialGradient_green_off.setColorAt(1.0, Qt::transparent);
	painter_led_green_off->setBackground(Qt::blue);
	painter_led_green_off->setBrush(radialGradient_green_off);
	painter_led_green_off->setPen(Qt::gray);
	painter_led_green_off->drawEllipse(2, 2, 25, 25);

	led_red_on->fill(Qt::transparent);
	painter_led_red_on = new QPainter(led_red_on);
	QRadialGradient radialGradient_red_on(12, 12, 17);
	radialGradient_red_on.setColorAt(0.0, 0xF0F0F0);
	radialGradient_red_on.setColorAt(0.7, 0xFF5050);
	radialGradient_red_on.setColorAt(1.0, Qt::transparent);
	painter_led_red_on->setBackground(Qt::blue);
	//painter_led_red->setBrush(Qt::red);
	painter_led_red_on->setBrush(radialGradient_red_on);
	painter_led_red_on->setPen(Qt::gray);
	painter_led_red_on->drawEllipse(2, 2, 25, 25);

	led_red_off->fill(Qt::transparent);
	painter_led_red_off = new QPainter(led_red_off);
	QRadialGradient radialGradient_red_off(12, 12, 17);
	radialGradient_red_off.setColorAt(0.0, 0xa0a0a0);
	radialGradient_red_off.setColorAt(0.7, 0x995050);
	radialGradient_red_off.setColorAt(1.0, Qt::transparent);
	painter_led_red_off->setBackground(Qt::blue);
	//painter_led_red->setBrush(Qt::red);
	painter_led_red_off->setBrush(radialGradient_red_off);
	painter_led_red_off->setPen(Qt::gray);
	painter_led_red_off->drawEllipse(2, 2, 25, 25);

	led_yellow_on->fill(Qt::transparent);
	painter_led_yellow_on = new QPainter(led_yellow_on);
	QRadialGradient radialGradient_yellow_on(12, 12, 17);
	radialGradient_yellow_on.setColorAt(0.0, 0xF0F0F0);
	radialGradient_yellow_on.setColorAt(0.7, 0xFFE100);
	radialGradient_yellow_on.setColorAt(1.0, Qt::transparent);
	painter_led_yellow_on->setBackground(Qt::blue);
	painter_led_yellow_on->setBrush(radialGradient_yellow_on);
	painter_led_yellow_on->setPen(Qt::gray);
	painter_led_yellow_on->drawEllipse(2, 2, 25, 25);

	led_yellow_off->fill(Qt::transparent);
	painter_led_yellow_off = new QPainter(led_yellow_off);
	QRadialGradient radialGradient_yellow_off(12, 12, 17);
	radialGradient_yellow_off.setColorAt(0.0, 0xa0a0a0);
	radialGradient_yellow_off.setColorAt(0.7, 0xC88C00);
	radialGradient_yellow_off.setColorAt(1.0, Qt::transparent);
	painter_led_yellow_off->setBackground(Qt::blue);
	painter_led_yellow_off->setBrush(radialGradient_yellow_off);
	painter_led_yellow_off->setPen(Qt::gray);
	painter_led_yellow_off->drawEllipse(2, 2, 25, 25);

	led_grey->fill(Qt::transparent);
	painter_led_grey = new QPainter(led_grey);
	QRadialGradient radialGradient_grey(12, 12, 17);
	radialGradient_grey.setColorAt(0.0, 0xF0F0F0);
	radialGradient_grey.setColorAt(0.7, 0x303030);
	radialGradient_grey.setColorAt(1.0, Qt::transparent);
	painter_led_grey->setBackground(Qt::blue);
	painter_led_grey->setBrush(radialGradient_grey);
	painter_led_grey->setPen(Qt::gray);
	painter_led_grey->drawEllipse(2, 2, 25, 25);

	this->setGreenLed(false);
	this->setRedLed(true);
	this->setYellowLed(false);


	connect(m_serial, &QSerialPort::readyRead, this, &didacticoop::readData);

	//connect(m_serial, &QSerialPort::readyRead, this, &didacticoop::handleReadyRead);
	connect(m_serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
		this, &didacticoop::handleError);
	connect(&m_timer, &QTimer::timeout, this, &didacticoop::handleTimeout);
	connect(&m_disconnect_timer, &QTimer::timeout, this, &didacticoop::disconnectCOM);
	connect(&m_initial_value_timer, &QTimer::timeout, this, &didacticoop::showInitialValue);
	connect(&m_final_value_timer, &QTimer::timeout, this, &didacticoop::showFinalValue);
	port_name = "COM7";

	ui->lcdNumber_distanceIR->hide();
	ui->lcdNumber_weight->hide();


	connect(ui->pushButton_goToExp1,
		SIGNAL(clicked()), this,
		SLOT(goToExp1()));



	connect(ui->pushButton_goToExp3,
		SIGNAL(clicked()), this,
		SLOT(goToExp2()));

	connect(ui->pushButton_simHooke,
			SIGNAL(clicked()), this,
			SLOT(simHooke()));

	connect(ui->actionAbout,
		SIGNAL(triggered()), this,
		SLOT(about()));
	
	connect(ui->actionHelp,
		SIGNAL(triggered()), this,
		SLOT(help()));

	connect(ui->actionEraser,
		SIGNAL(triggered()), this,
		SLOT(pulisci()));

	connect(ui->actionLoad,
		SIGNAL(triggered()), this,
		SLOT(openSettingsFile()));

	connect(ui->actionSave,
		SIGNAL(triggered()), this,
		SLOT(saveSettingsFile()));

	connect(ui->actionexportPDF,
		SIGNAL(triggered()), this,
		SLOT(exportToPdf()));
	
	connect(ui->actionswitchUnit,
		SIGNAL(triggered()), this,
		SLOT(changed_unit()));

	connect(ui->actionHome,
		SIGNAL(triggered()), this,
		SLOT(backToIntro()));

	connect(ui->actionIndietro,
		SIGNAL(triggered()), this,
		SLOT(back()));

	connect(ui->actionImpostazioni,
		SIGNAL(triggered()), this,
		SLOT(showToolDialog()));

	connect(ui->actionAvanti,
		SIGNAL(triggered()), this,
		SLOT(forward()));


	connect(ui->actionExit,
		SIGNAL(triggered()), this,
		SLOT(exit()));

	connect(ui->pushButton_forward_1,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_2()));

	
	connect(ui->pushButton_ricomincia_1,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_11,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_12,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_3,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_4,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_5,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_6,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_7,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_8,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_ricomincia_9,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_back_11,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	

	connect(ui->pushButton_back_12,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_1()));

	connect(ui->pushButton_forward_11,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_3()));

	connect(ui->pushButton_forward_12,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_3()));

	
	connect(ui->pushButton_back_3,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_2())); 
	
	connect(ui->pushButton_forward_3,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_4()));


	connect(ui->pushButton_back_4,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_3()));

	connect(ui->pushButton_forward_4,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_5()));

	connect(ui->pushButton_back_5,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_4()));

	connect(ui->pushButton_forward_5,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_6()));

	connect(ui->pushButton_back_6,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_5()));

	connect(ui->pushButton_forward_6,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_7()));

	connect(ui->pushButton_back_7,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_6()));

	connect(ui->pushButton_forward_7,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_8()));

	connect(ui->pushButton_back_8,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_7()));

	connect(ui->pushButton_forward_8,
		SIGNAL(clicked()), this,
		SLOT(hooke_page_9()));


	connect(ui->pushButton_backToExp1,
			SIGNAL(clicked()), this,
			SLOT(goToExp1()));

	connect(ui->actionSimula,
		SIGNAL(triggered()), this,
		SLOT(simulationOnly()));

	connect(ui->actionConnetti,
		SIGNAL(triggered()), this,
		SLOT(changePage()));

	connect(ui->pushButton_endPDF,
		SIGNAL(clicked()), this,
		SLOT(exportToPdf()));
	
	connect(m_hooke_simulator,
		&didacticoop_hooke::emitMsg, this,
		&didacticoop::showHookeMsg);
	
	connect(ui->checkBox_hooke_x, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxes_hooke_x(bool)));
	connect(ui->checkBox_hooke_F, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxes_hooke_F(bool)));
	connect(ui->checkBox_hooke_K, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxes_hooke_K(bool)));

	connect(ui->doubleSpinBox_hooke_x, SIGNAL(valueChanged(double)),
		this, SLOT(onDoubleSpinBox_hooke_x(double)));

	connect(ui->doubleSpinBox_hooke_F, SIGNAL(valueChanged(double)),
		this, SLOT(onDoubleSpinBox_hooke_F(double)));

	connect(ui->doubleSpinBox_hooke_K, SIGNAL(valueChanged(double)),
		this, SLOT(onDoubleSpinBox_hooke_K(double)));

	connect(ui->horizontalSlider_hooke_x, SIGNAL(valueChanged(int)),
		this, SLOT(onHorizontalSlider_hooke_x(int)));

	connect(ui->verticalSlider_hooke_x, SIGNAL(valueChanged(int)),
		this, SLOT(onVerticalSlider_hooke_x(int)));

	connect(ui->verticalSlider_hooke_F, SIGNAL(valueChanged(int)),
		this, SLOT(onVerticalSlider_hooke_F(int)));

	connect(ui->verticalSlider_hooke_K, SIGNAL(valueChanged(int)),
		this, SLOT(onVerticalSlider_hooke_K(int)));

	ui->textBrowser_message->setOpenExternalLinks(true);
}


void didacticoop::showToolDialog()
{
	m_dialog_tools->setParent(this);
	m_dialog_tools->setWindowFlags(Qt::Window);
	m_dialog_tools->show();
}

void didacticoop::setGreenLed(bool _ON_OFF) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::setGreenLed   " << _ON_OFF << endl;

	ui->led_verde->clear();
	if (_ON_OFF) {
		ui->led_verde->setPixmap(*led_green_on);
		if (m_serial->isOpen()) {
			m_serial->write("V\n");
			Sleep(50);
		}

	}
	else {
		ui->led_verde->setPixmap(*led_green_off);
		if (m_serial->isOpen()) {
			m_serial->write("F\n");
			Sleep(50);
		}

	}
	return;
}

void didacticoop::setRedLed(bool _ON_OFF) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::setRedLed   " << _ON_OFF << endl;

	ui->led_rosso->clear();
	if (_ON_OFF) {
		ui->led_rosso->setPixmap(*led_red_on);
		if (m_serial->isOpen()) {
			m_serial->write("R\n");
			Sleep(50);
		}
	}
	else {
		ui->led_rosso->setPixmap(*led_red_off);
		if (m_serial->isOpen()) {
			m_serial->write("T\n");
			Sleep(50);
		}
	}
	return;
}
void didacticoop::setYellowLed(bool _ON_OFF) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::setYellowLed   " << _ON_OFF << endl;

	ui->led_giallo->clear();
	if (_ON_OFF) {
		ui->led_giallo->setPixmap(*led_yellow_on);
		if (m_serial->isOpen()) {
			m_serial->write("G\n");
			Sleep(50);
		}
	}
	else {
		ui->led_giallo->setPixmap(*led_yellow_off);
		if (m_serial->isOpen()) {
			m_serial->write("C\n");
			Sleep(50);
		}
	}
	return;
}



bool didacticoop::connectCOM()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	cout << " connecting COM ...  \n" << endl;

	port_name = m_dialog_tools->getSerialPortName();

	cout << " connecting COM ... " << port_name.toStdString() <<   " \n" << endl;


	m_serial->setPortName(port_name); //(comSettings->name);
	m_serial->setBaudRate(9600);// (comSettings->baudRate);

	m_serial->setDataBits(QSerialPort::DataBits::Data8);
	m_serial->setParity(QSerialPort::Parity::NoParity);
	m_serial->setStopBits(QSerialPort::StopBits::OneStop);
	m_serial->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

	if (m_serial->open(QIODevice::ReadWrite)) {
		// enable/disable GUI buttons

		QString ss(tr("Connected to %1 : %2, %3, %4, %5, %6 \n")
			.arg(m_serial->portName()).arg(m_serial->baudRate()).arg(m_serial->dataBits())
			.arg(m_serial->parity()).arg(m_serial->stopBits()).arg(m_serial->flowControl()));
		cout << ss.toStdString() << endl;
		//ui_tools->textEdit_console->append(ss);
		m_timer.start(50);
		
		Sleep(100);
		this->setGreenLed(false);
		this->setRedLed(false);
		this->setYellowLed(false);
		Sleep(100);
		this->setGreenLed(true);
		ui->label_conDis->setText("CONNECTED");
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

		return true;
	}
	else {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QString ss(tr("Could not connect to %1 : %2, %3, %4, %5, %6 \n")
			.arg(m_serial->portName()).arg(m_serial->baudRate()).arg(m_serial->dataBits())
			.arg(m_serial->parity()).arg(m_serial->stopBits()).arg(m_serial->flowControl()));
		cout << ss.toStdString() << endl; 


		QString msg = "Please, connect the device to start the experiment <br>";
		//msg.append("... or visit our website <a href='http://www.didacticoop.com/'>DidactiCoop</a> to get it!");
		msg.append("<br> ... or go to");
		msg.append("<br>Tools -> Connection -> Update");
		
		QMessageBox::information(this, tr("Information"), msg);// m_serial->errorString());

		// try to connect first
		// m_dialog_tools->tryConnect();

		return false;
		//cout << "Open error" << endl;
	}


	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}


bool didacticoop::disconnectCOM()
{

	if (m_serial->isOpen()){
		
		Sleep(500);
		m_serial->close();
		ui->label_conDis->setText("DISCONNECTED");
		m_disconnect_timer.stop();
		return true;
	}

	return false;
}

void didacticoop::handleTimeout()
{
	if (m_data.isEmpty()) {
// do nothing for now
	}
	else {
		QString data = m_data;
		QStringList lista = data.split("\n");

		cout << lista.last().toStdString() << endl;
		//ui_tools->textEdit_console->append(lista.last());

		m_data.clear();
	}
}

void didacticoop::handleError(QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::ResourceError) {
		QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
		disconnectCOM();
	}
}

void didacticoop::handleReadyRead()
{
	QByteArray package;
	bool isHeaderRead = false;
	quint8 startByte = 0;
	quint8 dataSize = 0;

	//! Is there whole header appears?
	if (m_serial->bytesAvailable() < 100)
	{
		//! If not, waiting for other bytes
		return;
	}
	else {
		//! Read fixed size header.
		package.append(m_serial->readLine());

		cout << package.toStdString();
	}
}

void didacticoop::readData()
{

	if (m_serial->isReadable()) {


		if (m_serial->bytesAvailable() < 100)
		{   //if the package is not complete, wait for more data
			return;
		}
		else {

			m_data.append(m_serial->readLine());
			
			//(' '); // ('#');
			QByteArrayList data = m_data.split('#'); 

		    //string format = volts_IR#distance_mm#volts_CC#weight_g#/n
			QByteArray volts_IR = "0";
			QByteArray volts_CC = "0";
			if (data.size() > 1)
			{
				volts_IR = data.at(0);
				//distance_mm = data.at(1);
				volts_CC = data.at(1);
				//weight_g = data.at(3);
			
			//std::string str = m_data.toStdString();
			cout << data.at(0).toStdString() << "  "
				<< data.at(1).toStdString() << "  " << endl;
			}

			// calib data
			double a = 1595.5;
			double b = -1.066;
			
			if (m_volt_IR_d.size() < m_distance_vector_size)
			{
				double v = volts_IR.toDouble();
				m_volt_IR_d.push_back(v);
				m_distance_mm_d.push_back(a*std::pow(v,b)); //TODO function

			}
			else
			{
				m_volt_IR_d.erase(m_volt_IR_d.begin());
				m_distance_mm_d.erase(m_distance_mm_d.begin());

				double v = volts_IR.toDouble();
				m_volt_IR_d.push_back(v);
				m_distance_mm_d.push_back(a*std::pow(v, b)); //TODO function

			}

			if (m_volts_CC_d.size() < m_weight_vector_size)
			{
				m_volts_CC_d.push_back(volts_CC.toDouble());
				m_weight_g_d.push_back(m_calib * volts_CC.toDouble() + m_offset);
			}
			else
			{
				m_volts_CC_d.erase(m_volts_CC_d.begin());
				m_weight_g_d.erase(m_weight_g_d.begin());

				m_volts_CC_d.push_back(volts_CC.toDouble());
				m_weight_g_d.push_back(m_calib * volts_CC.toDouble() + m_offset);

			}

			//double value = atof(str.c_str());
			double volt_IR_d_sum = std::accumulate(m_volt_IR_d.begin(), m_volt_IR_d.end(), 0.0);
			double volt_IR_d = volt_IR_d_sum / m_volt_IR_d.size();

			double distance_mm_d_sum = std::accumulate(m_distance_mm_d.begin(), m_distance_mm_d.end(), 0.0);
			double distance_mm_d = distance_mm_d_sum / m_distance_mm_d.size();

			double volts_CC_d_sum = std::accumulate(m_volts_CC_d.begin(), m_volts_CC_d.end(), 0.0);
			double volts_CC_d = volts_CC_d_sum / m_volts_CC_d.size();

			double weight_g_d_sum = std::accumulate(m_weight_g_d.begin(), m_weight_g_d.end(), 0.0);
			double weight_g_d = (weight_g_d_sum / m_weight_g_d.size())/1000;


//			ui->lcdNumber_voltIR->display(volt_IR_d);
			m_current_distance = 10 * distance_mm_d;
			m_current_weight = weight_g_d;
			ui->lcdNumber_distanceIR->display(int(m_current_distance));
//			ui->lcdNumber_voltCC->display(volts_CC_d);
			ui->lcdNumber_weight->display(int(m_current_weight));

			m_data.clear();
		}
	}
}



void didacticoop::enumeratePorts()
{
	QString vendor = "0000"; //TODO 

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos) {
		QString s = QObject::tr("Port: ") + info.portName() + "\n"
			+ QObject::tr("Location: ") + info.systemLocation() + "\n"
			+ QObject::tr("Description: ") + info.description() + "\n"
			+ QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
			+ QObject::tr("Serial number: ") + info.serialNumber() + "\n"
			+ QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
			+ QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
			+ QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

		cout << s.toStdString() << endl;

		QString v = QString::number(info.vendorIdentifier(), 16);
		if (v == vendor)
		{//TODO: this only attempt to match the vendor, is it also possible to check the product?
			cout << "This could be the right device \n\n " << endl;
		}
	} // TODO: in debug the detor crushes !
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return;
}

void didacticoop::showInitialValue()
{
	ui->pushButton_forward_4->setEnabled(false);
	m_dist_measures.clear();
	m_weight_measures.clear();

	m_initial_distance = m_current_distance;
	m_initial_weight = m_current_weight;

	ui->doubleSpinBox_initialValue->setValue(m_initial_distance);

	if (m_wait_time < m_max_wait_time)
	{
		m_wait_time++;
	}
	else
	{
		m_initial_value_timer.stop();
		m_wait_time = 0;
		ui->pushButton_forward_4->setEnabled(true);
		m_hooke_simulator->addPointToMeasure(0.0, 0.0);

		m_dist_measures.push_back(0.0);
		m_weight_measures.push_back(0.0);
	}
	return;

}

void didacticoop::showFinalValue()
{
	ui->pushButton_forward_6->setEnabled(false);
	ui->pushButton_back_6->setEnabled(false);
	ui->pushButton_ricomincia_6->setEnabled(false);

	m_final_distance = m_current_distance;

	ui->doubleSpinBox_finalValue->setValue(m_final_distance);

	int weight = m_current_weight - m_initial_weight;
	if (ui->actionswitchUnit->isChecked() == false)
	{
		ui->doubleSpinBox_weightHooke->setValue(double(weight)/100.0);
	}
	else
	{
		ui->doubleSpinBox_weightHooke->setValue(weight);
	}

	

	int displacement = std::abs(m_final_distance - m_initial_distance);


	ui->doubleSpinBox_extComp->setValue(displacement);

	if (m_wait_time < m_max_wait_time)
	{
		m_wait_time++;
	}
	else
	{
		if (m_num_of_points < m_max_num_of_points)
		{
			m_num_of_points++;
			m_dist_measures.push_back(displacement);
			m_weight_measures.push_back(weight);

			m_hooke_simulator->addPointToMeasure(weight, displacement);

			QMessageBox::StandardButton resBtn =
				QMessageBox::question(this, "Information", 
					"Prepare the next weight and press Yes to continue or No to abort the experiment",
					QMessageBox::No | QMessageBox::Yes,
					QMessageBox::Yes);
			if (resBtn != QMessageBox::Yes) {


			}
			else {
				m_wait_time = 0;
				m_final_value_timer.stop();
				hooke_page_5();
				return;
			}

		}
		else {
			m_dist_measures.push_back(displacement);
			m_weight_measures.push_back(weight);
			m_hooke_simulator->addPointToMeasure(weight, displacement);
		}
		
		m_final_value_timer.stop();
		m_wait_time = 0;

		ui->pushButton_forward_6->setEnabled(true);
		ui->pushButton_back_6->setEnabled(true);
		ui->pushButton_ricomincia_6->setEnabled(true);
	}
}


void didacticoop::startStop(bool _enable)
{
	if (_enable)
	{
		if (m_serial->isOpen()) {
			m_serial->write("S\n");
			this->setYellowLed(true);
			ui->label_conDis->setText("RUNNING");
		}
	}
	else {
		if (m_serial->isOpen()) {
			m_serial->write("H\n");
			this->setYellowLed(false);
			ui->label_conDis->setText("CONNECTED");
		}
	}
}



void didacticoop::hooke_page_4()
{
	
	ui->stackedWidget_hooke->setCurrentIndex(5);
	startStop(true);

	m_initial_value_timer.start(100);
	

}

void didacticoop::hooke_page_5()
{
	ui->stackedWidget_hooke->setCurrentIndex(6);

	setFinalStiffness();
}

void didacticoop::hooke_page_6()
{
		ui->stackedWidget_hooke->setCurrentIndex(7);
		m_final_value_timer.start(100);
}

void didacticoop::setFinalStiffness()
{
	std::vector<double> k_measured;

	double k_real = 0.0;

	if (m_dist_measures.size() < 1 || m_weight_measures.size() < 1)
	{ // if we do not have any values, set to 0
		k_real = 0.0;
	}
	else // otherwise calculate the average k
	{
		k_measured.push_back(0.0);
		for (int i = 1; i < m_dist_measures.size(); i++)
		{
			k_measured.push_back(double(m_weight_measures.at(i)) / double(m_dist_measures.at(i)));
		}

		double k_sum = std::accumulate(k_measured.begin(), k_measured.end(), 0.0);
		k_real = k_sum / double(k_measured.size());
	}

	// set the calculation in the label
	if (ui->actionswitchUnit->isChecked() == false)
	{//N
		double kr = double(k_real) / 100.0;

		QString ss = QString::number(kr, 'g', 2);
		ss.append(" N/mm");
		ui->label_measured_stiffness->setText(ss);
	}
	else
	{//g
		QString ss = QString::number(k_real, 'g', 2);
		ss.append(" g/mm");
		ui->label_measured_stiffness->setText(ss);
	}
}

void didacticoop::simHooke()
{
	// get the number to be checked
	double number = ui->doubleSpinBox_kHooke->value();

	m_hooke_simulator->setKhooke(number);
	m_hooke_simulator->start();
}

void didacticoop::changed_unit()
{
	
	if (ui->actionswitchUnit->isChecked() == false)
	{
		ui->doubleSpinBox_kHooke->setSuffix(" N/mm");
		ui->doubleSpinBox_hooke_F->setSuffix(" N");
		ui->doubleSpinBox_hooke_K->setSuffix(" N/mm");
		ui->checkBox_hooke_F->setText("F [N]");
		ui->checkBox_hooke_K->setText("K [N/mm]");
		ui->doubleSpinBox_weightHooke->setSuffix(" N");
		ui->doubleSpinBox_weightHooke->setDecimals(2);
		m_hooke_simulator->changeChartUnits(true);
		ui->label_unit_measure->setText("Unit of measure: [ N ]");
		ui->label_unit_measure->setStyleSheet("color: rgb(39, 115, 192);");
		setFinalStiffness();

	}
	else
	{
		ui->doubleSpinBox_kHooke->setSuffix(" g/mm");
		ui->doubleSpinBox_hooke_F->setSuffix(" g"); 
		ui->doubleSpinBox_hooke_K->setSuffix(" g/mm");
		ui->checkBox_hooke_F->setText("F [g]");
		ui->checkBox_hooke_K->setText("K [g/mm]");
		ui->doubleSpinBox_weightHooke->setSuffix(" g");
		ui->doubleSpinBox_weightHooke->setDecimals(0);
		ui->label_unit_measure->setText("Unit of measure: [ g(wt) ]");
		ui->label_unit_measure->setStyleSheet("color: rgb(255, 0, 0);");
		m_hooke_simulator->changeChartUnits(false);
		setFinalStiffness();
	}
}


void didacticoop::simulationOnly()
{
	this->setGreenLed(false);
	this->setRedLed(true);
	this->setYellowLed(false);

	m_final_value_timer.stop();
	m_initial_value_timer.stop();

	//if (disconnectCOM() == true)
	{
		m_disconnect_timer.start(100); //TODO this is not safe
		ui->stackedWidget_hooke->setCurrentIndex(0);
		ui->actionConnetti->setEnabled(true);
		ui->actionSimula->setEnabled(false);
	}

}

void didacticoop::goToExp1() {
	ui->stackedWidget->setCurrentIndex(1);
	ui->actionConnetti->setEnabled(true);
	ui->actionSimula->setEnabled(false);
	ui->actionHome->setEnabled(true);
	ui->actionEraser->setEnabled(true);
	ui->actionexportPDF->setEnabled(true);
	ui->actionswitchUnit->setEnabled(true);
}


void didacticoop::about() {

	QMessageBox messageBox;
	QString msg_title = "Credits";
	QString msg_content = tr("<font size=\"+2\"><b>Galileo</b> is a <a href='http://www.didacticoop.com/'>DidactiCoop</a> open source software<br>"
		"Version: 1.0 <br>"
		"Copyright DidactiCoop, 2018 <br> <br>"
		"CEO, Lorenzo De Pascalis 2018 <br>"
		"Hardware development, Michele De Pascalis <br>"
		"Software development, Mauro Bellone <br>"
		"Graphic design, Nicholas Dima <br>"
		"<br></font>"
		"<img src = \":/icons/PIN_loghi.png\" alt=\"grafico\" >"
		);
	messageBox.about(this, msg_title, msg_content);
	messageBox.setIconPixmap(QPixmap("./icons/Logo.png"));
	messageBox.setFixedSize(600, 800);
}

void didacticoop::onCheckBoxes_hooke_x( bool _state)
{
	if (ui->checkBox_hooke_x->isChecked() == true)
	{
		ui->verticalSlider_hooke_x->setEnabled(true);
		ui->doubleSpinBox_hooke_x->setEnabled(true);
		if (ui->checkBox_hooke_F->isChecked() == true && ui->checkBox_hooke_K->isChecked() == true)
		{
			ui->checkBox_hooke_F->setChecked(false);
			ui->verticalSlider_hooke_F->setEnabled(false);
			ui->doubleSpinBox_hooke_F->setEnabled(false);
		}
	}
	else
	{
		ui->verticalSlider_hooke_x->setEnabled(false);
		ui->doubleSpinBox_hooke_x->setEnabled(false);
		if (ui->checkBox_hooke_K->isChecked())
		{
			ui->checkBox_hooke_F->setChecked(true);
		}
		else
		{
			ui->checkBox_hooke_K->setChecked(true);
		}
	}

}

void didacticoop::onCheckBoxes_hooke_F(bool _state)
{
	if (ui->checkBox_hooke_F->isChecked() == true)
	{
		ui->verticalSlider_hooke_F->setEnabled(true);
		ui->doubleSpinBox_hooke_F->setEnabled(true);
		if (ui->checkBox_hooke_K->isChecked() == true && ui->checkBox_hooke_x->isChecked() == true)
		{
			ui->checkBox_hooke_K->setChecked(false);
			ui->verticalSlider_hooke_K->setEnabled(false);
			ui->doubleSpinBox_hooke_K->setEnabled(false);
		}
	}
	else
	{
		ui->verticalSlider_hooke_F->setEnabled(false);
		ui->doubleSpinBox_hooke_F->setEnabled(true);

		if (ui->checkBox_hooke_K->isChecked())
		{
			ui->checkBox_hooke_x->setChecked(true);
		}
		else
		{
			ui->checkBox_hooke_K->setChecked(true);
		}

	}

}

void didacticoop::onCheckBoxes_hooke_K(bool _state)
{
	if (ui->checkBox_hooke_K->isChecked() == true)
	{
		ui->verticalSlider_hooke_K->setEnabled(true);
		ui->doubleSpinBox_hooke_K->setEnabled(true);

		if (ui->checkBox_hooke_F->isChecked() == true && ui->checkBox_hooke_x->isChecked() == true)
		{
			ui->checkBox_hooke_F->setChecked(false);
			ui->verticalSlider_hooke_F->setEnabled(false);
			ui->doubleSpinBox_hooke_F->setEnabled(false);
		}

	}
	else
	{
		ui->verticalSlider_hooke_K->setEnabled(false);
		ui->doubleSpinBox_hooke_K->setEnabled(false);

		if (ui->checkBox_hooke_F->isChecked())
		{
			ui->checkBox_hooke_x->setChecked(true);
		}
		else
		{
			ui->checkBox_hooke_F->setChecked(true);
		}
	}
}


void didacticoop::onDoubleSpinBox_hooke_x(double _value) {

	ui->verticalSlider_hooke_x->setValue(int(_value));

}

void didacticoop::onDoubleSpinBox_hooke_F(double _value) {

	ui->verticalSlider_hooke_F->setValue(int(_value));
}

void didacticoop::onDoubleSpinBox_hooke_K(double _value) {

	ui->verticalSlider_hooke_K->setValue(int(_value * 100));
}

void didacticoop::onHorizontalSlider_hooke_x(int _value)
{
	ui->doubleSpinBox_kHooke->setValue(_value / 1000.0);

}


void didacticoop::onVerticalSlider_hooke_x(int _value) 
{

	if (ui->verticalSlider_hooke_F->isEnabled() == false)
	{
		double F = _value * ui->doubleSpinBox_hooke_K->value();
		
		if (F < ui->doubleSpinBox_hooke_F->maximum())
		{
			ui->doubleSpinBox_hooke_x->setValue(_value);
			ui->doubleSpinBox_hooke_F->setValue(F);
		}
		else
		{
			ui->verticalSlider_hooke_x->setValue(ui->doubleSpinBox_hooke_F->maximum() / ui->doubleSpinBox_hooke_K->value());
		}

		return;
	}
	if (ui->verticalSlider_hooke_K->isEnabled() == false && _value != 0)
	{
		double K = ui->doubleSpinBox_hooke_F->value() / _value; 
		
		if (K < ui->doubleSpinBox_hooke_K->maximum())
		{
			ui->doubleSpinBox_hooke_x->setValue(_value);
			ui->doubleSpinBox_hooke_K->setValue(K);
		}
		else
		{
			ui->verticalSlider_hooke_x->setValue(ui->doubleSpinBox_hooke_F->value() / ui->doubleSpinBox_hooke_K->maximum() );
		}

		return;
	}

}

void didacticoop::onVerticalSlider_hooke_F(int _value) {
	

	if (ui->verticalSlider_hooke_K->isEnabled() == false && ui->doubleSpinBox_hooke_x->value() != 0)
	{
		double K = _value / ui->doubleSpinBox_hooke_x->value();

		if (K < ui->doubleSpinBox_hooke_K->maximum())
		{
			ui->doubleSpinBox_hooke_F->setValue(_value);
			ui->doubleSpinBox_hooke_K->setValue(K);
		}
		else
		{
			ui->verticalSlider_hooke_F->setValue(ui->doubleSpinBox_hooke_K->maximum() * ui->doubleSpinBox_hooke_x->value());
		}

		return;
	}
	if (ui->verticalSlider_hooke_x->isEnabled() == false && ui->doubleSpinBox_hooke_K->value() != 0)
	{
		double x = _value / ui->doubleSpinBox_hooke_K->value();

		if (x < ui->doubleSpinBox_hooke_x->maximum())
		{
			ui->doubleSpinBox_hooke_F->setValue(_value);
			ui->doubleSpinBox_hooke_x->setValue(x);
		}
		else
		{
			ui->verticalSlider_hooke_F->setValue(ui->doubleSpinBox_hooke_K->value() * ui->doubleSpinBox_hooke_x->maximum());
		}

		return;
	}
}

void didacticoop::onVerticalSlider_hooke_K(int _value) {

	double value = _value / 100.0;

	if (ui->verticalSlider_hooke_F->isEnabled() == false)
	{
		double F = value * ui->doubleSpinBox_hooke_x->value();
		
		if (F < ui->doubleSpinBox_hooke_F->maximum())
		{
			ui->doubleSpinBox_hooke_K->setValue(value);
			ui->doubleSpinBox_hooke_F->setValue(F);
		}
		else
		{
			ui->verticalSlider_hooke_K->setValue(100.0 * ui->doubleSpinBox_hooke_F->maximum() / ui->doubleSpinBox_hooke_x->value());
		}

		return;
	}
	if (ui->verticalSlider_hooke_x->isEnabled() == false && value != 0)
	{
		double x =  ui->doubleSpinBox_hooke_F->value() / value;

		if (x < ui->doubleSpinBox_hooke_x->maximum())
		{
			ui->doubleSpinBox_hooke_K->setValue(value);
			ui->doubleSpinBox_hooke_x->setValue(x);
		}
		else
		{
			ui->verticalSlider_hooke_K->setValue(100.0 * ui->doubleSpinBox_hooke_F->value() / ui->doubleSpinBox_hooke_x->maximum());
		}

		return;
	} 
}



void didacticoop::exportToPdf()
{

	QString fileName = QFileDialog::getSaveFileName((QWidget*)0, "Export PDF", QString(), "*.pdf");
	if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); return; }

	QPrinter printer(QPrinter::PrinterResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPageMargins(QMargins(30.0, 25.0, 25.0, 10.0));
	printer.setPaperSize(QPrinter::A4);
	printer.setPageOrientation(QPageLayout::Orientation::Portrait);
	printer.setOutputFileName(fileName);

	QTextDocument doc;
	//QFile file("./icons/rep.html");
	//file.open(QFile::ReadOnly);
	//QByteArray content = file.readAll();
	//QTextCodec *codec = Qt::codecForHtml(content);
	//QString str = codec->toUnicode(content);

	QPixmap p = m_chartView->grab();
	QString path = QDir::tempPath();
	string ss = path.toStdString();
	path.append("/grafico.png");
	p.save(path, "PNG",1000);
		
	doc.setHtml(generateHTMLfile());//(generateHTMLfile());//(str);//
	doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
	doc.print(&printer);


	QDesktopServices::openUrl(QUrl("file:///"+fileName));

}

QString didacticoop::generateHTMLfile()
{

	if (ui->actionConnetti->isEnabled())
	{
		m_dist_measures = m_hooke_simulator->getDistValuesFromSim();
		m_weight_measures = m_hooke_simulator->getForceValuesFromSim();
	}

	QString date = QDate::currentDate().toString();

	QString spring_type;
	QString configuration_type;

	if (ui->actionConnetti->isEnabled()) {
		spring_type = "NONE (simulation mode)";
		configuration_type = "NONE (simulation mode)";
	}
	else {
		if (ui->radioButton_trazione->isChecked())
			spring_type = "Extension";
		if (ui->radioButton_compressione->isChecked())
				spring_type = "Compression";


		if (ui->radioButton_singleSpring->isChecked())
			configuration_type = "Single spring";
		if (ui->radioButton_series->isChecked())
			configuration_type = "Springs in series";
		if (ui->radioButton_parallel->isChecked())
			configuration_type = "Spring in parallel";
		if (ui->radioButton_series_parallel->isChecked())
			configuration_type = "Springs in series and parallel";
	}
	

	QString html_content;
	html_content = ("<!DOCTYPE html>"
		"<html>"
		"<head>"
		"</head>"
		"<body>"// style=\"background:url('./icons/lavagna_pdf_mod.png'); background-repeat:no-repeat;  \" >" //background-size:cover;
		//"<body  background=\":/icons/lavagna.png\" >"
		"<font size = \"1\">"
		//Table 1 : logo and address
		"<table border=\"0\" align=\"center\" cellspacing=\"0\" cellpadding=\"4\">"
		"<tbody>"
		"<tr>"
		"<td width=\"120\"><img src=\":/icons/postit_hooke.png\" alt=\"Logo\" height=\"60\" width=\"60\"></td>"
		"<td width=\"230\">"
		"<p><em><img src=\":/icons/titolo_hooke.png\" alt=\"molle\" height=\"60\" width=\"300\"></em></p>"
		"</td>"
		"</tr>"
		"</tbody>"
		"</table>"
		//end table 1

		"<p style=\" margin - top:0px; margin - bottom:0px; margin - left:0px; margin - right:0px; -qt - block - indent:0; text - indent:0px; \">"
		"<strong><em><h2><div align=\"center\">Report of the experiment</div></h2></em></strong></p>"
		// just a bit of space
		"<p>&nbsp;</p>"

		//Table 2 : Students and class
		"<table border=\"0\" align=\"left\" cellspacing=\"0\" cellpadding=\"4\">"
		"<tbody>"
		"<tr>"

		"<td width=\"160\">"
		"<p><strong><em>Student:</em></strong></p>"
		"</td>"

		"<td width=\"180\">"
		"<p>");

	    html_content.append(m_dialog_tools->getStudentName());
		html_content.append("</p>"
		"</td>"

		"<td width=\"100\">"
		"<p><strong><em>Date : </em></strong></p>"
		"</td>"

		"<td width=\"100\">"
		"<p> ");

		html_content.append(date);
	    html_content.append("</p> "
		"</td>"

		"</tr>"


		"<tr>"
		"<td width=\"160\">"
		"<p><strong><em>Type of springs</em></strong></p>"
		"</td>"

		"<td width=\"180\">"
		"<p>");
		
		html_content.append(spring_type);
		html_content.append("</p>"
		"</td>"

		"<td width=\"100\">"
		"<p><strong><em> </em></strong></p>"
		"</td>"

		"<td width=\"100\">"
		"<p>   </p>"
		"</td>"

		"</tr>"

		"<tr>" 
		"<td width=\"160\">"
		"<p><strong><em>Type of configuration</em></strong></p>"
		"</td>"

		"<td width=\"180\">"
		"<p>");
		
		html_content.append(configuration_type);
		html_content.append("</p>"
		"</td>"

		"<td width=\"100\">"
		"<p><strong><em> </em></strong></p>"
		"</td>"

		"<td width=\"100\">"
		"<p>   </p>"
		"</td>"

		"</tr>"


		"</tbody>"
		"</table>"
		//end table 2

		// just a bit of space
			"<p>&nbsp;</p>"
			"<p align=\"center\" ><strong><em>Test table</em></strong></p>"

		// table 3
		"<table border=\"1\" align=\"center\" cellspacing=\"0\" cellpadding=\"4\">"
		"<tbody>"

		"<tr>"
		"<td width=\"120\">"
		"<p><strong><em># of test</em></strong>"
		"</td>"
		"<td width=\"120\">"
		"<p><strong><em>Force</em></strong></p>"
		"<p><strong><em> F [g(wt)] </em></strong></p>"
		"</td>"
		"<td width=\"120\">"
		"<p><strong><em>Stiffness</em></strong></p>"
		"<p><strong><em> k [g(wt)/mm] </em></strong></p>"
		"</td>"
		"<td width=\"120\">"
		"<p><strong><em>Deformation</em></strong></p>"
		"<p><strong><em> x [mm] </em></strong></p>"
		"</td>"
		"</tr>"
			);


		for (int i = 0; i < m_dist_measures.size(); i++)
		{
			html_content.append(generateColumnForTableHTML(i));
		}


		html_content.append(
			"</tbody>"
			"</table>"
			// add chart image here 
			"<p align=\"center\">"
			"<img src = \"");
		QString path = QDir::tempPath();
		path.append("/grafico.png");
		html_content.append(path);
		html_content.append("\" alt=\"grafico\" height=\"200\" width=\"200\" >"
			"&nbsp;</p>"

			"<div align=\"center\">"
			"<p><em> Hooke is an app made by </p>"
			"<p><img src=\":/icons/Logo.png\" alt=\"molle\" height=\"40\" width=\"90\"></p>"
			"<p><strong><em> Learning with fun! </em></strong></p>"
			"<p><a href='http://www.didacticoop.com/'>DidactiCoop srls</a> - via Don Bosco 4, Melendugno (LE, Italy) - info@didacticoop.com </p>"
			"</div>"
		"</font>"
		"</body>"
		"</html>");

	return html_content;
}

QString didacticoop::generateColumnForTableHTML(int _column)
{
	double k_hooke = 0; 
	if (ui->actionConnetti->isEnabled())
	{
		k_hooke = ui->doubleSpinBox_kHooke->value();
	}
	else
	{
		k_hooke = double(m_weight_measures.at(_column)) / 
			double(m_dist_measures.at(_column));

		if (isnan(k_hooke)) k_hooke = 0.0;

	}


	QString ss = (
		"<tr>"
			"<td width=\"120\">" + QString::number(_column + 1) + "</td>"
			"<td width=\"120\">"
			"<p> " + QString::number(m_weight_measures.at(_column)) + " </p>"
			"</td>"
			"<td width=\"120\">"
			"<p> " + QString::number(k_hooke, 'g', 2) + " </p>"			
			"</td>"
			"<td width=\"120\">"
			"<p> " + QString::number(m_dist_measures.at(_column)) + " </p>"
			"</td>"
		"</tr>"
		);

	return ss;
}





void didacticoop::showHookeMsg(const QString &_message, int _idx, double _F, double _k, double _x) {
	//ui->textEdit_statusMessageHooke->append(_message);

//	ui->tableWidget_hooke->setItem(_idx, 0, new QTableWidgetItem(QString::number(_F, 'g', 3)));
//	ui->tableWidget_hooke->setItem(_idx, 1, new QTableWidgetItem(QString::number(_k, 'g', 3)));
//	ui->tableWidget_hooke->setItem(_idx, 2, new QTableWidgetItem(QString::number(_x, 'g', 3)));


};

void didacticoop::pulisci()
{
	if (ui->stackedWidget->currentIndex() == 1)
	{

		QMessageBox::StandardButton resBtn =
			QMessageBox::question(this, "Information", "Are you sure?",
				QMessageBox::No | QMessageBox::Yes,
				QMessageBox::Yes);
		if (resBtn != QMessageBox::Yes) {
			
		}
		else {

			//ui->tableWidget_hooke->clear();
			m_hooke_simulator->pulisci();
		}
	}

}

void didacticoop::changePage() {

	if (connectCOM() == true)
	{
		ui->stackedWidget_hooke->setCurrentIndex(1);
		ui->actionSimula->setEnabled(true);
		ui->actionConnetti->setEnabled(false);

		// remove the curve in the chart
		m_hooke_simulator->pulisci();

	}
}

void didacticoop::hooke_page_1()
{
		ui->stackedWidget_hooke->setCurrentIndex(1);
		m_hooke_simulator->pulisci();
		m_num_of_points = 0;
}


void didacticoop::help()
{


	if (ui->stackedWidget->currentIndex() == 0)
	{
		QMessageBox messageBox;
		QString msg_title = "Help";
		QString msg_content = tr("You are in the home page, select an experiment" );
		//msg_content.append(m_version);
		messageBox.about(this, msg_title, msg_content);
		messageBox.setIconPixmap(QPixmap(":/icons/postit_hooke.png"));
		messageBox.setFixedSize(500, 700);
	}
	if (ui->stackedWidget->currentIndex() == 1)
	{
		QMessageBox messageBox;
		QString msg_title = "How it works";
		QString msg_content;
		QFile guida_hooke_html(":/guide/guidaHooke.html");
		if (!guida_hooke_html.open(QIODevice::ReadOnly))
		{
			msg_content = tr("<b>ERRORE Inserire qua una guida di Hooke <br>"
				"Hooke è nato bla bla bla <br> <br>"
				"e ha creato la sua legge fisica sulle molle nel .... <br>"
				"e quindi noi abbiamo adesso tante belle cose che funzionano!  ");
		}
		else {
			msg_content = QString(guida_hooke_html.readAll());
		}
		//msg_content.append(m_version);
		messageBox.about(this, msg_title, msg_content);
		messageBox.setIconPixmap(QPixmap("./icons/postit_hooke.png"));
		messageBox.setFixedSize(500, 700);
	}
	if (ui->stackedWidget->currentIndex() == 2)
	{
		QMessageBox messageBox;
		QString msg_title = "Help";
		QString msg_content = tr("DidactiCoop is constantly developing new engaging experiments ... <br>"
			"Keep your application up-to-date from our website <a href='http://www.didacticoop.com/'>DidactiCoop</a>!!!<br>"
			"  ");
		//msg_content.append(m_version);
		messageBox.about(this, msg_title, msg_content);
		messageBox.setIconPixmap(QPixmap("./icons/Logo.png"));
		messageBox.setFixedSize(500, 700);
	}



}



void didacticoop::openSettingsFile() {  // open setting file

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::openFile    " << endl;

	QString _path = QFileDialog::getOpenFileName(this, "Carica un esperimento", QDir::currentPath(),  
		"Profile file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) {
		QMessageBox::information(this, "Information", "Nessun esperimento caricato<br>" + _path);
		return;
	}




}

void didacticoop::saveSettingsFile() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::saveFile    " << endl;

	QString _path = QFileDialog::getSaveFileName(this, "Salva un esperimento", QDir::currentPath(), 
		"Profile file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) {
		QMessageBox::information(this, "Information", "Nessun esperimento salvato<br>" + _path);
		return;
	}


}

void didacticoop::exit() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::closeEvent   " << endl;


	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, "Information", "Are you sure?",
			QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		// do nothing
	}
	else {

		QCoreApplication::quit();
		
	}
}

void didacticoop::closeEvent(QCloseEvent *event) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "didacticoop::closeEvent   " << endl;


	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, "Information", "Are you sure?",
			QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {
		simulationOnly();
		Sleep(200);
		
		event->accept();
	}
}

didacticoop::~didacticoop()
{
	delete painter_led_green_on;
	delete led_green_on;
	delete painter_led_red_on;
	delete led_red_on;
	delete painter_led_yellow_on;
	delete led_yellow_on;
	delete painter_led_grey;
	delete led_grey;
	delete painter_led_green_off;
	delete led_green_off;
	delete painter_led_red_off;
	delete led_red_off;
	delete painter_led_yellow_off;
	delete led_yellow_off;
	delete ui;
}