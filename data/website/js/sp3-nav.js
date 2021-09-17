$(document).ready(() => {

    const menus = {
        'home': '#sp3-nav-home',
        'settings': '#sp3-nav-settings',
        'about': '#sp3-nav-about',
    };

    for (let menu in menus) {
        $(menu).removeClass('active');
    }

    // The location pathname will be like 192.168.0.100, 192.168.0.100/about.html
    const splitedPathname = window.location.pathname.split('/');

    if (splitedPathname[1].length == 0) {
        $(menus['home']).addClass('active');
    } else {
        let currentPage = splitedPathname[1].split('.')[0];
        let currentPageElement = null;

        switch (currentPage) {
            case 'index':
                currentPageElement = $(menus['home']);
                break;
            case 'settings':
                currentPageElement = $(menus['settings']);
                break;
            case 'about':
                currentPageElement = $(menus['about']);
                break;
            default:
                // Uknown page. Go to the home page in this case.
                currentPageElement = $(menus['home']);
                break;
        }
        currentPageElement.addClass('active');
    }

});
