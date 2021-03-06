<!DOCTYPE html>
<html>

<head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <title>Big Shadow config</title>

    <style>
        body {
            background-color: black;
            color: white;
            font-family: tahoma;
        }

        .cblock {
            width: 40px;
            height: 40px;
            border: 1px solid black;
        }

        .white {
            background-color: white;
        }

        .gray {
            background-color: #777777;
        }

        .black {
            background-color: #000;
            border: 1px solid white;
        }

        .negative {
            background-color: #000;
            border: 1px solid white;
            font-size: 30px;
        }

        .cyan {
            background-color: cyan;
        }

        .magenta {
            background-color: magenta;
        }

        .yellow {
            background-color: yellow;
        }

        #save_button {
            width: 80px;
            height: 40px;
            color: black;
            text-align: center;
            font-size: 30px;
            background-color: #CCC;
        }

        td {
            padding: 0px;
            border: 0px;
        }

        #cn {
            width: 80px;
            height: 120px;
            margin-top: 20px;
        }


        .sp-palette {
            max-width: 400px;
        }

        .label {
            background: #444;
            padding: 8px;
            border-radius: 4px;
            color: #eee;
            margin-top: 10px;
            display: inline-block;
        }
    </style>

    <script type='text/javascript' src='//dav.li/jquery/3.4.1.min.js'></script>
    <link rel="stylesheet" type="text/css" href="spectrum.css">
    <script type='text/javascript' src="spectrum.js"></script>

    <script>
        //Setup to allow easy adding more options later
        function color2pebcol(color) {
            return 192 + 16 * (color.r >> 6) + 4 * (color.g >> 6) + (color.b >> 6);
        }

        function pebcol2color(pebcol) {
            var r, g, b, color;
            b = (pebcol & 3);
            g = (pebcol & 12) >> 2;
            r = (pebcol & 48) >> 4;
            if (pebcol == 213) {
                color = tinycolor("#777777")
            } else {
                color = tinycolor("rgb " + r * 85 + " " + g * 85 + " " + b * 85);
            }

            return color.toHexString();

        }

        function saveOptions() {
            var formular = document.getElementById("cfgform").elements;
            var options = {
                "c1": color2pebcol($("#face").spectrum("get").toRgb()),
                "c2": color2pebcol($("#shadow").spectrum("get").toRgb()),
                "c3": color2pebcol($("#shadow").spectrum("get").toRgb()),
                "c4": color2pebcol($("#back").spectrum("get").toRgb()),
                "c5": document.getElementById("cfgform").elements.namedItem("c5").checked,
                "c6": document.getElementById("cfgform").elements.namedItem("c6").checked
            }
            console.log(JSON.stringify(options));
            return options;
        };

        function vubecnic() {
            var c1 = gup('c1');
            var c2 = gup('c2');
            var c3 = gup('c3');
            var c4 = gup('c4');
            var c5 = gup('c5');
            var c6 = gup('c6');
            if (c1[3] == "l") {
                c1 = ""
            } // null
            if (c1 > "") {
                document.getElementById("cfgform").elements.namedItem("c1").value = c1;
                setColors(pebcol2color(c1), pebcol2color(c2), pebcol2color(c4));
                document.getElementById("cfgform").elements.namedItem("c5").checked = (c5 == "true");
                document.getElementById("cfgform").elements.namedItem("c6").checked = (c6 == "true");

            } else {
                c1 = document.getElementById("cfgform").elements.namedItem("c1").value;
                setColors("#FFFFFF", "#777777", "#000");
            }

            //drawNumber("#FFF","#AAA","#555");    
        }

        function drawNumber(face, lside, dside, back) {
            var c = document.getElementById("cn");
            var ctx = c.getContext("2d");
            ctx.fillStyle = back;
            ctx.fillRect(0, 0, 120, 160);
            ctx.translate(20, 20);
            ctx.fillStyle = face;
            ctx.fillRect(0, 0, 60, 20);
            ctx.fillRect(0, 20, 20, 80);
            ctx.fillRect(20, 40, 40, 20);
            ctx.fillRect(20, 80, 40, 20);
            ctx.fillRect(40, 60, 20, 20);
            ctx.fillStyle = lside;
            ctx.beginPath();
            ctx.moveTo(60, 0);
            ctx.lineTo(60, 20);
            ctx.lineTo(80, 40);
            ctx.lineTo(80, 20);
            ctx.closePath();
            ctx.fill();
            ctx.beginPath();
            ctx.moveTo(60, 40);
            ctx.lineTo(60, 100);
            ctx.lineTo(80, 120);
            ctx.lineTo(80, 60);
            ctx.closePath();
            ctx.fill();
            ctx.beginPath();
            ctx.moveTo(20, 20);
            ctx.lineTo(40, 40);
            ctx.lineTo(20, 40);
            ctx.closePath();
            ctx.fill();
            ctx.beginPath();
            ctx.moveTo(20, 60);
            ctx.lineTo(40, 80);
            ctx.lineTo(20, 80);
            ctx.closePath();
            ctx.fill();
            ctx.fillStyle = dside;
            ctx.beginPath();
            ctx.moveTo(20, 20);
            ctx.lineTo(40, 40);
            ctx.lineTo(80, 40);
            ctx.lineTo(60, 20);
            ctx.closePath();
            ctx.fill();
            ctx.beginPath();
            ctx.moveTo(0, 100);
            ctx.lineTo(20, 120);
            ctx.lineTo(80, 120);
            ctx.lineTo(60, 100);
            ctx.closePath();
            ctx.fill();
            ctx.beginPath();
            ctx.moveTo(20, 60);
            ctx.lineTo(40, 80);
            ctx.lineTo(40, 60);
            ctx.closePath();
            ctx.fill();
            ctx.translate(-20, -20);
        }

        function gup(name) {
            name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
            var regexS = "[\\?&]" + name + "=([^&#]*)";
            var regex = new RegExp(regexS);
            var results = regex.exec(window.location.href);
            if (results == null)
                return null;
            else
                return results[1];
        }


        function setColors(face, dside, back) {

            $("#face").spectrum("set", face);
            $("#shadow").spectrum("set", dside);
            $("#back").spectrum("set", back);

            drawNumber(face, dside, dside, back);
        }
    </script>



