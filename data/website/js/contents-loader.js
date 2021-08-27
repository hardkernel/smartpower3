$(document).ready(() => {
  var currentUrl = $(location).attr('href');

  // In AP mode, the URL address will contain the fixed string
  if (currentUrl.includes('192.168.4')) {
    console.log("Loading ap-contents.html as the main contents");
    $('#sp3-contents').load('pages/ap-contents.html');
  } else {
    console.log("Loading sta-contents.html as the main contents");
    $('#sp3-contents').load('pages/sta-contents.html');
  }
});
