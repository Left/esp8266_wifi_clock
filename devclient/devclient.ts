const wsUrl = "ws://192.168.121.75:8081";
// Main communications socket
var socket:WebSocket;
var opening:boolean = false;

function checkOnline(onAlive: (s: WebSocket) => void, onDead: () => void) {
    setInterval(() => {
        const tempSocket = new WebSocket(wsUrl);
        var wasOpened = false;
        const tmt = setTimeout(() => {
            if (!wasOpened && tempSocket.readyState != tempSocket.OPEN) {
                onDead();
                // zed's dead
                tempSocket.close();
            }
        }, 2000);
        
        tempSocket.onopen = () => {
            wasOpened = true;
            clearTimeout(tmt);
            if (!socket && !opening) {
                onAlive(tempSocket);
            }
        };
        tempSocket.onerror = () => {
            clearTimeout(tmt);
            tempSocket.close();
        }
    }, 2000);
}

document.addEventListener('DOMContentLoaded', () => {
    const div = document.createElement('div');
    div.style.width = '100%';
    // div.style.height = '100%';
    document.body.appendChild(div);

    function appendText(txt: string, colour?: string) {
        const now = new Date();
        const dd = document.createElement('div');
        dd.innerText = txt;
        if (colour) {
            dd.style.color = colour;
        }
        dd.className = 'line';
        dd.dataset['at'] = now.toString();
        div.appendChild(dd);
    }

    checkOnline((tempSocket: WebSocket) => {
        if (!socket) {
            appendText("--- ONLINE ---", 'green')
            socket = tempSocket;
            socket.onclose = () => {
                socket = null;
            }
        
            socket.onmessage = (m) => {
                // console.log(m.data);
                const d = JSON.parse(m.data);
                if (d.type === 'log') {
                    appendText(d.val);
                }
            }
        } else {
            tempSocket.close();
        }
    },
    () => {
        if (socket) {
            appendText("--- OFFLINE ---", 'red')
            socket.close();
            socket = null;
        }
    });


});