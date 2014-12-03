/*
 * Server 
 * IPRO 497-319
 * Authors: Christopher Hannon
 * Based off of codeProjects article titled:
 * 'Arduino and the Web using NodeJS and SerialPort'
 * 
 * WoodStove
 * Used to collect data from arduino and format it for excell. 
 * Version 1.1 xubuntu 
 *
 */

//note can not run serial monitor (Arduino IDE) at the same time

//open serial port.. (may need to change port name)
var SerialPort  = require('serialport').SerialPort;
var portName = "/dev/ttyACM0"; //change this to whatever the arduino IDE reports the serial com port 
//port settings
var sp = new SerialPort(portName, { // portName is instatiated to be COM3, replace as necessary
    baudRate: 9600, // this is synced to what was set for the Arduino Code
    dataBits: 8, // this is the default for Arduino serial communication
    parity: 'none', // this is the default for Arduino serial communication
    stopBits: 1, // this is the default for Arduino serial communication
    flowControl: false // this is the default for Arduino serial communication
}); // instantiate the serial port.

var cleanData = ''; // this stores the clean data
var readData = '';  // this stores the buffer

sp.on('open', function(){
    //open log file    
    var fs = require('fs');
        
    console.log('dateTime;Temp1F;Temp1C;O2vm;ParticulatesPPM;CO;fan'); //headers
    //change file name to whatever location you want
    fs.appendFile('/tmp/stoveLog.csv','dateTime;Temp1F;Temp1C;O2vm;ParticulatesPPM;CO;fan\n',function(err) {
	if(err) throw err;
    });
    sp.on('data', function (data) { // call back when data is received
	readData += data.toString();
	//console.log("readData = " + readData);
	//console.log( data.toString()); 
	if (readData.indexOf('B') >= 0 && readData.indexOf('A') >= 0) {
            cleanData = readData.substring(readData.indexOf('A') + 1, readData.indexOf('B'));
	    //uses ABSerial protocol wrap 'sample text' as 'Asample textB'
	    //parse and tokenize
	    Float64Array = cleanData.split(";");
	    
	    // change file name here too
	    fs.appendFile('/tmp/stoveLog.csv',getDateTime()+';'+cleanData+"\n",function(err) {
		if(err)throw err;
	    });
	    console.log(getDateTime()+';'+cleanData);
            
	    readData = '';
	}
    });
});

function getDateTime() {
    var now     = new Date(); 
    var year    = now.getFullYear();
    var month   = now.getMonth()+1; 
    var day     = now.getDate();
    var hour    = now.getHours();
    var minute  = now.getMinutes();
    var second  = now.getSeconds(); 
    if(month.toString().length == 1) {
        var month = '0'+month;
    }
    if(day.toString().length == 1) {
        var day = '0'+day;
    }   
    if(hour.toString().length == 1) {
        var hour = '0'+hour;
    }
    if(minute.toString().length == 1) {
        var minute = '0'+minute;
    }
    if(second.toString().length == 1) {
        var second = '0'+second;
    }   
    var dateTime = year+'/'+month+'/'+day+';'+hour+':'+minute+':'+second;   
    return dateTime;
}
