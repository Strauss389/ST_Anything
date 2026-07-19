//******************************************************************************************
//  File: PS_AdafruitSHT4x_TempHumid.h
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

#ifndef ST_PS_AdafruitSHT4x_TempHumid_H
#define ST_PS_AdafruitSHT4x_TempHumid_H

#include "PollingSensor.h"
#include <Adafruit_SHT4x.h>

namespace st
{
	class PS_AdafruitSHT4x_TempHumid: public PollingSensor
	{
		private:
			float m_fTemperatureSensorValue; //current Temperature value
			float m_fHumiditySensorValue;	   //current Humidity Value
			Adafruit_SHT4x SHT4x;		    //I2C SHT4x library object
			String m_strTemperature;		//name of temparature sensor to use when transferring data to ST Cloud / Hubitat
			String m_strHumidity;			//name of humidity sensor to use when transferring data to ST Cloud / Hubitat		
			bool m_In_C;					//Return temp in C (true or false)
			float m_fFilterConstant;        //Filter constant % as floating point from 0.00 to 1.00

		public:
			//constructor - called in your sketch's global variable declaration section 
			PS_AdafruitSHT4x_TempHumid(const __FlashStringHelper *name, unsigned int interval, int offset, String strTemp = "temperature1", String strHumid = "humidity1", bool In_C = false, byte filterConstant = 100);

			//destructor
			virtual ~PS_AdafruitSHT4x_TempHumid();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline float getTemperatureSensorValue() const { return m_fTemperatureSensorValue; }
			inline float getHumiditySensorValue() const { return m_fHumiditySensorValue; }
	};
}
#endif
