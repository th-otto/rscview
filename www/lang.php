<?php
header('Cache-Control: no-cache');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
          "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<?php
echo "<title>EmuTOS $which resources - $lang</title>\n";
?>
<meta name="keywords" content="ORCS, CAT, GC, PBEM, PBM, GC-Ork, GCORK, ARAnyM, UDO, EmuTOS" />
<link rel="stylesheet" type="text/css" href="../emutos.css" />
</head>

<body>

<hr />
<?php
echo "<h1>EmuTOS $which resources - $lang</h1>\n";
?>
<hr />
<?php
$with_aes3d = $which != "Icons" && $which != "Mouse Cursor";
if ($with_aes3d)
{
	if (isset($_GET['aes3d']) && $_GET['aes3d'] != 0)
	{
		echo '<a href="' . $_SERVER['PHP_SELF'] . '?aes3d=0">Normal version</a>';
	} else
	{
		echo '<a href="' . $_SERVER['PHP_SELF'] . '?aes3d=1">Version with 3D-effects</a>';
	}
	echo "<hr />\n";
}

include('vars.php');
if ($with_aes3d && isset($_GET['aes3d']) && $_GET['aes3d'] != 0)
{
	include($pngdir . '/pngout_3d.php');
} else
{
	include($pngdir . '/pngout.php');
}
?>

<p></p>
<p></p>

<div style="text-align:center">
<p>
<?php
echo "<a href=\"$top/emutos.php\"> <img src=\"$top/images/home1.png\" width=\"180\" height=\"60\" style=\"border:0\" alt=\"Back\" /></a>\n";
?>
</p>
</div>


</body>
</html>
