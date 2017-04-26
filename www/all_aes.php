<?php
$lang = "All Languages";
$pngdir = 'aes';
$which = 'AES';
$top = '.';
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
<link rel="stylesheet" type="text/css" href="rscview.css" />
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
read_pngs('en/aes');
if (isset($_GET['type']) && $_GET['type'] == 'table')
{
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
	foreach ($files as $entry)
	{
		echo "<tr valign=\"top\">\n";
		$name = $entry['name'];
		$file = $entry['file'];
		echo "<td>$name</td>\n";
		foreach ($linguas as $lang)
		{
			$lang = $lang['lang'];
			$trans = $languages[$lang];
			$en = $trans['en'];
			$native = $trans['native'];
			$file = preg_replace('/([0-9]+)_[a-z][a-z]_(.*)\.png/', '${1}_' . $lang . '_${2}.png', $file);
			echo "<td style=\"width:320px\"><img src=\"$lang/aes/$file\" alt=\"$en\" style=\"max-width: 320px\"/></td>\n";
		}
		echo "</tr>\n";
	}
	echo "</table>\n";
} else
{
	echo "<table>\n";
	foreach ($files as $entry)
	{
		$name = $entry['name'];
		$file = $entry['file'];
		echo "<tr valign=\"bottom\"><td>$name:</td></tr>\n";
		foreach ($linguas as $lang)
		{
			$lang = $lang['lang'];
			$trans = $languages[$lang];
			$en = $trans['en'];
			$native = $trans['native'];
			$flag = $trans['flag'];
			$file = preg_replace('/([0-9]+)_[a-z][a-z]_(.*)\.png/', '${1}_' . $lang . '_${2}.png', $file);
			echo "<tr valign=\"top\"><td><img src=\"images/$flag\" width=\"32\" height=\"21\" alt=\"$en\" /></td>";
			echo "<td><img src=\"$lang/aes/$file\" alt=\"$name\" /></td></tr>\n";
		}
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
