//******************************************************************************************
//  File: PS_AdafruitSHT4x_TempHumid.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitSHT4x_TempHumid is a class which implements the "Temperature Measurement"  
//			  and "Relative Humidity Measurement" device capabilities.
//			  It inherits from the st::PollingSensor class.  The current version uses I2C to measure the 
//			  temperature and humidity from an SHT4x series sensor using the Adafruit_SHT4x library.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitSHT4x_TempHumid sensor1(F("SHT4x_1)", 60, 0, "temperature1", "humidity1", false, 100);
//
//			  st::PS_AdafruitSHT4x_TempHumid() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must be unique, but is not used for data transfer for this device
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- String strTemp - REQUIRED - name of temperature sensor to send to ST Cloud (e.g."temperature1")
//				- String strHumid - REQUIRED - name of humidity sensor to send to ST Cloud (e.g. "humidity1")
//				- bool In_C - OPTIONAL - true = Report Celsius, false = Report Fahrenheit (Fahrenheit is the default)
//				- byte filterConstant - OPTIONAL - Value from 5% to 100% to determine how much filtering/averaging is performed 100 = none (default), 5 = maximum
//
//            Filtering/Averaging
//
//            Filtering the value sent to ST is performed per the following equation
//
//            filteredValue = (filterConstant/100 * currentValue) + ((1 - filterConstant/100) * filteredValue) 
//
//			  This class supports receiving configuration data from the SmartThings cloud via the ST App.  A user preference
//			  can be configured in your phone's ST App, and then the "Configure" tile will send the data for all sensors to 
//			  the ST Shield.  For PollingSensors, this data is handled in the beSMart() function.
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2026-07-19  Dan Ogorchock  Original Creation
//
//******************************************************************************************

#include "PS_AdafruitSHT4x_TempHumid.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
    PS_AdafruitSHT4x_TempHumid::PS_AdafruitSHT4x_TempHumid(const __FlashStringHelper *name, unsigned int interval, int offset, String strTemp, String strHumid, bool In_C, byte filterConstant) :
	    PollingSensor(name, interval, offset),
		SHT4x(),
		m_fTemperatureSensorValue(-1.0),
		m_fHumiditySensorValue(-1.0),
		m_strTemperature(strTemp),
		m_strHumidity(strHumid),
		m_In_C(In_C)
	{
		//check for upper and lower limit and adjust accordingly
		if ((filterConstant <= 0) || (filterConstant >= 100))
		{
			m_fFilterConstant = 1.0;
		}
		else if (filterConstant <= 5)
		{
			m_fFilterConstant = 0.05;
		}
		else
		{
			m_fFilterConstant = float(filterConstant) / 100;
		}

	}
	
	//destructor
	PS_AdafruitSHT4x_TempHumid::~PS_AdafruitSHT4x_TempHumid()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitSHT4x_TempHumid::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_AdafruitSHT4x_TempHumid::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug) 
			{
				Serial.print(F("PS_AdafruitSHT4x_TempHumid::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_AdafruitSHT4x_TempHumid::init()
	{
	    bool status = SHT4x.begin();
		if (st::PollingSensor::debug)
		{
			if (!status) {
				Serial.println();
				Serial.println("Could not find a valid SHT4x sensor, check wiring!");
				Serial.println();
				delay(3000);
			}
		}
		// Set the SHT4x sensor precision (SHT4X_HIGH_PRECISION, SHT4X_MED_PRECISION, or SHT4X_LOW_PRECISION)
		SHT4x.setPrecision(SHT4X_HIGH_PRECISION);
		if (st::PollingSensor::debug)
		{
            switch (SHT4x.getPrecision()) {
                case SHT4X_HIGH_PRECISION: 
                    Serial.println("High precision");
                    break;
                case SHT4X_MED_PRECISION: 
                    Serial.println("Med precision");
                    break;
                case SHT4X_LOW_PRECISION: 
                    Serial.println("Low precision");
                    break;
            }
		}
		
		// Set the SHT4x sensor heater
	    // You can have 6 different heater settings
        // higher heat and longer times uses more power
        // and reads will take longer too!
		SHT4x.setHeater(SHT4X_NO_HEATER);
		if (st::PollingSensor::debug)
		{
            switch (SHT4x.getHeater()) {
                case SHT4X_NO_HEATER: 
                    Serial.println("No heater");
                    break;
                case SHT4X_HIGH_HEATER_1S: 
                    Serial.println("High heat for 1 second");
                    break;
                case SHT4X_HIGH_HEATER_100MS: 
                    Serial.println("High heat for 0.1 second");
                    break;
                case SHT4X_MED_HEATER_1S: 
                    Serial.println("Medium heat for 1 second");
                    break;
                case SHT4X_MED_HEATER_100MS: 
                    Serial.println("Medium heat for 0.1 second");
                    break;
                case SHT4X_LOW_HEATER_1S: 
                    Serial.println("Low heat for 1 second");
                    break;
                case SHT4X_LOW_HEATER_100MS: 
                    Serial.println("Low heat for 0.1 second");
                    break;
            }
		}		

		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitSHT4x_TempHumid::getData()
	{
		    sensors_event_t humidity, temp;
            SHT4x.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
			
			//Humidity
			if (m_fHumiditySensorValue == -1.0)
			{
				Serial.println("First time through Humidity");
				m_fHumiditySensorValue = humidity.relative_humidity;  //first time through, no filtering
			}
			else
			{
				m_fHumiditySensorValue = (m_fFilterConstant * humidity.relative_humidity) + (1 - m_fFilterConstant) * m_fHumiditySensorValue;
			}

			//Temperature
			if (m_fTemperatureSensorValue == -1.0)
			{
				Serial.println("First time through Temperature");
				//first time through, no filtering
				if (m_In_C == false)
				{
					m_fTemperatureSensorValue = (temp.temperature * 1.8) + 32.0;		//Scale from Celsius to Farenheit
				}
				else
				{
					m_fTemperatureSensorValue = temp.temperature;
				}
			}
			else
			{
				if (m_In_C == false)
				{
					m_fTemperatureSensorValue = (m_fFilterConstant * ((temp.temperature * 1.8) + 32.0)) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
				}
				else
				{
					m_fTemperatureSensorValue = (m_fFilterConstant * temp.temperature) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
				}

			}

		Everything::sendSmartString(m_strTemperature + " " + String(m_fTemperatureSensorValue));
		Everything::sendSmartString(m_strHumidity + " " + String(m_fHumiditySensorValue));

	}
	
}