</head>

<body onload="vubecnic()">
    <form id="cfgform">

        <h2>Select color</h2>
        <table>
            <tr>
                <td align="center"><label onclick="setColors('#FFFFFF','#777777','#000')"><input type="radio" name="c1" value="255" checked="checked">
                        <div class="cblock white"></div>
                    </label></td>
                <td align="center"><label onclick="setColors('#777777','#000','#FFFFFF')"><input type="radio" name="c1" value="240">
                        <div class="cblock gray"></div>
                    </label></td>
                <td align="center"><label onclick="setColors('#000','#777777','#FFFFFF')"><input type="radio" name="c1" value="204">
                        <div class="cblock black"></div>
                    </label></td>
                <td align="center"><label onclick="setColors('#000','#FFFFFF','#000')"><input type="radio" name="c1" value="204">
                        <div class="cblock negative">R</div>
                    </label></td>
            </tr>
            <tr>
                <td colspan="2">
                    <div id="save_button">SAVE</div>
                </td>
                <td colspan="5" valign="top" align="right"><canvas width="120" height="160" id="cn"></canvas></td>
            </tr>
        </table>

        <br>


        <input id="face" type='text'>
        <input type='text' id="shadow">
        <input type='text' id="back">

        <p><label><input type="checkbox" name="c5" id="c5" value="on" checked=""> Show a status screen</label>
        <p><label><input type="checkbox" name="c6" id="c6" value="on" checked=""> Vibrate on lost Bluetooth</label>
    </form>

    <p>&nbsp;<br>

    <p>


        <script type='text/javascript'>
            var palette = ["#000", "#777777", "#FFFFFF", "#000055", "#0000AA", "#0000FF", "#005500", "#005555", "#0055AA", "#0055FF", "#00AA00", "#00AA55", "#00AAAA", "#00AAFF", "#00FF00", "#00FF55", "#00FFAA", "#00FFFF", "#550000", "#550055", "#5500AA", "#5500FF", "#555500", "#555555", "#5555AA", "#5555FF", "#55AA00", "#55AA55", "#55AAAA", "#55AAFF", "#55FF00", "#55FF55", "#55FFAA", "#55FFFF", "#AA0000", "#AA0055", "#AA00AA", "#AA00FF", "#AA5500", "#AA5555", "#AA55AA", "#AA55FF", "#AAAA00", "#AAAA55", "#AAAAAA", "#AAAAFF", "#AAFF00", "#AAFF55", "#AAFFAA", "#AAFFFF", "#FF0000", "#FF0055", "#FF00AA", "#FF00FF", "#FF5500", "#FF5555", "#FF55FF", "#FFAA00", "#FFAA55", "#FFAAAA", "#FFFF00", "#FFFF55", "#FFFFAA", "#FFAAFF", "#FF55AA"];

            function printColor(source, color) {
                var f, h, s, b;
                $(source).spectrum("set", color);
                f = $("#face").spectrum("get");
                s = $("#shadow").spectrum("get");
                b = $("#back").spectrum("get");
                drawNumber(f.toHexString(), s.toHexString(), s.toHexString(), b.toHexString());
            }


            $("#face").spectrum({
                color: "#000",
                showPaletteOnly: true,
                change: function(color) {
                    printColor("#face", color);
                },
                hide: function(color) {
                    printColor("#face", color);
                },
                palette: [
                    palette
                ]
            });

            $("#shadow").spectrum({
                color: "#000",
                showPaletteOnly: true,
                change: function(color) {
                    printColor("#shadow", color);
                },
                hide: function(color) {
                    printColor("#shadow", color);
                },
                palette: [
                    palette
                ]
            });
            $("#back").spectrum({
                color: "#000",
                showPaletteOnly: true,
                change: function(color) {
                    printColor("#back", color);
                },
                hide: function(color) {
                    printColor("#back", color);
                },
                palette: [
                    palette
                ]
            });



            //Setup to allow easy adding more options later

            var submitButton = document.getElementById("save_button");

            submitButton.addEventListener("click",
                function() {
                    console.log("Submit");
                    var options = saveOptions();


                    var return_to = decodeURIComponent(gup('return_to'));
                    if (return_to == "null") {
                        return_to = 'pebblejs://close#'
                    };

                    var loc = return_to + encodeURIComponent(JSON.stringify(options));
                    console.log(return_to);

                    console.log(loc);
                    document.location = loc;
                },
                false);




            //]]>  
        </script>


</body>


</html>