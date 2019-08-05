var startTime;
var debounceStatisticsTime;

var menu, nav;

// Function modified from https://github.com/daneden/animate.css
function animateCSS(element, animationName, isPermanent, callback) {
  const node = document.querySelector(element);
  node.classList.add("animated", animationName);

  function handleAnimationEnd() {
    if (!isPermanent)
      node.classList.remove("animated", animationName);
    node.removeEventListener("animationend", handleAnimationEnd);

    if (typeof callback === "function") callback();
  }

  node.addEventListener("animationend", handleAnimationEnd);
}

document.addEventListener("DOMContentLoaded", function(event) {
  menu = document.getElementById("links");
  nav = document.getElementById("nav");

  animateCSS("#rick", "fadeIn", false, null);
  document.getElementById("rick").style.opacity = 1;
  setTimeout(function() {
    animateCSS("#message", "fadeIn", true, null);
    animateCSS("#notification", "fadeInUp", true, null);
    var content = document.getElementById("contentBlock");
    content.style.display = "block";
    animateCSS("#contentBlock", "fadeIn", true, null);
  }, 1000);

  startTime = new Date();

  loadStatistics();

  setTimeout(function() {
    animateCSS("#rick", "fadeOut", true, null);
  }, 77000);
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
      document.getElementById("visitors").innerHTML = responseJSON.v;
      document.getElementById("avgtime").innerHTML = responseJSON.s + " seconds";
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