<?php
$lang = "All Languages";
$pngdir = '.';
$which = 'desktop';
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
<link rel="stylesheet" type="text/css" href="emutos.css" />
</head>

<body>

<hr />
<?php
echo "<h1>EmuTOS $which resources - $lang</h1>\n";
?>
<hr />

<?php
include('vars.php');
read_linguas('tmp/LINGUAS');
$files = array();
include('en/pnglist.php');
$en_files = $files;
$all_files = array();
foreach ($linguas as $lang)
{
	$lang = $lang['lang'];
	$files = array();
	include($lang . '/pnglist.php');
	$all_files[$lang] = $files;
}
if (isset($_GET['type']) && $_GET['type'] == 'list')
{
	/* output as list */
	echo "<table>\n";
	foreach ($en_files as $entry)
	{
		$name = $entry['name'];
		$file = $entry['file'];
		$num = $entry['num'];
		echo "<tr valign=\"bottom\"><td>$name:</td></tr>\n";
		foreach ($linguas as $lang)
		{
			$lang = $lang['lang'];
			$trans = $languages[$lang];
			$en = $trans['en'];
			$native = $trans['native'];
			$flag = $trans['flag'];
			$file = $all_files[$lang][$num]['file'];
			echo "<tr valign=\"top\"><td><img src=\"images/$flag\" width=\"32\" height=\"21\" alt=\"$en\" /></td>";
			echo "<td><img src=\"$lang/$file\" alt=\"$name\" /></td></tr>\n";
		}
	}
	echo "</table>\n";
} else
{
	/* output as large table */
	echo "<table>\n";
	echo "<tr valign=\"bottom\">\n";
	echo "<td>&nbsp;</td>\n";
	foreach ($linguas as $lang)
	{
		echo "<td>\n";
		$lang = $lang['lang'];
		$trans = $languages[$lang];
		$en = $trans['en'];
		$native = $trans['native'];
		$flag = $trans['flag'];
		echo "<p><img src=\"images/$flag\" width=\"32\" height=\"21\" alt=\"$en\" />$en</p>\n";
		echo "</td>\n";
	}
	echo "</tr>\n";
	foreach ($en_files as $entry)
	{
		echo "<tr valign=\"top\">\n";
		$name = $entry['name'];
		$file = $entry['file'];
		$num = $entry['num'];
		echo "<td>$name</td>\n";
		foreach ($linguas as $lang)
		{
			$lang = $lang['lang'];
			$trans = $languages[$lang];
			$en = $trans['en'];
			$native = $trans['native'];
			$file = $all_files[$lang][$num]['file'];
			echo "<td style=\"width:320px\"><img src=\"$lang/$file\" alt=\"$en\" style=\"max-width: 320px\"/></td>\n";
		}
		echo "</tr>\n";
	}
	echo "</table>\n";
}
?>

<p></p>
<p></p>

<div style="text-align:center">
<p>
<a href="../emutos.php"> <img src="../images/home1.png" width="180" height="60" style="border:0" alt="Back" /></a>
</p>
</div>


</body>
</html>
