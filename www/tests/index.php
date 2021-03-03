<?php
error_reporting(E_ALL & ~E_WARNING);
ini_set("display_errors", 1);
?>
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
<h1>EmuTOS resources (test)</h1>
<hr />

<table>
<?php
include('../vars.php');
read_linguas('LINGUAS');
foreach ($linguas as $lang)
{
	echo "<tr valign=\"bottom\">\n";
	echo "<td>\n";
	$lang = $lang['lang'];
	if ($lang == 'en')
		continue;
	$trans = $languages[$lang];
	$en = $trans['en'];
	$native = $trans['native'];
	echo "<p><img src=\"../images/$lang.gif\" width=\"32\" height=\"21\" alt=\"$en\" />$en";
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
		fputs($out, "include('../../lang.php');\n");
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
		fputs($out, "include('../../lang.php');\n");
		fputs($out, "?>\n");
		fclose($out);
	}
}
?>

</table>

<br />
<br />

<div style="text-align:center">
<p>
<a href="../emutos.php"> <img src="../../images/home1.png" width="180" height="60" style="border:0" alt="Home" /></a>
</p>
</div>

</div>

</body>
</html>
