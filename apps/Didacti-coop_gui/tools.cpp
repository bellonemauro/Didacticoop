/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "tools.h"
#include  <QCheckBox>

Didacticoop_tools::Didacticoop_tools(QWidget *parent ):
	QMainWindow(parent),
	ui_tools(new Ui::Didacticoop_tools), 
	m_setting_file_name("./settings/settings.ini"),
	m_timer(new QTimer),
	m_serial(new QSerialPort)
{
	ui_tools->setupUi(this );

	m_IR_measure = new std::vector<double>();

	//load settings from file
	loadSettings(m_setting_file_name);

	//make sure to start from the initial page
	ui_tools->actionGeneral->setChecked(true);
	ui_tools->stackedWidget->setCurrentIndex(0);

	ui_tools->comboBox_serialInfo->clear();

	connect(ui_tools->comboBox_serialInfo,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Didacticoop_tools::showPortInfo);

	ui_tools->descriptionLabel->hide();
	ui_tools->manufacturerLabel->hide();
	ui_tools->serialNumberLabel->hide();
	ui_tools->locationLabel->hide();
	ui_tools->vidLabel->hide();
	ui_tools->pidLabel->hide();

	// enumerate connected com ports
	enumeratePorts();

	// connect GUI elements: communication tab

	connect(ui_tools->actionGeneral,
		SIGNAL(triggered()), this,
		SLOT(goToPage1()));

	connect(ui_tools->actionCommunication,
		SIGNAL(triggered()), this,
		SLOT(goToPage2()));

	connect(ui_tools->actionTest,
		SIGNAL(triggered()), this,
		SLOT(goToPage3()));

	connect(ui_tools->pushButton_toDefault,
		SIGNAL(clicked()), this, SLOT(resetToDefaultValues()));

	// connect tool window events Ok, Cancel, Apply
	connect(ui_tools->buttonBox->button(QDialogButtonBox::Ok), 
		SIGNAL(clicked()), this, SLOT(okPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Cancel), 
		SIGNAL(clicked()), this, SLOT(cancelPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Apply), 
		SIGNAL(clicked()), this, SLOT(applyPressed()));

	connect(m_serial, &QSerialPort::readyRead, this, &Didacticoop_tools::readData);

	//connect(m_serial, &QSerialPort::readyRead, this, &didacticoop::handleReadyRead);
	connect(m_serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
		this, &Didacticoop_tools::handleError);
	connect(&m_timer, &QTimer::timeout, this, &Didacticoop_tools::handleTimeout);

	 // enable/disable GUI buttons
	ui_tools->pushButton_connectCOM->setEnabled(true);
	ui_tools->pushButton_disconnectCOM->setEnabled(false);
	ui_tools->pushButton_sendMessage->setEnabled(false);
	ui_tools->lineEdit_writeData->setEnabled(false);

	// connect buttons 
	connect(ui_tools->pushButton_connectCOM, SIGNAL(clicked()), this, SLOT(connectCOM()));
	connect(ui_tools->pushButton_disconnectCOM, SIGNAL(clicked()), this, SLOT(disconnectCOM()));
	connect(ui_tools->pushButton_sendMessage, SIGNAL(clicked()), this, SLOT(sendData()));
	connect(ui_tools->lineEdit_writeData, SIGNAL(returnPressed()), this, SLOT(sendData()));
	connect(ui_tools->pushButton_enumerate, SIGNAL(clicked()), this, SLOT(enumeratePorts()));

	ui_tools->comboBox_serialInfo->clear();
	connect(ui_tools->comboBox_serialInfo,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Didacticoop_tools::showPortInfo);
}

void Didacticoop_tools::goToPage1()
{
	ui_tools->actionGeneral->setChecked(true);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(0);

}

void Didacticoop_tools::tryConnect()
{
	enumeratePorts();
	okPressed();
}


void Didacticoop_tools::goToPage2()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionCommunication->setChecked(true);
	ui_tools->actionTest->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(1);
}
void Didacticoop_tools::goToPage3()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->actionTest->setChecked(true);
	ui_tools->stackedWidget->setCurrentIndex(2);
}

void Didacticoop_tools::okPressed() {

	//TODO manual save for now
	//saveSettings();
	applyPressed();

	emit ok();
	this->close();
}

void Didacticoop_tools::cancelPressed() {

	emit cancel();
	this->close();
}


void Didacticoop_tools::applyPressed() {

	//TODO manual save for now
	//saveSettings();
	m_serial->setPortName(ui_tools->comboBox_serialInfo->currentText()); //(comSettings->name);
	m_serial->setBaudRate(ui_tools->comboBox_baudRate->currentText().toInt());// (comSettings->baudRate);

	m_serial->setDataBits(static_cast<QSerialPort::DataBits>(ui_tools->comboBox_dataBit->currentText().toInt()));
	m_serial->setParity(static_cast<QSerialPort::Parity>(ui_tools->comboBox_parity->currentIndex()));
	m_serial->setStopBits(static_cast<QSerialPort::StopBits>(ui_tools->comboBox_stopBit->currentIndex()));
	m_serial->setFlowControl(static_cast<QSerialPort::FlowControl>(ui_tools->comboBox_flowControl->currentIndex()));

	saveSettings(m_setting_file_name);

	emit apply();
}



