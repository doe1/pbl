var serialPort = require('serialPort').SerialPort;

var sp =new serialPort.SerialPort("/dev/ttyUSB1",{
    baudrate: 115200,
    parser:serialPort.parsers.readline("\n")
});

sp.on('data', function(data){
    console.log('data received: ' +data);
});
