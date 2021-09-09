$(document).ready(() => {
  var apListDiv = $("#sp3-ap-list");
  var apConnectedDiv = $("#sp3-ap-connected");
  var apConnectingDiv = $("#sp3-ap-connecting");
  var apErrorDiv = $("#sp3-ap-error");

  var showApListPage = () => {
    apConnectingDiv.hide();
    apConnectedDiv.hide();
    apErrorDiv.hide();

    $.ajax("/api/get_ap_list", {
      method: "GET",
      dataType: "json",
      success: (data) => {
        console.log("Discovered AP list received: ", data);

        apListDiv.append('<ul class="list-group">');
        data.forEach(apInfo => {
          var rssiColor = 'bg-secondary';

          switch (true) {
            case (apInfo.rssi < -80):
              rssiColor = 'bg-danger';
              break;
            case (apInfo.rssi < -60):
              rssiColor = 'bg-warning';
              break;
            case (apInfo.rssi < -40):
              rssiColor = 'bg-info';
              break;
            case (apInfo.rssi < -20):
              rssiColor = 'bg-success';
              break;
          }

          var apItem = document.createElement('li');
          apItem.id = apInfo.macaddr;
          apItem.className = 'list-group-item d-flex justify-content-between align-items-center';

          var apItemContent = document.createElement('div');
          apItemContent.className = 'd-flex justify-content-between';
          if (apInfo.encrypted) {
            apItemContent.innerHTML += '<span class="badge text-muted">locked</span>';
          }
          apItemContent.innerHTML += '<span class="badge ' + rssiColor + ' rounded-pill">' +
            apInfo.rssi + '</span>';

          apItem.appendChild(document.createTextNode(apInfo.ssid));
          apItem.appendChild(apItemContent);
          apItem.addEventListener('click', () => {
            var apWifiPassword = null;

            if (apInfo.encrypted) {
              // Show a prompt to enter the password
              apWifiPassword = prompt("Enter the password for " + apInfo.ssid);

              // Cancel the connection if the user presses cancel
              if (apWifiPassword == null || apWifiPassword.trim() == "") {
                return;
              }
            }

            showApConnectingPage();
            $.ajax("/api/connect_to_ap", {
              method: "GET",
              dataType: "json",
              data: {
                ssid: apInfo.ssid,
                password: apWifiPassword || "",
                macaddr: apInfo.macaddr
              },
              success: (data) => {
                console.log("Results about requesting STA connection: ", data);

                if (data.success) {
                  console.log("Start checking the STA connection established...");

                  var checkingStaConnInterval = setInterval(() => {
                    // TODO: Have to check the previous Ajax completed before sending another Ajax
                    $.ajax("/api/is_sta_connected", {
                      method: "GET",
                      dataType: "json",
                      success: (data) => {
                        console.log("Results about STA connection: ", data);
                        if (data.established) {
                          showApConnectedPage(apInfo, data.ipAddress);
                          clearInterval(checkingStaConnInterval);
                        } else if (data.error) {
                          showApErrorPage(data.errorMessage);
                          clearInterval(checkingStaConnInterval);
                        } else {
                          // Do nothing
                          // The board maybe in IDLE state or currently trying to connect to AP
                          // Do not clear the interval in this condition because it should wait for
                          // the clear results from the server
                        }
                      },
                      error: (err) => {
                        console.log("Unknown error checking the STA connection: ", err);
                        showApErrorPage();
                        clearInterval(checkingStaConnInterval);
                      },
                    });
                  }, 2000);
                } else {
                  showApErrorPage("Requesting STA connection failed with unknown reason");
                }
              },
              error: (err) => {
                console.log("Unknown error connecting to AP: ", err);
                showApErrorPage();
              }
            });
          });

          $("#sp3-ap-list").append(apItem);
        });
        apListDiv.append('</ul>');
      },
      error: (err) => {
        console.log("Error: " + err);

        $('#sp3-ap-list').html(
          '<div class="alert alert-danger" role="alert">' +
          '- <strong>Error:</strong> ' +
          err + '</br>' +
          '- <strong>Please reboot SmartPower 3 and try again</strong>' +
          '</div>'
        );
      }
    });
  };

  var showApConnectingPage = () => {
    apListDiv.hide();
    apConnectedDiv.hide();
    apErrorDiv.hide();

    apConnectingDiv.empty();
    apConnectingDiv.append(
      '<p class="lead">Connecting to AP...</p>' +
      '<p>Please wait for a minute.</br>Do not inerrupt the progress.</p>');
    apConnectingDiv.show();
  };

  var showApConnectedPage = (apInfo, ipAddress) => {
    apListDiv.hide();
    apConnectingDiv.hide();
    apErrorDiv.hide();

    apConnectedDiv.empty();
    apConnectedDiv.append(
      '<p class="lead">Connected!</p>' +
      '<p>Connected to <strong>' + apInfo.ssid + '</strong>' +
      '<p>Now turn off this window and connect to your AP, ' +
      'and enter this IP address to the web browser: <strong>' + ipAddress + '</strong></p>');
    apConnectedDiv.show();
  };

  var showApErrorPage = (errorMessage) => {
    apListDiv.hide();
    apConnectingDiv.hide();
    apConnectedDiv.hide();

    apErrorDiv.empty();
    apErrorDiv.append('<p class="lead">Error connecting to AP.</p>' +
      '<p>Something goes wrong</p>');
    if (typeof errorMessage != "undefined") {
      apErrorDiv.append('<p>Error meesage: ' + errorMessage + '</p>');
    }
    apErrorDiv.append(
      '<p>Please refresh this page and try to connect to another AP ' +
      'or reboot SmartPower 3 and try again.</p>');
    apErrorDiv.show();
  };

  showApListPage();
});
