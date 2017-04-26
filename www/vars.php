<?php

$linguas = array();

$languages = array(
	'en' => array('en' => 'English',   'html' => 'en-US', 'flag' => 'uk.gif', 'native' => 'English'),
	'de' => array('en' => 'German',    'html' => 'de-DE', 'flag' => 'de.gif', 'native' => 'Deutsch'),
	'fr' => array('en' => 'French',    'html' => 'fr-FR', 'flag' => 'fr.gif', 'native' => 'Fran&#231;ais'),
	'es' => array('en' => 'Spanish',   'html' => 'es-ES', 'flag' => 'es.gif', 'native' => 'Espa&#241;ol'),
	'it' => array('en' => 'Italian',   'html' => 'it-IT', 'flag' => 'it.gif', 'native' => 'Italiano'),
	'sv' => array('en' => 'Swedish',   'html' => 'sv-SV', 'flag' => 'se.gif', 'native' => 'Svenska'),
	'tr' => array('en' => 'Turkish',   'html' => 'tr-TR', 'flag' => 'tr.gif', 'native' => 'T&#252;rk&#231;e'),
	'fi' => array('en' => 'Finnish',   'html' => 'fi-FI', 'flag' => 'fi.gif', 'native' => 'Suomi'),
	'nn' => array('en' => 'Norwegian', 'html' => 'nn-NO', 'flag' => 'no.gif', 'native' => 'Norsk nynorsk'),
	'da' => array('en' => 'Danish',    'html' => 'da-DK', 'flag' => 'dk.gif', 'native' => 'Dansk'),
	'ar' => array('en' => 'Arabic',    'html' => 'ar-SA', 'flag' => 'sa.gif', 'native' => '&#x0627;&#x0644;&#x0639;&#x0631;&#x0628;&#x064a;&#x0629;'),
	'nl' => array('en' => 'Dutch',     'html' => 'nl-NL', 'flag' => 'nl.gif', 'native' => 'Nederlands'),
	'cs' => array('en' => 'Czech',     'html' => 'cs-CZ', 'flag' => 'cz.gif', 'native' => '&#268;esky'),
	'hu' => array('en' => 'Hungarian', 'html' => 'hu-HU', 'flag' => 'hu.gif', 'native' => 'Magyar'),
	'pl' => array('en' => 'Polish',    'html' => 'pl-PL', 'flag' => 'pl.gif', 'native' => 'Polskie'),
	'lt' => array('en' => 'Lithuanian','html' => 'lt-LT', 'flag' => 'lt.gif', 'native' => 'Lietuvi&#x0173; kalba'),
	'ru' => array('en' => 'Russian',   'html' => 'ru-RU', 'flag' => 'ru.gif', 'native' => '&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081;'),
	'et' => array('en' => 'Estonian',  'html' => 'et-EE', 'flag' => 'ee.gif', 'native' => 'Eesti'),
	'be' => array('en' => 'Belarusian','html' => 'be-bY', 'flag' => 'by.gif', 'native' => '&#x0431;&#x0435;&#x043b;&#x0430;&#x0440;&#x0443;&#x0441;&#x043a;&#x0430;&#x044f; &#x043c;&#x043e;&#x0432;&#x0430;'),
	'uk' => array('en' => 'Ukrainian', 'html' => 'uk-uA', 'flag' => 'ua.gif', 'native' => '&#x0423;&#x043a;&#x0440;&#x0430;&#x0457;&#x043d;&#x0441;&#x044c;&#x043a;&#x0430;'),
	'sk' => array('en' => 'Slovak',    'html' => 'sk-SK', 'flag' => 'sk.gif', 'native' => 'Sloven&#x010d;ina'),
	'hr' => array('en' => 'Croatian',  'html' => 'hr-HR', 'flag' => 'hr.gif', 'native' => 'Hrvatski jezik'),
	'sr' => array('en' => 'Serbian',   'html' => 'sr-RS', 'flag' => 'rs.gif', 'native' => '&#x0441;&#x0440;&#x043f;&#x0441;&#x043a;&#x0438; &#x0458;&#x0435;&#x0437;&#x0438;&#x043a;'),
	'mk' => array('en' => 'Macedonian','html' => 'mk-MK', 'flag' => 'mk.gif', 'native' => '&#x043c;&#x0430;&#x043a;&#x0435;&#x0434;&#x043e;&#x043d;&#x0441;&#x043a;&#x0438; &#x0458;&#x0430;&#x0437;&#x0438;&#x043a;'),
	'gr' => array('en' => 'Greek',     'html' => 'el-GR', 'flag' => 'gr.gif', 'native' => '&#917;&#955;&#955;&#951;&#957;&#953;&#954;&#940;'), /* deprecated; for compatibility */
	'el' => array('en' => 'Greek',     'html' => 'el-GR', 'flag' => 'gr.gif', 'native' => '&#917;&#955;&#955;&#951;&#957;&#953;&#954;&#940;'),
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
			$trans = array('en' => $lang, 'native' => $lang, 'flag' => $lang . '.gif');
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
