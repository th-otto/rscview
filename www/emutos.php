<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
          "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>EmuTOS desktop resources</title>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<meta name="keywords" content="ORCS, CAT, GC, PBEM, PBM, GC-Ork, GCORK, ARAnyM, UDO, EmuTOS" />
<style type="text/css">
body {
	margin:1em;
	padding:1em;
	background-color: #ffffff;
	color: #0000FF;
	link-color: #8000FF;
}
a:link, a:visited {
	color: #8000FF;
	text-decoration: underline;
}
tr {
  margin:0;
  padding:0;
}
td {
  margin:0;
  padding:0 0 0 10px;
}
p {
  margin:0;
  padding:0;
}
</style>
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
	echo "<p><img src=\"images/$lang.gif\" width=\"32\" height=\"21\" alt=\"$en\" />$en";
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

</table>

<!-- ******************************************************************************* -->

<hr />
<h1>Builtin fonts</h1>
<hr />

<p>Atari-ST encoding, used for English, German, French, Finnish and Italian.

<a href="fonts/font8x16_atarist_small.html">Original Size</a>
<a href="fonts/font8x16_atarist_large.html">Large Size</a></p>

<p>Latin2 encoding, used for Czech.

<a href="fonts/font8x16_latin2_small.html">Original Size</a>
<a href="fonts/font8x16_latin2_large.html">Large Size</a></p>

<p>Latin9 encoding, used for Spanish.

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
<form action="update.php" method="get" id="emudeskform">
<fieldset>
<input id="updateemudesk" style="background-color: #cccccc; font-weight: bold;" type="submit" value="Update from GitHub" />
<!-- <input type="checkbox" name="download" value="0" />Don't download -->
</fieldset>
</form>
</td>

<td>
<form action="viewlog.php" method="get" id="viewlogform">
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
<a href="../index.html"> <img src="../images/home1.gif" width="180" height="60" style="border:0" alt="Home" /></a>
</p>
</div>

</div>

</body>
</html>
