<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
          "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<?php
echo "<title>EmuTOS $which resources - $lang</title>\n";
?>
<meta name="keywords" content="ORCS, CAT, GC, PBEM, PBM, GC-Ork, GCORK, ARAnyM, UDO, EmuTOS" />
<link rel="stylesheet" type="text/css" href="../rscview.css" />
</head>

<body>

<hr />
<?php
echo "<h1>EmuTOS $which resources - $lang</h1>\n";
?>
<hr />

<?php
include('vars.php');
read_pngs($pngdir);
foreach ($files as $entry)
{
	$name = $entry['name'];
	$file = $entry['file'];
	echo "<p>$name:<br /><img src=\"$pngdir/$file\" alt=\"$name\" /></p>\n";
}
?>

<p></p>
<p></p>

<div style="text-align:center">
<p>
<?php
echo "<a href=\"$top/emutos.php\"> <img src=\"$top/images/home1.gif\" width=\"180\" height=\"60\" style=\"border:0\" alt=\"Back\" /></a>\n";
?>
</p>
</div>


</body>
</html>
