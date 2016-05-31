var PORT = 55559;
//var HOST = '127.0.0.1';

var TOUCHPIN = 7;

var dgram = require('dgram');
var server = dgram.createSocket('udp4');
//const client = dgram.createSocket('udp4');

var serialport = require('serialport');	// serial library

var gpio = require("pi-gpio");


// start the serial port connection and read on newlines
var serial = new serialport.SerialPort('/dev/ttyAMA0', { //process.argv[2], {
    parser: serialport.parsers.readline('\n'),
    baudrate: 115200
});

// this is the serial port event handler.
// read the serial data coming from arduino - you must use 'data' as the first argument
// and send it off to the client using a socket message
serial.on('data', function(data) {
    console.log('serial in data - \"' + data + "\"");
/*
	if(data == 't') {
		//messageToSim = new Buffer('touched');
		//client.send(messageToSim, 41234, '192.168.0.1', (err) => {
		//  client.close();
		//});
		gpio.open(16, "output", function(err) {		// Open pin 16 for output 
			gpio.write(TOUCHPIN, 1, function() {			// Set pin 16 high (1) 
				gpio.close(16);						// Close pin 16 
			});
		});

	}
	else if(data == 'n') {
		//messageToSim = new Buffer('not');
		//client.send(messageToSim, 41234, '192.168.0.1', (err) => {
		//  client.close();
		//});
		gpio.open(16, "output", function(err) {		// Open pin 16 for output 
			gpio.write(TOUCHPIN, 0, function() {			// Set pin 16 low (0) 
				gpio.close(16);						// Close pin 16 
			});
		});
	}
*/
});


server.bind(PORT); //, HOST);

server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
    //serial.write('TEST\n');
});


server.on('message', function (message, remote) {
    //console.log('message from ' + remote.address + ':' + remote.port +' - ' + message);
    //var steerAngle = parseFloat(message);
    serial.write(message);// +'\n');
    serial.write('\n');
});
