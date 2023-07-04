//Initate plot with Google Charts API
google.charts.load('current', { 'packages': ['corechart'] });
google.charts.setOnLoadCallback(drawChart);

//Label Button
const bigButton = document.getElementById('bigButton');

//Label Text
const batteryLevel = document.getElementById('Batterylevel');
const startTime = performance.now();

// Initiate Variable
let constData = new Array([0, 0]);
let counter = 0;
let timestamp = 0;
let volts = 0;

//Callback if Button clicked
bigButton.addEventListener('click', function (event) {
    connectBLE();
});

function connectBLE() //function to connect with peripheral
{
    let serviceUuid = '4fafc201-1fb5-459e-8fcc-c5c9c331914b'; // define service UUID
    //let characteristicUuid = '';
    navigator.bluetooth.requestDevice({//filter BLE peripheral with defined service UUID
        filters: [{
            services: [serviceUuid]
        }]
    })
        .then(device => device.gatt.connect()) //connect to peripheral
        //console.log("Connected");
        .then(server => {
            console.log('Getting Service...');
            console.log(server.getPrimaryService(serviceUuid));
            return server.getPrimaryService(serviceUuid);
        })
        .then(service => {
            console.log('Getting Characteristics...');
            // Get all characteristics.
            return service.getCharacteristics();  //read all device Characteristics
        })
        .then(characteristics => {
            // Zuordnung UUIDs fest nach tatsächlicher UUID
            //      let batteryUuid = "2A19";  //create objects for sensor and battery characteristic
            //let sensorUuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

            // setup notifications and event listerner for changed characteristic value
            //      batteryLevelCharacteristic = characteristics[0];
            //     batteryLevelCharacteristic.startNotifications();
            //batteryLevelCharacteristic.addEventListener('characteristicvaluechanged',
            //  handleBatteryLevelChanged);

            //      sensorLevelCharacteristic = characteristics[1];
            sensorLevelCharacteristic = characteristics[0];
            console.log(sensorLevelCharacteristic.uuid);
            sensorLevelCharacteristic.startNotifications();
            console.log("sensorLevelCharacteristic.uuid");
            console.log(sensorLevelCharacteristic.uuid);

            if (sensorLevelCharacteristic.uuid == "4a980baa-1cc4-e7c1-c757-f1267dd021e8") {
                console.log('adding EventListener for Sensor and Time Values...');
                sensorLevelCharacteristic.addEventListener('characteristicvaluechanged',
                    handleSensorTimeValueChanged);
            }
            if (sensorLevelCharacteristic.uuid == "4fafc201-1cc4-e7c1-c757-f1267dd021e8") {
                console.log('adding EventListener for Sensor Values...');
                sensorLevelCharacteristic.addEventListener('characteristicvaluechanged',
                    handleSensorValueChanged);
            }

            // alert("Recording starts...");

            //      console.log(batteryUuid + sensorUuid);
            //console.log(sensorUuid);
            var elem = document.getElementById("myBar");
            var width = 1;
            var id = setInterval(frame, 5 * 60 * 1000);

            function frame() {
                if (width >= 100) {
                    clearInterval(id);
                    i = 0;
                } else {
                    width++;
                    elem.style.width = width + "%";
                }

            }
        })

}

function handleSensorValueChanged(event) { //fuction for changed sensor value
    let newData = event.target.value.getFloat32(0, true);  //save value in newData
    for (var i = 0; i < newData.length; i += 2) {
        var byte = parseInt(newData.substring(i, i + 2), 16);
        if (byte > 127) {
            byte = -(~byte & 0xFF) - 1;
        }
        bytes.push(byte);
    }
    console.log(byte);

    timestamp = performance.now() - startTime;  //save timestamp
    //volts = newData; //& 0x00000FFF;//save voltage from ecg value
    //constData.push([timestamp, volts]); //save data in array
    constData.push([timestamp, newData]); //save data in array
    counter++;
    if (constData.length >= 400)  // only collect last 400 values
    {
        constData.shift();
    }
    if (counter > 50)  // update plot after 50 values
    {
        counter = 0;
        drawChart();
    }
}

function handleSensorTimeValueChanged(event) { //fuction for changed sensor value
    timestamp =event.target.value.getUint32(0, true);  //save timestamp
    volts = event.target.value.getFloat32(4, true);//save voltage from ecg value
    constData.push([timestamp, volts]); //save data in array
    counter++;
    if (constData.length >= 400)  // only collect last 400 values
    {
        constData.shift();
    }
    if (counter > 50)  // update plot after 50 values
    {
        counter = 0;
        drawChart();
    }
}

function drawChart() {  //update plot function
    var data = new google.visualization.DataTable();
    data.addColumn('number', 'time');
    data.addColumn('number', 'signal');
    data.addRows(constData);

    var options = {
        title: 'EKG',
        curveType: 'function',
        legend: { position: 'bottom' }
    };
    var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));
    chart.draw(data, options);
}
