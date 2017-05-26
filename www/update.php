<?php

include('vars.php');

error_reporting(E_ALL & ~E_WARNING);
ini_set("display_errors", 1);

echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n";
echo "          \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
echo "<html xml:lang=\"en\" lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n";
echo "<head>\n";
echo "<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\" />\n";
echo "<pre>\n";

$top = getcwd();
	
if ($_SERVER['REQUEST_METHOD'] == 'POST')
{
	$tests = array();
	foreach ($_FILES['files']['error'] as $key => $error)
	{
		$tmp_name = $_FILES['files']['tmp_name'][$key];
		$po = basename($_FILES['files']['name'][$key]);
		if (!move_uploaded_file($tmp_name, "tmp/$po"))
		{
			echo "Error: file upload failed\n";
			die();
		}
		$lang = str_replace('.po', '', $po);
		$tests[] = $lang;
	}
}	

$log = fopen('update.log', 'a');
$currdate = date('Y-m-d H:i:s');
fprintf($log, "%s: start update\n", $currdate);
fclose($log);

$github = 'https://raw.githubusercontent.com/emutos/emutos/master/';

function log_output($str)
{
	global $log;
	fputs($log, $str);
	return $str;
}

function gen_images($lang, $dir)
{
	global $languages;
	global $top;
	
	echo "generating images for $lang\n";
	system("LD_LIBRARY_PATH=$top $top/rscview --lang $lang --podir . --create-html pngout.php --html-dir . --imagemap desktop.rsc 2>&1");
	$trans = $languages[$lang];
	$stat = stat($dir);
	if (!$stat)
	{
		echo "creating directory for new language $lang\n";
		mkdir($dir);
		mkdir("$dir/aes");
	}
	system("rm -f $dir/*.png; mv *.png pngout.php $dir");

	system("LD_LIBRARY_PATH=$top $top/rscview --lang $lang --podir . --create-html pngout.php --html-dir aes --imagemap gem.rsc 2>&1");
	$dir .= "/aes";
	system("rm -f $dir/*.png; mv *.png pngout.php $dir");
	echo "\n";
}

if ($_SERVER['REQUEST_METHOD'] == 'POST')
{
	# a single po-file to be tested has been uploaded
	chdir('tmp');

	read_linguas('LINGUAS', 1);
	$out = fopen('../tests/LINGUAS', 'w');
	foreach ($tests as $lang)
	{
		$charset = '';
		foreach ($linguas as $lingua)
		{
			if ($lingua['lang'] == $lang)
			{
				$charset = $lingua['charset'];
				break;
			}
		}
		if ($charset == '')
		{
			$charset = 'atarist';
			echo "warning: unknown language $lang, using $charset as character set\n";
		}
		fprintf($out, "$lang $charset\n");
	}
	fclose($out);
	
	foreach ($tests as $lang)
	{
		gen_images($lang, "../tests/$lang");
	}

	echo "</pre>\n";

	echo "<div style=\"text-align:center\">\n";
	echo "<p>\n";
	echo "<a href=\"tests/index.php\">View results</a>\n";
	echo "</p>\n";
	echo "</div>\n";
	
} else if ($_SERVER['REQUEST_METHOD'] == 'GET')
{
	# recreate all images for all languages
	$log = fopen('github.log', 'w');

	flush();
	ob_start("log_output");
	
	#
	# create new images in temporary directory,
	# to avoid leaving inconsistent, old images behind
	# just in case the rscview tool crashes
	#
	chdir('tmp');

	#
	# download needed files from master repo
	#
	if (!isset($_GET['download']) || $_GET['download'] != 0)
	{
		echo "getting LINGUAS\n";
		$rsc = file_get_contents($github . 'po/LINGUAS');
		file_put_contents('LINGUAS', $rsc);
		read_linguas('LINGUAS', 1);
		
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
			$lang = $lang['lang'];
			if ($lang != 'en')
			{
				echo "getting $lang.po\n";
				$po = file_get_contents($github . 'po/' . $lang . '.po');
				file_put_contents($lang . '.po', $po);
			}
		}
	} else {
		read_linguas('LINGUAS', 1);
	}

	#
	# create images for desktop & aes
	#
	foreach ($linguas as $lang)
	{
		$lang = $lang['lang'];
		gen_images($lang, "../$lang");
	}
	
	#
	# create images for icons
	#
	system("LD_LIBRARY_PATH=$top $top/rscview --create-html pngout.php --html-dir . --imagemap icon.rsc 2>&1");
	$dir = '../icons';
	$stat = stat($dir);
	if (!$stat)
	{
		mkdir($dir);
	}
	system("rm -f $dir/*.png; mv *.png pngout.php $dir");

	ob_end_flush();
	fclose($log);
	
	echo "</pre>\n";
}
	
echo "</body>\n";
echo "</html>\n";

?>
