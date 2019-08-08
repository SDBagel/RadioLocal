var menu, nav;
var radio;

var nowPlayingJSON;
var channelsJSON;
var ch = "a";

document.addEventListener("DOMContentLoaded", function(event) {
  menu = document.getElementById("links");
  nav = document.getElementById("nav");
  radio = document.getElementById("radio");

  var urlParams = new URLSearchParams(window.location.search);
  if (urlParams.has("c")) ch = urlParams.get("c");

  loadChannels();
  loadPlaying(true, null);
});

// Get channels from server
function loadChannels() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      channelsJSON = JSON.parse(this.responseText);
      document.getElementById("channelAName").innerText = channelsJSON["a"];
      document.getElementById("channelBName").innerText = channelsJSON["b"];
      document.getElementById("channelCName").innerText = channelsJSON["c"];
    }
  };
  xhttp.open("GET", "channels.txt", true);
  xhttp.send();
}

// Get currently playing songs, set player
function loadPlaying(isCalledByScript, callback) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      nowPlayingJSON = JSON.parse(this.responseText);
      document.getElementById("channelASong").innerText = nowPlayingJSON["a"];
      document.getElementById("channelBSong").innerText = nowPlayingJSON["b"];
      document.getElementById("channelCSong").innerText = nowPlayingJSON["c"];
      if (isCalledByScript) {
        document.getElementById("nowplaying").innerText = nowPlayingJSON[ch];
        document.getElementById("artist").innerText = nowPlayingJSON[ch+"_a"];
        document.getElementById("currentChannel").innerText = channelsJSON[ch];
        callback();
      }
    }
  };
  xhttp.open("GET", "playing.txt", true);
  xhttp.send();
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

function start() {
  document.getElementById("src").src = nowPlayingJSON[ch+"_u"];
  radio.load();
  radio.currentTime = nowPlayingJSON[ch+"_t"];
  radio.play();
  play();
}

function play() {
  radio.muted = false;
  document.getElementById("playbutton").onclick = pause;
  document.getElementById("playbutton").innerText = "⏸ Mute";
}

function pause() {
  radio.muted = true;
  document.getElementById("playbutton").onclick = play;
  document.getElementById("playbutton").innerText = "▶ Play";
}

var loadNext = function retry() {
  var currentSong = nowPlayingJSON[ch];
  loadPlaying(true, function() {
    if (currentSong == nowPlayingJSON[ch]) {
      setTimeout(retry(), 1000);
    }
    else {
      start();
    }
  });
}