void Didacticoop_tools::showPortInfo(int idx)
{
	if (idx == -1)
		return;


	/*QStringList list = ui_tools->comboBox_serialInfo->itemData(idx).toStringList();
	ui_tools->descriptionLabel->setText(
		tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr("N/A")));
	ui_tools->manufacturerLabel->setText(
		tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr("N/A")));
	ui_tools->serialNumberLabel->setText(
		tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr("N/A")));
	ui_tools->locationLabel->setText(
		tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr("N/A")));
	ui_tools->vidLabel->setText(
		tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr("N/A")));
	ui_tools->pidLabel->setText(
		tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr("N/A")));*/
}


void Didacticoop_tools::connectCOM()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	ui_tools->textEdit_console->append(" connecting COM ...  \n");
	m_serial->setPortName(ui_tools->comboBox_serialInfo->currentText()); //(comSettings->name);
	m_serial->setBaudRate(ui_tools->comboBox_baudRate->currentText().toInt());// (comSettings->baudRate);

	m_serial->setDataBits(static_cast<QSerialPort::DataBits>(ui_tools->comboBox_dataBit->currentText().toInt()));
	m_serial->setParity(static_cast<QSerialPort::Parity>(ui_tools->comboBox_parity->currentIndex()));
	m_serial->setStopBits(static_cast<QSerialPort::StopBits>(ui_tools->comboBox_stopBit->currentIndex()));
	m_serial->setFlowControl(static_cast<QSerialPort::FlowControl>(ui_tools->comboBox_flowControl->currentIndex()));

	if (m_serial->open(QIODevice::ReadWrite)) {
		// enable/disable GUI buttons
		ui_tools->pushButton_connectCOM->setEnabled(false);
		ui_tools->pushButton_disconnectCOM->setEnabled(true);
		ui_tools->pushButton_sendMessage->setEnabled(true);
		ui_tools->lineEdit_writeData->setEnabled(true);
		//ui_tools->groupBox_comSettings->setEnabled(false);
		QString ss(tr("Connected to %1 : %2, %3, %4, %5, %6 \n")
			.arg(m_serial->portName()).arg(m_serial->baudRate()).arg(m_serial->dataBits())
			.arg(m_serial->parity()).arg(m_serial->stopBits()).arg(m_serial->flowControl()));
		//cout << ss.toStdString() << endl;
		ui_tools->textEdit_console->append(ss);
		m_timer.start(50);

	}
	else {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QString ss(tr("Could not connect to %1 : %2, %3, %4, %5, %6 \n")
			.arg(m_serial->portName()).arg(m_serial->baudRate()).arg(m_serial->dataBits())
			.arg(m_serial->parity()).arg(m_serial->stopBits()).arg(m_serial->flowControl()));
		ui_tools->textEdit_console->append(ss);
		QMessageBox::critical(this, tr("Error"), m_serial->errorString());
		return;
		//cout << "Open error" << endl;
	}

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}

void Didacticoop_tools::disconnectCOM()
{
	if (m_serial->isOpen())
		m_serial->close();

	// enable/disable GUI buttons
	ui_tools->pushButton_connectCOM->setEnabled(true);
	ui_tools->pushButton_disconnectCOM->setEnabled(false);
	ui_tools->pushButton_sendMessage->setEnabled(false);
	ui_tools->lineEdit_writeData->setEnabled(false);
	//	ui_tools->groupBox_comSettings->setEnabled(true);
	return;
}

void Didacticoop_tools::handleTimeout()
{
	if (m_data.isEmpty()) {
		//ui_tools->textEdit_console->append(QObject::tr("No data was currently available for reading from port %1").arg(m_serial->portName()));
	}
	else {
		//ui_tools->textEdit_console->append(QObject::tr("Data successfully received from port %1").arg(m_serial->portName()));
		//ui_tools->textEdit_console->append(QString::number(m_data.size()));
		QString data = m_data;
		QStringList lista = data.split("\n");
		ui_tools->textEdit_console->append(lista.last());

		m_data.clear();
	}

}

void Didacticoop_tools::handleError(QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::ResourceError) {
		QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
		disconnectCOM();
	}
}

void Didacticoop_tools::handleReadyRead()
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

