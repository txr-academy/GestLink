const GESTURE_MAP = {
    "1": { name: "SINGLE_FINGER"},
    "2": { name: "DOUBLE_FINGER" },
    "3": { name: "THREE_FINGER"},
    "4": { name: "FOUR_FINGER"},
    "5": { name: "FIVE_FINGER"},
    "7": { name: "SWIPE_RIGHT", icon: "➡️" },
    "6": { name: "SWIPE_LEFT", icon: "⬅️" }
};

let isConnected = false;
const BROKER_IP = "35.172.255.228";
const WS_PORT = 8083; // Try 8083 first, if that fails try 9001 or 8000

const client = new Paho.MQTT.Client(BROKER_IP, WS_PORT, "gest_pro_" + Math.random().toString(16).substr(2, 4));

// THE TOGGLE BUTTON LOGIC
function toggleConnection() {
    const btn = document.getElementById("auth-btn");
    
    if (!isConnected) {
        btn.innerText = "CONNECTING...";
        client.connect({
            onSuccess: () => {
                isConnected = true;
                client.subscribe("stm32/gesture/data/gestlink");
                btn.innerText = "DISCONNECT";
                btn.classList.add("btn-disconnect");
                document.getElementById("gesture-name").innerText = "CONNECTED";
            },
            onFailure: (err) => {
                alert("Connection Failed: " + err.errorMessage);
                btn.innerText = "CONNECT";
            },
            useSSL: false
        });
    } else {
        client.disconnect();
        isConnected = false;
        btn.innerText = "CONNECT";
        btn.classList.remove("btn-disconnect");
        document.getElementById("gesture-name").innerText = "OFFLINE";
    }
}

client.onMessageArrived = (msg) => {
    const id = msg.payloadString.replace(/\D/g,'');
    const data = GESTURE_MAP[id] || { name: `UNKNOWN_${id}`, icon: "❓" };
    const time = new Date().toLocaleTimeString('en-GB', { hour12: false });
    
    document.getElementById("gesture-name").innerText = data.name;
    document.getElementById("gesture-icon").innerText = data.icon;
    
    // Confidence & Log Logic (Keeping your original)
    const conf = Math.floor(Math.random() * (98 - 85) + 85);
    document.getElementById("conf-bar").style.width = conf + "%";
    document.getElementById("conf-val").innerText = conf + "%";
    
    const list = document.getElementById("event-list");
    const li = document.createElement("li");
    li.className = "log-item";
    li.innerHTML = `<div><span class="tag tag-mqtt">MQTT</span><span>${data.name}</span></div><span class="log-time">${time}</span>`;
    list.prepend(li);
};

function clearLog() { document.getElementById("event-list").innerHTML = ""; }