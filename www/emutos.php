<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
          "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>EmuTOS desktop resources</title>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<meta name="keywords" content="ORCS, CAT, GC, PBEM, PBM, GC-Ork, GCORK, ARAnyM, UDO, EmuTOS" />
<link rel="stylesheet" type="text/css" href="emutos.css" />
</head>

<body>
<div>

<hr />
<h1>EmuTOS resources</h1>
<hr />

<table>
<?php
include('vars.php');
read_linguas('tmp/LINGUAS');
foreach ($linguas as $lang)
{
	echo "<tr valign=\"bottom\">\n";
	echo "<td>\n";
	$lang = $lang['lang'];
	$trans = $languages[$lang];
	$en = $trans['en'];
	$native = $trans['native'];
	$flag = $trans['flag'];
	echo "<p><img src=\"images/$flag\" width=\"32\" height=\"21\" alt=\"$en\" />$en";
	if ($en != $native)
	{
		echo " / $native";
	}
	echo "</p>\n";
	echo "</td>\n";
	echo "<td><a href=\"$lang/index.php\">Desktop</a></td>\n";
	echo "<td><a href=\"$lang/aes.php\">AES</a></td>\n";
	echo "</tr>\n";
	$stat = stat("$lang/index.php");
	if (!$stat)
	{
		$out = fopen("$lang/index.php", 'w');
		fputs($out, "<?php\n");
		fputs($out, "\$lang = \"$en\";\n");
		fputs($out, "\$pngdir = '.';\n");
		fputs($out, "\$top = '..';\n");
		fputs($out, "\$which = 'desktop';\n");
		fputs($out, "include('../lang.php');\n");
		fputs($out, "?>\n");
		fclose($out);
	}
	$stat = stat("$lang/aes.php");
	if (!$stat)
	{
		$out = fopen("$lang/aes.php", 'w');
		fputs($out, "<?php\n");
		fputs($out, "\$lang = \"$en\";\n");
		fputs($out, "\$pngdir = 'aes';\n");
		fputs($out, "\$top = '..';\n");
		fputs($out, "\$which = 'AES';\n");
		fputs($out, "include('../lang.php');\n");
		fputs($out, "?>\n");
		fclose($out);
	}
}
?>
<tr valign="bottom">
<td>
<p><br />All languages (table)</p>
</td>
<td><a href="all_desk.php?type=table">Desktop</a></td>
<td><a href="all_aes.php?type=table">AES</a></td>
</tr>
<tr valign="bottom">
<td>
<p><br />All languages (list)</p>
</td>
<td><a href="all_desk.php?type=list">Desktop</a></td>
<td><a href="all_aes.php?type=list">AES</a></td>
</tr>

<tr valign="bottom">
<td>
<p><br />All languages</p>
</td>
<td><a href="icons/index.php">Icons</a></td>
</tr>

<tr valign="bottom">
<td>
<p><br />All languages</p>
</td>
<td><a href="cursors/index.php">Mouse Cursors</a></td>
</tr>

</table>

<!-- ******************************************************************************* -->

<hr />
<h1>Builtin fonts</h1>
<hr />

<p>Atari-ST encoding, used for English, German, French, Spanish, Finnish and Italian.

<a href="fonts/font8x16_atarist_small.html">Original Size</a>
<a href="fonts/font8x16_atarist_large.html">Large Size</a></p>

<p>Latin2 encoding, used for Czech, Polish and Hungarian.

<a href="fonts/font8x16_latin2_small.html">Original Size</a>
<a href="fonts/font8x16_latin2_large.html">Large Size</a></p>

<p>Latin9 encoding, formerly used for Spanish.

<a href="fonts/font8x16_latin9_small.html">Original Size</a>
<a href="fonts/font8x16_latin9_large.html">Large Size</a></p>

<p>cp737 encoding, used for Greek.

<a href="fonts/font8x16_greek_small.html">Original Size</a>
<a href="fonts/font8x16_greek_large.html">Large Size</a></p>

<p>russian-atarist encoding, used for Russian.

<a href="fonts/font8x16_russian_small.html">Original Size</a>
<a href="fonts/font8x16_russian_large.html">Large Size</a></p>


<br />
<br />

<table>
<tr>
<td>
<form action="update.php" method="get" id="emudeskform" enctype="application/x-www-form-urlencoded">
<fieldset>
<input id="updateemudesk" style="background-color: #cccccc; font-weight: bold;" type="submit" value="Update from GitHub" />
<?php
if (isset($_COOKIE['itsme']))
{
	echo "<input type=\"checkbox\" name=\"download\" value=\"0\" />Don't download\n";
}
?>
</fieldset>
</form>
</td>

<td>
<form action="viewlog.php" method="get" id="viewlogform" enctype="application/x-www-form-urlencoded">
<fieldset>
<input id="viewlog" style="background-color: #cccccc; font-weight: bold;" type="submit" value="View logfile" />
</fieldset>
</form>
</td>

<td>
<form action="update.php" method="post" id="testform" enctype="multipart/form-data">
<fieldset>
<input type="file" name="files[]" size="60" accept=".po,.PO" multiple="multiple" required="required" style="margin-top: 1ex;" />
<input id="submitfile" type="submit" value="Test translation" />
</fieldset>
</form>
</td>

</tr>
</table>

<div style="text-align:center">
<p>
<a href="../index.html"> <img src="../images/home1.png" width="180" height="60" style="border:0" alt="Home" /></a>
</p>
</div>

<hr />
<div style="text-align:center">
<p style="font-size: 60%">Images of flags are from
<a href="http://flagspot.net/flags/"><img src="images/fotwlink.gif" width="45" height="30" style="border:0" alt="FOTW"> Flags Of The World</a>
</p>
</div>

</div>

</body>
</html>
