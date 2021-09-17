$(document).ready(() => {
  const POWER_CHANNEL_NUM = 2;

  /* Common used div for each component */
  const powerChannelDiv = [
    $('.sp3-power-channel-0'),
    $('.sp3-power-channel-1')
  ];

  /* Initialize the power buttons for each channel */
  for (let i = 0; i < POWER_CHANNEL_NUM; i++) {
    powerChannelDiv[i].find('.sp3-power-button').on('click', () => {
      webSocket.send(JSON.stringify({
        command: 3, // The command number for setting something
        data: {
          channel: i,
          channelPowerSwitch: true, // Switch the current state of the channel
          voltage: null,
          currentLimit: null,
          backlightLevel: null,
          fanSpeedLevel: null,
          logIntervalLevel: null,
          apName: null,
          apPassword: null,
        }
      }));
    });
  }

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

  /* Adjustment input widgets initialize */
  let isInputWidgetsChanging = false;
  let newVolSetValue = [0, 0];
  let newAmpCurLimValue = [0, 0];

  for (let i = 0; i < POWER_CHANNEL_NUM; i++) {
    const volAdjInput = powerChannelDiv[i].find(".sp3-voltage-adjustment-input");
    const volAdjValue = powerChannelDiv[i].find(".sp3-voltage-adjustment-value");
    const volAdjApply = powerChannelDiv[i].find(".sp3-voltage-adjustment-apply");
    const ampCurLimInput = powerChannelDiv[i].find(".sp3-ampere-current-limit-input");
    const ampCurLimValue = powerChannelDiv[i].find(".sp3-ampere-current-limit-value");
    const ampCurLimApply = powerChannelDiv[i].find(".sp3-ampere-current-limit-apply");

    // TODO: These numbers should be set by the connected SmartPower 3 device
    volAdjInput.attr({
      "min": 5.0,
      "max": 19.0,
      "step": 0.1
    });
    ampCurLimInput.attr({
      "min": 0.5,
      "max": 3.0,
      "step": 0.1
    });

    volAdjInput.on("input", () => {
      volAdjValue.text(volAdjInput.val());

      if (volAdjInput.val() != newVolSetValue[i]) {
        volAdjValue.css("color", "#CCCC00");
      } else {
        volAdjValue.css("color", "#212529");
      }
    });
    volAdjInput.on("mousedown", () => {
      isInputWidgetsChanging = true;
    });
    volAdjInput.on("mouseup", () =>{
      isInputWidgetsChanging = false;
    });

    ampCurLimInput.on("input", () => {
      ampCurLimValue.text(ampCurLimInput.val());

      if (ampCurLimInput.val() != newAmpCurLimValue[i]) {
        ampCurLimValue.css("color", "#CCCC00");
      } else {
        ampCurLimValue.css("color", "#212529");
      }
    });
    ampCurLimInput.on("mousedown", () => {
      isInputWidgetsChanging = true;
    });
    ampCurLimInput.on("mouseup", () =>{
      isInputWidgetsChanging = false;
    });

    volAdjApply.on("click", () => {
      webSocket.send(JSON.stringify({
        command: 3, // The command number for setting something
        data: {
          channel: i,
          channelPowerSwitch: null,
          voltage: volAdjInput.val() * 1000, // The voltage set value in mV
          currentLimit: null,
          backlightLevel: null,
          fanSpeedLevel: null,
          logIntervalLevel: null,
          apName: null,
          apPassword: null,
        }
      }));
    });
    ampCurLimApply.on("click", () => {
      webSocket.send(JSON.stringify({
        command: 3, // The command number for setting something
        data: {
          channel: i,
          channelPowerSwitch: null,
          voltage: null,
          currentLimit: ampCurLimInput.val() * 1000, // The current limit set value in mA
          backlightLevel: null,
          fanSpeedLevel: null,
          logIntervalLevel: null,
          apName: null,
          apPassword: null,
        }
      }));
    });
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
    chart: { title: 'Realtime Chart', width: 'auto', height: 'auto' },
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
              "watt": "36000",
              "setVoltage": "12000",
              "setCurrentLimit": "30",
            },
            {
              "channel": 1,
              "active": false,
              "voltage": "5000",
              "ampere": "0",
              "watt": "0",
              "setVoltage": "5000",
              "setCurrentLimit": "30",
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
          const setVol = Number(data[i]['setVoltage']) / 1000;
          const setCurLim = Number(data[i]['setCurrentLimit']) / 10;

          // Update the seven segment widgets
          powerChannelDiv[ch].find(".sp3-sevenseg-voltage").sevenSeg({ value: vol });
          powerChannelDiv[ch].find(".sp3-sevenseg-ampere").sevenSeg({ value: amp });
          powerChannelDiv[ch].find(".sp3-sevenseg-watt").sevenSeg({ value: watt });

          // Update the power related adjustment widgets
          newVolSetValue[ch] = setVol;
          newAmpCurLimValue[ch] = setCurLim;

          // Currently widget is not changing
          if (!isInputWidgetsChanging) {

            if (powerChannelDiv[i].find(".sp3-voltage-adjustment-value").css("color") == "rgb(33, 37, 41)") {
              // If the value is not changed by the user, then update the widgets
              powerChannelDiv[ch].find(".sp3-voltage-adjustment-input").val(setVol);
              powerChannelDiv[ch].find(".sp3-voltage-adjustment-value").text(setVol);
            } else {
               if (parseFloat(powerChannelDiv[ch].find(".sp3-voltage-adjustment-value").text()) == setVol) {
                // If the value is the same as the new value, then corrent its color
                powerChannelDiv[ch].find(".sp3-voltage-adjustment-value").css("color", "#212529");
              }
            }

            if (powerChannelDiv[i].find(".sp3-ampere-current-limit-value").css("color") == "rgb(33, 37, 41)") {
              powerChannelDiv[ch].find(".sp3-ampere-current-limit-input").val(setCurLim);
              powerChannelDiv[ch].find(".sp3-ampere-current-limit-value").text(setCurLim);
            } else {
              if (parseFloat(powerChannelDiv[ch].find(".sp3-ampere-current-limit-value").text()) == setCurLim) {
                powerChannelDiv[ch].find(".sp3-ampere-current-limit-value").css("color", "#212529");
              }
            }
          }

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
