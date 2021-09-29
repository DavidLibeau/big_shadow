Pebble.addEventListener('ready',
    function (e) {
        //     console.log('JavaScript app ready and running!');
    }
);

Pebble.addEventListener('showConfiguration', function (e) {
    // Show config page
    var c1, c2, c3, c4, c5, c6, url;
    var watchinfo, platform;
    c1 = localStorage.getItem("KEY_BASECOLOR");
    c2 = localStorage.getItem("KEY_RSHADECOLOR");
    c3 = localStorage.getItem("KEY_BSHADECOLOR");
    c4 = localStorage.getItem("KEY_BACKCOLOR");
    c5 = localStorage.getItem("KEY_SHOWSTATUS");
    if (c5 !== null) c5 = c5.toLowerCase();
    c6 = localStorage.getItem("KEY_BTVIBRATE");

    if (Pebble.getActiveWatchInfo) {
        try {
            watchinfo = Pebble.getActiveWatchInfo();
            platform = watchinfo.platform;
        } catch (err) {
            platform = "basalt";
        }
    } else {
        platform = "aplite";
    }

    //if ((platform=="basalt")||(platform=="chalk"))  {
    url = "http://dav.li/pebble/bigshadow/index.php?c1=" + c1 + "&c2=" + c2 + "&c3=" + c3 + "&c4=" + c4 + "&c5=" + c5 + "&c6=" + c6;
    console.log(url);
    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed',
    function (e) {
        var configuration = JSON.parse(decodeURIComponent(e.response));
        //console.log('Configuration window returned: ' + configuration.c1);
        localStorage.setItem("KEY_BASECOLOR", configuration.c1);
        localStorage.setItem("KEY_RSHADECOLOR", configuration.c2);
        localStorage.setItem("KEY_BSHADECOLOR", configuration.c3);
        localStorage.setItem("KEY_BACKCOLOR", configuration.c4);
        localStorage.setItem("KEY_SHOWSTATUS", configuration.c5);
        localStorage.setItem("KEY_BTVIBRATE", configuration.c6);

        //Send to Pebble, persist there
        var dictionary = {
            "KEY_BASECOLOR": configuration.c1,
            "KEY_RSHADECOLOR": configuration.c2,
            "KEY_BSHADECOLOR": configuration.c3,
            "KEY_BACKCOLOR": configuration.c4,
            "KEY_SHOWSTATUS": configuration.c5,
            "KEY_BTVIBRATE": configuration.c6
        };
        console.log(JSON.stringify(dictionary));
        Pebble.sendAppMessage(
            dictionary,
            function (e) {
                console.log("Sending settings data...");
            },
            function (e) {
                console.log("Settings feedback failed!");
            }
        );
    }
);