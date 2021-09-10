$(document).ready(() => {
  const POWER_CHANNEL_NUM = 2;

  /* Common used div for each component */
  const powerChannelDiv = [
    $('#sp3-power-channel-1'),
    $('#sp3-power-channel-2')
  ];

  /* Seven Segment initialize */
  const sevenSegCommonOpt = {
    digits: 5,
    decimalPlaces: 3,
    value: 0,
  }

  for (let i = 0; i < POWER_CHANNEL_NUM; i++) {
    powerChannelDiv[i].find(".sp3-sevenseg-voltage").sevenSeg(sevenSegCommonOpt);
    powerChannelDiv[i].find(".sp3-sevenseg-ampere").sevenSeg(sevenSegCommonOpt);
    powerChannelDiv[i].find(".sp3-sevenseg-watt").sevenSeg(sevenSegCommonOpt);
  }

  /* Charts initialize */
  const toastLineChartDom = [
    powerChannelDiv[0].find('.sp3-line-chart')[0],
    powerChannelDiv[1].find('.sp3-line-chart')[0],
  ]

  // TODO: How can I fix the column numbers without this dummy data?
  const toastLineChartData = {
    categories: [
      '', '', '', '', '', '', '', '',
      '', '', '', '', '', '', '', ''
    ],
    series: [
      {
        name: 'Voltage',
        data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      },
      {
        name: 'Ampere',
        data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      },
      {
        name: 'Watt',
        data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      }
    ],
  };

  const toastLineChartOptions = {
    chart: { title: 'Voltage/Ampere/Watts', width: 'auto', height: 'auto' },
    xAxis: {
      title: 'Timeline',
    },
    yAxis: {
      title: 'Vol/Amp/Wat',
    },
    tooltip: {
      formatter: (value, tooltipDataInfo) => {
        return `${value}`;
      }
    },
    legend: {
      align: 'bottom',
    },
    series: {
      shift: true
    },
    // responsive: { }
  };

  const toastLineChart = [
    toastui.Chart.lineChart({
      el: toastLineChartDom[0],
      data: toastLineChartData,
      options: toastLineChartOptions
    }),
    toastui.Chart.lineChart({
      el: toastLineChartDom[1],
      data: toastLineChartData,
      options: toastLineChartOptions
    })
  ];

  /* Socket initialize */
  const webSocket = new WebSocket('ws://' + window.location.host + '/socket');

  webSocket.onopen = () => {
    console.log("Socket connection established!");
  }

  webSocket.onclose = (event) => {
    console.log("Socket connection closed!");

    // Try to reconnect
    console.log("Reconnect will be attempted in 1 second...", event.reason);
    setTimeout(() => {
      webSocket.close();
      webSocket.open();
    } , 1000);
  }

  webSocket.onmessage = (event) => {
    // The data will be a JSON formatted string
    const received = JSON.parse(event.data);
    const dataType = received['type'];
    const data = received['data'];

    console.log(event.data);

    switch (dataType) {
      case undefined:
        console.log("Unformatted message recevied", received);
        break;
      case 'log':
        // Log levels: 0 - info, 1 - warning, 2 - error
        /*
        {
          "type": "log",
          "data": {
            "message": "Some message",
            "level": 0,
          }
        }
        */
        console.log("[" + data['level'] + "]", data['message']);
        break;
      case 'power':
        /*
        {
          "type": "power",
          "data": [
            {
              "channel": 0,
              "active": true,
              "voltage": "12000",
              "ampere": "3000",
              "watt": "36000"
            },
            {
              "channel": 1,
              "active": false,
              "voltage": "5000",
              "ampere": "0",
              "watt": "0"
            }
          ]
        }
        */
        for (let i = 0; i < POWER_CHANNEL_NUM; i++) {
          const ch = data[i]['channel'];
          const isActive = data[i]['active'];
          const vol = isActive ? Number(data[i]['voltage']) / 1000 || "0" : null;
          const amp = isActive ? Number(data[i]['ampere']) / 1000 || "0" : null;
          const watt = isActive ? Number(data[i]['watt']) / 1000 || "0" : null;

          // Update the seven segment widgets
          powerChannelDiv[ch].find(".sp3-sevenseg-voltage").sevenSeg({ value: vol });
          powerChannelDiv[ch].find(".sp3-sevenseg-ampere").sevenSeg({ value: amp });
          powerChannelDiv[ch].find(".sp3-sevenseg-watt").sevenSeg({ value: watt });

          // Update the chart widgets
          if (isActive) {
            toastLineChart[i].addData([vol, amp, watt], moment().format("HH:mm:ss SSS"));
          } else {
            toastLineChart[i].addData([0, 0, 0], moment().format("HH:mm:ss SSS"));
          }
        }

        // TODO: Debug the data format
        console.log("Power data received", data);
        break;
      case 'setting':
        /*
        {
          "type": "setting",
          "data": {
            "lcdBacklightLevel": 3,
            "fanSpeedLevel": 3,
          },
        }
        */
        // TODO: Debug the data format
        console.log("Setting data received", data);
        break;
    }
  }

  webSocket.onerror = (error) => {
    console.log("Socket error!", error);
    webSocket.close();
  }

  // Request the current power information every 1 second
  setInterval(() => {
    webSocket.send(JSON.stringify({
      command: 1, // The command number for getting current power information
    }));
  }, 1000);
});
