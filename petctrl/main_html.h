
#ifndef H_MAIN_HTML
#define H_MAIN_HTML

#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


//const char MAIN_public_page[] PROGMEM = R"=====(
String MAIN_public_page = R"=====(
 <!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<style>
p { line-height:0; }
</style>
<title>*ap ssid*</title>
</head>
<body>
<p><a href="/settings">Configuración</a></p>
  <center>
  <table><tr>
  <td><img width="256px" src=foto.jpg /> </td>
  <td>
  <p>Hola! mi nombre es *ap ssid*</p>
  <p>Mi telefono es: *cfg telefono*</p>
  <p>Mi chip es: *cfg chip*</p>
  </td>
  </tr>
  </table>
  </center>
</body>
</html>
)=====";


void handleNotFound();


String SettingsPageHTML =  R"=====(
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<style>
p { line-height:0; }
</style>
<title>*pet name*</title>

</head>
<body>

<p><a href="/">->Inicio</a></p>
<center>
<p>Foto:</p>
<p><<img width="256px"  src=foto.jpg /> </p>
<br>
<form action="photoupload" method="post" enctype="multipart/form-data">
    <input type="file" name="name">
    <input class="button" type="submit" value="Subir" name="upload">
</form>

<form action='settings' method="post" id="usrform">
<table><tr><th>

<br><br>Datos publicos:
</th></tr>
<tr><th><p align="right">Nombre:</p></th><th><input maxlength="30" type="text" name="petName" value="*pet name*"></th></tr>
<tr><th><p align="right">Chip:<br></th><th><input maxlength="30" type="text" name="petChip" value="*pet chip*"></th></tr>
<tr><th><p align="right">Telefono:<br></th><th><input maxlength="30" type="text" name="telefono" value="*telefono*"></th></tr>

<tr><th>
Conexion HOTSPOT:
</th></tr>
<tr><th><p align="right">Nombre AP:</p></th><th><input maxlength="30" type="text" name="APName" value="*ap name*"></th></tr>
<tr><th><p align="right">Contraseña:</p></th><th><input maxlength="32" type="text" name="APPasswd" value="*ap pass*"></th></tr>

<tr><th><br><br>Otros:</th></tr>
<tr><th><p align="right">OTA URL <small>(Dejar en blanco por defecto)</small> :</p></th><th><input maxlength="64" type="text" name="OtaURL" value=""></th></tr>

<tr><th></th><th>
<input type="submit" value="Guardar" name="save">
</th></tr></table></form>

<form action="settings" method="post" id="otheroptions">
<input type="submit" value="Actualizar" name="update">
<input type="submit" value="Reiniciar" name="restart">
</form>
</center>
</body>
</html>
)=====";

#endif
