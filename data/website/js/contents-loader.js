$(document).ready(() => {
  var currentUrl = $(location).attr('href');

  // In AP mode, the URL address will contain the fixed string
  if (currentUrl.includes('192.168.4')) {
    console.log("Loading ap-mode.html as the main contents");
    $('#sp3-contents').load('pages/ap-mode.html');
  } else {
    console.log("Loading sta-mode.html as the main contents");
    $('#sp3-contents').load('pages/sta-mode.html');
  }
});
