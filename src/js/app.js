var myAPIKey = '';
var city;

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // to fake current lat/lon for testing
  // pos.coords.latitude = '29.5411941';
  // pos.coords.longitude = '-98.5760687';

  // Construct URLs
  var cityUrl = 'http://nominatim.openstreetmap.org/reverse?format=json&lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
  var weatherUrl = 'https://api.forecast.io/forecast/' + myAPIKey + '/' + pos.coords.latitude + ',' + pos.coords.longitude;
  
  // get city
  xhrRequest(cityUrl, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      
      // grab city from lat/lon
      city = json.address.city;
      console.log("City is " + city);
    });
  
  // get forecast through dark sky
  xhrRequest(weatherUrl, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      
      // summary for the day (under hour in json)
      var summary = json.hourly.summary;
      console.log("Summary is " + summary);
      
      // round temperature
      var curTemp = Math.round(json.currently.temperature);
      console.log("Temperature is " + curTemp);
      
      // summary of current weather
      var currentSummary = json.currently.summary;
      console.log("Current summary is " + currentSummary);
      
      // icon for weather condition
      var icon = json.currently.icon;
      console.log("Current icon is " + icon);
      
      // assemble dictionary using keys
      var dictionary = {
        "KEY_CITY": city,
        "KEY_TEMP": curTemp,
        "KEY_SUMMARY": summary,
        "KEY_CURRENT_SUMMARY": currentSummary,
        "KEY_TEMP_EXTENDED": curTemp + '°, ' + currentSummary,
        "KEY_ICON": icon,
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);
