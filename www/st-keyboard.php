<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Atari Keyboard Layouts</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="kbd.css" rel="stylesheet">

<style type="text/css">
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

<!--
<h1>UK layout</h1>
<?php
passthru("./mkhtml british.src -");
?>

<h1>French layout</h1>
<?php
passthru("./mkhtml french.src -");
?>

<h1>Swedish/Finnish layout</h1>
<?php
passthru("./mkhtml swedish.src -");
?>

<h1>Italian layout</h1>
<?php
passthru("./mkhtml italian.src -");
?>

<h1>Czech layout</h1>
<?php
passthru("./mkhtml czech_iso.src -");
?>

<h1>Hungarian layout</h1>
<?php
passthru("./mkhtml hungarian.src -");
?>
-->

</div>

</body>
</html>
