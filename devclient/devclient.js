var wsUrl = "ws://192.168.121.75:8081";
// Main communications socket
var socket;
var opening = false;
function checkOnline(onAlive, onDead) {
    setInterval(function () {
        var tempSocket = new WebSocket(wsUrl);
        var wasOpened = false;
        var tmt = setTimeout(function () {
            if (!wasOpened && tempSocket.readyState != tempSocket.OPEN) {
                onDead();
                // zed's dead
                tempSocket.close();
            }
        }, 1000);
        tempSocket.onopen = function () {
            wasOpened = true;
            clearTimeout(tmt);
            if (!socket && !opening) {
                onAlive(tempSocket);
            }
        };
        tempSocket.onerror = function () {
            clearTimeout(tmt);
            tempSocket.close();
        };
    }, 1000);
}
document.addEventListener('DOMContentLoaded', function () {
    var div = document.createElement('div');
    div.style.width = '100%';
    // div.style.height = '100%';
    document.body.appendChild(div);
    function appendText(txt, colour) {
        var now = new Date();
        var dd = document.createElement('div');
        dd.innerText = txt;
        if (colour) {
            dd.style.color = colour;
        }
        dd.className = 'line';
        dd.dataset['at'] = now.toString();
        div.appendChild(dd);
    }
    checkOnline(function (tempSocket) {
        if (!socket) {
            appendText("--- ONLINE ---", 'green');
            socket = tempSocket;
            socket.onclose = function () {
                socket = null;
            };
            socket.onmessage = function (m) {
                // console.log(m.data);
                var d = JSON.parse(m.data);
                if (d.type === 'log') {
                    appendText(d.val);
                }
            };
        }
        else {
            tempSocket.close();
        }
    }, function () {
        if (socket) {
            appendText("--- OFFLINE ---", 'red');
            socket.close();
            socket = null;
        }
    });
});
//# sourceMappingURL=devclient.js.map