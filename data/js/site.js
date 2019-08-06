var menu, nav;
var radio;

document.addEventListener("DOMContentLoaded", function(event) {
  menu = document.getElementById("links");
  nav = document.getElementById("nav");
  radio = document.getElementById("radio");

  loadStatistics();
});

// Get stats from server
function loadStatistics() {
  var now = new Date();
  if ((debounceStatisticsTime - now) < 10000) return;
  else debounceStatisticsTime = new Date();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var responseJSON = JSON.parse(this.responseText);
      document.getElementById("nowplaying").innerText = responseJSON.t;
      document.getElementById("artist").innerText = responseJSON.a;
    }
  };
  xhttp.open("GET", "persistent.txt", true);
  xhttp.send();
}

// On browser close, send stats data
window.addEventListener("unload", logData, false);
function logData() {
  var data = new FormData();
  // Seconds spent on site
  var endTime = new Date();
  data.set("s", Math.round((endTime - startTime) / 1000));
  navigator.sendBeacon("/addvisit", data);
}

function showMenu() {
  if (menu.style.display === "none" || menu.style.display === "") {
    menu.style.display = "flex";
    nav.style.height = "unset";
  }
  else {
    menu.style.display = "none";
    nav.style.height = "4rem";
  }
}

function onResize() {
  if (menu.style.height !== "" || nav.style.height !== "") {
    menu.style.display = "";
    nav.style.height = "";
  }
};

function play() {
  radio.play();
  radio.muted = false;
  document.getElementById("playbutton").onclick = pause;
  document.getElementById("playbutton").innerText = "⏸ Mute";
}

function pause() {
  radio.muted = true;
  document.getElementById("playbutton").onclick = play;
  document.getElementById("playbutton").innerText = "▶ Play";
}