<?php

error_reporting(E_ALL);
ini_set("display_errors", 1);

$log = fopen('update.log', 'a');
$currdate = date('Y-m-d H:i:s');
fprintf($log, "%s: start update\n", $currdate);
fclose($log);

$github = 'https://raw.githubusercontent.com/emutos/emutos/master/';

echo '<pre>';

$top = getcwd();

chdir('tmp');

echo "getting LINGUAS\n";

$linguas = array();
$in = fopen($github . 'po/LINGUAS', 'r');
$out = fopen('LINGUAS', 'w');
while (($line = fgets($in)) !== false)
{
	$line = rtrim($line, "\x0D\x0A");
	if (preg_match('/^#.*/', $line, $matches)) continue;
	if (preg_match('/^([a-z]+) (.*)/', $line, $matches))
	{
		$linguas[] = $matches[1];
		fprintf($out, "%s\n", $line);
	}
}
fclose($in);
fclose($out);

echo "getting desk/desktop.rsc\n";
$rsc = file_get_contents($github . 'desk/desktop.rsc');
file_put_contents('desktop.rsc', $rsc);
echo "getting desk/desktop.def\n";
$def = file_get_contents($github . 'desk/desktop.def');
file_put_contents('desktop.def', $def);

echo "getting desk/icon.rsc\n";
$rsc = file_get_contents($github . 'desk/icon.rsc');
file_put_contents('icon.rsc', $rsc);
echo "getting desk/icon.def\n";
$def = file_get_contents($github . 'desk/icon.def');
file_put_contents('icon.def', $def);

echo "getting aes/gem.rsc\n";
$rsc = file_get_contents($github . 'aes/gem.rsc');
file_put_contents('gem.rsc', $rsc);
echo "getting aes/gem.def\n";
$def = file_get_contents($github . 'aes/gem.def');
file_put_contents('gem.def', $def);

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
	system('rm -f ' . $dir . '/*.png; mv *.png ' . $dir);

	system('LD_LIBRARY_PATH=' . $top . ' ' . $top . '/rscview --lang ' . $lang . ' --podir . gem.rsc 2>&1');
	$dir = '../' . $lang . '/aes';
	$stat = stat($dir);
	if (!$stat)
	{
		mkdir($dir);
	}
	system('rm -f ' . $dir . '/*.png; mv *.png ' . $dir);
}

system('LD_LIBRARY_PATH=' . $top . ' ' . $top . '/rscview icon.rsc 2>&1');
$dir = '../icons';
$stat = stat($dir);
if (!$stat)
{
	mkdir($dir);
}
system('rm -f ' . $dir . '/*.png; mv *.png ' . $dir);

echo '</pre>';

?>
