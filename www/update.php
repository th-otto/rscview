<?php

error_reporting(E_ALL);
ini_set("display_errors", 1);

$github = 'https://raw.githubusercontent.com/emutos/emutos/master/';

echo '<pre>';

$top = getcwd();

chdir('tmp');

echo "getting LINGUAS\n";

$linguas = array();
$in = fopen($github . 'po/LINGUAS', 'r');
while (($line = fgets($in)) !== false)
{
	$line = rtrim($line, "\x0D\x0A");
	if (preg_match('/^#.*/', $line, $matches)) continue;
	if (preg_match('/^([a-z]+) (.*)/', $line, $matches))
		$linguas[] = $matches[1];
}
fclose($in);

echo "getting desk/desktop.rsc\n";
$rsc = file_get_contents($github . 'desk/desktop.rsc');
file_put_contents('desktop.rsc', $rsc);
echo "getting desk/desktop.def\n";
$def = file_get_contents($github . 'desk/desktop.def');
file_put_contents('desktop.def', $def);

foreach ($linguas as $lang)
{
	echo "getting $lang.po\n";
	$po = file_get_contents($github . 'po/' . $lang . '.po');
	file_put_contents($lang . '.po', $po);
}

$linguas[] = 'en';

foreach ($linguas as $lang)
{
	echo "generating images for $lang\n";
	system('LD_LIBRARY_PATH=' . $top . ' ' . $top . '/rscview --lang ' . $lang . ' --podir . desktop.rsc 2>&1');
	$dir = '../' . $lang;
	$stat = stat($dir);
	if (!$stat)
	{
		mkdir($dir);
	}
	system('mv *.png ' . $dir);
}

echo '</pre>';

?>
