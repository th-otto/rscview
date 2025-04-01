<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Atari Keyboard Layouts</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="kbd.css" rel="stylesheet">

<style>
.rect {
	opacity:0.71428573;
	fill:none;
	fill-opacity:1.0000000;
	fill-rule:evenodd;
	stroke:#000000;
	stroke-width:2;
	stroke-linecap:round;
	stroke-linejoin:round;
	stroke-miterlimit:4.0000000;
	stroke-dasharray:none;
	stroke-dashoffset:0.0000000;
	stroke-opacity:1.0000000
}
title {
	margin-top: 0;
	margin-bottom: 0;
}
</style>

</head>
<body>

<div style="width: 1276px; margin: auto;">

<h1>US layout</h1>
<?php
passthru("./mkhtml usa.src -");
?>

<h1>German layout</h1>
<?php
passthru("./mkhtml german.src -");
?>

<h1>French layout</h1>
<?php
passthru("./mkhtml french.src -");
?>

<h1>UK layout</h1>
<?php
passthru("./mkhtml british.src -");
?>

<h1>Spanish layout</h1>
<?php
passthru("./mkhtml spanish.src -");
?>

<h1>Italian layout</h1>
<?php
passthru("./mkhtml italian.src -");
?>

<h1>Swedish layout</h1>
<?php
passthru("./mkhtml swedish.src -");
?>

<h1>Swiss (french) layout</h1>
<?php
passthru("./mkhtml swiss_french.src -");
?>

<h1>Swiss (german) layout</h1>
<?php
passthru("./mkhtml swiss_german.src -");
?>

<h1>Finnish layout</h1>
<?php
passthru("./mkhtml finnish.src -");
?>

<h1>Norwegian layout</h1>
<?php
passthru("./mkhtml norwegian.src -");
?>

<h1>Danish layout</h1>
<?php
passthru("./mkhtml dk.inc -");
?>

<h1>Dutch layout</h1>
<?php
passthru("./mkhtml dutch.src -");
?>

<h1>Czech layout</h1>
<?php
passthru("./mkhtml czech_iso.src -");
?>

<h1>Hungarian layout</h1>
<?php
passthru("./mkhtml hungarian.src -");
?>

<h1>Polish layout</h1>
<?php
passthru("./mkhtml usa-pl.src -");
?>

<h1>Romanian layout</h1>
<?php
passthru("./mkhtml ro_ro.src -");
?>

<div style="text-align:center">
<p>
<a href="https://validator.w3.org/check?uri=referer"><img
        src="../images/valid-xhtml11.png" height="31" width="88"
        alt="Valid XHTML 1.1!" /></a>
</p>
</div>

</div>

</body>
</html>
