var menu, nav;
var radio;

var nowPlayingJSON;
var ch = "a";

document.addEventListener("DOMContentLoaded", function(event) {
  menu = document.getElementById("links");
  nav = document.getElementById("nav");
  radio = document.getElementById("radio");

  loadChannels();
  loadPlaying(true);
});

// Get channels from server
function loadChannels() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var responseJSON = JSON.parse(this.responseText);
      document.getElementById("channelAName").innerText = responseJSON["channelAName"];
      document.getElementById("channelBName").innerText = responseJSON["channelBName"];
      document.getElementById("channelCName").innerText = responseJSON["channelCName"];
    }
  };
  xhttp.open("GET", "channels.txt", true);
  xhttp.send();
}

// Get currently playing songs, set player
function loadPlaying(isCalledByScript) {
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
        document.getElementById("currentChannel").innerText = ch;
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

function play() {
  document.getElementById("src").src = nowPlayingJSON[ch+"_u"];
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