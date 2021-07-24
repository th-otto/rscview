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

function gen_images($lang, $dir, $genlist)
{
	global $languages;
	global $top;
	
	echo "generating images for $lang\n";
	$cmd = "$top/rscview --lang $lang --podir . --timestamps --quote-html --create-html pngout.php --report-po --report-rsc";
	if ($genlist)
		$cmd .= " --create-pnglist pnglist.php";
	$cmd .= " --html-dir . --imagemap desktop.rsc 2>&1";
	system($cmd);
	$trans = $languages[$lang];
	$stat = stat($dir);
	if (!$stat)
	{
		echo "creating directory for new language $lang\n";
		mkdir($dir);
		mkdir("$dir/aes");
	}
	system("rm -f $dir/*.png; mv *.png pngout.php $dir 2>&1");
	if ($genlist)
		system("mv pnglist.php $dir 2>&1");
	
	$cmd = "$top/rscview --lang $lang --podir . --timestamps --quote-html --create-html pngout.php --report-rsc";
	if ($genlist)
		$cmd .= " --create-pnglist pnglist.php";
	$cmd .= " --html-dir aes --imagemap gem.rsc 2>&1";
	system($cmd);
	$dir .= "/aes";
	system("rm -f $dir/*.png; mv *.png pngout.php $dir 2>&1");
	if ($genlist)
		system("mv pnglist.php $dir 2>&1");
	echo "\n";
}

function emutos_download($remotefile, $localfile, $must_exist = true)
{
	global $github;
	echo "getting $remotefile\n";
	$rsc = file_get_contents($github . $remotefile);
	if (!$rsc)
	{
		if ($must_exist)
			echo "<span style=\"color:red\"";
		echo "downloading $remotefile failed";
		if ($must_exist)
			echo "</span>";
		echo "\n";
	} else
	{
		if (file_put_contents($localfile, $rsc) === false)
		{
			echo "<span style=\"color:red\">error creating $localfile</span>\n";
		}
	}
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
			if ($lang == 'pl' || $lang == 'hu')
			{
				$charset = 'latin2';
			} else if ($lang == 'tr')
			{
				$charset = 'latin5';
			} else
			{
				$charset = 'atarist';
			}
			echo "warning: unknown language $lang, using $charset as character set\n";
			$app = fopen('LINGUAS', 'a');
			fprintf($app, "$lang $charset\n");
			fclose($app);
		}
		fprintf($out, "$lang $charset\n");
	}
	fclose($out);
	
	foreach ($tests as $lang)
	{
		gen_images($lang, "../tests/$lang", 0);
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
		emutos_download('po/LINGUAS', 'LINGUAS', false);
		read_linguas('LINGUAS', 1);
		
		emutos_download('desk/desktop.rsc', 'desktop.rsc');
		emutos_download('desk/desktop.def', 'desktop.def');
		
		emutos_download('desk/icon.rsc', 'icon.rsc');
		emutos_download('desk/icon.def', 'icon.def');

		emutos_download('aes/gem.rsc', 'gem.rsc');
		emutos_download('aes/gem.def', 'gem.def');
		
		emutos_download('aes/mform.rsc', 'mform.rsc');
		emutos_download('aes/mform.def', 'mform.def');
		
		foreach ($linguas as $lang)
		{
			$lang = $lang['lang'];
			if ($lang != 'en')
			{
				emutos_download("po/$lang.po", "$lang.po");
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
		gen_images($lang, "../$lang", 1);
	}
	
	#
	# create images for icons
	#
	system("$top/rscview --quote-html --create-html pngout.php --html-dir . --imagemap icon.rsc 2>&1");
	$dir = '../icons';
	$stat = stat($dir);
	if (!$stat)
	{
		mkdir($dir);
	}
	system("rm -f $dir/*.png; mv *.png pngout.php $dir 2>&1");

	#
	# create images for cursors
	#
	system("$top/rscview --quote-html --create-html pngout.php --html-dir . --imagemap mform.rsc 2>&1");
	$dir = '../cursors';
	$stat = stat($dir);
	if (!$stat)
	{
		mkdir($dir);
	}
	system("rm -f $dir/*.png; mv *.png pngout.php $dir 2>&1");

	ob_end_flush();
	fclose($log);
	
	echo "</pre>\n";
}
	
echo "</body>\n";
echo "</html>\n";

?>