void Didacticoop_tools::readData()
{

	if (m_serial->isReadable()) {


		if (m_serial->bytesAvailable() < 100)
		{   //if the package is not complete, wait for more data
			return;
		}
		else {

			//QByteArray data = serial->readAll(); // / readLine();//->readAll();

			m_data.append(m_serial->readLine());

			QByteArrayList data = m_data.split('#'); //(' '); // ('#');

			//string format = volts_IR#distance_mm#volts_CC#weight_g#/n
			QByteArray volts_IR = "0";
			QByteArray distance_mm = "0";
			QByteArray volts_CC = "0";
			QByteArray weight_g = "0";
			if (data.size() > 2)
			{
				volts_IR = data.at(0);
				distance_mm = data.at(1);
				volts_CC = data.at(2);
				weight_g = data.at(3);
			}
			//std::string str = m_data.toStdString();
			//ui_tools->textEdit_console->append(QString::fromStdString(str));


			//ui_tools->textEdit_console->append("Received data : ");
			ui_tools->textEdit_console->append(m_data);//(number);

			double volt_IR_d = volts_IR.toDouble();
			double distance_mm_d = distance_mm.toDouble();
			double volts_CC_d = volts_CC.toDouble();
			double weight_g_d = weight_g.toDouble();
			//double value = atof(str.c_str());

			ui_tools->lcdNumber_voltIR->display(volt_IR_d);
			ui_tools->lcdNumber_distanceIR->display(distance_mm_d);
			ui_tools->lcdNumber_voltCC->display(volts_CC_d);
			ui_tools->lcdNumber_weight->display(weight_g_d);
			m_data.clear();
		}
	}
}



void Didacticoop_tools::sendData()
{
	QByteArray data;
	data = ui_tools->lineEdit_writeData->text().toUtf8();
	data.append("\n");
	m_serial->write(data);

	//cout << " sent string " << data.toStdString() << endl;
	ui_tools->textEdit_console->append(" sent string ");
	ui_tools->textEdit_console->append(data);
	ui_tools->textEdit_console->append(" \n");
	return;
}


void Didacticoop_tools::enumeratePorts()
{
	QString vendor = "16d0"; //TODO 

	ui_tools->comboBox_serialInfo->clear();

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos) {
		QString s = QObject::tr("Port: ") + info.portName() + "\n"
			+ QObject::tr("Location: ") + info.systemLocation() + "\n"
			+ QObject::tr("Description: ") + info.description() + "\n"
			//+ QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
			+ QObject::tr("Serial number: ") + info.serialNumber() + "\n"
			+ QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
			+ QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
			+ QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

		ui_tools->textEdit_console->append(s);
		ui_tools->comboBox_serialInfo->addItem(info.portName());

		QString v = QString::number(info.vendorIdentifier(), 16);
		if (v == vendor)
		{//TODO: this only attempt to match the vendor, is it also possible to check the product?
			ui_tools->textEdit_console->append(" This could be the right device \n\n");
		}
	} // TODO: in debug the detor crushes !
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return;
}


bool Didacticoop_tools::loadSettings(QString _path)
{

	if (_path.isEmpty())
	{
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			<< "Warning  ::  Setting file not found \n please set the setting file " << endl;
		//_path = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		//	"Ini file (*.ini);; Data file (*.dat);; Binary File (*.bin);; All Files(*.*)", 0);
	}

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "the path is : " << _path.toStdString() << endl;

	m_settings = new QSettings(_path, QSettings::IniFormat);

	// read default group
	QString user = m_settings->value("default/user", "Student").toString();
	ui_tools->lineEdit_userName->setText(user);

	//QDate date = m_settings->value("default/year", "2018").toInt();
	ui_tools->dateEdit->setDate(QDate::currentDate());

	// read com group
	//ComName
	QString comPort = m_settings->value("COM/ComName", "COM7").toString();
	m_serial->setPortName(comPort);

	//BaudRate
	int baudRate = m_settings->value("COM/BaudRate", "115200").toInt();
	m_serial->setBaudRate(baudRate);// (comSettings->baudRate);
	ui_tools->comboBox_baudRate->setCurrentIndex(7);  // baudrate forced value 115200 


	return true;
}

bool Didacticoop_tools::saveSettings(QString _file_name)
{

	//m_settings->setPath(QSettings::IniFormat, QSettings::UserScope, _file_name);
	QSettings *settings = new QSettings(_file_name, QSettings::IniFormat);
	// [default]
	settings->setValue("default/user", ui_tools->lineEdit_userName->text());


	// [COM]
	settings->setValue("COM/ComName", ui_tools->comboBox_serialInfo->currentText()); 
	settings->setValue("COM/BaudRate", ui_tools->comboBox_baudRate->currentText()); 
	settings->setValue("COM/DataBits", ui_tools->comboBox_dataBit->currentText());
	settings->setValue("COM/Parity", ui_tools->comboBox_parity->currentText());
	settings->setValue("COM/StopBits", ui_tools->comboBox_stopBit->currentText());
	settings->setValue("COM/FlowControl", ui_tools->comboBox_flowControl->currentText());

	settings->sync();
	return true;
}


void Didacticoop_tools::resetToDefaultValues()
{
	loadSettings("");
}


Didacticoop_tools::~Didacticoop_tools() {
	delete ui_tools;
}