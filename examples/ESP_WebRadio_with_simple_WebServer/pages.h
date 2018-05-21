static const char index_html[] PROGMEM = R"=====(
<html>
<head>
    <meta charset="utf-8" />
    <title>Webradio</title>
    <meta name="viewport" content="width=device-width">
</head>
<script>
var xhttp = new XMLHttpRequest();

window.onload = function() {
  document.getElementById("sel").value = document.getElementById("url").value
}
function urlchange() {
  var uri = document.getElementById("url").value;
  document.getElementById("sel").value = uri;
  seturl();
}
function seturl() {
  var uri = document.getElementById("url").value;
  xhttp.open("GET", '/set?url=' + uri, true);
  xhttp.send();
}
function setvol() {
  var vol = document.getElementById("vol").value;
  xhttp.open("GET", '/set?volume=' + vol, true);
  xhttp.send();
}
function selecting(obj) {
  console.log(obj.value);
  document.getElementById("url").value = obj.value;
  seturl(obj.value);
}
</script>
<body>
  <div style="width:60%;margin:auto">
  <h2>ESP8266 Webradio</h2>
  <table style="width:100%">
  <tr><td style="width:10%">Url:</td><td style="width:85%"><input style="width:100%" id="url" type="text" value="%%URL%%" name="url"></td><td style="width:20%"><button onclick="urlchange();">Submit</button></td></tr>  <tr><td>Volume:</td><td colspan="2"><input style="width:100%" id="vol" type="range" value="%%VOL%%" onchange="setvol()"></td></tr>
  </table>
  <select id="sel" style="width:100%;font-size:larger" onchange="selecting(this)" id="prog">
      <option value="">select station</option>
      <option value="http://streaming.shoutcast.com/80sPlanet?lang=eng-US">80' Planet</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A02H.mp3">DR Nyheder</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A03H.mp3">DR P1</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A04H.mp3">DR P2 Klassisk</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A05H.mp3">DR P3</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A06H.mp3">DR P4 Bornholm</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A15H.mp3">DR P4 Esbjerg</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A07H.mp3">DR P4 Fyn</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A08H.mp3">DR P4 København</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A09H.mp3">DR P4 Midt & Vest</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A10H.mp3">DR P4 Nordjylland</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A11H.mp3">DR P4 Sjælland</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A12H.mp3">DR P4 Syd</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A13H.mp3">DR P4 Trekanten</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A14H.mp3">DR P4 Østjylland</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A25H.mp3">DR P5</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A29H.mp3">DR P6 Beat</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A21H.mp3">DR P7 Mix</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A22H.mp3">DR P8 Jazz</option>
      <option value="http://live-icy.gss.dr.dk:8000/A/A24H.mp3">DR Ramasjang/Ultra</option>
      <option value="http://onair.100fmlive.dk:80/klassisk_live.mp3">Klassisk Live</option>
      <option value="http://stream.novafm.dk:80/nova128">Nova FM</option>
      <option value="http://stream.popfm.dk:80/pop128">Pop FM</option>
      <option value="http://onair.100fmlive.dk:80/100fm_live.mp3">Radio 100 FM</option>
      <option value="http://195.184.101.203/voice128">The Voice</option>
      <option value="http://streammp3.retro-radio.dk/retro-mp3?type=.mp3">Retro-radio</option>
      <option value="http://tx-2.retro-radio.dk/TX-2?type=.mp3">Retro Millenium</option>
  </select>
  </div>
</body>
</html>
)=====";


