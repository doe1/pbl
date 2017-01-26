var serialPort = require('serialPort').SerialPort;

var sp =new serialPort.SerialPort("/dev/xxxx",{ //xxxx:自分の刺しているポート名
    baudrate: 115200,
    parser:serialPort.parsers.readline("\n")
});

sp.on('data', function(data){
    console.log('data received: ' +data);
});
