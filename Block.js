var express = require('express');
var app = express(); //app used to host mysql database
var mysql = require('mysql'); //mysql database object
var opn = require('opn'); //object for automated link opening
var events = require('events'); //event object to provide actions to Node.js code
var emitter = new events.EventEmitter(); 
var serialport = require('serialport'); //used to communitcate with arduino
var port = new serialport('COM5', {baudRate: 9600});
var numOne; //the numbers passed for each rfid code
var numTwo;
var numThree;
var numFour; 

emitter.on("tagged", function(link) { //emitter event to open localhost link and provide data
  opn(link);
});

port.on('data', function(data) { //the reader that activates the emitter to open localhost
  emitter.emit("tagged", "http://localhost:1337");
});

var con = mysql.createConnection({ //database connection information
  host: "localhost",
  user: "root",
  password: "",
  database: 'BlockBase'
});

con.connect(function(err) { //connecting to the database with provided information
  if (!!err){
    console.log("error!");
  } else {
    console.log("Connected");
  }
});

app.get('/', function(req, res) { //activates on access to the localhost server
  con.query("SELECT * FROM TagTable", function(err, rows, fields) { //querys database for info
    if(!!err) {
      console.log("Error in query");
    } else  {
      console.log("Successful query");
      numOne = rows[0].NumOne; //grabs the individual numbers in the 4 number rfid code
      numTwo = rows[0].NumTwo;
      numThree = rows[0].NumThree;
      numFour = rows[0].NumFour;
      port.on('data', function(err, numOne, numTwo, numThree, numFour) { 
        //opening serial port to send data to arduino (incomplete)
        if(!!err) {
          console.log("Error in send");
        } else  {
          console.log("Successful code send");
          port.write(numOne);
          port.write(numTwo);
          port.write(numThree);
          port.write(numFour);
        }
      });
    }
  });
});

app.listen(1337); //mysql server listens on 1337 port and sets localhost server to that port