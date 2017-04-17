<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
          "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<title>EmuTOS desktop resources - English</title>
<meta name="keywords" content="ORCS, CAT, GC, PBEM, PBM, GC-Ork, GCORK, ARAnyM, UDO, EmuTOS" />
<link rel="stylesheet" type="text/css" href="../rscview.css" />
</head>

<body>

<hr />
<h1>EmuTOS desktop resources - English</h1>
<hr />

<?php

function cmp_name($a, $b)
{
	if ($a > $b)
		return 1;
	if ($a < $b)
		return -1;
	return 0;
}
if ($dir = opendir('.'))
{
	$files = array();
	while (false !== ($entry = readdir($dir))) {
		if ($entry == ".") continue;
		if ($entry == "..") continue;
		if (!fnmatch("*.png", $entry)) continue;
	    $files[] = $entry;
    }
 	closedir($dir);
    usort($files, 'cmp_name');
    foreach ($files as $entry)
    {
    	$name = $entry;
		if (preg_match('/^[0-9]+_[a-z][a-z]_(.*)\.png/', $entry, $matches))
			$name = strtoupper($matches[1]);
		echo '<p>' . $name . ':<br /><img src="' . $entry . '" alt="' . $name . '" />' . "</p>\n";
	}
}
?>

<p></p>
<p></p>

<div style="text-align:center">
<p>
<a href="../index.html"> <img src="../images/home1.gif" width="180" height="60" style="border:0" alt="Back" /></a>
</p>
</div>


</body>
</html>
