<?php

$linguas = array();

$languages = array(
	'en' => array('en' => 'English',   'html' => 'en-US', 'native' => 'English'),
	'de' => array('en' => 'German',    'html' => 'de-DE', 'native' => 'Deutsch'),
	'cs' => array('en' => 'Czech',     'html' => 'cs-CZ', 'native' => '&#268;esky'),
	'es' => array('en' => 'Spanish',   'html' => 'es-ES', 'native' => 'Espa&#241;ol'),
	'fi' => array('en' => 'Finnish',   'html' => 'fi-FI', 'native' => 'Suomi'),
	'fr' => array('en' => 'French',    'html' => 'fr-FR', 'native' => 'Fran&#231;ais'),
	'gr' => array('en' => 'Greek',     'html' => 'el-GR', 'native' => '&#917;&#955;&#955;&#951;&#957;&#953;&#954;&#940;'), /* deprecated; for compatibility */
	'el' => array('en' => 'Greek',     'html' => 'el-GR', 'native' => '&#917;&#955;&#955;&#951;&#957;&#953;&#954;&#940;'),
	'it' => array('en' => 'Italian',   'html' => 'it-IT', 'native' => 'Italiano'),
	'ru' => array('en' => 'Russian',   'html' => 'ru-RU', 'native' => '&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081;'),
	'nn' => array('en' => 'Norwegian', 'html' => 'nn-NO', 'native' => 'Norsk nynorsk'),
	'sv' => array('en' => 'Swedish',   'html' => 'sv-SV', 'native' => 'Svenska'),
	'nl' => array('en' => 'Dutch',     'html' => 'nl-NL', 'native' => 'Nederlands'),
	'da' => array('en' => 'Danish',    'html' => 'da-DK', 'native' => 'Dansk'),
	'tr' => array('en' => 'Turkish',   'html' => 'tr-TR', 'native' => 'T&#252;rk&#231;e'),
	'ar' => array('en' => 'Arabic',    'html' => 'ar-AR', 'native' => '&#x0627;&#x0644;&#x0639;&#x0631;&#x0628;&#x064a;&#x0629;'),
);

function read_linguas($filename, $warn = 0)
{
	global $linguas;
	global $languages;
	$linguas = array();
	$linguas[] = array('lang' => 'en', 'charset' => 'atarist');
	$in = fopen($filename, 'r');
	while (($line = fgets($in)) !== false)
	{
		$line = rtrim($line, "\x0D\x0A");
		if (preg_match('/^#.*/', $line, $matches)) continue;
		if (preg_match('/^([a-z]+) (.*)/', $line, $matches))
		{
			$linguas[] = array('lang' => $matches[1], 'charset' => $matches[2]);
		}
	}
	fclose($in);
	foreach ($linguas as $lang)
	{
		$lang = $lang['lang'];
		$trans = $languages[$lang];
		if (!$trans)
		{
			$trans = array('en' => $lang, 'native' => $lang);
			$languages[$lang] = $trans;
			if ($warn)
			{
				echo "warning: unknown language $lang\n";
			}
		}
	}
}

function cmp_name($a, $b)
{
	if ($a['file'] > $b['file'])
		return 1;
	if ($a['file'] < $b['file'])
		return -1;
	return 0;
}

function read_pngs($dirname)
{
	global $files;
	$files = array();
	if ($dir = opendir($dirname))
	{
		while (false !== ($entry = readdir($dir))) {
			if ($entry == ".") continue;
			if ($entry == "..") continue;
			if (!fnmatch("*.png", $entry)) continue;
			if (preg_match('/^[0-9]+_[a-z][a-z]_(.*)\.png/', $entry, $matches))
				$name = strtoupper($matches[1]);
			else if (preg_match('/^[0-9]+_(.*)\.png/', $entry, $matches))
				$name = strtoupper($matches[1]);
		    $files[] = array('file' => $entry, 'name' => $name);
	    }
	 	closedir($dir);
	    usort($files, 'cmp_name');
	}
}

?>
