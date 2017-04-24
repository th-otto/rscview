<?php

include('vars.php');

error_reporting(E_ALL);
ini_set("display_errors", 1);

$log = fopen('update.log', 'a');
$currdate = date('Y-m-d H:i:s');
fprintf($log, "%s: start update\n", $currdate);
fclose($log);

$log = fopen('github.log', 'w');

$github = 'https://raw.githubusercontent.com/emutos/emutos/master/';

function log_output($str)
{
	global $log;
	fputs($log, $str);
	return $str;
}

echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n";
echo "          \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
echo "<html xml:lang=\"en\" lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n";
echo "<head>\n";
echo "<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\" />\n";
echo "<pre>\n";
flush();
ob_start("log_output");

$top = getcwd();

chdir('tmp');

echo "getting LINGUAS\n";
ob_flush(); flush();
$rsc = file_get_contents($github . 'po/LINGUAS');
file_put_contents('LINGUAS', $rsc);
read_linguas('LINGUAS', 1);
ob_flush(); flush();

echo "getting desk/desktop.rsc\n";
ob_flush(); flush();
$rsc = file_get_contents($github . 'desk/desktop.rsc');
file_put_contents('desktop.rsc', $rsc);
ob_flush(); flush();
echo "getting desk/desktop.def\n";
ob_flush(); flush();
$def = file_get_contents($github . 'desk/desktop.def');
file_put_contents('desktop.def', $def);
ob_flush(); flush();

echo "getting desk/icon.rsc\n";
ob_flush(); flush();
$rsc = file_get_contents($github . 'desk/icon.rsc');
file_put_contents('icon.rsc', $rsc);
ob_flush(); flush();
echo "getting desk/icon.def\n";
ob_flush(); flush();
$def = file_get_contents($github . 'desk/icon.def');
file_put_contents('icon.def', $def);
ob_flush(); flush();

echo "getting aes/gem.rsc\n";
ob_flush(); flush();
$rsc = file_get_contents($github . 'aes/gem.rsc');
file_put_contents('gem.rsc', $rsc);
ob_flush(); flush();
echo "getting aes/gem.def\n";
$def = file_get_contents($github . 'aes/gem.def');
file_put_contents('gem.def', $def);
ob_flush(); flush();

foreach ($linguas as $lang)
{
	$lang = $lang['lang'];
	if ($lang != 'en')
	{
		echo "getting $lang.po\n";
		$po = file_get_contents($github . 'po/' . $lang . '.po');
		file_put_contents($lang . '.po', $po);
	}
}

foreach ($linguas as $lang)
{
	$lang = $lang['lang'];
	echo "generating images for $lang\n";
	ob_flush(); flush();
	system("LD_LIBRARY_PATH=$top $top/rscview --lang $lang --podir . desktop.rsc 2>&1");
	ob_flush(); flush();
	$dir = "../$lang";
	$trans = $languages[$lang];
	$stat = stat($dir);
	if (!$stat)
	{
		echo "creating directory for new language $lang\n";
		mkdir($dir);
		mkdir("$dir/aes");
		ob_flush(); flush();
	}
	system("rm -f $dir/*.png; mv *.png $dir");
	ob_flush(); flush();

	system("LD_LIBRARY_PATH=$top $top/rscview --lang $lang --podir . gem.rsc 2>&1");
	ob_flush(); flush();
	$dir = "../$lang/aes";
	system("rm -f $dir/*.png; mv *.png $dir");
	echo "\n";
	ob_flush(); flush();
}

system("LD_LIBRARY_PATH=$top $top/rscview icon.rsc 2>&1");
$dir = '../icons';
$stat = stat($dir);
if (!$stat)
{
	mkdir($dir);
}
system("rm -f $dir/*.png; mv *.png $dir");
ob_flush(); flush();
ob_end_flush();
fclose($log);

echo "</pre>\n";
echo "</body>\n";
echo "</html>\n";

?>
