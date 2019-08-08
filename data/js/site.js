var menu, nav;
var radio, playbutton;

var nowPlayingJSON;
var ch = "a";

document.addEventListener("DOMContentLoaded", function(event) {
  menu = document.getElementById("links");
  nav = document.getElementById("nav");
  radio = document.getElementById("radio");
  playbutton = document.getElementById("playbutton");

  var urlParams = new URLSearchParams(window.location.search);
  if (urlParams.has("c")) ch = urlParams.get("c");

  loadPlaying(true);
});

function setError() {
  document.getElementById("error").style.display = "block";

  document.getElementById("src").src = "media/rick.mp3";
  radio.load();
  radio.loop = true;
  pause();
}

function remError() {
  document.getElementById("error").style.display = "none";

  radio.loop = false;
  if (playbutton.onclick === play)
    playbutton.onclick = start;
}

// Get currently playing songs, set player
function loadPlaying(isCalledByScript) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      remError();
      nowPlayingJSON = JSON.parse(this.responseText);
      document.getElementById("chASong").innerText = nowPlayingJSON["a"];
      document.getElementById("chBSong").innerText = nowPlayingJSON["b"];
      document.getElementById("chCSong").innerText = nowPlayingJSON["c"];
      if (isCalledByScript) {
        document.getElementById("nowplaying").innerText = nowPlayingJSON[ch];
        document.getElementById("artist").innerText = nowPlayingJSON[ch+"_a"];
        document.getElementById("currentCh").innerText = document.getElementById(ch).innerText;
        document.getElementById("src").src = nowPlayingJSON[ch+"_u"];
        radio.load();
      }
    }
    else if (this.readyState == 4 && this.status != 200)
      setError();
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
  playbutton.onmouseover = "";
  radio.play();
  radio.currentTime = nowPlayingJSON[ch+"_t"];
  resume();
}

function resume() {
  radio.muted = false;
  playbutton.onclick = pause;
  playbutton.innerText = "⏸ Mute";
}

function pause() {
  radio.muted = true;
  playbutton.onclick = play;
  playbutton.innerText = "▶ Unmute";
}